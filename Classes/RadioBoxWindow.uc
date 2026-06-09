//=============================================================================
// RadioBoxWindow.
//=============================================================================
class RadioBoxWindow extends TabGroupWindow
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

var bool                        bOneCheck;

var native private DynamicArray toggleButtons;
var private ToggleWindow        currentSelection;

native(1820) final function ToggleWindow GetEnabledToggle();

defaultproperties
{
}
