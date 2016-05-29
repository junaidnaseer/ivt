# Microsoft Developer Studio Project File - Name="IVTOpenCVLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=IVTOpenCVLib - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "IVTOpenCVLib.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "IVTOpenCVLib.mak" CFG="IVTOpenCVLib - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "IVTOpenCVLib - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "IVTOpenCVLib - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IVTOpenCVLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release-opencv"
# PROP BASE Intermediate_Dir "Release-opencv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release-opencv"
# PROP Intermediate_Dir "Release-opencv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../src" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\win32\ivtopencv.lib"

!ELSEIF  "$(CFG)" == "IVTOpenCVLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug-opencv"
# PROP BASE Intermediate_Dir "Debug-opencv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug-opencv"
# PROP Intermediate_Dir "Debug-opencv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../src" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\win32\ivtopencvd.lib"

!ENDIF 

# Begin Target

# Name "IVTOpenCVLib - Win32 Release"
# Name "IVTOpenCVLib - Win32 Debug"
# Begin Group "VideoAccess"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\VideoAccess\VideoWriterCV.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\VideoAccess\VideoWriterCV.h
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Math\LinearAlgebraCV.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Math\LinearAlgebraCV.h
# End Source File
# End Group
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\gui\OpenCVApplicationHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\OpenCVApplicationHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\OpenCVWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\OpenCVWindow.h
# End Source File
# End Group
# Begin Group "VideoCapture"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\VideoCapture\OpenCVCapture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\VideoCapture\OpenCVCapture.h
# End Source File
# End Group
# Begin Group "ObjectFinder"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\ObjectFinder\HaarClassifierCV.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ObjectFinder\HaarClassifierCV.h
# End Source File
# End Group
# Begin Group "Image"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Image\ImageAccessCV.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\ImageAccessCV.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\ImageProcessorCV.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\ImageProcessorCV.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\IplImageAdaptor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\IplImageAdaptor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\PrimitivesDrawerCV.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Image\PrimitivesDrawerCV.h
# End Source File
# End Group
# Begin Group "Calibration"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Calibration\ExtrinsicParameterCalculatorCV.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\ExtrinsicParameterCalculatorCV.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\StereoCalibrationCV.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Calibration\StereoCalibrationCV.h
# End Source File
# End Group
# End Target
# End Project
