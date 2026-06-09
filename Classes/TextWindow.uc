//=============================================================================
// TextWindow.
//=============================================================================
class TextWindow extends Window
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

// Text drawing options
var const  EHAlign                 hAlign;             // Horizontal alignment (left, center, right justified)
var const  EVAlign                 vAlign;             // Vertical alignment   (top, center, bottom justified)
var const  bool                    bWordWrap;          // Is word wrapping on?
var const  bool                    bTextIsAccelerator; // Is our text also used for keyboard acceleration?

// Margins
var const  float                   hMargin;   // Horizontal margin between the window and the text
var const  float                   vMargin;   // Vertical margin between the window and the text

// Preferred sizes
var const  int                     minLines;  // Preferred minimum number of lines
var const  int                     maxLines;  // Preferred maximum number of lines
var const  float                   minWidth;  // Preferred minimum width

var native private string          text;      // Text string to draw


// ----------------------------------------------------------------------
// Intrinsics

native(1550) final function SetText(coerce string newText);
native(1551) final function AppendText(coerce string newText);
native(1552) final function string GetText();
native(1553) final function int GetTextLength();
native(1554) final function int GetTextPart(int startPos, int count, out string outText);
native(1555) final function SetWordWrap(bool bNewWordWrap);
native(1556) final function SetTextAlignments(EHAlign newHAlign, EVAlign newVAlign);
native(1557) final function SetTextMargins(float newHMargin, float newVMargin);
native(1558) final function SetLines(int newMinLines, int newMaxLines);
native(1559) final function SetMinLines(int newMinLines);
native(1560) final function SetMaxLines(int newMaxLines);
native(1561) final function ResetLines();
native(1562) final function SetMinWidth(float newMinWidth);
native(1563) final function ResetMinWidth();
native(1564) final function EnableTextAsAccelerator(optional bool bEnable);

defaultproperties
{
}
