//=============================================================================
// ToggleWindow.
//=============================================================================
class ToggleWindow extends ButtonWindow
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

var const sound enableSound;
var const sound disableSound;

// ----------------------------------------------------------------------
// Intrinsics

native(1610) final function ChangeToggle();
native(1611) final function SetToggle(bool bNewToggle);
native(1612) final function bool GetToggle();
native(1613) final function SetToggleSounds(optional sound enableSound,
                                            optional sound disableSound);

defaultproperties
{
}
