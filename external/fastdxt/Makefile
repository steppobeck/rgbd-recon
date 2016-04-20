
CXXFLAGS=-O3 -funroll-loops -fomit-frame-pointer -fPIC -msse2 -DDXT_INTR
SRC = 2dxt.cpp  dxt.cpp  intrinsic.cpp  libdxt.cpp  util.cpp

OBJ = $(SRC:%.cpp=%.o)

LIB = libfastdxt.so

all : $(LIB)


$(LIB) : $(OBJ)
	$(CXX) -shared -o$@ $(OBJ)

%.o : %.cpp
	$(CXX) $(CPPFLAGS) -fPIC -I. $(CXXFLAGS) -o$@ -c $<

clean :
	$(RM) $(OBJ) $(LIB)



