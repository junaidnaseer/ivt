TARGET=binarizedemo

include ../../src/Makefile.base

OBJFILES = main.o
INCPATHS = $(INCPATHS_BASE)
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

main.o: main.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS) -c main.cpp -o main.o
