SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=objs/%.o) 
DEPS = $(SOURCES:%.cpp=objs/%.d)
EXE = tracer

DEFINES = -DGLEW_STATIC
INCLUDES = -I$(shell pwd)
LIBS = -lopengl32 -lglu32 -lgdi32 -lfreeglut

CFLAGS += -MMD $(INCLUDES) $(DEFINES)
CXXFLAGS += -MMD -std=gnu++11 -fopenmp -O3 $(INCLUDES) $(DEFINES)

all: code

.PHONY: clean

code: $(OBJECTS) objs/glew.o
	g++ -o $(EXE) $(CXXFLAGS) $(OBJECTS) $(LIBS)

objs/%.o: %.cpp
	mkdir -p $(dir $@)
	g++ -c $(CXXFLAGS) $< -o $@

objs/%.o: %.c
	mkdir -p objs
	gcc -c $(CFLAGS) $< -o $@

clean:
	$(RM) -rf objs $(EXE)
	
run: code
	$(EXE)
	
	
-include $(DEPS)