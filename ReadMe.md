# Deus Ex Extension.dll Reconstructed Source

This repository contains an AI-assisted reverse-engineered / reconstructed source version of the original Deus Ex `Extension.dll`.

It is intended for preservation, research, modding, compatibility work, and general study of the Deus Ex / Unreal Engine 1 UI native layer.

This is **not** an official Ion Storm, Eidos, Epic Games, or Square Enix source release.

## Notes

Most of the reconstructed C++ code was produced with AI assistance, based on decompiler output, ASM, original headers, exported class headers, UnrealScript classes, and native function declarations.

The result was manually compiled, tested in-game and corrected through multiple iterations.

## Status

The reconstructed DLL builds with the original VC++ 6.0 / VC98 toolchain.

It has been tested in-game up to Hong Kong without known current critical issues.

Several reconstruction bugs were found during testing and fixed.

## Build requirements

You need:

* Microsoft Visual C++ 6.0 / VC98
* Windows XP or Windows 2000, preferably in a virtual machine
* Original Deus Ex 1 project/game directory
* Access to the repository files from inside the VM

## My build setup

My Deus Ex project directory on the host machine was:

```text
C:\Games\DeusEx_RTX\
C:\Games\DeusEx_RTX\Extension\Inc\
C:\Games\DeusEx_RTX\Extension\Src\
C:\Games\DeusEx_RTX\Extension\Classes\
C:\Games\DeusEx_RTX\Extension\Textures\
```

I shared this folder with a Windows XP virtual machine.

Inside the VM it appeared as:

```text
Z:\DeusEx_RTX\
```

Then I opened the Visual C++ workspace file:

```text
Extension.dsw
```

from inside the virtual machine and built the project with VC++ 6.0.

## Batch files

Check these files for path and build/install details:

```text
_PrepareBuildDir_Extension.bat
_PostBuildInstall_Extension.bat
```

`_PrepareBuildDir_Extension.bat` prepares the build directory.

`_PostBuildInstall_Extension.bat` copies the resulting `Extension.dll` back into the Deus Ex installation.

You may need to edit paths inside these batch files to match your local setup.

## Virtual machine

I used VMware Workstation with a Windows XP VM.

Other virtual machines should also work, as long as they can run Windows XP / Windows 2000 and provide access to the shared Deus Ex project directory.

## Disclaimer

This source is provided as-is.

Use it at your own risk.

Again: this is a reconstructed source release, not an official source release.
