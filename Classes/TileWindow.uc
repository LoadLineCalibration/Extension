//=============================================================================
// TileWindow.
//=============================================================================
class TileWindow extends Window
	native
	noexport;


// ----------------------------------------------------------------------
// Variables

// Order information
var const byte  orientation;        // Horizontal or vertical
var const byte  hDirection;         // Left-right or right-left
var const byte  vDirection;         // Top-bottom or bottom-top

// Margins
var const float hMargin;            // Horizontal margin
var const float vMargin;            // Vertical margin

// Spacing
var const float minorSpacing;       // Child spacing along orientation
var const float majorSpacing;       // Child spacing when wrapped

// Child alignment relative to other children in the same row
var const byte  hChildAlign;        // Horizontal child alignment
var const byte  vChildAlign;        // Vertical child alignment

// Wrapping
var const bool  bWrap;              // Wrap windows if not enough space?

// Parent filling
var const bool  bFillParent;        // Fill the parent with children?

// Child sizing
var const bool  bEqualWidth;        // Make all children equal width?
var const bool  bEqualHeight;       // Make all children equal height?

// Internal information
var native private DynamicArray rowArray;  // Temporary array of row information


// ----------------------------------------------------------------------
// Intrinsics

native(1535) final function SetMargins(float newHMargin, float newVMargin);
native(1536) final function SetOrientation(EOrientation newOrientation);
native(1537) final function SetDirections(EHDirection newHDir, EVDirection newVDir);
native(1538) final function SetOrder(EOrder newOrder);
native(1539) final function SetMinorSpacing(float newSpacing);
native(1540) final function SetMajorSpacing(float newSpacing);
native(1541) final function SetChildAlignments(EHAlign newHAlign, EVAlign newVAlign);
native(1542) final function EnableWrapping(bool bWrapOn);
native(1543) final function FillParent(bool FillParent);
native(1544) final function MakeWidthsEqual(bool bEqual);
native(1545) final function MakeHeightsEqual(bool bEqual);

defaultproperties
{
}
