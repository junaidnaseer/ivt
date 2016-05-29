# Microsoft Developer Studio Project File - Name="IVTLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=IVTLib - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "IVTLib.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "IVTLib.mak" CFG="IVTLib - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "IVTLib - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "IVTLib - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IVTLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../src" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0400 /D "_CRT_SECURE_NO_DEPRECATE" /FR /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\win32\ivt.lib"

!ELSEIF  "$(CFG)" == "IVTLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../src" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0400 /D "_CRT_SECURE_NO_DEPRECATE" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\win32\ivtd.lib"

!ENDIF 

# Begin Target

# Name "IVTLib - Win32 Release"
# Name "IVTLib - Win32 Debug"
# Begin Group "Calibration"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Calibration\Calibration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\Calibration.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\DLTCalibration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\DLTCalibration.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\ExtrinsicParameterCalculator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\ExtrinsicParameterCalculator.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\Rectification.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\Rectification.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\StereoCalibration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\StereoCalibration.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\Undistortion.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\Undistortion.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\UndistortionSimple.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\UndistortionSimple.h
# End Source File
# End Group
# Begin Group "Image"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Image\BitmapFont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\BitmapFont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\ByteImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\ByteImage.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\CornerSubpixel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\CornerSubpixel.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\ImageMapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\ImageMapper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\ImageProcessor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\ImageProcessor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\IntImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\IntImage.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\PrimitivesDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\PrimitivesDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\ShortImage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\ShortImage.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\StereoMatcher.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\StereoMatcher.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\StereoVision.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\StereoVision.h
# End Source File
# End Group
# Begin Group "ObjectFinder"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\ObjectFinder\CompactRegionFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ObjectFinder\CompactRegionFilter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ObjectFinder\ObjectColorSegmenter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ObjectFinder\ObjectColorSegmenter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ObjectFinder\ObjectFinder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ObjectFinder\ObjectFinder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ObjectFinder\ObjectFinderStereo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ObjectFinder\ObjectFinderStereo.h
# End Source File
# End Group
# Begin Group "VideoCapture"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\VideoCapture\BitmapCapture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\VideoCapture\BitmapCapture.h
# End Source File
# Begin Source File

SOURCE=..\..\src\VideoCapture\BitmapSequenceCapture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\VideoCapture\BitmapSequenceCapture.h
# End Source File
# Begin Source File

SOURCE=..\..\src\VideoCapture\OpenGLCapture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\VideoCapture\OpenGLCapture.h
# End Source File
# Begin Source File

SOURCE=..\..\src\VideoCapture\UncompressedAVICapture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\VideoCapture\UncompressedAVICapture.h
# End Source File
# Begin Source File

SOURCE=..\..\src\VideoCapture\VFWCapture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\VideoCapture\VFWCapture.h
# End Source File
# End Group
# Begin Group "Helpers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Helpers\BasicFileIO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\BasicFileIO.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\Configuration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\Configuration.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\helpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\helpers.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\OptimizedFunctions.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\OptimizedFunctionsList.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\PerformanceLib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\PerformanceLib.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\Quicksort.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\Quicksort.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\Timer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Helpers\Timer.h
# End Source File
# End Group
# Begin Group "DataProcessing"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\DataProcessing\MeanFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DataProcessing\MeanFilter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DataProcessing\Normalizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DataProcessing\Normalizer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DataProcessing\RANSAC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DataProcessing\RANSAC.h
# End Source File
# End Group
# Begin Group "Interfaces"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Interfaces\ApplicationHandlerInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Interfaces\FeatureCalculatorInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Interfaces\FilterInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Interfaces\GUIEventInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Interfaces\GUIInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Interfaces\MainWindowInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Interfaces\ObjectClassifierInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Interfaces\ObjectEntryFilterInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Interfaces\RegionFilterInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Interfaces\RigidObjectTrackingInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Interfaces\VideoCaptureInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Interfaces\WindowEventInterface.h
# End Source File
# End Group
# Begin Group "Color"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Color\Color.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Color\Color.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Color\ColorParameterSet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Color\ColorParameterSet.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Color\RGBColorModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Color\RGBColorModel.h
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Math\Constants.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\DoubleMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\DoubleMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\DoubleVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\DoubleVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\FloatMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\FloatMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\FloatVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\FloatVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\LinearAlgebra.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\LinearAlgebra.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\Matd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\Matd.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\Math2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\Math2d.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\Math3d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\Math3d.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\SVD.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\Vecd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\Vecd.h
# End Source File
# End Group
# Begin Group "ParticleFilter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\ParticleFilter\ParticleFilterFramework.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ParticleFilter\ParticleFilterFramework.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ParticleFilter\ParticleFilterFrameworkFloat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ParticleFilter\ParticleFilterFrameworkFloat.h
# End Source File
# End Group
# Begin Group "VideoAccess"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\VideoAccess\VideoReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\VideoAccess\VideoReader.h
# End Source File
# End Group
# Begin Group "Structs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Structs\ObjectDefinitions.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Structs\Structs.h
# End Source File
# End Group
# Begin Group "Visualizer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Visualizer\OpenGLVisualizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Visualizer\OpenGLVisualizer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Visualizer\OpenGLVisualizerControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Visualizer\OpenGLVisualizerControl.h
# End Source File
# End Group
# Begin Group "Threading"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Threading\Event.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Threading\Event.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Threading\Mutex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Threading\Mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Threading\Thread.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Threading\ThreadBase.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Threading\Threading.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Threading\Threading.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Threading\WindowsThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Threading\WindowsThread.h
# End Source File
# End Group
# Begin Group "DataStructures"

# PROP Default_Filter ""
# Begin Group "KdTree"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\DataStructures\KdTree\KdPriorityQueue.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DataStructures\KdTree\KdStructs.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DataStructures\KdTree\KdTree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DataStructures\KdTree\KdTree.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DataStructures\KdTree\KdUtils.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\DataStructures\DynamicArray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\DataStructures\DynamicArray.h
# End Source File
# Begin Source File

SOURCE=..\..\src\DataStructures\DynamicArrayTemplate.h
# End Source File
# End Group
# Begin Group "Tracking"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Tracking\ICP.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Tracking\ICP.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Tracking\KLTTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Tracking\KLTTracker.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Tracking\ObjectPose.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Tracking\ObjectPose.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Tracking\POSIT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Tracking\POSIT.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Tracking\RAPiD.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Tracking\RAPiD.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Tracking\Tracker2d3d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Tracking\Tracker2d3d.h
# End Source File
# End Group
# Begin Group "Features"

# PROP Default_Filter ""
# Begin Group "SIFTFeatures"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Features\SIFTFeatures\SIFTFeatureCalculator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Features\SIFTFeatures\SIFTFeatureCalculator.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Features\SIFTFeatures\SIFTFeatureEntry.h
# End Source File
# End Group
# Begin Group "PatchFeatures"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Features\PatchFeatures\PatchFeatureEntry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Features\PatchFeatures\PatchFeatureEntry.h
# End Source File
# End Group
# Begin Group "HarrisSIFTFeatures"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Features\HarrisSIFTFeatures\HarrisSIFTFeatureCalculator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Features\HarrisSIFTFeatures\HarrisSIFTFeatureCalculator.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\Features\FeatureEntry.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Features\FeatureSet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Features\FeatureSet.h
# End Source File
# End Group
# Begin Group "Networking"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Networking\TCPSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Networking\TCPSocket.h
# End Source File
# End Group
# Begin Group "Classification"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Classification\NearestNeighbor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Classification\NearestNeighbor.h
# End Source File
# End Group
# Begin Group "Contours"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Contours\ContourHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Contours\ContourHelper.h
# End Source File
# End Group
# End Target
# End Project
