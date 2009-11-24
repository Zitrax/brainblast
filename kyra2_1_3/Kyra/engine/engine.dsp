# Microsoft Developer Studio Project File - Name="engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=engine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "engine.mak" CFG="engine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "engine - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "engine - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "engine"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "engine - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "KYRA_SUPPORT_OPENGL" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

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
# ADD CPP /nologo /MD /W3 /Gm /ZI /Od /D "_DEBUG" /D "_LIB" /D "DEBUG" /D "WIN32" /D "_MBCS" /D "KYRA_SUPPORT_OPENGL" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "engine - Win32 Release"
# Name "engine - Win32 Debug"
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\action.cpp
# End Source File
# Begin Source File

SOURCE=.\action.h
# End Source File
# Begin Source File

SOURCE=.\box.cpp
# End Source File
# Begin Source File

SOURCE=.\box.h
# End Source File
# Begin Source File

SOURCE=.\boxresource.cpp
# End Source File
# Begin Source File

SOURCE=.\boxresource.h
# End Source File
# Begin Source File

SOURCE=.\canvas.cpp
# End Source File
# Begin Source File

SOURCE=.\canvas.h
# End Source File
# Begin Source File

SOURCE=.\canvasresource.cpp
# End Source File
# Begin Source File

SOURCE=.\canvasresource.h
# End Source File
# Begin Source File

SOURCE=.\color.cpp
# End Source File
# Begin Source File

SOURCE=.\color.h
# End Source File
# Begin Source File

SOURCE=.\dataresource.cpp
# End Source File
# Begin Source File

SOURCE=.\dataresource.h
# End Source File
# Begin Source File

SOURCE=.\dirtyrectangle.cpp
# End Source File
# Begin Source File

SOURCE=.\dirtyrectangle.h
# End Source File
# Begin Source File

SOURCE=.\encoder.cpp
# End Source File
# Begin Source File

SOURCE=.\encoder.h
# End Source File
# Begin Source File

SOURCE=.\engine.cpp
# End Source File
# Begin Source File

SOURCE=.\engine.h
# End Source File
# Begin Source File

SOURCE=.\fontresource.cpp
# End Source File
# Begin Source File

SOURCE=.\fontresource.h
# End Source File
# Begin Source File

SOURCE=.\image.cpp
# End Source File
# Begin Source File

SOURCE=.\image.h
# End Source File
# Begin Source File

SOURCE=.\imagetree.cpp
# End Source File
# Begin Source File

SOURCE=.\imagetree.h
# End Source File
# Begin Source File

SOURCE=.\imnode.cpp
# End Source File
# Begin Source File

SOURCE=.\imnode.h
# End Source File
# Begin Source File

SOURCE=.\krmath.cpp
# End Source File
# Begin Source File

SOURCE=.\krmath.h
# End Source File
# Begin Source File

SOURCE=.\kyra.h
# End Source File
# Begin Source File

SOURCE=.\kyrabuild.h
# End Source File
# Begin Source File

SOURCE=.\kyraresource.cpp
# End Source File
# Begin Source File

SOURCE=.\kyraresource.h
# End Source File
# Begin Source File

SOURCE=.\namefield.cpp
# End Source File
# Begin Source File

SOURCE=.\ogltexture.cpp
# End Source File
# Begin Source File

SOURCE=.\painter.cpp
# End Source File
# Begin Source File

SOURCE=.\painter.h
# End Source File
# Begin Source File

SOURCE=.\parser.cpp
# End Source File
# Begin Source File

SOURCE=.\parser.h
# End Source File
# Begin Source File

SOURCE=.\pixelblock.cpp
# End Source File
# Begin Source File

SOURCE=.\pixelblock.h
# End Source File
# Begin Source File

SOURCE=.\rle.cpp
# End Source File
# Begin Source File

SOURCE=.\rle.h
# End Source File
# Begin Source File

SOURCE=.\sdlutil.cpp
# End Source File
# Begin Source File

SOURCE=.\sdlutil.h
# End Source File
# Begin Source File

SOURCE=.\sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\sprite.h
# End Source File
# Begin Source File

SOURCE=.\spriteresource.cpp
# End Source File
# Begin Source File

SOURCE=.\spriteresource.h
# End Source File
# Begin Source File

SOURCE=.\textbox.cpp
# End Source File
# Begin Source File

SOURCE=.\textbox.h
# End Source File
# Begin Source File

SOURCE=.\tile.cpp
# End Source File
# Begin Source File

SOURCE=.\tile.h
# End Source File
# Begin Source File

SOURCE=.\tileresource.cpp
# End Source File
# Begin Source File

SOURCE=.\tileresource.h
# End Source File
# Begin Source File

SOURCE=.\vault.cpp
# End Source File
# Begin Source File

SOURCE=.\vault.h
# End Source File
# End Group
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\gui\button.cpp
# End Source File
# Begin Source File

SOURCE=..\gui\button.h
# End Source File
# Begin Source File

SOURCE=..\gui\console.cpp
# End Source File
# Begin Source File

SOURCE=..\gui\console.h
# End Source File
# Begin Source File

SOURCE=..\gui\eventmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\gui\eventmanager.h
# End Source File
# Begin Source File

SOURCE=..\gui\listbox.cpp
# End Source File
# Begin Source File

SOURCE=..\gui\listbox.h
# End Source File
# Begin Source File

SOURCE=..\gui\textwidget.cpp
# End Source File
# Begin Source File

SOURCE=..\gui\textwidget.h
# End Source File
# Begin Source File

SOURCE=..\gui\widget.cpp
# End Source File
# Begin Source File

SOURCE=..\gui\widget.h
# End Source File
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\util\glbitstream.cpp
# End Source File
# Begin Source File

SOURCE=..\util\glbitstream.h
# End Source File
# Begin Source File

SOURCE=..\Util\glcirclelist.h
# End Source File
# Begin Source File

SOURCE=..\util\glfixed.h
# End Source File
# Begin Source File

SOURCE=..\Util\glinsidelist.h
# End Source File
# Begin Source File

SOURCE=..\util\glisomath.cpp
# End Source File
# Begin Source File

SOURCE=..\Util\glisomath.h
# End Source File
# Begin Source File

SOURCE=..\Util\gllist.h
# End Source File
# Begin Source File

SOURCE=..\util\glstring.cpp
# End Source File
# Begin Source File

SOURCE=..\util\glstring.h
# End Source File
# End Group
# Begin Group "TinyXml"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\tinyxml\tinystr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tinyxml\tinystr.h
# End Source File
# Begin Source File

SOURCE=..\..\tinyxml\tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tinyxml\tinyxml.h
# End Source File
# Begin Source File

SOURCE=..\..\tinyxml\tinyxmlerror.cpp
# End Source File
# Begin Source File

SOURCE=..\..\tinyxml\tinyxmlparser.cpp
# End Source File
# End Group
# Begin Group "GuiExtended"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\guiExtended\KrImageListBox.cpp
# End Source File
# Begin Source File

SOURCE=..\guiExtended\KrImageListBox.h
# End Source File
# Begin Source File

SOURCE=..\guiExtended\progress.cpp
# End Source File
# Begin Source File

SOURCE=..\guiExtended\progress.h
# End Source File
# End Group
# Begin Group "grinliz"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\grinliz\glbitarray.h
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\gldebug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\gldebug.h
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\gldynamic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\gldynamic.h
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\glgeometry.h
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\glmemorypool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\glmemorypool.h
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\glperformance.cpp
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\glperformance.h
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\glprime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\glprime.h
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\glpublisher.h
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\glrandom.cpp
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\glrandom.h
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\gltypes.h
# End Source File
# Begin Source File

SOURCE=..\..\grinliz\glutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\kyra.h
# End Source File
# End Target
# End Project
