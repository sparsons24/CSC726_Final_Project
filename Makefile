#Target to make and objects needed
TARGET = LCS
OBJS = $(TARGET).o

#Compiler, remove command, and OS we're working on
CC = g++-9
RM = rm -f
UNAME := $(shell uname)

#Check if we're on a Mac or a Linux machine

#Linux
ifeq ($(UNAME), Linux)
	OS_LFLAGS :=
	OS_INCLUDE :=
	OS_LDIRS := -L/opt/AMDAPP/lib/x86_64/
	OS_EXTRA_LIB := -L/usr/lib 
	OS_GLFW := glfw
	OS_GL := -lGL
	OS_EXTRA_LINK := -Wl,-rpath=/usr/local/lib/
endif

#Mac
ifeq ($(UNAME), Darwin)
	OS_LFLAGS := -framework Cocoa -framework OpenGl -framework IOKit -framework Corevideo
	OS_INCLUDE := 
	OS_LDIRS := 
	OS_EXTRA_LIB := 
	OS_GLFW := glfw.3
	OS_GL :=
	OS_EXTRA_LINK :=
endif

#Compiler flags
CXXFLAGS=-c -O3 -g3 \
    -Wall -Wextra -pedantic-errors \
    -D__GXX_EXPERIMENTAL_CXX0X__ \
    -I/usr/local/include/TSGL/ \
    -I/usr/local/include/ \
	-I/usr/local/include/freetype2 \
	-I/usr/local/include/freetype2/freetype \
	-I/usr/include/TSGL \
    -I/opt/AMDAPP/include/ \
    ${OS_INCLUDE} \
	-I/usr/include/freetype2/freetype  \
	-I/usr/include/freetype2/ \
    -std=c++0x -fopenmp \
    -Wno-unused-function #Supress warnings

#Linking flags
LFLAGS=-o $(TARGET) \
    -L/usr/local/lib/ \
	${OS_EXTRA_LIB} \
    -L/opt/X11/lib \
    ${OS_LDIRS} \
    ${OS_EXTRA_LINK} \
    -ltsgl -lfreetype \
    -lGLEW -l${OS_GLFW} \
    -lX11 ${OS_GL} -lXrandr \
    -fopenmp \
    ${OS_LFLAGS} \

.SUFFIXES: .cpp .o

#all command
all: $(TARGET)

#Linking
$(TARGET): $(OBJS)
	@echo "\nLinking $(TARGET)..."
	$(CC) $(OBJS) $(LFLAGS)
	@echo
	
#Compiling
.cpp.o:
	@echo "\nCompiling $<..."
	$(CC) $(CXXFLAGS) $< 

#Indirect dependencies (Multiple file/classes only)
$(TARGET).o:

#Clean command
clean:
	$(RM) $(TARGET) $(OBJS) *~
