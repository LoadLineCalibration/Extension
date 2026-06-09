//=============================================================================
// ListWindow.
//=============================================================================
class ListWindow extends Window
	native
	noexport;


// ----------------------------------------------------------------------
// Variables

var const string            delimiter;
var const color             inverseColor;
var const texture           highlightTexture;
var const texture           focusTexture;
var const color             highlightColor;
var const color             focusColor;
var const float             focusThickness;

var const native DynamicArray rows;
var const native DynamicArray cols;

var const bool              bAutoSort;
var const bool              bAutoExpandColumns;
var const bool              bMultiSelect;
var const float             colMargin;
var const float             rowMargin;

var const bool              bHotKeys;
var const int               hotKeyCol;

var const float             lineSize;

var const sound             activateSound;
var const sound             moveSound;

var private int             numSelected;
var private native int      focusLine;
var private native int      anchorLine;
var private bool            bDragging;
var private int             lastIndex;
var private float           remainingDelay;

var private string          hotKeyString;
var private float           hotKeyTimer;


// ----------------------------------------------------------------------
// Intrinsics

native(1720) final function int IndexToRowId(int index);
native(1721) final function int RowIdToIndex(int rowId);
native(1722) final function SetRowClientInt(int rowId, int clientInt);
native(1723) final function int GetRowClientInt(int rowId);
native(1724) final function SetRowClientObject(int rowId, object clientObj);
native(1725) final function object GetRowClientObject(int rowId);

native(1730) final function int AddRow(coerce string rowStr, optional int clientData);
native(1731) final function DeleteRow(int rowId);
native(1732) final function ModifyRow(int rowId, coerce string rowStr);
native(1733) final function DeleteAllRows();
native(1734) final function SetField(int rowId, int colIndex, coerce string fieldStr);
native(1735) final function string GetField(int rowId, int colIndex);
native(1736) final function SetFieldValue(int rowId, int colIndex, float newValue);
native(1737) final function float GetFieldValue(int rowId, int colIndex);

native(1740) final function int GetNumRows();
native(1741) final function int GetNumSelectedRows();
native(1742) final function SelectRow(int rowId, optional bool bSelect);
native(1743) final function SelectAllRows(optional bool bSelect);
native(1744) final function SelectToRow(int rowId, optional bool bClearRows,
                                        optional bool bInvert, optional bool bSpanRows);
native(1745) final function ToggleRowSelection(int rowId);
native(1746) final function bool IsRowSelected(int rowId);
native(1747) final function int GetSelectedRow();

native(1750) final function MoveRow(EMoveList move, optional bool bSelect,
                                    optional bool bClearRows, optional bool bDrag);
native(1751) final function SetRow(int rowId, optional bool bSelect,
                                   optional bool bClearRows, optional bool bDrag);

native(1755) final function SetFocusRow(int rowId, optional bool bMoveTo, optional bool bAnchor);
native(1756) final function int GetFocusRow();

native(1760) final function SetNumColumns(int newCols);
native(1761) final function int GetNumColumns();
native(1762) final function ResizeColumns(optional bool bExpandOnly);

native(1765) final function SetColumnTitle(int colIndex, coerce string title);
native(1766) final function string GetColumnTitle(int colIndex);
native(1767) final function SetColumnWidth(int colIndex, float newWidth);
native(1768) final function float GetColumnWidth(int colIndex);
native(1769) final function SetColumnAlignment(int colIndex, EHAlign newAlign);
native(1770) final function EHAlign GetColumnAlignment(int colIndex);
native(1771) final function SetColumnColor(int colIndex, color newColor);
native(1772) final function GetColumnColor(int colIndex, out color colColor);
native(1773) final function SetColumnFont(int colIndex, font newFont);
native(1774) final function font GetColumnFont(int colIndex);
native(1775) final function SetColumnType(int colIndex, EColumnType newType, optional coerce string newFmt);
native(1776) final function EColumnType GetColumnType(int colIndex);
native(1777) final function HideColumn(int colIndex, optional bool bHide);
native(1778) final function bool IsColumnHidden(int colIndex);

native(1780) final function SetSortColumn(int colIndex, optional bool bReverse,
                                          optional bool bCaseSensitive);
native(1781) final function AddSortColumn(int colIndex, optional bool bReverse,
                                          optional bool bCaseSensitive);
native(1782) final function RemoveSortColumn(int colIndex);
native(1783) final function ResetSortColumns(optional bool bSort);
native(1784) final function Sort();

native(1785) final function EnableHotKeys(optional bool bEnable);
native(1786) final function SetHotKeyColumn(int colIndex);

native(1790) final function EnableAutoSort(optional bool bAutoSort);
native(1791) final function bool IsAutoSortEnabled();
native(1792) final function EnableAutoExpandColumns(optional bool bAutoExpand);
native(1793) final function bool IsAutoExpandColumnsEnabled();
native(1794) final function EnableMultiSelect(optional bool bEnableMultiSelect);
native(1795) final function bool IsMultiSelectEnabled();

native(1800) final function SetFieldMargins(float newMarginWidth, float newMarginHeight);
native(1801) final function GetFieldMargins(out float marginWidth, out float marginHeight);
native(1802) final function int GetPageSize();
native(1803) final function SetDelimiter(string newDelimiter);
native(1804) final function SetHighlightTextColor(color newColor);
native(1805) final function SetHighlightTexture(texture newTexture);
native(1806) final function SetHighlightColor(color newColor);
native(1807) final function SetFocusTexture(texture newTexture);
native(1808) final function SetFocusColor(color newColor);
native(1809) final function SetFocusThickness(float newThickness);

native(1810) final function ShowFocusRow();

native(1811) final function SetListSounds(optional sound activateSound,
                                          optional sound moveSound);
native(1812) final function PlayListSound(sound listSound,
                                          optional float volume,
                                          optional float pitch);


// ----------------------------------------------------------------------

function SetColumnColorRGB(INT colIndex, INT red, INT green, INT blue)
{
	local color newColor;
	newColor.r = red;
	newColor.g = green;
	newColor.b = blue;
	SetColumnColor(colIndex, newColor);
}


// ----------------------------------------------------------------------

function SetHighlightTextColorRGB(INT red, INT green, INT blue)
{
	local color newColor;
	newColor.r = red;
	newColor.g = green;
	newColor.b = blue;
	SetHighlightTextColor(newColor);
}


// ----------------------------------------------------------------------

function SetHighlightColorRGB(INT red, INT green, INT blue)
{
	local color newColor;
	newColor.r = red;
	newColor.g = green;
	newColor.b = blue;
	SetHighlightColor(newColor);
}


// ----------------------------------------------------------------------

function SetFocusColorRGB(INT red, INT green, INT blue)
{
	local color newColor;
	newColor.r = red;
	newColor.g = green;
	newColor.b = blue;
	SetFocusColor(newColor);
}


// ----------------------------------------------------------------------
// VirtualKeyPressed() : Handles virtual keypresses

event bool VirtualKeyPressed(EInputKey key, bool bRepeat)
{
	local bool retval;
	local bool bDrag;
	local bool bSelect;
	local bool bClear;

	retval = Super.VirtualKeyPressed(key, bRepeat);

	bDrag   = IsKeyDown(IK_Shift);
	bSelect = !IsKeyDown(IK_Ctrl);
	bClear  = !IsKeyDown(IK_Ctrl);

	// Handle keys
	switch (key)
	{
		case IK_Up:
			MoveRow(MOVELIST_Up, bSelect, bClear, bDrag);
			retval = true;
			break;
		case IK_Down:
			MoveRow(MOVELIST_Down, bSelect, bClear, bDrag);
			retval = true;
			break;
		case IK_PageUp:
			MoveRow(MOVELIST_PageUp, bSelect, bClear, bDrag);
			retval = true;
			break;
		case IK_PageDown:
			MoveRow(MOVELIST_PageDown, bSelect, bClear, bDrag);
			retval = true;
			break;
		case IK_Home:
			MoveRow(MOVELIST_Home, bSelect, bClear, bDrag);
			retval = true;
			break;
		case IK_End:
			MoveRow(MOVELIST_End, bSelect, bClear, bDrag);
			retval = true;
			break;

		case IK_Space:  // toggle selection
			ToggleRowSelection(GetFocusRow());
			retval = true;
			break;

	}

	// Return TRUE if we handled this
	return (retval);

}

defaultproperties
{
     highlightTexture=Texture'Extension.Solid'
     focusTexture=Texture'Extension.Dithered'
}
