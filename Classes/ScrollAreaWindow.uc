//=============================================================================
// ScrollAreaWindow.
//=============================================================================
class ScrollAreaWindow extends Window
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

var const ScaleManagerWindow hScaleMgr;
var const ScaleManagerWindow vScaleMgr;
var const ScaleWindow        hScale;
var const ScaleWindow        vScale;
var const ButtonWindow       leftButton;
var const ButtonWindow       rightButton;
var const ButtonWindow       upButton;
var const ButtonWindow       downButton;
var const ClipWindow         clipWindow;

var const float              marginWidth;
var const float              marginHeight;

var const float              scrollbarDistance;

var const bool               bHideScrollbars;
var const bool               bHLastShow;
var const bool               bVLastShow;


// ----------------------------------------------------------------------
// Intrinsics

native(1700) final function EnableScrolling(optional bool bHScrolling,
                                            optional bool bVScrolling);
native(1701) final function SetScrollbarDistance(float newDistance);
native(1702) final function SetAreaMargins(float newMarginWidth,
                                           float newMarginHeight);
native(1703) final function AutoHideScrollbars(optional bool bHide);


// ----------------------------------------------------------------------
// VirtualKeyPressed() : Handles virtual keypresses

event bool VirtualKeyPressed(EInputKey key, bool bRepeat)
{
	local bool retval;

	retval = Super.VirtualKeyPressed(key, bRepeat);

// I'm commenting this out for now, because I'm not sure we should do this
// by default...
/*
	if (key == IK_Home)
	{
		if (vScale != None)
			vScale.MoveThumb(MOVETHUMB_Home);
		retval = true;
	}
	else if (key == IK_End)
	{
		if (vScale != None)
			vScale.MoveThumb(MOVETHUMB_End);
		retval = true;
	}
	else if (key == IK_PageUp)
	{
		if (vScale != None)
			vScale.MoveThumb(MOVETHUMB_PageUp);
		retval = true;
	}
	else if (key == IK_PageDown)
	{
		if (vScale != None)
			vScale.MoveThumb(MOVETHUMB_PageDown);
		retval = true;
	}

	if (hScale != None)
	{
		if (hScale.IsVisible())
		{
			if (key == IK_Left)
			{
				hScale.MoveThumb(MOVETHUMB_StepUp);
				retval = true;
			}
			else if (key == IK_Right)
			{
				hScale.MoveThumb(MOVETHUMB_StepDown);
				retval = true;
			}
		}
	}
*/

	return (retval);

}

defaultproperties
{
}
