TARGET=particlefilterdemo

include ../../src/Makefile.base

OBJFILES = main.o particle_filter_2d.o particle_filter_3d.o
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

particle_filter_2d.o: src/ParticleFilter2D.h src/ParticleFilter2D.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS) -c src/ParticleFilter2D.cpp -o particle_filter_2d.o
	
particle_filter_3d.o: src/ParticleFilter3D.h src/ParticleFilter3D.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS) -c src/ParticleFilter3D.cpp -o particle_filter_3d.o
	
main.o: src/main.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS) -c src/main.cpp -o main.o
