//=============================================================================
// EditWindow.
//=============================================================================
class EditWindow extends LargeTextWindow
	native
	noexport;


// ----------------------------------------------------------------------
// Variables

var const bool bEditable;
var const bool bSingleLine;
var const bool bUppercaseOnly;

var const int                 insertPos;
var const int                 insertHookPos;
var const EInsertionPointType insertType;

var const int  selectStart;
var const int  selectEnd;

var const int  maxSize;

var const texture insertTexture;
var const color   insertColor;
var const texture selectTexture;
var const color   selectColor;
var const color   inverseColor;

var const texture editCursor;
var const texture editCursorShadow;
var const color   editCursorColor;

var const sound   typeSound;
var const sound   deleteSound;
var const sound   enterSound;
var const sound   moveSound;

var private DynamicArray bufferList;
var private int          currentUndo;
var private int          maxUndos;
var private int          unchangedUndo;

var private float dragDelay;
var private float blinkDelay;
var private float blinkStart;
var private float blinkPeriod;
var private bool  bCursorShowing;

var private bool  bDragging;
var private bool  bSelectWords;

var private float insertX;
var private float insertY;
var private float insertWidth;
var private float insertHeight;
var private float insertPrefWidth;
var private float insertPrefHeight;
var private float showAreaX;
var private float showAreaY;
var private float showAreaWidth;
var private float showAreaHeight;

var private float insertPreferredCol;

var private float lastConfigWidth;
var private float lastConfigHeight;

var private int   selectStartRow;
var private int   selectEndRow;
var private float selectStartX;
var private float selectEndX;


// ----------------------------------------------------------------------
// Intrinsics

native(1880) final function MoveInsertionPoint(EMoveInsert moveInsert, optional bool bDrag);
native(1881) final function SetInsertionPoint(int newPos, optional bool bDrag);
native(1882) final function int GetInsertionPoint();
native(1883) final function SetSelectedArea(int startPos, int count);
native(1884) final function GetSelectedArea(out int startPos, out int count);

native(1885) final function EnableEditing(optional bool bEdit);
native(1886) final function bool IsEditingEnabled();

native(1887) final function EnableSingleLineEditing(optional bool bSingle);
native(1888) final function bool IsSingleLineEditingEnabled();

native(1889) final function EnableUppercaseOnly(optional bool bUppercase);

native(1890) final function ClearTextChangedFlag();
native(1891) final function SetTextChangedFlag(optional bool bSet);
native(1892) final function bool HasTextChanged();

native(1895) final function SetMaxSize(int newMaxSize);
native(1896) final function SetMaxUndos(int newMaxUndos);

native(1900) final function bool InsertText(optional coerce string insertText,
                                            optional bool bUndo, optional bool bSelect);
native(1901) final function DeleteChar(optional bool bBefore, optional bool bUndo);

native(1902) final function SetInsertionPointBlinkRate(optional float blinkStart,
                                                       optional float blinkPeriod);

native(1905) final function SetInsertionPointTexture(optional texture newTexture,
                                                     optional color newColor);
native(1906) final function SetInsertionPointType(EInsertionPointType newType,
                                                  optional float prefWidth,
                                                  optional float prefHeight);
native(1907) final function SetSelectedAreaTexture(optional texture newTexture,
                                                   optional color newColor);
native(1908) final function SetSelectedAreaTextColor(optional color newColor);

native(1909) final function SetEditCursor(optional texture newCursor, 
                                          optional texture newCursorShadow, 
										  optional color newColor);

native(1910) final function Undo();
native(1911) final function Redo();
native(1912) final function ClearUndo();

native(1915) final function Copy();
native(1916) final function Cut();
native(1917) final function Paste();

native(1920) final function SetEditSounds(optional sound typeSound,
                                          optional sound deleteSound,
                                          optional sound enterSound,
                                          optional sound moveSound);
native(1921) final function PlayEditSound(sound playSound,
                                          optional float volume, optional float pitch);


// ----------------------------------------------------------------------
// VirtualKeyPressed() : Handles virtual keypresses

event bool VirtualKeyPressed(EInputKey key, bool bRepeat)
{
	local bool retval;
	local bool bShift;
	local bool bCtrl;
	local bool bDrag;
	local bool bWord;

	retval = Super.VirtualKeyPressed(key, bRepeat);

	if (!bEditable)
		return (retval);

	bShift = IsKeyDown(IK_Shift);
	bCtrl  = IsKeyDown(IK_Ctrl);
	bDrag  = bShift;
	bWord  = bCtrl;

	// Movement
	if (key == IK_Left)
	{
		if (bWord)
			MoveInsertionPoint(MOVEINSERT_WordLeft, bDrag);
		else
			MoveInsertionPoint(MOVEINSERT_Left, bDrag);
		PlayEditSound(moveSound);
		retval = true;
	}
	else if (key == IK_Right)
	{
		if (bWord)
			MoveInsertionPoint(MOVEINSERT_WordRight, bDrag);
		else
			MoveInsertionPoint(MOVEINSERT_Right, bDrag);
		PlayEditSound(moveSound);
		retval = true;
	}
	else if (key == IK_Up)
	{
		MoveInsertionPoint(MOVEINSERT_Up, bDrag);
		PlayEditSound(moveSound);
		retval = true;
	}
	else if (key == IK_Down)
	{
		MoveInsertionPoint(MOVEINSERT_Down, bDrag);
		PlayEditSound(moveSound);
		retval = true;
	}
	else if (key == IK_PageUp)
	{
		MoveInsertionPoint(MOVEINSERT_PageUp, bDrag);
		PlayEditSound(moveSound);
		retval = true;
	}
	else if (key == IK_PageDown)
	{
		MoveInsertionPoint(MOVEINSERT_PageDown, bDrag);
		PlayEditSound(moveSound);
		retval = true;
	}
	else if (key == IK_Home)
	{
		if (bWord)
			MoveInsertionPoint(MOVEINSERT_Home, bDrag);
		else
			MoveInsertionPoint(MOVEINSERT_StartOfLine, bDrag);
		PlayEditSound(moveSound);
		retval = true;
	}
	else if (key == IK_End)
	{
		if (bWord)
			MoveInsertionPoint(MOVEINSERT_End, bDrag);
		else
			MoveInsertionPoint(MOVEINSERT_EndOfLine, bDrag);
		PlayEditSound(moveSound);
		retval = true;
	}

	// Editing
	else if (key == IK_Backspace)
	{
		PlayEditSound(deleteSound);
		DeleteChar(true, true);
		retval = true;
	}
	else if (key == IK_Delete)
	{
		PlayEditSound(deleteSound);
		DeleteChar(false, true);
		retval = true;
	}
	else if (key == IK_Enter)
	{
		InsertText("|n", true);
		retval = true;
	}
	else if (key == IK_C)
	{
		PlayEditSound(moveSound);
		if (bCtrl)
		{
			Copy();
			retval = true;
		}
	}
	else if (key == IK_X)
	{
		PlayEditSound(moveSound);
		if (bCtrl)
		{
			Cut();
			retval = true;
		}
	}
	else if (key == IK_V)
	{
		PlayEditSound(moveSound);
		if (bCtrl)
		{
			Paste();
			retval = true;
		}
	}
	else if (key == IK_Z)
	{
		PlayEditSound(moveSound);
		if (bCtrl)
		{
			Undo();
			retval = true;
		}
	}
	else if (key == IK_Y)
	{
		PlayEditSound(moveSound);
		if (bCtrl)
		{
			Redo();
			retval = true;
		}
	}

	return (retval);

}


// ----------------------------------------------------------------------
// SetInsertionPointTextureRGB() : Sets the texture and color for the
//                                 insertion point cursor

function SetInsertionPointTextureRGB(texture newTexture, byte red, byte green, byte blue)
{
	local color newColor;

	newColor.R = red;
	newColor.G = green;
	newColor.B = blue;

	SetInsertionPointTexture(newTexture, newColor);
}


// ----------------------------------------------------------------------
// SetSelectedAreaTextureRGB() : Sets the texture and color for the
//                               selected area

function SetSelectedAreaTextureRGB(texture newTexture, byte red, byte green, byte blue)
{
	local color newColor;

	newColor.R = red;
	newColor.G = green;
	newColor.B = blue;

	SetSelectedAreaTexture(newTexture, newColor);
}


// ----------------------------------------------------------------------
// SetSelectedAreaTextColorRGB() : Sets the color of selected text

function SetSelectedAreaTextColorRGB(byte red, byte green, byte blue)
{
	local color newColor;

	newColor.R = red;
	newColor.G = green;
	newColor.B = blue;

	SetSelectedAreaTextColor(newColor);
}


/* FOR PROTOTYPE ONLY
function bool FilterChar(out string chStr)
{
	return true;
}
*/

defaultproperties
{
     insertTexture=Texture'Extension.Solid'
     selectTexture=Texture'Extension.Solid'
}
