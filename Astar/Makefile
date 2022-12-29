PROJECT = AStar
CFLAGS = -m64 -fpic -shared
OBJECTS = AStar.o

all: $(PROJECT)

.cpp.o:
	g++ -c $(CFLAGS) $<

$(PROJECT): $(OBJECTS)
	g++ $(CFLAGS) -o $(PROJECT) $(OBJECTS)
	
Main.o: Main.cpp
	g++ $(CFLAGS) -c Main.cpp
	
clean:
	rm $(OBJECTS) $(PROJECT)
