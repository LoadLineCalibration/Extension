@echo off
rem Optional helper. Run this once in the VC98 VM before opening Extension.dsw.

if not exist C:\DX_ExtBuild mkdir C:\DX_ExtBuild
if not exist C:\DX_ExtBuild\Obj mkdir C:\DX_ExtBuild\Obj

if exist Z:\DeusEx_RTX\System goto Done
echo WARNING: Z:\DeusEx_RTX\System was not found. Check the VM shared-folder mapping.

:Done
echo Build directories prepared.
