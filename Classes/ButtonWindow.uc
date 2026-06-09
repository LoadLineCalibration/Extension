//=============================================================================
// ButtonWindow.
//=============================================================================
class ButtonWindow extends TextWindow
	native
	noexport;

// ----------------------------------------------------------------------

struct ButtonDisplayInfo
{
	var texture tx;
	var color   tlColor;
	var color   txColor;
};


// ----------------------------------------------------------------------
// Variables

var const bool                bButtonPressed;
var const bool                bMousePressed;
var const bool                bAutoRepeat;
var const bool                bEnableRightMouseClick;

var const float               activateDelay;
var const float               initialDelay;
var const float               repeatRate;

var const texture             curTexture;
var const color               curTileColor;
var const color               curTextColor;

var private sound             pressSound;
var private sound             clickSound;

var private int               activateTimer;
var private float             repeatTime;
var private EInputKey         lastInputKey;

var private ButtonDisplayInfo info[6];

// ----------------------------------------------------------------------
// Intrinsics

native(1590) final function ActivateButton(EInputKey key);

native(1591) final function SetActivateDelay(optional float newDelay);

native(1592) final function SetButtonTextures(optional texture normal,
                                              optional texture pressed,
                                              optional texture normalFocus,
                                              optional texture pressedFocus,
                                              optional texture normalInsensitive,
                                              optional texture pressedInsensitive);

native(1593) final function SetButtonColors(optional color normal,
                                            optional color pressed,
                                            optional color normalFocus,
                                            optional color pressedFocus,
                                            optional color normalInsensitive,
                                            optional color pressedInsensitive);

native(1594) final function SetTextColors(optional color normal,
                                          optional color pressed,
                                          optional color normalFocus,
                                          optional color pressedFocus,
                                          optional color normalInsensitive,
                                          optional color pressedInsensitive);

native(1595) final function EnableAutoRepeat(optional bool bEnable,
                                             optional float initialDelay,
                                             optional float repeatRate);

native(1596) final function EnableRightMouseClick(optional bool bEnable);

native(1597) final function SetButtonSounds(optional sound pressSound,
                                            optional sound clickSound);

native(1598) final function PressButton(optional EInputKey key);



// ----------------------------------------------------------------------

event bool AcceleratorKeyPressed(string key)
{
	local bool retval;

	retval = Super.AcceleratorKeyPressed(key);

	if (!retval)
	{
		PressButton();

		retval = true;
	}

	return retval;
}


// ----------------------------------------------------------------------

event bool VirtualKeyPressed(EInputKey key, bool bRepeat)
{
	local bool retval;

	retval = Super.VirtualKeyPressed(key, bRepeat);

	if ((key == IK_Enter) || (key == IK_Space))
	{
		PressButton(key);

		retval = TRUE;
	}

	return retval;
}

defaultproperties
{
}
