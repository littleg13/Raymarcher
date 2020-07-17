#version 460

#define REFLECT 1

const int CUBE_TYPE = 0;
const int MAX_NUM_LIGHTS = 3;

const float MinDistance = 0.001;
const int MaxRaySteps = 64;

const vec2 iRes = vec2(0.005, 0.005);

struct Primitive{
    vec4 position;
    vec4 quaternion;
    vec4 ka;
    vec4 kd;
    vec4 ks;
    float m;
    float alpha;
    float ref;
    uint type;
    float scale;
};

layout(std430, binding = 5) buffer primitive_data{
    Primitive primitives[ ]; 
};

struct Camera{
    vec4 position;
    vec4 lookAt;
    float zoom;
};

uniform Camera cam;
uniform int width;
uniform int height;
uniform int n_primitives;

uniform int actualNumLights = 3;
uniform vec4 lightPos[MAX_NUM_LIGHTS];
uniform bool lightEnabled[MAX_NUM_LIGHTS];
uniform vec3 lightStrength[MAX_NUM_LIGHTS];
uniform vec3 globalAmbient = vec3(0.1, 0.1, 0.1);

uniform float currentTime = 0;

layout(rgba32f, binding = 0) uniform writeonly  image2D img;
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

float cube_est(vec3 p, Primitive box){
    vec3 q = abs(p) - vec3(box.scale, box.scale, box.scale);
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - box.scale / 16;//- (sin(currentTime * 5) + 1) * box.scale / 4;
}

float displacement(vec3 p){
    return 0.3 * sin(10 * sin(currentTime)*p.x)*sin(10 * sin(currentTime)*p.y)*sin(10 * sin(currentTime)*p.z);
}

vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}

/* skew constants for 3d simplex functions */
const float F3 =  0.3333333;
const float G3 =  0.1666667;

/* 3d simplex noise */
float simplex3d(vec3 p) {
	 /* 1. find current tetrahedron T and it's four vertices */
	 /* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */
	 /* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/
	 
	 /* calculate s and x */
	 vec3 s = floor(p + dot(p, vec3(F3)));
	 vec3 x = p - s + dot(s, vec3(G3));
	 
	 /* calculate i1 and i2 */
	 vec3 e = step(vec3(0.0), x - x.yzx);
	 vec3 i1 = e*(1.0 - e.zxy);
	 vec3 i2 = 1.0 - e.zxy*(1.0 - e);
	 	
	 /* x1, x2, x3 */
	 vec3 x1 = x - i1 + G3;
	 vec3 x2 = x - i2 + 2.0*G3;
	 vec3 x3 = x - 1.0 + 3.0*G3;
	 
	 /* 2. find four surflets and store them in d */
	 vec4 w, d;
	 
	 /* calculate surflet weights */
	 w.x = dot(x, x);
	 w.y = dot(x1, x1);
	 w.z = dot(x2, x2);
	 w.w = dot(x3, x3);
	 
	 /* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
	 w = max(0.6 - w, 0.0);
	 
	 /* calculate surflet components */
	 d.x = dot(random3(s), x);
	 d.y = dot(random3(s + i1), x1);
	 d.z = dot(random3(s + i2), x2);
	 d.w = dot(random3(s + 1.0), x3);
	 
	 /* multiply d by w^4 */
	 w *= w;
	 w *= w;
	 d *= w;
	 
	 /* 3. return the sum of the four surflets */
	 return dot(d, vec4(52.0));
}

float opDisplace(vec3 p, Primitive prim){
    
    //Reflect
    vec3 c = vec3(2, 10, 2);
    vec3 l = vec3(4, 2, 1);
    vec3 q1 = mod(p+0.5*c,c)-0.5*c;

    const float k = 1.0 * sin(currentTime * 20) * sin(mod(currentTime, 3.14 / 2) + 3.14/2);
    //Bend
    // float c1 = cos(k*q.x);
    // float s = sin(k*q.x);
    // mat2  m = mat2(c1,-s,s,c1);
    // vec3  q1 = vec3(m*q.xy,q.z);
    return cube_est(p, prim) + 0.01 * max(0.0, simplex3d(p * 10)) * max(0.0, simplex3d(p * 3));
}
float distWrapper(vec3 p, Primitive prim){
    p += prim.position.xyz;
    if(prim.type == CUBE_TYPE)
        return opDisplace(p, prim);
}



float distanceEstimator(vec3 p, out int primClosest){
    float min = 99999;
    float distance;
    primClosest = 0;
    for(int i=0;i<n_primitives;i++){
        distance = distWrapper(p, primitives[i]);
        if(distance < min){
            min = distance;
            primClosest = i;
        }
    }
    return min;
}

vec3 calculateNormal(vec3 p, Primitive prim){
    float d = 0.01;
    float xDiff = distWrapper(p + vec3(d, 0, 0), prim) - distWrapper(p + vec3(-d, 0, 0), prim);
    float yDiff = distWrapper(p + vec3(0, d, 0), prim) - distWrapper(p + vec3(0, -d, 0), prim);
    float zDiff = distWrapper(p + vec3(0, 0, d), prim) - distWrapper(p + vec3(0, 0, -d), prim);
    return normalize(vec3(xDiff, yDiff, zDiff));
}

float shadow(vec3 origin, vec3 direction, float tMax, float k){
    int primHit;
    float t;
    float res = 1.0;
    float prevDistance = 1e20;
    for(t=100*MinDistance;t<tMax;){
        vec3 p = origin + t * direction;
        float distance = distanceEstimator(p, primHit);
        if(distance < MinDistance) 
            return 0.0;
        float y = distance * distance / (2.0 * prevDistance);
        float d = sqrt(distance * distance - y * y);
        res = min(res, k*d/max(0.0, t-y));
        t += distance;
    }
    return res;
}

float trace(vec3 origin, vec3 direction, out int primHit){
    float tMin = 1.0;
    float tMax = 6.0;
    float totalDistance = tMin;
    primHit = 0;
    int steps;
    for(steps = 0; steps< MaxRaySteps; steps++){
        vec3 p = origin + totalDistance * direction;
        float distance = distanceEstimator(p, primHit);
        if(distance > tMax)
            return -1.0;
        totalDistance += distance;
        if(distance < MinDistance) 
            return totalDistance;
    }
    return -1.0;
}

vec3 getCamRayDir(vec2 screenCoord){
    vec3 lookDir = normalize(cam.lookAt.xyz - cam.position.xyz);
    vec3 up = vec3(0.0,1.0,0.0);
    vec3 u = cross(up, lookDir);
    vec3 v = cross(lookDir, u);
    vec3 planePos = cam.position.xyz + lookDir * cam.zoom;
    vec3 pixelOnPlane = planePos + screenCoord.x * u + screenCoord.y * v;
    return normalize(pixelOnPlane - cam.position.xyz);
}

float attenuation(int i, float distToLight)
{
	return 1.0 / (0.01 + 0.05*distToLight*distToLight);
}

void phong(in int primIndex, in vec3 Q, in vec3 vHat, in vec3 nHat, out vec3 ambient, out vec3 diffuse, out vec3 specular)
{
	diffuse = vec3(0.0, 0.0, 0.0);
	specular = vec3 (0.0, 0.0, 0.0);
	vec3 liHat;

	// Does nHat need to be flipped?
	if (dot(nHat, vHat) < 0.0)
		nHat = -nHat;
   
	for (int lsi=0 ; lsi<actualNumLights ; lsi++)
	{
		if (lightEnabled[lsi])
		{
			vec3 liHat;
			if (lightPos[lsi].w > 0.0) // positional source in scene
			{
				liHat = normalize(lightPos[lsi].xyz - Q);
			}
			else // directional light
				liHat = normalize(lightPos[lsi].xyz);
			float nHatDotLiHat = dot(nHat, liHat);
			if (nHatDotLiHat > 0.0) // Light source on correct side of surface, so:
			{
                float distToLight = length(lightPos[lsi].xyz - Q);
				vec3 liStrength = lightStrength[lsi] * shadow(Q, liHat, distToLight, 32);
				if (lightPos[lsi].w > 0.0)
					liStrength *= attenuation(lsi, distToLight);
				diffuse  += liStrength * nHatDotLiHat;
				vec3 riHat = 2.0*nHatDotLiHat*nHat - liHat;
				float riHatDotVHat = dot(riHat,vHat);
				if (riHatDotVHat > 0.0) // Viewer on correct side of normal vector, so:
					specular += liStrength * pow(riHatDotVHat, primitives[primIndex].m);
			}
		}
	}

	ambient = primitives[primIndex].ka.rgb * globalAmbient;
	diffuse *= primitives[primIndex].kd.rgb;
	specular *= primitives[primIndex].ks.rgb;
}

vec4 reflectiveColor(inout vec3 Q, inout vec3 normal, inout vec3 rayDir){
    int refPrimHit;
    vec4 reflectColor;
    rayDir = reflect(rayDir, normal);
    float distToHit = trace(Q , rayDir, refPrimHit);
    if(distToHit != -1.0){
        Q += distToHit * rayDir;
        normal = calculateNormal(Q, primitives[refPrimHit]);
        vec3 ambient, diffuse, specular;
        phong(refPrimHit, Q, -rayDir, normal, ambient, diffuse, specular);
        reflectColor = vec4(ambient + diffuse + specular, 1.0);
    }
    else{
        reflectColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    return reflectColor;
}


void main(){
    vec2 screenSize = vec2(width, height);
    vec3 rayDir = getCamRayDir((gl_GlobalInvocationID.xy - screenSize/2.0) * iRes);
    int primHit;
    float distToHit = trace(cam.position.xyz, rayDir, primHit);
    
    vec4 pixelColor;
    if(distToHit == -1.0){
        pixelColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else{
        vec3 Q = cam.position.xyz + distToHit * rayDir;
        vec3 normal = calculateNormal(Q, primitives[primHit]);
        vec3 ambient, diffuse, specular;
		phong(primHit, Q, -rayDir, normal, ambient, diffuse, specular);
        pixelColor = vec4(ambient + diffuse + specular, 1.0);
        #if (REFLECT)
        
        if(primitives[primHit].ref > 0.0)
            pixelColor += reflectiveColor(Q, normal, rayDir) * primitives[primHit].ref;
        #endif
    }
    imageStore(img, ivec2(gl_GlobalInvocationID.xy), pixelColor);
}
