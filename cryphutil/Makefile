CPP = cl.exe
C_FLAGS = -c /EHsc

LINK = lib.exe

OBJS = AffPoint.obj AffVector.obj Matrix3x3.obj Matrix4x4.obj ProjPoint.obj

libcryph.lib: $(OBJS)
	$(LINK) /OUT:libcryph.lib $(OBJS)
	copy "libcryph.lib" "../lib/"

AffPoint.obj: AffPoint.h AffPoint.cpp
	$(CPP) $(C_FLAGS) AffPoint.cpp

AffVector.obj: AffVector.h AffVector.cpp
	$(CPP) $(C_FLAGS) AffVector.cpp

Matrix3x3.obj: Matrix3x3.h Matrix3x3.cpp
	$(CPP) $(C_FLAGS) Matrix3x3.cpp

Matrix4x4.obj: Matrix4x4.h Matrix4x4.cpp
	$(CPP) $(C_FLAGS) Matrix4x4.cpp

ProjPoint.obj: ProjPoint.h ProjPoint.cpp
	$(CPP) $(C_FLAGS) ProjPoint.cpp
