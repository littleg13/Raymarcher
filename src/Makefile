CPP = cl.exe /Zi /EHsc
INC =  -I/usr/local/include -I. -I../include

# >>> FOR LINUX, uncomment next few lines; comment out the MAC ones.
C_FLAGS = -c /EHsc /Zi $(INC) -DGL_GLEXT_PROTOTYPES
GL_LIB_LOC = -L/usr/lib/nvidia-375
GL_LIBRARIES = kernel32.lib user32.lib shell32.lib legacy_stdio_definitions.lib ../lib/glfw3dll.lib ../lib/glew32.lib opengl32.lib 
MAKE = nmake
# >>> FOR MAC, uncomment next few lines; comment out previous linux ones.
# C_FLAGS = -fPIC -g -c $(INC) -DGLFW_INCLUDE_GLEXT -DGLFW_INCLUDE_GLCOREARB -DGL_SILENCE_DEPRECATION
# GL_LIBRARIES = -L/usr/local/lib -lglfw -framework OpenGL
# MAKE = make -f MakefileMac
# >>> END: FOR LINUX - FOR MAC
LOCAL_UTIL_LIBRARIES = 

OBJS = main.obj controller.obj

main: $(OBJS) $(LOCAL_UTIL_LIBRARIES)
	$(CPP) /Fe:"../bin/Raymarcher.exe" $(OBJS) $(LOCAL_UTIL_LIBRARIES) $(GL_LIBRARIES)

main.obj: main.cpp
	$(CPP) $(C_FLAGS) main.cpp
controller.obj: Controller.cpp
	$(CPP) $(C_FLAGS) Controller.cpp