# Microsoft Developer Studio Project File - Name="IVTQTGUILib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=IVTQTGUILib - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "IVTQTGUILib.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "IVTQTGUILib.mak" CFG="IVTQTGUILib - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "IVTQTGUILib - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "IVTQTGUILib - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IVTQTGUILib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release-gui"
# PROP BASE Intermediate_Dir "Release-gui"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release-gui"
# PROP Intermediate_Dir "Release-gui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\win32\ivtguiqt.lib"

!ELSEIF  "$(CFG)" == "IVTQTGUILib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug-gui"
# PROP BASE Intermediate_Dir "Debug-gui"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug-gui"
# PROP Intermediate_Dir "Debug-gui"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../src" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\win32\ivtguiqtd.lib"

!ENDIF 

# Begin Target

# Name "IVTQTGUILib - Win32 Release"
# Name "IVTQTGUILib - Win32 Debug"
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\gui\moc_QTApplicationHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\moc_QTColorParameterWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QTApplicationHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QTApplicationHandler.h

!IF  "$(CFG)" == "IVTQTGUILib - Win32 Release"

# Begin Custom Build
InputDir=\libs\IVT\src\gui
InputPath=..\..\src\gui\QTApplicationHandler.h
InputName=QTApplicationHandler

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(QTDIR)\bin\moc.exe" "$(InputPath)" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "IVTQTGUILib - Win32 Debug"

# Begin Custom Build
InputDir=\libs\IVT\src\gui
InputPath=..\..\src\gui\QTApplicationHandler.h
InputName=QTApplicationHandler

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(QTDIR)\bin\moc.exe" "$(InputPath)" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QTColorParameterWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QTColorParameterWindow.h

!IF  "$(CFG)" == "IVTQTGUILib - Win32 Release"

# Begin Custom Build
InputDir=\libs\IVT\src\gui
InputPath=..\..\src\gui\QTColorParameterWindow.h
InputName=QTColorParameterWindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(QTDIR)\bin\moc.exe" "$(InputPath)" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "IVTQTGUILib - Win32 Debug"

# Begin Custom Build
InputDir=\libs\IVT\src\gui
InputPath=..\..\src\gui\QTColorParameterWindow.h
InputName=QTColorParameterWindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(QTDIR)\bin\moc.exe" "$(InputPath)" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QTGLWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QTGLWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QTPainter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QTPainter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QTWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QTWindow.h
# End Source File
# End Group
# End Target
# End Project
