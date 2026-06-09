//=============================================================================
// ExtString
//=============================================================================
class ExtString extends Object
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

var native private int speechPage;
var native private string text;


// ----------------------------------------------------------------------
// natives

native(1140) final function SetText(coerce string newText);
native(1141) final function AppendText(coerce string newText);
native(1142) final function string GetText();
native(1143) final function int GetTextLength();
native(1144) final function int GetTextPart(int startPos, int count, out string outText);
native(1145) final function int GetFirstTextPart(out string outText);
native(1146) final function int GetNextTextPart(out string outText);

defaultproperties
{
}
