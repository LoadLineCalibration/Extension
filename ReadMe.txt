This is an AI-assisted reverse-engineered/reconstructed source release of Deus Ex Extension.dll.

This is not an official Ion Storm, Eidos, Epic Games, or Square Enix source release. Use it at your own risk.

It is intended for preservation, research, modding, and compatibility work.
No new gameplay features were added, except optional interface mouse sensitivity / UI-scale related fixes.

The project targets VC++ 98 and the original Deus Ex 1.x codebase.
Some headers were adjusted to make the project compile.
The resulting DLL has been tested in-game up to Hong Kong without known current issues.

Also, the header files had to be modified for successful compilation.
If you want to compile it, you’ll need VC++ 98; you can install it in a Windows XP/2000 virtual machine. 


In my case, I used C:\Games\DeusEx_RTX\, and added this directory as shared folder (Z:\DeusEx_RTX\) to my Windows XP virtual machine.
Then opened the .dsw file from the VM to compile. Check the _PostBuildInstall_Extension.bat and _PrepareBuildDir_Extension.bat for details.

I used VMWare Workstation, its free for personal use. 

To modify UI mouse sensitivity, edit your DeusEx.ini and add these options:

[Extension.UI]
MouseSensitivity=0.75
bLogDiagnostics=False // Log messages

[Extension.Compatibility]
bCompensateMouseDeltaForInterfaceScale=True