TARGET=$(TARGET_IVT) $(TARGET_IVT_GUI) $(TARGET_IVT_VIDEO_CAPTURE) $(TARGET_IVT_OPENCV)
FLAGS=$(FLAGS_BASE)

include Makefile.base

OBJFILES_COMMON=build/math_2d.o build/math_3d.o build/matd.o build/vecd.o build/byte_image.o build/short_image.o build/int_image.o build/float_image.o build/float_matrix.o build/float_vector.o build/double_matrix.o build/double_vector.o build/image_processor.o build/stereo_vision.o build/rgb_color_model.o build/color_parameter_set.o build/color.o build/helpers.o build/timer.o build/quicksort.o build/basicfileio.o build/configuration.o build/dlt_calibration.o build/calibration.o build/stereo_calibration.o build/video_reader.o build/uncompressed_avi_capture.o build/bitmap_capture.o build/bitmap_sequence_capture.o build/posix_thread.o build/particle_filter_framework.o build/particle_filter_framework_float.o build/linear_algebra.o build/svd.o build/normalizer.o build/primitives_drawer.o build/bitmap_font.o build/event.o build/mutex.o build/threading.o build/mean_filter.o build/ransac.o build/stereo_matcher.o build/dynamic_array.o build/kdtree.o build/icp.o build/object_finder.o build/object_finder_stereo.o build/object_color_segmenter.o build/compact_region_filter.o build/patch_feature_entry.o build/sift_feature_calculator.o build/harris_sift_feature_calculator.o build/object_pose.o build/posit.o build/rapid.o build/tracker_2d3d.o build/rectification.o build/undistortion.o build/undistortion_simple.o build/image_mapper.o build/performance_lib.o build/nearest_neighbor.o build/klt_tracker.o build/extrinsic_parameter_calculator.o build/corner_subpixel.o build/feature_set.o build/contour_helper.o
INCPATHS_COMMON = -I.

ifeq ($(LOAD_KPP), 1)
	FLAGS_PERFORMANCE_LIB += -DLOAD_KPP
endif

ifeq ($(USE_NETWORKING), 1)
	OBJFILES_COMMON += build/tcp_socket.o
endif

ifeq ($(USE_OPENGL), 1)
	OBJFILES_COMMON += build/opengl_visualizer.o build/opengl_visualizer_control.o build/opengl_capture.o
endif

ifeq ($(USE_SVS), 1)
	OBJFILES_COMMON += build/stereo_vision_svs.o
endif



OBJFILES_COMMON_CV =
INCPATHS_COMMON_CV = $(INCPATHS_BASE) -I.

ifeq ($(USE_OPENCV), 1)
	OBJFILES_COMMON_CV += build/image_processor_cv.o build/primitives_drawer_cv.o build/ipl_image_adaptor_cv.o build/stereo_calibration_cv.o build/extrinsic_parameter_calculator_cv.o
endif

ifeq ($(USE_OPENCV_HIGHGUI), 1)
	OBJFILES_COMMON_CV += build/haar_classifier_cv.o build/image_access_cv.o build/video_writer_cv.o
endif



OBJFILES_GUI =
INCPATHS_GUI = $(INCPATHS_BASE) -I.

ifeq ($(USE_QT), 1)
	OBJFILES_GUI += build/qt_window.o build/qt_painter.o build/qt_application_handler.o build/moc_qt_application_handler.o build/qt_color_parameter_window.o build/moc_qt_color_parameter_window.o

	ifeq ($(USE_QTGUI), 1)
		OBJFILES_GUI += build/gui_factory.o build/qtgui_application_handler.o build/moc_qtgui_application_handler.o build/qtgui_main_window.o build/qtgui_main_window_widget.o build/moc_qtgui_main_window_widget.o
		ifeq ($(USE_OPENGL), 1)
			OBJFILES_GUI += build/gl_context.o
		endif
	endif # USE_QTGUI

	ifeq ($(USE_OPENGL), 1)
		OBJFILES_GUI += build/qt_gl_window.o
	endif
endif # USE_QT

ifeq ($(USE_GTKGUI), 1)
	OBJFILES_GUI += build/gui_factory.o build/gtk_application_handler.o build/gtk_main_window.o build/gtk_main_window_widget.o
	ifeq ($(USE_OPENGL), 1)
		OBJFILES_GUI += build/gl_context.o build/gtk_gl_context.o
	endif
endif # USE_GTKGUI

ifeq ($(USE_COCOAGUI), 1)
	OBJFILES_GUI += build/gui_factory.o build/gui_factory_cocoa_implementation.o build/cocoagui_application_handler.o build/cocoagui_main_window.o build/cocoagui_implementation.o
	ifeq ($(USE_OPENGL), 1)
		OBJFILES_GUI += build/gl_context.o build/gl_context_cocoa_implementation.o
	endif
endif # USE_COCOAGUI

ifeq ($(USE_REMOTEGUI), 1)
	OBJFILES_GUI += build/gui_factory.o build/remotegui_application_handler.o build/remotegui_main_window.o
endif # USE_REMOTEGUI

ifeq ($(USE_OPENCV_HIGHGUI), 1)
	OBJFILES_GUI +=  build/opencv_application_handler.o build/opencv_window.o
endif

ifeq ($(USE_OPEN_INVENTOR), 1)
	OBJFILES_GUI += build/open_inventor_visualizer.o
endif



OBJFILES_VIDEO_CAPTURE =
INCPATHS_VIDEO_CAPTURE = $(INCPATHS_BASE) -I.

ifeq ($(USE_LIBDC1394), 1)
	OBJFILES_VIDEO_CAPTURE += build/linux_1394_capture.o
endif
ifeq ($(USE_LIBDC1394_V2), 1)
	OBJFILES_VIDEO_CAPTURE += build/linux_1394_capture_2.o
endif
ifeq ($(USE_QUICKTIME), 1)
	OBJFILES_VIDEO_CAPTURE += build/quicktime_capture.o
endif
ifeq ($(USE_SVS), 1)
	OBJFILES_VIDEO_CAPTURE += build/svs_capture.o
endif
ifeq ($(USE_UNICAP), 1)
	OBJFILES_VIDEO_CAPTURE += build/unicap_1394_capture.o
endif
ifeq ($(USE_OPENCV_HIGHGUI), 1)
	OBJFILES_VIDEO_CAPTURE += build/opencv_capture.o
endif
ifeq ($(USE_V4L), 1)
	OBJFILES_VIDEO_CAPTURE += build/v4l_capture.o
endif

ifeq ($(shell uname), Darwin)
	TARGET_IVT=../lib/macos/libivt.a

	# GUI
	ifeq ($(USE_QT), 1)
		TARGET_IVT_GUI=../lib/macos/libivtgui.a
	endif

	ifeq ($(USE_COCOAGUI), 1)
		TARGET_IVT_GUI=../lib/macos/libivtgui.a
	endif

	ifeq ($(USE_REMOTEGUI), 1)
		TARGET_IVT_GUI=../lib/macos/libivtgui.a
	endif

	# Video capture
	ifeq ($(USE_QUICKTIME), 1)
		TARGET_IVT_VIDEO_CAPTURE=../lib/macos/libivtvideocapture.a
	endif

	ifeq ($(USE_OPENCV_HIGHGUI), 1)
		TARGET_IVT_VIDEO_CAPTURE=../lib/macos/libivtvideocapture.a
	endif

	# OpenCV support
	ifeq ($(USE_OPENCV), 1)
		TARGET_IVT_OPENCV=../lib/macos/libivtopencv.a
	endif

else
	TARGET_IVT=../lib/linux/libivt.a

	# GUI
	ifeq ($(USE_QT), 1)
		TARGET_IVT_GUI=../lib/linux/libivtgui.a
	endif

	ifeq ($(USE_OPENCV_HIGHGUI), 1)
		TARGET_IVT_GUI=../lib/linux/libivtgui.a
	endif

	ifeq ($(USE_REMOTEGUI), 1)
		TARGET_IVT_GUI=../lib/linux/libivtgui.a
	endif

	ifeq ($(USE_GTKGUI), 1)
		TARGET_IVT_GUI=../lib/linux/libivtgui.a
	endif

	# Video capture
	ifeq ($(USE_LIBDC1394), 1)
		TARGET_IVT_VIDEO_CAPTURE=../lib/linux/libivtvideocapture.a
	endif

	ifeq ($(USE_LIBDC1394_V2), 1)
		TARGET_IVT_VIDEO_CAPTURE=../lib/linux/libivtvideocapture.a
	endif

	ifeq ($(USE_UNICAP), 1)
		TARGET_IVT_VIDEO_CAPTURE=../lib/linux/libivtvideocapture.a
	endif

	ifeq ($(USE_SVS), 1)
		TARGET_IVT_VIDEO_CAPTURE=../lib/linux/libivtvideocapture.a
	endif

	ifeq ($(USE_OPENCV_HIGHGUI), 1)
		TARGET_IVT_VIDEO_CAPTURE=../lib/linux/libivtvideocapture.a
	endif

	ifeq ($(USE_V4L), 1)
		TARGET_IVT_VIDEO_CAPTURE=../lib/linux/libivtvideocapture.a
	endif

	# OpenCV support
	ifeq ($(USE_OPENCV), 1)
		TARGET_IVT_OPENCV=../lib/linux/libivtopencv.a
	endif
endif



all: $(TARGET)

clean:
	rm -f $(OBJFILES_COMMON) $(OBJFILES_GUI) $(OBJFILES_COMMON_CV) $(OBJFILES_OPEN_INVENTOR) $(OBJFILES_VIDEO_CAPTURE) $(TARGET_IVT) $(TARGET_IVT_OPENCV) $(TARGET_IVT_GUI) $(TARGET_IVT_VIDEO_CAPTURE) gui/moc_* gui/Qt/moc_*

$(TARGET_IVT): $(OBJFILES_COMMON)
	ar -rc $(TARGET_IVT) $(OBJFILES_COMMON)
	ranlib $(TARGET_IVT)

$(TARGET_IVT_OPENCV): $(OBJFILES_COMMON_CV)
	ar -rc $(TARGET_IVT_OPENCV) $(OBJFILES_COMMON_CV)
	ranlib $(TARGET_IVT_OPENCV)

$(TARGET_IVT_GUI): $(OBJFILES_GUI)
	ar -rc $(TARGET_IVT_GUI) $(OBJFILES_GUI)
	ranlib $(TARGET_IVT_GUI)

$(TARGET_IVT_VIDEO_CAPTURE): $(OBJFILES_VIDEO_CAPTURE)
	ar -rc $(TARGET_IVT_VIDEO_CAPTURE) $(OBJFILES_VIDEO_CAPTURE)
	ranlib $(TARGET_IVT_VIDEO_CAPTURE)


build/dlt_calibration.o: Calibration/DLTCalibration.cpp Calibration/DLTCalibration.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Calibration/DLTCalibration.cpp -o build/dlt_calibration.o

build/calibration.o: Calibration/Calibration.cpp Calibration/Calibration.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Calibration/Calibration.cpp -o build/calibration.o

build/stereo_calibration.o: Calibration/StereoCalibration.cpp Calibration/StereoCalibration.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Calibration/StereoCalibration.cpp -o build/stereo_calibration.o

build/helpers.o: Helpers/helpers.cpp Helpers/helpers.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Helpers/helpers.cpp -o build/helpers.o

build/timer.o: Helpers/Timer.cpp Helpers/Timer.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Helpers/Timer.cpp -o build/timer.o

build/quicksort.o: Helpers/Quicksort.cpp Helpers/Quicksort.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Helpers/Quicksort.cpp -o build/quicksort.o

build/basicfileio.o: Helpers/BasicFileIO.cpp Helpers/BasicFileIO.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Helpers/BasicFileIO.cpp -o build/basicfileio.o

build/configuration.o: Helpers/Configuration.cpp Helpers/Configuration.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Helpers/Configuration.cpp -o build/configuration.o

build/tcp_socket.o: Networking/TCPSocket.cpp Networking/TCPSocket.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Networking/TCPSocket.cpp -o build/tcp_socket.o

build/performance_lib.o: Helpers/PerformanceLib.cpp Helpers/PerformanceLib.h Helpers/OptimizedFunctions.h Helpers/OptimizedFunctionsList.h
	$(COMPILER) $(FLAGS) $(FLAGS_PERFORMANCE_LIB) $(INCPATHS_COMMON) -c Helpers/PerformanceLib.cpp -o build/performance_lib.o

build/byte_image.o: Image/ByteImage.cpp Image/ByteImage.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Image/ByteImage.cpp -o build/byte_image.o

build/short_image.o: Image/ShortImage.cpp Image/ShortImage.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Image/ShortImage.cpp -o build/short_image.o

build/int_image.o: Image/IntImage.cpp Image/IntImage.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Image/IntImage.cpp -o build/int_image.o
	
build/float_image.o: Image/FloatImage.cpp Image/FloatImage.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Image/FloatImage.cpp -o build/float_image.o

build/float_matrix.o: Math/FloatMatrix.cpp Math/FloatMatrix.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Math/FloatMatrix.cpp -o build/float_matrix.o

build/float_vector.o: Math/FloatVector.cpp Math/FloatVector.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Math/FloatVector.cpp -o build/float_vector.o

build/double_matrix.o: Math/DoubleMatrix.cpp Math/DoubleMatrix.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Math/DoubleMatrix.cpp -o build/double_matrix.o

build/double_vector.o: Math/DoubleVector.cpp Math/DoubleVector.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Math/DoubleVector.cpp -o build/double_vector.o

build/image_processor.o: Image/ImageProcessor.cpp Image/ImageProcessor.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Image/ImageProcessor.cpp -o build/image_processor.o

build/primitives_drawer.o: Image/PrimitivesDrawer.cpp Image/PrimitivesDrawer.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Image/PrimitivesDrawer.cpp -o build/primitives_drawer.o

build/bitmap_font.o: Image/BitmapFont.cpp Image/BitmapFont.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Image/BitmapFont.cpp -o build/bitmap_font.o

build/stereo_vision.o: Image/StereoVision.cpp Image/StereoVision.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Image/StereoVision.cpp -o build/stereo_vision.o

build/image_mapper.o: Image/ImageMapper.cpp Image/ImageMapper.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Image/ImageMapper.cpp -o build/image_mapper.o

build/rgb_color_model.o: Color/RGBColorModel.cpp Color/RGBColorModel.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Color/RGBColorModel.cpp -o build/rgb_color_model.o

build/color_parameter_set.o: Color/ColorParameterSet.cpp Color/ColorParameterSet.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Color/ColorParameterSet.cpp -o build/color_parameter_set.o

build/color.o: Color/Color.cpp Color/Color.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Color/Color.cpp -o build/color.o

build/math_2d.o: Math/Math2d.cpp Math/Math2d.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Math/Math2d.cpp -o build/math_2d.o

build/math_3d.o: Math/Math3d.cpp Math/Math3d.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Math/Math3d.cpp -o build/math_3d.o

build/matd.o: Math/Matd.cpp Math/Matd.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Math/Matd.cpp -o build/matd.o

build/vecd.o: Math/Vecd.cpp Math/Vecd.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Math/Vecd.cpp -o build/vecd.o

build/linear_algebra.o: Math/LinearAlgebra.cpp Math/LinearAlgebra.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Math/LinearAlgebra.cpp -o build/linear_algebra.o

build/svd.o: Math/SVD.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Math/SVD.cpp -o build/svd.o

build/video_reader.o: VideoAccess/VideoReader.cpp VideoAccess/VideoReader.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c VideoAccess/VideoReader.cpp -o build/video_reader.o

build/uncompressed_avi_capture.o: VideoCapture/UncompressedAVICapture.cpp VideoCapture/UncompressedAVICapture.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c VideoCapture/UncompressedAVICapture.cpp -o build/uncompressed_avi_capture.o

build/bitmap_capture.o: VideoCapture/BitmapCapture.cpp VideoCapture/BitmapCapture.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c VideoCapture/BitmapCapture.cpp -o build/bitmap_capture.o

build/bitmap_sequence_capture.o: VideoCapture/BitmapSequenceCapture.cpp VideoCapture/BitmapSequenceCapture.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c VideoCapture/BitmapSequenceCapture.cpp -o build/bitmap_sequence_capture.o

build/posix_thread.o: Threading/PosixThread.cpp Threading/PosixThread.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Threading/PosixThread.cpp -o build/posix_thread.o

build/event.o: Threading/Event.cpp Threading/Event.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Threading/Event.cpp -o build/event.o

build/mutex.o: Threading/Mutex.cpp Threading/Mutex.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Threading/Mutex.cpp -o build/mutex.o

build/threading.o: Threading/Threading.cpp Threading/Threading.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Threading/Threading.cpp -o build/threading.o

build/particle_filter_framework.o: ParticleFilter/ParticleFilterFramework.cpp ParticleFilter/ParticleFilterFramework.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c ParticleFilter/ParticleFilterFramework.cpp -o build/particle_filter_framework.o

build/particle_filter_framework_float.o: ParticleFilter/ParticleFilterFrameworkFloat.cpp ParticleFilter/ParticleFilterFrameworkFloat.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c ParticleFilter/ParticleFilterFrameworkFloat.cpp -o build/particle_filter_framework_float.o

build/normalizer.o: DataProcessing/Normalizer.cpp DataProcessing/Normalizer.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c DataProcessing/Normalizer.cpp -o build/normalizer.o

build/mean_filter.o: DataProcessing/MeanFilter.cpp DataProcessing/MeanFilter.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c DataProcessing/MeanFilter.cpp -o build/mean_filter.o

build/ransac.o: DataProcessing/RANSAC.cpp DataProcessing/RANSAC.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c DataProcessing/RANSAC.cpp -o build/ransac.o

build/opengl_visualizer.o: Visualizer/OpenGLVisualizer.h Visualizer/OpenGLVisualizer.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) $(INCPATHS_BASE) -c Visualizer/OpenGLVisualizer.cpp -o build/opengl_visualizer.o

build/opengl_visualizer_control.o: Visualizer/OpenGLVisualizerControl.h Visualizer/OpenGLVisualizerControl.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) $(INCPATHS_BASE) -c Visualizer/OpenGLVisualizerControl.cpp -o build/opengl_visualizer_control.o

build/opengl_capture.o: VideoCapture/OpenGLCapture.h VideoCapture/OpenGLCapture.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) $(INCPATHS_BASE) -c VideoCapture/OpenGLCapture.cpp -o build/opengl_capture.o

build/stereo_matcher.o: Image/StereoMatcher.h Image/StereoMatcher.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Image/StereoMatcher.cpp -o build/stereo_matcher.o

build/dynamic_array.o: DataStructures/DynamicArray.h DataStructures/DynamicArray.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c DataStructures/DynamicArray.cpp -o build/dynamic_array.o

build/kdtree.o: DataStructures/KdTree/KdTree.h DataStructures/KdTree/KdTree.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c DataStructures/KdTree/KdTree.cpp -o build/kdtree.o

build/icp.o: Tracking/ICP.cpp Tracking/ICP.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Tracking/ICP.cpp -o build/icp.o

build/klt_tracker.o: Tracking/KLTTracker.cpp Tracking/KLTTracker.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Tracking/KLTTracker.cpp -o build/klt_tracker.o

build/rectification.o: Calibration/Rectification.cpp Calibration/Rectification.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Calibration/Rectification.cpp -o build/rectification.o

build/undistortion.o: Calibration/Undistortion.cpp Calibration/Undistortion.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Calibration/Undistortion.cpp -o build/undistortion.o

build/undistortion_simple.o: Calibration/UndistortionSimple.cpp Calibration/UndistortionSimple.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Calibration/UndistortionSimple.cpp -o build/undistortion_simple.o

build/object_finder.o: ObjectFinder/ObjectFinder.h ObjectFinder/ObjectFinder.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c ObjectFinder/ObjectFinder.cpp -o build/object_finder.o

build/object_finder_stereo.o: ObjectFinder/ObjectFinderStereo.h ObjectFinder/ObjectFinderStereo.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c ObjectFinder/ObjectFinderStereo.cpp -o build/object_finder_stereo.o

build/object_color_segmenter.o: ObjectFinder/ObjectColorSegmenter.h ObjectFinder/ObjectColorSegmenter.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c ObjectFinder/ObjectColorSegmenter.cpp -o build/object_color_segmenter.o

build/compact_region_filter.o: ObjectFinder/CompactRegionFilter.h ObjectFinder/CompactRegionFilter.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c ObjectFinder/CompactRegionFilter.cpp -o build/compact_region_filter.o

build/object_pose.o: Tracking/ObjectPose.cpp Tracking/ObjectPose.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Tracking/ObjectPose.cpp -o build/object_pose.o

build/posit.o: Tracking/POSIT.cpp Tracking/POSIT.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Tracking/POSIT.cpp -o build/posit.o

build/rapid.o: Tracking/RAPiD.cpp Tracking/RAPiD.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Tracking/RAPiD.cpp -o build/rapid.o

build/tracker_2d3d.o: Tracking/Tracker2d3d.cpp Tracking/Tracker2d3d.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Tracking/Tracker2d3d.cpp -o build/tracker_2d3d.o

build/feature_set.o: Features/FeatureSet.h Features/FeatureSet.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Features/FeatureSet.cpp -o build/feature_set.o

build/patch_feature_entry.o: Features/PatchFeatures/PatchFeatureEntry.h Features/PatchFeatures/PatchFeatureEntry.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Features/PatchFeatures/PatchFeatureEntry.cpp -o build/patch_feature_entry.o

build/sift_feature_calculator.o: Features/SIFTFeatures/SIFTFeatureCalculator.h Features/SIFTFeatures/SIFTFeatureCalculator.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Features/SIFTFeatures/SIFTFeatureCalculator.cpp -o build/sift_feature_calculator.o

build/harris_sift_feature_calculator.o: Features/HarrisSIFTFeatures/HarrisSIFTFeatureCalculator.h Features/HarrisSIFTFeatures/HarrisSIFTFeatureCalculator.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Features/HarrisSIFTFeatures/HarrisSIFTFeatureCalculator.cpp -o build/harris_sift_feature_calculator.o

build/nearest_neighbor.o: Classification/NearestNeighbor.h Classification/NearestNeighbor.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Classification/NearestNeighbor.cpp -o build/nearest_neighbor.o

build/extrinsic_parameter_calculator.o: Calibration/ExtrinsicParameterCalculator.cpp Calibration/ExtrinsicParameterCalculator.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Calibration/ExtrinsicParameterCalculator.cpp -o build/extrinsic_parameter_calculator.o

build/corner_subpixel.o: Image/CornerSubpixel.cpp Image/CornerSubpixel.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Image/CornerSubpixel.cpp -o build/corner_subpixel.o

build/contour_helper.o: Contours/ContourHelper.cpp Contours/ContourHelper.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) -c Contours/ContourHelper.cpp -o build/contour_helper.o



build/image_processor_cv.o: Image/ImageProcessorCV.cpp Image/ImageProcessorCV.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON_CV) -c Image/ImageProcessorCV.cpp -o build/image_processor_cv.o

build/stereo_calibration_cv.o: Calibration/StereoCalibrationCV.cpp Calibration/StereoCalibrationCV.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON_CV) -c Calibration/StereoCalibrationCV.cpp -o build/stereo_calibration_cv.o

build/primitives_drawer_cv.o: Image/PrimitivesDrawerCV.cpp Image/PrimitivesDrawerCV.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON_CV) -c Image/PrimitivesDrawerCV.cpp -o build/primitives_drawer_cv.o

build/ipl_image_adaptor_cv.o: Image/IplImageAdaptor.cpp Image/IplImageAdaptor.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON_CV) -c Image/IplImageAdaptor.cpp -o build/ipl_image_adaptor_cv.o

build/haar_classifier_cv.o: ObjectFinder/HaarClassifierCV.cpp ObjectFinder/HaarClassifierCV.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON_CV) -c ObjectFinder/HaarClassifierCV.cpp -o build/haar_classifier_cv.o

build/image_access_cv.o: Image/ImageAccessCV.cpp Image/ImageAccessCV.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON_CV) -c Image/ImageAccessCV.cpp -o build/image_access_cv.o

build/video_writer_cv.o: VideoAccess/VideoWriterCV.cpp VideoAccess/VideoWriterCV.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON_CV) -c VideoAccess/VideoWriterCV.cpp -o build/video_writer_cv.o

build/extrinsic_parameter_calculator_cv.o: Calibration/ExtrinsicParameterCalculatorCV.cpp Calibration/ExtrinsicParameterCalculatorCV.h
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON_CV) -c Calibration/ExtrinsicParameterCalculatorCV.cpp -o build/extrinsic_parameter_calculator_cv.o



build/qt_gl_window.o: gui/QTGLWindow.cpp gui/QTGLWindow.h
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(INCPATHS_GUI) -c gui/QTGLWindow.cpp -o build/qt_gl_window.o

build/qt_color_parameter_window.o: gui/QTColorParameterWindow.cpp gui/QTColorParameterWindow.h
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(INCPATHS_GUI) -c gui/QTColorParameterWindow.cpp -o build/qt_color_parameter_window.o

build/moc_qt_color_parameter_window.o: gui/QTColorParameterWindow.h
	$(MOC_PATH) gui/QTColorParameterWindow.h -o gui/moc_QTColorParameterWindow.cpp
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(INCPATHS_GUI) -c gui/moc_QTColorParameterWindow.cpp -o build/moc_qt_color_parameter_window.o

build/qt_window.o: gui/QTWindow.cpp gui/QTWindow.h
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(INCPATHS_GUI) -c gui/QTWindow.cpp -o build/qt_window.o

build/qt_painter.o: gui/QTPainter.cpp gui/QTPainter.h
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(INCPATHS_GUI) -c gui/QTPainter.cpp -o build/qt_painter.o

build/qt_application_handler.o: gui/QTApplicationHandler.cpp gui/QTApplicationHandler.h
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(INCPATHS_GUI) -c gui/QTApplicationHandler.cpp -o build/qt_application_handler.o

build/moc_qt_application_handler.o: gui/QTApplicationHandler.h
	$(MOC_PATH) gui/QTApplicationHandler.h -o gui/moc_QTApplicationHandler.cpp
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(INCPATHS_GUI) -c gui/moc_QTApplicationHandler.cpp -o build/moc_qt_application_handler.o

build/opencv_window.o: gui/OpenCVWindow.cpp gui/OpenCVWindow.h
	$(COMPILER) $(FLAGS) $(OPENCV_FLAGS_BASE) $(INCPATHS_GUI) -c gui/OpenCVWindow.cpp -o build/opencv_window.o

build/opencv_application_handler.o: gui/OpenCVApplicationHandler.cpp gui/OpenCVApplicationHandler.h
	$(COMPILER) $(FLAGS) $(OPENCV_FLAGS_BASE) $(INCPATHS_GUI) -c gui/OpenCVApplicationHandler.cpp -o build/opencv_application_handler.o

build/open_inventor_visualizer.o: Visualizer/OpenInventorVisualizer.h Visualizer/OpenInventorVisualizer.cpp
	$(COMPILER) $(FLAGS) $(OPENCV_FLAGS_BASE) $(INCPATHS_GUI) -c Visualizer/OpenInventorVisualizer.cpp -o build/open_inventor_visualizer.o


build/qtgui_application_handler.o: gui/Qt/QtApplicationHandler.cpp gui/Qt/QtApplicationHandler.h
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/Qt/QtApplicationHandler.cpp -o build/qtgui_application_handler.o

build/moc_qtgui_application_handler.o: gui/Qt/QtApplicationHandler.h
	$(MOC_PATH) gui/Qt/QtApplicationHandler.h -o gui/Qt/moc_QtApplicationHandler.cpp
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/Qt/moc_QtApplicationHandler.cpp -o build/moc_qtgui_application_handler.o

build/qtgui_main_window.o: gui/Qt/QtMainWindow.cpp gui/Qt/QtMainWindow.h
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/Qt/QtMainWindow.cpp -o build/qtgui_main_window.o

build/qtgui_main_window_widget.o: gui/Qt/QtMainWindowWidget.cpp gui/Qt/QtMainWindowWidget.h
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/Qt/QtMainWindowWidget.cpp -o build/qtgui_main_window_widget.o

build/moc_qtgui_main_window_widget.o: gui/Qt/QtMainWindowWidget.h
	$(MOC_PATH) gui/Qt/QtMainWindowWidget.h -o gui/Qt/moc_QtMainWindowWidget.cpp
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/Qt/moc_QtMainWindowWidget.cpp -o build/moc_qtgui_main_window_widget.o



build/gtk_application_handler.o: gui/GTK/GTKApplicationHandler.cpp gui/GTK/GTKApplicationHandler.h
	$(COMPILER) $(FLAGS) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/GTK/GTKApplicationHandler.cpp -o build/gtk_application_handler.o

build/gtk_main_window.o: gui/GTK/GTKMainWindow.cpp gui/GTK/GTKMainWindow.h
	$(COMPILER) $(FLAGS) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/GTK/GTKMainWindow.cpp -o build/gtk_main_window.o

build/gtk_main_window_widget.o: gui/GTK/GTKMainWindowWidget.cpp gui/GTK/GTKMainWindowWidget.h
	$(COMPILER) $(FLAGS) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/GTK/GTKMainWindowWidget.cpp -o build/gtk_main_window_widget.o

build/gtk_gl_context.o: gui/GTK/GTKGLContext.cpp gui/GTK/GTKGLContext.h
	$(COMPILER) $(FLAGS) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/GTK/GTKGLContext.cpp -o build/gtk_gl_context.o



build/gui_factory.o: gui/GUIFactory.cpp gui/GUIFactory.h gui/Cocoa/CocoaApplicationHandler.h gui/Cocoa/CocoaMainWindow.h
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/GUIFactory.cpp -o build/gui_factory.o

build/gui_factory_cocoa_implementation.o: gui/Cocoa/GUIFactory.m
	$(COMPILER) $(FLAGS) -c gui/Cocoa/GUIFactory.m -o build/gui_factory_cocoa_implementation.o

build/gl_context.o: gui/GLContext.cpp gui/GLContext.h
	$(COMPILER) $(FLAGS) $(QT_FLAGS_BASE) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/GLContext.cpp -o build/gl_context.o

build/gl_context_cocoa_implementation.o: gui/Cocoa/GLContext.m
	$(COMPILER) $(FLAGS) -c gui/Cocoa/GLContext.m -o build/gl_context_cocoa_implementation.o

build/cocoagui_application_handler.o: gui/Cocoa/CocoaApplicationHandler.cpp gui/Cocoa/CocoaApplicationHandler.h
	$(COMPILER) $(FLAGS) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/Cocoa/CocoaApplicationHandler.cpp -o build/cocoagui_application_handler.o

build/cocoagui_main_window.o: gui/Cocoa/CocoaMainWindow.cpp gui/Cocoa/CocoaMainWindow.h
	$(COMPILER) $(FLAGS) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/Cocoa/CocoaMainWindow.cpp -o build/cocoagui_main_window.o

build/cocoagui_implementation.o: gui/Cocoa/CocoaImplementation.m gui/Cocoa/CocoaImplementation.h
	$(COMPILER) $(FLAGS) -c gui/Cocoa/CocoaImplementation.m -o build/cocoagui_implementation.o


build/remotegui_application_handler.o: gui/Remote/RemoteApplicationHandler.cpp gui/Remote/RemoteApplicationHandler.h
	$(COMPILER) $(FLAGS) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/Remote/RemoteApplicationHandler.cpp -o build/remotegui_application_handler.o

build/remotegui_main_window.o: gui/Remote/RemoteMainWindow.cpp gui/Remote/RemoteMainWindow.h
	$(COMPILER) $(FLAGS) $(GUI_FLAGS_BASE) $(INCPATHS_GUI) -c gui/Remote/RemoteMainWindow.cpp -o build/remotegui_main_window.o


build/linux_1394_capture.o: VideoCapture/Linux1394Capture.h VideoCapture/Linux1394Capture.cpp
	$(COMPILER) $(FLAGS) $(LIBDC_FLAGS_BASE) $(INCPATHS_VIDEO_CAPTURE) -c VideoCapture/Linux1394Capture.cpp -o build/linux_1394_capture.o

build/linux_1394_capture_2.o: VideoCapture/Linux1394Capture2.h VideoCapture/Linux1394Capture2.cpp
	$(COMPILER) $(FLAGS) $(LIBDC_FLAGS_BASE) $(INCPATHS_VIDEO_CAPTURE) -c VideoCapture/Linux1394Capture2.cpp -o build/linux_1394_capture_2.o

build/quicktime_capture.o: VideoCapture/QuicktimeCapture.h VideoCapture/QuicktimeCapture.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_VIDEO_CAPTURE) -c VideoCapture/QuicktimeCapture.cpp -o build/quicktime_capture.o

build/svs_capture.o: VideoCapture/SVSCapture.h VideoCapture/SVSCapture.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_VIDEO_CAPTURE) -c VideoCapture/SVSCapture.cpp -o build/svs_capture.o

build/unicap_1394_capture.o: VideoCapture/Unicap1394Capture.h VideoCapture/Unicap1394Capture.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_VIDEO_CAPTURE) -c VideoCapture/Unicap1394Capture.cpp -o build/unicap_1394_capture.o

build/opencv_capture.o: VideoCapture/OpenCVCapture.h VideoCapture/OpenCVCapture.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_VIDEO_CAPTURE) $(INCPATHS_COMMON_CV) -c VideoCapture/OpenCVCapture.cpp -o build/opencv_capture.o

build/v4l_capture.o: VideoCapture/V4LCapture.h VideoCapture/V4LCapture.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_VIDEO_CAPTURE) -c VideoCapture/V4LCapture.cpp -o build/v4l_capture.o



build/stereo_vision_svs.o: Image/StereoVisionSVS.h Image/StereoVisionSVS.cpp
	$(COMPILER) $(FLAGS) $(INCPATHS_COMMON) $(INCPATHS_BASE) -c Image/StereoVisionSVS.cpp -o build/stereo_vision_svs.o
