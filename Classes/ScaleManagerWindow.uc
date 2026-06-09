//=============================================================================
// ScaleManagerWindow.
//=============================================================================
class ScaleManagerWindow extends Window
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

var const buttonwindow decButton;
var const buttonwindow incButton;
var const textwindow   valueField;
var const scalewindow  scale;

var const EOrientation orientation;

var const bool         bStretchScaleField;
var const bool         bStretchValueField;

var const float        marginWidth;
var const float        marginHeight;
var const float        spacing;

var const EHAlign      childHAlign;
var const EVAlign      childVAlign;


// ----------------------------------------------------------------------
// Intrinsics

native(1660) final function SetScaleButtons(buttonwindow newDecButton,
                                            buttonwindow newIncButton);
native(1661) final function SetValueField(textwindow newValueField);
native(1662) final function SetScale(scalewindow newScale);
native(1663) final function SetManagerOrientation(EOrientation newOrientation);
native(1664) final function StretchScaleField(optional bool bNewStretch);
native(1665) final function StretchValueField(optional bool bNewStretch);
native(1666) final function SetManagerMargins(optional float newMarginWidth,
                                              optional float newMarginHeight);
native(1667) final function SetMarginSpacing(optional float newSpacing);
native(1668) final function SetManagerAlignments(EHAlign newHAlign, EVAlign newVAlign);

defaultproperties
{
}
