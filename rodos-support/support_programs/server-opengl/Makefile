CC=g++
CFLAGS=-c -Wall
LDFLAGS= -lm -lGL -lGLU -lglut
SOURCES= Defos-server.cpp $(wildcard Visualisation/*.cpp) $(wildcard Importer/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=defos

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean: 
	rm -f $(EXECUTABLE) $(OBJECTS)