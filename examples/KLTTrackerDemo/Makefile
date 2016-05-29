TARGET=klttrackerdemo

include ../../src/Makefile.base

ifeq ($(shell uname), Darwin)
	ifeq ($(USE_QUICKTIME), 1)
		TARGET_ = $(TARGET)
	else
		TARGET_ = errormessage
	endif
else
	ifeq ($(USE_LIBDC1394_V2), 1)
		TARGET_ = $(TARGET)
	else
		TARGET_ = errormessage
	endif
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

errormessage:
	@echo "For this application to compile and run a suitable video capture module must be activated. Per default, under Mac OS X the module CQuicktimeCapture is used: set USE_QUICKTIME = 1 in IVT/src/Makefile.base. Per default, under Linux the module CLinux1394Capture2 is used: set USE_LINUX1394_V2 = 1 in IVT/src/Makefile.base. After setting the appropriate variable, run 'make clean && make' in IVT/src first and then run 'make' here again."