//=============================================================================
// ModalWindow.
//=============================================================================
class ModalWindow extends TabGroupWindow
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

// Mouse focus mode
var EMouseFocusMode             focusMode;

// List of tab groups
var private Window              preferredFocus;
var native private DynamicArray tabGroupWindowList;

var private Window              acceleratorTable[0xFF];
var private bool                bDirtyAccelerators;

// Intrinsics
native(1500) final function SetMouseFocusMode(EMouseFocusMode newFocusMode);
native(1501) final function bool IsCurrentModal();

defaultproperties
{
}
