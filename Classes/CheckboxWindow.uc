//=============================================================================
// CheckboxWindow.
//=============================================================================
class CheckboxWindow extends ToggleWindow
	native
	noexport;

// ----------------------------------------------------------------------
// Execs

#exec TEXTURE IMPORT FILE=Textures\CheckboxOn.pcx  NAME=CheckboxOn
#exec TEXTURE IMPORT FILE=Textures\CheckboxOff.pcx NAME=CheckboxOff


// ----------------------------------------------------------------------
// Variables

var const texture    toggleOff;
var const texture    toggleOn;

var const float      textureWidth;
var const float      textureHeight;

var const float      checkboxSpacing;
var const bool       bRightSide;

var const EDrawStyle checkboxStyle;
var const Color      checkboxColor;


// ----------------------------------------------------------------------
// Intrinsics

native(1840) final function SetCheckboxTextures(optional texture toggleOff,
                                                optional texture toggleOn,
                                                optional float   textureWidth,
                                                optional float   textureHeight);
native(1841) final function SetCheckboxSpacing(float newSpacing);
native(1842) final function ShowCheckboxOnRightSide(optional bool bRight);
native(1843) final function SetCheckboxStyle(EDrawStyle newStyle);
native(1844) final function SetCheckboxColor(Color newColor);


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// SetCheckboxColorRGB() : RGB version of SetCheckboxColor()

function SetCheckboxColorRGB(INT red, INT green, INT blue)
{
	local color newColor;
	newColor.r = red;
	newColor.g = green;
	newColor.b = blue;
	SetCheckboxColor(newColor);
}

defaultproperties
{
     toggleOff=Texture'Extension.CheckboxOff'
     toggleOn=Texture'Extension.CheckboxOn'
}
