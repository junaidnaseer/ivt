TARGET=simpleappcv

include ../../src/Makefile.base

ifeq ($(USE_OPENCV), 1)
	ifeq ($(USE_OPENCV_HIGHGUI), 1)
		TARGET_ = $(TARGET)
	else
		TARGET_ = errormessage_opencv
	endif
else
	TARGET_ = errormessage_opencv
endif


OBJFILES = main.o
INCPATHS = $(INCPATHS_BASE)
LIBPATHS = $(LIBPATHS_BASE)
LIBS = $(LIBS_BASE)
FLAGS = $(FLAGS_BASE)
LDFLAGS = $(LDFLAGS_BASE)


all: $(TARGET_)

clean:
	rm -f $(OBJFILES)
	rm -f $(TARGET)

$(TARGET): $(OBJFILES)
	$(COMPILER) $(FLAGS) $(OBJFILES) $(LIBPATHS) $(LIBS) $(LDFLAGS) -o $(TARGET)

main.o: main.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS) -c main.cpp -o main.o

errormessage_opencv:
	@echo "USE_OPENCV = 1 and USE_OPENCV_HIGHGUI must be set in IVT/src/Makefile.base for this application to compile and run. After setting USE_OPENCV = 1 and USE_OPENCV_HIGHGUI, first run 'make clean && make' in IVT/src and then run 'make' here again."
