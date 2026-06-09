# Microsoft Developer Studio Project File - Name="Extension" - Package Owner=<4>

# Microsoft Developer Studio Generated Build File, Format Version 6.00

# ** DO NOT EDIT **



# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102



CFG=Extension - Win32 Release

!MESSAGE This is not a valid makefile. To build this project using NMAKE,

!MESSAGE use the Export Makefile command and run

!MESSAGE 

!MESSAGE NMAKE /f "Extension.mak" CFG="Extension - Win32 Release"

!MESSAGE 

!MESSAGE Possible configuration:

!MESSAGE 

!MESSAGE "Extension - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")

!MESSAGE 



# Begin Project

# PROP AllowPerConfigDependencies 0

# PROP Scc_ProjName ""

# PROP Scc_LocalPath "."

CPP=cl.exe

MTL=midl.exe

RSC=rc.exe



!IF  "$(CFG)" == "Extension - Win32 Release"



# PROP BASE Use_MFC 0

# PROP BASE Use_Debug_Libraries 0

# PROP BASE Output_Dir "Release"

# PROP BASE Intermediate_Dir "Release"

# PROP BASE Target_Dir ""

# PROP Use_MFC 0

# PROP Use_Debug_Libraries 0

# PROP Output_Dir "C:\DX_ExtBuild"

# PROP Intermediate_Dir "C:\DX_ExtBuild\Obj"

# PROP Ignore_Export_Lib 0

# PROP Target_Dir ""

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c

# ADD CPP /nologo /Zp4 /MD /W4 /WX /vd0 /GX /O2 /I ".\Inc" /I ".\Src" /I ".\Classes" /I ".\Textures" /I "Z:\DeusEx_RTX" /I "Z:\DeusEx_RTX\System" /I "Z:\DeusEx_RTX\Extension\Inc" /I "Z:\DeusEx_RTX\Extension\Src" /I "Z:\DeusEx_RTX\Extension\Classes" /I "Z:\DeusEx_RTX\Extension\Textures" /I "Z:\DeusEx_RTX\Extension\SDK" /I "Z:\DeusEx_RTX\Extension\SDK\Core\Inc" /I "Z:\DeusEx_RTX\Extension\SDK\Core\Src" /I "Z:\DeusEx_RTX\Extension\SDK\Engine\Inc" /I "Z:\DeusEx_RTX\Extension\SDK\Engine\Src" /D "NDEBUG" /D ThisPackage=Extension /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D EXTENSION_API=__declspec(dllexport) /D "EXTENSION_EXPORTS" /D EXTENSION_ENABLE_NATIVE_WRAPPER_BATCH01=1 /FR /FD /c

# SUBTRACT CPP /YX

# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32

# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32

# ADD BASE RSC /l 0x409 /d "NDEBUG"

# ADD RSC /l 0x409 /d "NDEBUG"

BSC32=bscmake.exe

# ADD BASE BSC32 /nologo

# ADD BSC32 /nologo /o"C:\DX_ExtBuild\Extension.bsc"

LINK32=link.exe

# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib /nologo /dll /machine:I386

# ADD LINK32 Z:\DeusEx_RTX\Extension\SDK\Core\Lib\Core.lib Z:\DeusEx_RTX\Extension\SDK\Engine\Lib\Engine.lib /libpath:"Z:\DeusEx_RTX\System" /libpath:"Z:\DeusEx_RTX\Extension\SDK\Core\Lib" /libpath:"Z:\DeusEx_RTX\Extension\SDK\Engine\Lib" /nologo /dll /incremental:no /machine:I386 /out:"C:\DX_ExtBuild\Extension.dll" /implib:"C:\DX_ExtBuild\Extension.lib" /pdb:"C:\DX_ExtBuild\Extension.pdb" /def:".\Src\ExtensionAbiAliases.def"

# Begin Special Build Tool

SOURCE="$(InputPath)"

PostBuild_Desc=Installing Extension.dll to Z:\DeusEx_RTX\System

PostBuild_Cmds=.\_PostBuildInstall_Extension.bat

# End Special Build Tool



!ENDIF 



# Begin Target



# Name "Extension - Win32 Release"

# Begin Group "Classes"



# PROP Default_Filter "uc"

# Begin Source File



SOURCE=.\Classes\BorderWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\ButtonWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\CheckboxWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\ClipWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\ComputerWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\EditWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\ExtString.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\ExtensionObject.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\Flag.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\FlagBase.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\FlagBool.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\FlagByte.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\FlagFloat.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\FlagInt.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\FlagName.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\FlagRotator.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\FlagVector.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\GC.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\LargeTextWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\ListWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\ModalWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\PlayerPawnExt.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\RadioBoxWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\RootWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\ScaleManagerWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\ScaleWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\ScrollAreaWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\TabGroupWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\TextLogWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\TextWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\TileWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\ToggleWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\ViewportWindow.uc

# End Source File

# Begin Source File



SOURCE=.\Classes\Window.uc

# End Source File

# End Group

# Begin Group "Source Files"



# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"


# Begin Source File



SOURCE=.\Src\ExtensionNativeRegistration_All.cpp

# End Source File

# Begin Source File

SOURCE=.\Src\ExtensionNativeWrappers_All.cpp

# End Source File
# Begin Source File

SOURCE=.\Src\ExtensionAbiAliases.def

# End Source File
# Begin Source File

SOURCE=.\Src\ExtensionOriginalExports.def

# End Source File



# Begin Source File



SOURCE=.\Src\ExtensionPackage_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XBorderWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XButtonToggleCheckbox_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XClipWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XComputerWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XEditWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XExtString_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XExtensionObject_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XFlagBase_Flags_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XGC_Core_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XGC_FontHelpers_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XGC_Text_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XGC_TilesAndActors_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XLargeTextWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XListWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XModalTabRadio_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XPlayerInputGameBridge_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XRootWindow_Integration_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XScaleManagerWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XScaleWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XScrollAreaWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XTextLogWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XTextWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XTileWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XViewportWindow_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\Src\XWindow_Core_Reconstructed.cpp

# End Source File

# Begin Source File



SOURCE=.\_PostBuildInstall_Extension.bat

# End Source File

# Begin Source File



SOURCE=.\_PrepareBuildDir_Extension.bat

# End Source File



# End Group

# Begin Group "Header Files"



# PROP Default_Filter "h;hpp;hxx;hm;inl"

# Begin Source File



SOURCE=.\Inc\\ExtBorder.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtButton.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtCheckbox.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtClipWindow.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtComputerWindow.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtEdit.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtFlag.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtFlagBase.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtGC.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtGameEngine.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtInput.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtLargeText.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtList.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtModal.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtObject.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtPlayerPawn.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtRadioBox.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtRoot.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtScale.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtScaleManager.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtScrollArea.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtString.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtTabGroup.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtText.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtTextLog.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtTile.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtToggle.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtViewport.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtWindow.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtWindowCore.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\Extension.h

# End Source File

# Begin Source File



SOURCE=.\Inc\\ExtensionCore.h

# End Source File

# Begin Source File



SOURCE=.\Src\\ExtensionPrivate_Reconstructed.h

# End Source File

# End Group

# Begin Group "Textures"



# PROP Default_Filter "bmp;pcx"

# Begin Source File



SOURCE=.\Textures\\CheckboxOff.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\CheckboxOff.pcx

# End Source File

# Begin Source File



SOURCE=.\Textures\\CheckboxOn.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\CheckboxOn.pcx

# End Source File

# Begin Source File



SOURCE=.\Textures\\DefaultCursor.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\DefaultHMoveCursor.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\DefaultMoveCursor.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\DefaultTLMoveCursor.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\DefaultTRMoveCursor.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\DefaultTextCursor.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\DefaultVMoveCursor.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\Dithered.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\Solid.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\SolidGreen.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\SolidRed.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\SolidYellow.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\TechMedium.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\TechMedium_B.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\TechMedium_DS.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\TechSmall.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\TechSmall_DS.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\TechTiny.bmp

# End Source File

# Begin Source File



SOURCE=.\Textures\\VisionBlue.bmp

# End Source File

# End Group

# End Target

# End Project

