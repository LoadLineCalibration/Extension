//=============================================================================
// ScaleWindow.
//=============================================================================
class ScaleWindow extends Window
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

var const EOrientation orientation;

var const texture      scaleTexture;
var const texture      thumbTexture;
var const texture      tickTexture;
var const texture      preCapTexture;
var const texture      postCapTexture;

var const bool         bRepeatScaleTexture;
var const bool         bRepeatThumbTexture;
var const bool         bDrawEndTicks;

var const bool         bStretchScale;
var const bool         bSpanThumb;

var const texture      borderPattern;

var const float        scaleBorderSize;
var const float        thumbBorderSize;

var const color        scaleBorderColor;
var const color        thumbBorderColor;

var const EDrawStyle   scaleStyle;
var const EDrawStyle   thumbStyle;
var const EDrawStyle   tickStyle;

var const color        scaleColor;
var const color        thumbColor;
var const color        tickColor;

var const float        scaleWidth;
var const float        scaleHeight;

var const float        thumbWidth;
var const float        thumbHeight;

var const float        tickWidth;
var const float        tickHeight;

var const float        preCapWidth;
var const float        preCapHeight;
var const float        postCapWidth;
var const float        postCapHeight;

var const float        startOffset;
var const float        endOffset;

var const float        marginWidth;
var const float        marginHeight;

var const int          numPositions;
var const int          currentPos;
var const int          spanRange;
var const int          thumbStep;

var const float        fromValue;
var const float        toValue;
var const string       valueFmt;

var const float        initialDelay;
var const float        repeatRate;

var const int          initialPos;

var const sound        setSound;
var const sound        clickSound;
var const sound        dragSound;

var private float      scaleX;
var private float      scaleY;
var private float      scaleW;
var private float      scaleH;

var private float      thumbX;
var private float      thumbY;
var private float      thumbW;
var private float      thumbH;

var private float      tickX;
var private float      tickY;
var private float      tickW;
var private float      tickH;

var private float      preCapXOff;
var private float      preCapYOff;
var private float      preCapW;
var private float      preCapH;

var private float      postCapXOff;
var private float      postCapYOff;
var private float      postCapW;
var private float      postCapH;

var private float      absStartScale;
var private float      absEndScale;

var private bool       bDraggingThumb;
var private float      mousePos;
var private byte       repeatDir;
var private float      remainingTime;

var private native DynamicArray enumStrings;


// ----------------------------------------------------------------------
// Intrinsics

native(1620) final function SetScaleOrientation(EOrientation newOrientation);
native(1621) final function SetScaleTexture(texture newTexture,
                                            optional float newWidth, optional float newHeight,
                                            optional float newStart, optional float newEnd);
native(1622) final function SetThumbTexture(texture newTexture,
                                            optional float newWidth, optional float newHeight);
native(1623) final function SetTickTexture(texture tickTexture, optional bool bDrawEndTicks,
                                           optional float newWidth, optional float newHeight);
native(1624) final function SetThumbCaps(texture preCap, texture postCap,
                                         optional float preCapWidth, optional float preCapHeight,
                                         optional float postCapWidth, optional float postCapHeight);
native(1625) final function EnableStretchedScale(optional bool bNewStretch);
native(1626) final function SetBorderPattern(texture newTexture);
native(1627) final function SetScaleBorder(optional float newBorderSize, optional color newColor);
native(1628) final function SetThumbBorder(optional float newBorderSize, optional color newColor);
native(1629) final function SetScaleStyle(EDrawStyle newStyle);
native(1630) final function SetThumbStyle(EDrawStyle newStyle);
native(1631) final function SetTickStyle(EDrawStyle newStyle);
native(1632) final function SetScaleColor(color newColor);
native(1633) final function SetThumbColor(color newColor);
native(1634) final function SetTickColor(color newColor);
native(1635) final function SetScaleMargins(optional float marginWidth,
                                            optional float marginHeight);
native(1636) final function SetNumTicks(int newNumTicks);
native(1637) final function int GetNumTicks();
native(1638) final function SetThumbSpan(optional int newRange);
native(1639) final function int GetThumbSpan();
native(1640) final function SetTickPosition(int newPosition);
native(1641) final function int GetTickPosition();
native(1642) final function SetValueRange(float newFrom, float newTo);
native(1643) final function SetValue(float newValue);
native(1644) final function float GetValue();
native(1645) final function GetValues(out float fromValue, out float toValue);
native(1646) final function SetValueFormat(coerce string newFmt);
native(1647) final function string GetValueString();
native(1648) final function SetEnumeration(int tickPos, coerce string newStr);
native(1649) final function ClearAllEnumerations();
native(1650) final function MoveThumb(EMoveThumb moveThumb);
native(1651) final function SetThumbStep(int newStep);
native(1652) final function SetScaleSounds(optional sound setSound, optional sound clickSound,
                                           optional sound dragSound);
native(1653) final function PlayScaleSound(sound newSound,
                                           optional float volume, optional float pitch);

// ----------------------------------------------------------------------
// SetScaleBorderRGB() : Sets the size and color of the scale border

function SetScaleBorderRGB(float borderSize, byte red, byte green, byte blue)
{
	local color newColor;

	newColor.R = red;
	newColor.G = green;
	newColor.B = blue;

	SetScaleBorder(borderSize, newColor);
}


// ----------------------------------------------------------------------
// SetThumbBorderRGB() : Sets the size and color of the thumb border

function SetThumbBorderRGB(float borderSize, byte red, byte green, byte blue)
{
	local color newColor;

	newColor.R = red;
	newColor.G = green;
	newColor.B = blue;

	SetThumbBorder(borderSize, newColor);
}


// ----------------------------------------------------------------------
// SetScaleColorRGB() : Sets the color of the scale

function SetScaleColorRGB(byte red, byte green, byte blue)
{
	local color newColor;

	newColor.R = red;
	newColor.G = green;
	newColor.B = blue;

	SetScaleColor(newColor);
}


// ----------------------------------------------------------------------
// SetThumbColorRGB() : Sets the color of the thumb

function SetThumbColorRGB(byte red, byte green, byte blue)
{
	local color newColor;

	newColor.R = red;
	newColor.G = green;
	newColor.B = blue;

	SetThumbColor(newColor);
}


// ----------------------------------------------------------------------
// SetTickColorRGB() : Sets the color of the thumb

function SetTickColorRGB(byte red, byte green, byte blue)
{
	local color newColor;

	newColor.R = red;
	newColor.G = green;
	newColor.B = blue;

	SetTickColor(newColor);
}


// ----------------------------------------------------------------------
// VirtualKeyPressed() : Handles virtual keypresses

event bool VirtualKeyPressed(EInputKey key, bool bRepeat)
{
	local bool retval;

	retval = Super.VirtualKeyPressed(key, bRepeat);

	if ((key == IK_Left) || (key == IK_Up))
	{
		MoveThumb(MOVETHUMB_StepUp);
		PlayScaleSound(setSound);
		retval = true;
	}
	else if ((key == IK_Right) || (key == IK_Down))
	{
		MoveThumb(MOVETHUMB_StepDown);
		PlayScaleSound(setSound);
		retval = true;
	}
	else if (key == IK_Home)
	{
		MoveThumb(MOVETHUMB_Home);
		PlayScaleSound(setSound);
		retval = true;
	}
	else if (key == IK_End)
	{
		MoveThumb(MOVETHUMB_End);
		PlayScaleSound(setSound);
		retval = true;
	}
	else if (key == IK_PageUp)
	{
		MoveThumb(MOVETHUMB_PageUp);
		PlayScaleSound(setSound);
		retval = true;
	}
	else if (key == IK_PageDown)
	{
		MoveThumb(MOVETHUMB_PageDown);
		PlayScaleSound(setSound);
		retval = true;
	}

	return (retval);

}

defaultproperties
{
     borderPattern=Texture'Extension.Solid'
}
