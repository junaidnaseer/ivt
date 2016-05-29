TARGET=hsvcolorsegmentationapp

include ../../src/Makefile.base

OBJFILES = main.o object_finder_organizer.o
INCPATHS = $(INCPATHS_BASE) -Isrc
LIBPATHS = $(LIBPATHS_BASE)
LIBS = $(LIBS_BASE)
FLAGS = $(FLAGS_BASE)
LDFLAGS = $(LDFLAGS_BASE)


all: $(TARGET)

clean:
	rm -f $(OBJFILES)
	rm -f $(TARGET)

$(TARGET): $(OBJFILES)
	$(COMPILER) $(FLAGS) $(OBJFILES) $(LIBPATHS) $(LIBS) $(LDFLAGS) -o $(TARGET)

object_finder_organizer.o: src/ObjectFinderOrganizer.h src/ObjectFinderOrganizer.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS) $(EXTRA_FLAGS) -c src/ObjectFinderOrganizer.cpp -o object_finder_organizer.o

main.o: src/main.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS) -c src/main.cpp -o main.o
