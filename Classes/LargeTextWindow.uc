//=============================================================================
// LargeTextWindow.
//=============================================================================
class LargeTextWindow extends TextWindow
	native
	noexport;

// ----------------------------------------------------------------------
// Structures

struct XTextParams
{
	var bool  bDirty;
	var bool  bSpecialText;
	var int   dirtyStart;
	var int   dirtyCount;
	var font  normalFont;
	var font  boldFont;
	var float destWidth;
};


// ----------------------------------------------------------------------
// Variables

var const float vSpace;

var const float lineHeight;

var private XTextParams textParams;
var native private DynamicArray rowData;

var private XTextParams queryTextParams;
var native private DynamicArray queryRowData;

var native private DynamicArray tempRowData;


// ----------------------------------------------------------------------
// Intrinsics

native(1860) final function SetVerticalSpacing(optional float newVSpace);

defaultproperties
{
}
