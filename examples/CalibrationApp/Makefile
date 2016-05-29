TARGET=calibrationapp

include ../../src/Makefile.base


ifeq ($(USE_OPENCV), 1)
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
else
	TARGET_ = errormessage_opencv
endif


OBJFILES = main.o organizer.o open_cv_calibration_algorithm.o
INCPATHS = $(INCPATHS_BASE) -Isrc
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

main.o: src/main.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS) -c src/main.cpp -o main.o

organizer.o: src/Organizer.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS) -c src/Organizer.cpp -o organizer.o

open_cv_calibration_algorithm.o: src/OpenCVCalibrationAlgorithm.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS) -c src/OpenCVCalibrationAlgorithm.cpp -o open_cv_calibration_algorithm.o

errormessage_opencv:
	@echo "USE_OPENCV = 1 must be set in IVT/src/Makefile.base for this application to compile and run. After setting USE_OPENCV = 1, first run 'make clean && make' in IVT/src and then run 'make' here again."

errormessage:
	@echo "For this application to compile and run a suitable video capture module must be activated. Per default, under Mac OS X the module CQuicktimeCapture is used: set USE_QUICKTIME = 1 in IVT/src/Makefile.base. Per default, under Linux the module CLinux1394Capture2 is used: set USE_LINUX1394_V2 = 1 in IVT/src/Makefile.base. After setting the appropriate variable, run 'make clean && make' in IVT/src first and then run 'make' here again."
