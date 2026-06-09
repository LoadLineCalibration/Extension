/*=============================================================================
	XListWindow_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 06.

	Focus: ListWindow behavior: row storage, row ids, column metadata,
	auto-width, sorted insertion, selection/focus routing, hot-key search,
	visible-row drawing and mouse/keyboard interaction.

	This is reconstruction-first source. It is intentionally explicit and
	behavioral; it is not yet a final VC98 drop-in translation unit.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XListWindow);

static const INT RECON_MAX_SORT_COLS = 256;
static XListColData* GReconSortColumns[RECON_MAX_SORT_COLS];
static INT GReconSortColumnCount = 0;
static FString GReconFieldConvertStringBuffer;

struct XReconListKeyPressedParams
{
	FString key;
	UBOOL bResult;
};

static const TCHAR* ReconSafeString(const FString& Text)
{
	if (Text.Len() > 0)
		return *Text;

	return TEXT("");
}

static UBOOL ReconIsHotKeyChar(TCHAR Ch)
{
	if (Ch >= TEXT('a') && Ch <= TEXT('z'))
		return TRUE;

	if (Ch >= TEXT('A') && Ch <= TEXT('Z'))
		return TRUE;

	if (Ch >= TEXT('0') && Ch <= TEXT('9'))
		return TRUE;

	if (Ch == TEXT('_'))
		return TRUE;

	return FALSE;
}

static TCHAR ReconToUpper(TCHAR Ch)
{
	if (Ch >= TEXT('a') && Ch <= TEXT('z'))
		return Ch - TEXT('a') + TEXT('A');

	return Ch;
}

static INT ReconCompareStrings(const TCHAR* Left, const TCHAR* Right, UBOOL bCaseSensitive)
{
	if (Left == NULL)
		Left = TEXT("");

	if (Right == NULL)
		Right = TEXT("");

	if (bCaseSensitive == TRUE)
		return appStrcmp(Left, Right);

	return appStricmp(Left, Right);
}

static INT ReconCompareRows(XListRowData* Left, XListRowData* Right)
{
	guard(ReconCompareRows);

	if (Left == NULL && Right == NULL)
		return 0;

	if (Left == NULL)
		return -1;

	if (Right == NULL)
		return 1;

	for (INT i = 0; i < GReconSortColumnCount; i++)
	{
		XListColData* Col = GReconSortColumns[i];
		if (Col == NULL)
			continue;

		INT ColIndex = Col->index;
		if (ColIndex < 0)
			continue;

		if (ColIndex >= Left->fieldData.Num())
			continue;

		if (ColIndex >= Right->fieldData.Num())
			continue;

		XListFieldData& LeftField = Left->fieldData(ColIndex);
		XListFieldData& RightField = Right->fieldData(ColIndex);

		INT Result = 0;

		if (Col->colType == COLTYPE_Float || Col->colType == COLTYPE_Time)
		{
			if (LeftField.fieldValue < RightField.fieldValue)
				Result = -1;
			else if (LeftField.fieldValue > RightField.fieldValue)
				Result = 1;
		}
		else
		{
			Result = ReconCompareStrings(ReconSafeString(LeftField.field), ReconSafeString(RightField.field), Col->bCaseSensitive);
		}

		if (Result != 0)
		{
			if (Col->bReverse == TRUE)
				Result = -Result;

			return Result;
		}
	}

	if (Left->index < Right->index)
		return -1;

	if (Left->index > Right->index)
		return 1;

	return 0;

	unguard;
}

static INT ReconCompareRowPointers(const void* A, const void* B)
{
	XListRowData* Left = *(XListRowData**)A;
	XListRowData* Right = *(XListRowData**)B;
	return ReconCompareRows(Left, Right);
}

XListWindow::XListWindow(XWindow* Parent)
	: XWindow(Parent)
{
}

void XListWindow::Init(XWindow* Parent)
{
	guard(XListWindow::Init);

	XWindow::Init(Parent);

	SetDelimiter(TEXT(';'));

	inverseColor = FColor(0, 0, 0);
	highlightColor = FColor(255, 255, 255);
	focusColor = FColor(128, 128, 0);
	focusThickness = 1.0f;

	bAutoSort = FALSE;
	bAutoExpandColumns = TRUE;
	bMultiSelect = FALSE;

	colMargin = 3.0f;
	rowMargin = 1.0f;
	lineSize = 0.0f;

	bHotKeys = FALSE;
	hotKeyCol = 0;
	hotKeyString = TEXT("");
	hotKeyTimer = 0.0f;

	activateSound = NULL;
	moveSound = NULL;

	numSelected = 0;
	focusLine = NULL;
	anchorLine = NULL;
	bDragging = FALSE;
	lastIndex = 0;
	remainingDelay = 0.0f;

	maxClicks = 2;

	SetNumColumns(1);
	ResetSortColumns(TRUE);
	SetSelectability(TRUE);

	unguard;
}

void XListWindow::CleanUp()
{
	guard(XListWindow::CleanUp);

	/*
	The original Extension.dll does not call DeleteAllRows() from CleanUp().
	DeleteAllRows() is the public/script-visible path and intentionally emits
	ListSelectionChanged() when the selection count drops. During window teardown
	that callback can re-enter the owning menu while its child list is already
	being destroyed. MenuScreenSaveGame hits this path when the root/menu is
	destroyed and its ListSelectionChanged handler tries to move the edit control
	over a row that is no longer valid.

	CleanUp() must therefore do the private destruction path: release row/column
	storage silently, exactly like the decompiled native does, with no selection
	callback and no parent reconfigure request.
	*/
	numSelected = 0;
	focusLine = NULL;
	anchorLine = NULL;
	bDragging = FALSE;

	for (INT i = rows.Num() - 1; i >= 0; i--)
	{
		XListRowData* Row = rows(i);
		delete Row;
	}
	rows.Empty();

	cols.Empty();
	ClearHotKeyString();

	XWindow::CleanUp();

	unguard;
}

void XListWindow::Serialize(FArchive& Ar)
{
	guard(XListWindow::Serialize);
	XWindow::Serialize(Ar);
	unguard;
}

INT XListWindow::IndexToRowId(INT Index)
{
	guard(XListWindow::IndexToRowId);

	if (Index < 0)
		return 0;

	if (Index >= rows.Num())
		return 0;

	return (INT)rows(Index);

	unguard;
}

INT XListWindow::RowIdToIndex(INT RowId)
{
	guard(XListWindow::RowIdToIndex);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row == NULL)
		return -1;

	return Row->index;

	unguard;
}

void XListWindow::SetClientData(INT RowId, INT ClientData)
{
	guard(XListWindow::SetClientData);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row != NULL)
		Row->clientData = ClientData;

	unguard;
}

INT XListWindow::GetClientData(INT RowId)
{
	guard(XListWindow::GetClientData);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row != NULL)
		return Row->clientData;

	return 0;

	unguard;
}

INT XListWindow::AddRow(const TCHAR* LineStr, INT ClientData)
{
	guard(XListWindow::AddRow);

	XListRowData* Row = CreateRow(LineStr, ClientData);
	if (Row == NULL)
		return 0;

	InsertRow(Row);
	AskParentForReconfigure();
	return (INT)Row;

	unguard;
}

void XListWindow::DeleteRow(INT RowId)
{
	guard(XListWindow::DeleteRow);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row == NULL)
		return;

	UBOOL bSelectionChanged = FALSE;
	ChangeSelectRow(Row->index, FALSE, bSelectionChanged);
	RemoveRow(Row);

	if (focusLine == Row)
		focusLine = NULL;

	if (anchorLine == Row)
		anchorLine = NULL;

	delete Row;

	if (bSelectionChanged == TRUE)
		ChangeListSelection();

	AskParentForReconfigure();

	unguard;
}

void XListWindow::ModifyRow(INT RowId, const TCHAR* LineStr)
{
	guard(XListWindow::ModifyRow);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row == NULL)
		return;

	UBOOL bExpanded = FillRow(Row, LineStr);
	CheckPosition(Row);

	if (bExpanded == TRUE)
		AskParentForReconfigure();

	unguard;
}

void XListWindow::DeleteAllRows()
{
	guard(XListWindow::DeleteAllRows);

	UBOOL bSelectionChanged = FALSE;
	if (numSelected > 0)
		bSelectionChanged = TRUE;

	numSelected = 0;
	focusLine = NULL;
	anchorLine = NULL;

	for (INT i = rows.Num() - 1; i >= 0; i--)
	{
		XListRowData* Row = rows(i);
		delete Row;
	}

	rows.Empty();

	if (bSelectionChanged == TRUE)
		ChangeListSelection();

	AskParentForReconfigure();

	unguard;
}

void XListWindow::SetField(INT RowId, INT ColIndex, const TCHAR* NewField)
{
	guard(XListWindow::SetField);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row == NULL)
		return;

	if (IsColumnValid(ColIndex) == FALSE)
		return;

	SetFieldByString(Row, ColIndex, NewField);

	XGC* GC = GetGC();
	ComputeFieldSize(GC, Row, ColIndex);
	ReleaseGC(GC);

	UBOOL bExpanded = FALSE;
	if (bAutoExpandColumns == TRUE)
		bExpanded = ExpandColumnByField(Row, ColIndex);

	CheckPosition(Row);

	if (bExpanded == TRUE)
		AskParentForReconfigure();

	unguard;
}

const TCHAR* XListWindow::GetField(INT RowId, INT ColIndex)
{
	guard(XListWindow::GetField);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row == NULL)
		return NULL;

	if (IsColumnValid(ColIndex) == FALSE)
		return NULL;

	return ReconSafeString(Row->fieldData(ColIndex).field);

	unguard;
}

void XListWindow::SetFieldValue(INT RowId, INT ColIndex, FLOAT NewValue)
{
	guard(XListWindow::SetFieldValue);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row == NULL)
		return;

	if (IsColumnValid(ColIndex) == FALSE)
		return;

	SetFieldByValue(Row, ColIndex, NewValue);

	XGC* GC = GetGC();
	ComputeFieldSize(GC, Row, ColIndex);
	ReleaseGC(GC);

	UBOOL bExpanded = FALSE;
	if (bAutoExpandColumns == TRUE)
		bExpanded = ExpandColumnByField(Row, ColIndex);

	CheckPosition(Row);

	if (bExpanded == TRUE)
		AskParentForReconfigure();

	unguard;
}

FLOAT XListWindow::GetFieldValue(INT RowId, INT ColIndex)
{
	guard(XListWindow::GetFieldValue);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row == NULL)
		return 0.0f;

	if (IsColumnValid(ColIndex) == FALSE)
		return 0.0f;

	return Row->fieldData(ColIndex).fieldValue;

	unguard;
}

void XListWindow::SelectRow(INT RowId, UBOOL bSelected)
{
	guard(XListWindow::SelectRow);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row == NULL)
		return;

	UBOOL bSelectionChanged = FALSE;

	// Original Extension behavior: in single-select mode, selecting one row
	// first clears any existing selection. SelectRow() is not a raw bit setter.
	if (bMultiSelect == FALSE && bSelected == TRUE && numSelected > 0)
	{
		for (INT i = 0; i < rows.Num(); i++)
			ChangeSelectRow(i, FALSE, bSelectionChanged);
	}

	ChangeSelectRow(Row->index, bSelected, bSelectionChanged);

	if (bSelectionChanged == TRUE)
		ChangeListSelection();

	unguard;
}

void XListWindow::SelectAllRows(UBOOL bSelected)
{
	guard(XListWindow::SelectAllRows);

	UBOOL bSelectionChanged = FALSE;
	UBOOL bSelectRows = FALSE;

	if (bMultiSelect == TRUE)
		bSelectRows = bSelected;

	for (INT i = 0; i < rows.Num(); i++)
		ChangeSelectRow(i, bSelectRows, bSelectionChanged);

	if (bSelectionChanged == TRUE)
		ChangeListSelection();

	unguard;
}

void XListWindow::SelectToRow(INT RowId, UBOOL bClearRows, UBOOL bInvert, UBOOL bDrag)
{
	guard(XListWindow::SelectToRow);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row != NULL)
		MoveToRow(Row, TRUE, bClearRows, bInvert, bDrag, FALSE);

	unguard;
}

void XListWindow::ToggleRowSelection(INT RowId)
{
	guard(XListWindow::ToggleRowSelection);

	// Original Extension routes ToggleRowSelection() through SelectRow(), not
	// directly through ChangeSelectRow().  In single-select mode this matters:
	// SelectRow(TRUE) clears any previous selected row first.
	SelectRow(RowId, IsRowSelected(RowId) == FALSE);

	unguard;
}

UBOOL XListWindow::IsRowSelected(INT RowId)
{
	guard(XListWindow::IsRowSelected);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row != NULL)
		return Row->bSelected;

	return FALSE;

	unguard;
}

INT XListWindow::GetSelectedRow()
{
	guard(XListWindow::GetSelectedRow);

	if (focusLine != NULL && focusLine->bSelected == TRUE)
		return (INT)focusLine;

	for (INT i = 0; i < rows.Num(); i++)
	{
		if (rows(i)->bSelected == TRUE)
			return (INT)rows(i);
	}

	return 0;

	unguard;
}

void XListWindow::MoveRow(EMoveList Move, UBOOL bSelect, UBOOL bClearRows, UBOOL bDrag)
{
	guard(XListWindow::MoveRow);

	INT NewIndex = 0;
	if (focusLine != NULL)
		NewIndex = focusLine->index;

	if (Move == MOVELIST_Up)
		NewIndex--;
	else if (Move == MOVELIST_Down)
		NewIndex++;
	else if (Move == MOVELIST_PageUp)
		NewIndex -= GetPageSize();
	else if (Move == MOVELIST_PageDown)
		NewIndex += GetPageSize();
	else if (Move == MOVELIST_Home)
		NewIndex = 0;
	else if (Move == MOVELIST_End)
		NewIndex = rows.Num() - 1;

	// Match the original Extension.dll clamp order.  This matters for an
	// empty list: high clamp first makes NewIndex = -1, then low clamp
	// brings it back to 0, so the final NewIndex >= rows.Num() test routes
	// through SetFocusRow(0, ...) instead of indexing rows(-1).
	if (NewIndex >= rows.Num())
		NewIndex = rows.Num() - 1;

	if (NewIndex < 0)
		NewIndex = 0;

	if (NewIndex >= rows.Num())
	{
		// Original calls SetFocusRow(0, TRUE, !bDrag) when the requested
		// target is outside the row array, including the empty-list case.
		SetFocusRow(0, TRUE, bDrag == FALSE);
	}
	else
	{
		MoveToRow(rows(NewIndex), bSelect, bClearRows, FALSE, bDrag, TRUE);
	}

	unguard;
}

void XListWindow::SetRow(INT RowId, UBOOL bSelect, UBOOL bClearRows, UBOOL bDrag)
{
	guard(XListWindow::SetRow);

	XListRowData* Row = (XListRowData*)RowId;
	if (Row != NULL)
	{
		MoveToRow(Row, bSelect, bClearRows, FALSE, bDrag, TRUE);
	}
	else
	{
		SetFocusRow(0, TRUE, bDrag == FALSE);
	}

	unguard;
}

void XListWindow::SetFocusRow(INT RowId, UBOOL bMoveTo, UBOOL bAnchor)
{
	guard(XListWindow::SetFocusRow);

	XListRowData* Row = (XListRowData*)RowId;
	focusLine = Row;

	if (bAnchor == TRUE)
		anchorLine = Row;

	if (bMoveTo == TRUE)
		ShowFocusRow();

	unguard;
}

INT XListWindow::GetFocusRow()
{
	guard(XListWindow::GetFocusRow);
	return (INT)focusLine;
	unguard;
}

void XListWindow::SetNumColumns(INT NumColumns)
{
	guard(XListWindow::SetNumColumns);

	if (NumColumns < 0)
		NumColumns = 0;

	INT OldNum = cols.Num();

	if (NumColumns > OldNum)
	{
		INT InsertAt = cols.AddZeroed(NumColumns - OldNum);
		for (INT i = InsertAt; i < cols.Num(); i++)
			InitColumn(cols(i), i);
	}
	else if (NumColumns < OldNum)
	{
		if (NumColumns == 0)
			cols.Empty();
		else
			cols.Remove(NumColumns, OldNum - NumColumns);
	}

	for (INT RowIndex = 0; RowIndex < rows.Num(); RowIndex++)
		ExtendRow(rows(RowIndex));

	ComputeRowSize();
	AskParentForReconfigure();

	unguard;
}

void XListWindow::ResizeColumns(UBOOL bExpandOnly)
{
	guard(XListWindow::ResizeColumns);

	if (bExpandOnly == FALSE)
	{
		for (INT ColIndex = 0; ColIndex < cols.Num(); ColIndex++)
			cols(ColIndex).colWidth = colMargin + colMargin;
	}

	for (INT RowIndex = 0; RowIndex < rows.Num(); RowIndex++)
	{
		for (INT ColIndex = 0; ColIndex < cols.Num(); ColIndex++)
			ExpandColumnByField(rows(RowIndex), ColIndex);
	}

	AskParentForReconfigure();

	unguard;
}

void XListWindow::SetColumnTitle(INT ColIndex, const TCHAR* NewTitle)
{
	guard(XListWindow::SetColumnTitle);

	if (IsColumnValid(ColIndex) == FALSE)
		return;

	if (NewTitle == NULL)
		NewTitle = TEXT("");

	cols(ColIndex).title = NewTitle;
	AskParentForReconfigure();

	unguard;
}

const TCHAR* XListWindow::GetColumnTitle(INT ColIndex)
{
	guard(XListWindow::GetColumnTitle);

	if (IsColumnValid(ColIndex) == FALSE)
		return NULL;

	return ReconSafeString(cols(ColIndex).title);

	unguard;
}

void XListWindow::SetColumnWidth(INT ColIndex, FLOAT NewColWidth)
{
	guard(XListWindow::SetColumnWidth);

	if (IsColumnValid(ColIndex) == FALSE)
		return;

	if (cols(ColIndex).colWidth != NewColWidth)
	{
		cols(ColIndex).colWidth = NewColWidth;

		if (bAutoExpandColumns == TRUE)
			ResizeColumns(TRUE);

		AskParentForReconfigure();
	}

	unguard;
}

FLOAT XListWindow::GetColumnWidth(INT ColIndex)
{
	guard(XListWindow::GetColumnWidth);

	if (IsColumnValid(ColIndex) == FALSE)
		return 0.0f;

	if (cols(ColIndex).bHide == TRUE)
		return 0.0f;

	return cols(ColIndex).colWidth;

	unguard;
}

void XListWindow::SetColumnAlignment(INT ColIndex, EHAlign NewHAlign)
{
	guard(XListWindow::SetColumnAlignment);

	if (IsColumnValid(ColIndex) == TRUE)
		cols(ColIndex).alignment = NewHAlign;

	unguard;
}

EHAlign XListWindow::GetColumnAlignment(INT ColIndex)
{
	guard(XListWindow::GetColumnAlignment);

	if (IsColumnValid(ColIndex) == TRUE)
		return (EHAlign)cols(ColIndex).alignment;

	return HALIGN_Left;

	unguard;
}

void XListWindow::SetColumnColor(INT ColIndex, FColor NewColor)
{
	guard(XListWindow::SetColumnColor);

	if (IsColumnValid(ColIndex) == TRUE)
		cols(ColIndex).colColor = NewColor;

	unguard;
}

FColor XListWindow::GetColumnColor(INT ColIndex)
{
	guard(XListWindow::GetColumnColor);

	if (IsColumnValid(ColIndex) == TRUE)
		return cols(ColIndex).colColor;

	return FColor(0, 0, 0);

	unguard;
}

void XListWindow::SetColumnFont(INT ColIndex, UFont* NewFont)
{
	guard(XListWindow::SetColumnFont);

	if (IsColumnValid(ColIndex) == FALSE)
		return;

	if (NewFont == NULL)
		NewFont = normalFont;

	if (cols(ColIndex).colFont != NewFont)
	{
		cols(ColIndex).colFont = NewFont;

		XGC* GC = GetGC();
		if (GC != NULL)
		{
			for (INT RowIndex = 0; RowIndex < rows.Num(); RowIndex++)
				ComputeFieldSize(GC, rows(RowIndex), ColIndex);
		}
		ReleaseGC(GC);

		ComputeRowSize();

		if (bAutoExpandColumns == TRUE)
			ResizeColumns(TRUE);

		AskParentForReconfigure();
	}

	unguard;
}

UFont* XListWindow::GetColumnFont(INT ColIndex)
{
	guard(XListWindow::GetColumnFont);

	if (IsColumnValid(ColIndex) == TRUE)
		return cols(ColIndex).colFont;

	return normalFont;

	unguard;
}

void XListWindow::SetColumnType(INT ColIndex, EColumnType ColType, const TCHAR* NewFmt)
{
	guard(XListWindow::SetColumnType);

	if (IsColumnValid(ColIndex) == FALSE)
		return;

	if (NewFmt == NULL)
	{
		if (ColType == COLTYPE_Float)
			NewFmt = TEXT("%f");
		else if (ColType == COLTYPE_Time)
			NewFmt = TEXT("%02h:%02m");
		else
			NewFmt = TEXT("");
	}

	cols(ColIndex).colType = ColType;
	cols(ColIndex).colFmt = NewFmt;

	XGC* GC = GetGC();
	if (GC != NULL)
	{
		for (INT RowIndex = 0; RowIndex < rows.Num(); RowIndex++)
		{
			FString Copy = rows(RowIndex)->fieldData(ColIndex).field;
			SetFieldByString(rows(RowIndex), ColIndex, *Copy);
			ComputeFieldSize(GC, rows(RowIndex), ColIndex);
		}
	}
	ReleaseGC(GC);

	if (bAutoExpandColumns == TRUE)
		ResizeColumns(TRUE);

	unguard;
}

EColumnType XListWindow::GetColumnType(INT ColIndex)
{
	guard(XListWindow::GetColumnType);

	if (IsColumnValid(ColIndex) == TRUE)
		return (EColumnType)cols(ColIndex).colType;

	return COLTYPE_String;

	unguard;
}

void XListWindow::HideColumn(INT ColIndex, UBOOL bHide)
{
	guard(XListWindow::HideColumn);

	if (IsColumnValid(ColIndex) == FALSE)
		return;

	if (cols(ColIndex).bHide != bHide)
	{
		cols(ColIndex).bHide = bHide;
		AskParentForReconfigure();
	}

	unguard;
}

UBOOL XListWindow::IsColumnHidden(INT ColIndex)
{
	guard(XListWindow::IsColumnHidden);

	if (IsColumnValid(ColIndex) == TRUE)
		return cols(ColIndex).bHide;

	return TRUE;

	unguard;
}

void XListWindow::SetSortColumn(INT ColIndex, UBOOL bReverse, UBOOL bCaseSensitive)
{
	guard(XListWindow::SetSortColumn);

	if (IsColumnValid(ColIndex) == FALSE)
		return;

	for (INT i = 0; i < cols.Num(); i++)
	{
		cols(i).sortIndex = 0;
		cols(i).bReverse = FALSE;
		cols(i).bCaseSensitive = FALSE;
	}

	cols(ColIndex).sortIndex = 1;
	cols(ColIndex).bReverse = bReverse;
	cols(ColIndex).bCaseSensitive = bCaseSensitive;

	if (bAutoSort == TRUE)
		Sort();

	unguard;
}

void XListWindow::AddSortColumn(INT ColIndex, UBOOL bReverse, UBOOL bCaseSensitive)
{
	guard(XListWindow::AddSortColumn);

	if (IsColumnValid(ColIndex) == FALSE)
		return;

	for (INT i = 0; i < cols.Num(); i++)
	{
		if (cols(i).sortIndex > 0)
			cols(i).sortIndex++;
	}

	cols(ColIndex).sortIndex = 1;
	cols(ColIndex).bReverse = bReverse;
	cols(ColIndex).bCaseSensitive = bCaseSensitive;

	if (bAutoSort == TRUE)
		Sort();

	unguard;
}

void XListWindow::RemoveSortColumn(INT ColIndex)
{
	guard(XListWindow::RemoveSortColumn);

	if (IsColumnValid(ColIndex) == FALSE)
		return;

	cols(ColIndex).sortIndex = -1;

	if (bAutoSort == TRUE)
		Sort();

	unguard;
}

void XListWindow::ResetSortColumns(UBOOL bSort)
{
	guard(XListWindow::ResetSortColumns);

	for (INT i = 0; i < cols.Num(); i++)
	{
		if (bSort == TRUE)
			cols(i).sortIndex = i;
		else
			cols(i).sortIndex = -1;

		cols(i).bReverse = FALSE;
		cols(i).bCaseSensitive = FALSE;
	}

	if (bAutoSort == TRUE)
		Sort();

	unguard;
}

void XListWindow::Sort()
{
	guard(XListWindow::Sort);

	SortRows();
	AskParentForReconfigure();

	unguard;
}

void XListWindow::EnableHotKeys(UBOOL bEnable)
{
	guard(XListWindow::EnableHotKeys);

	// Original only toggles the bit.  It does not clear the accumulated
	// hot-key string here.
	bHotKeys = bEnable;

	unguard;
}

void XListWindow::SetHotKeyColumn(INT ColIndex)
{
	guard(XListWindow::SetHotKeyColumn);

	// Original stores the column index verbatim; validity is checked later by
	// FindRowByKey().
	hotKeyCol = ColIndex;

	unguard;
}

INT XListWindow::FindRowByKey(TCHAR Key)
{
	guard(XListWindow::FindRowByKey);

	if (bHotKeys == FALSE)
		return 0;

	if (IsColumnValid(hotKeyCol) == FALSE)
		return 0;

	if (ReconIsHotKeyChar(Key) == FALSE)
		return 0;

	if (rows.Num() <= 0)
		return 0;

	TCHAR HotKeyTemp[2];
	HotKeyTemp[0] = Key;
	HotKeyTemp[1] = 0;
	hotKeyString += HotKeyTemp;
	hotKeyTimer = 1.0f;

	const TCHAR* SearchText = *hotKeyString;
	if (SearchText == NULL)
		SearchText = TEXT("");

	INT SearchCount = 0;
	while (SearchText[SearchCount] != 0 && SearchText[SearchCount] == SearchText[0])
		SearchCount++;

	if (SearchText[SearchCount] != 0)
	{
		SearchCount = hotKeyString.Len();
	}
	else
	{
		SearchCount = 1;
	}

	INT RowIndex = 0;
	if (focusLine != NULL)
		RowIndex = focusLine->index;

	if (SearchCount > 1)
		RowIndex--;

	for (INT TryIndex = 0; TryIndex < rows.Num(); TryIndex++)
	{
		RowIndex++;
		if (RowIndex >= rows.Num())
			RowIndex = 0;

		XListRowData* Row = rows(RowIndex);
		if (Row == NULL)
			continue;

		if (hotKeyCol >= Row->fieldData.Num())
			continue;

		const TCHAR* Field = ReconSafeString(Row->fieldData(hotKeyCol).field);
		UBOOL bMatch = TRUE;

		for (INT CharIndex = 0; CharIndex < SearchCount; CharIndex++)
		{
			if (Field[CharIndex] == 0)
			{
				bMatch = FALSE;
				break;
			}

			if (ReconToUpper(Field[CharIndex]) != ReconToUpper(SearchText[CharIndex]))
			{
				bMatch = FALSE;
				break;
			}
		}

		if (bMatch == TRUE)
			return (INT)Row;
	}

	return 0;

	unguard;
}

void XListWindow::EnableAutoSort(UBOOL bNewAutoSort)
{
	guard(XListWindow::EnableAutoSort);

	if (bAutoSort != bNewAutoSort)
	{
		bAutoSort = bNewAutoSort;
		if (bAutoSort == TRUE)
			SortRows();
	}

	unguard;
}

void XListWindow::EnableAutoExpandColumns(UBOOL bNewAutoExpand)
{
	guard(XListWindow::EnableAutoExpandColumns);

	if (bAutoExpandColumns != bNewAutoExpand)
	{
		bAutoExpandColumns = bNewAutoExpand;
		if (bAutoExpandColumns == TRUE)
			ResizeColumns(TRUE);
	}

	unguard;
}

void XListWindow::EnableMultiSelect(UBOOL bNewMultiSelect)
{
	guard(XListWindow::EnableMultiSelect);

	if (bMultiSelect == bNewMultiSelect)
		return;

	bMultiSelect = bNewMultiSelect;

	if (bMultiSelect == FALSE && numSelected > 1)
	{
		UBOOL bSelectionChanged = FALSE;
		for (INT i = rows.Num() - 1; i >= 0; i--)
		{
			if (rows(i)->bSelected == TRUE && numSelected > 1)
				ChangeSelectRow(i, FALSE, bSelectionChanged);
		}

		if (bSelectionChanged == TRUE)
			ChangeListSelection();
	}

	unguard;
}

void XListWindow::SetFieldMargins(FLOAT NewMarginWidth, FLOAT NewMarginHeight)
{
	guard(XListWindow::SetFieldMargins);

	if (rowMargin != NewMarginHeight)
	{
		rowMargin = NewMarginHeight;
		ComputeRowSize();
		AskParentForReconfigure();
	}

	if (colMargin != NewMarginWidth)
	{
		colMargin = NewMarginWidth;
		if (bAutoExpandColumns == TRUE)
			ResizeColumns(TRUE);
	}

	unguard;
}

INT XListWindow::GetPageSize()
{
	guard(XListWindow::GetPageSize);

	if (lineSize <= 0.0f)
		return 1;

	INT Count = appFloor(height / lineSize);
	if (Count < 1)
		Count = 1;

	return Count;

	unguard;
}

void XListWindow::SetDelimiter(TCHAR NewDelimiter)
{
	guard(XListWindow::SetDelimiter);

	delimiter = TEXT("");
	TCHAR DelimiterTemp[2];
	DelimiterTemp[0] = NewDelimiter;
	DelimiterTemp[1] = 0;
	delimiter += DelimiterTemp;

	unguard;
}

void XListWindow::SetHighlightTextColor(FColor NewColor)
{
	guard(XListWindow::SetHighlightTextColor);
	inverseColor = NewColor;
	unguard;
}

void XListWindow::SetHighlightTexture(UTexture* NewTexture)
{
	guard(XListWindow::SetHighlightTexture);
	highlightTexture = NewTexture;
	unguard;
}

void XListWindow::SetHighlightColor(FColor NewColor)
{
	guard(XListWindow::SetHighlightColor);
	highlightColor = NewColor;
	unguard;
}

void XListWindow::SetFocusTexture(UTexture* NewTexture)
{
	guard(XListWindow::SetFocusTexture);
	focusTexture = NewTexture;
	unguard;
}

void XListWindow::SetFocusColor(FColor NewColor)
{
	guard(XListWindow::SetFocusColor);
	focusColor = NewColor;
	unguard;
}

void XListWindow::SetFocusThickness(FLOAT NewThickness)
{
	guard(XListWindow::SetFocusThickness);
	focusThickness = NewThickness;
	unguard;
}

void XListWindow::ShowFocusRow()
{
	guard(XListWindow::ShowFocusRow);

	if (focusLine != NULL)
	{
		FLOAT FocusY = (FLOAT)focusLine->index * lineSize;
		AskParentToShowArea(0.0f, FocusY, width, lineSize);
	}

	unguard;
}

void XListWindow::SetListSounds(USound* NewActivateSound, USound* NewMoveSound)
{
	guard(XListWindow::SetListSounds);
	activateSound = NewActivateSound;
	moveSound = NewMoveSound;
	unguard;
}

void XListWindow::PlayListSound(USound* Sound, FLOAT Volume, FLOAT Pitch)
{
	guard(XListWindow::PlayListSound);

	XWindow* Test = this;
	while (Test != NULL && Test->bIsVisible == TRUE)
		Test = Test->GetParent();

	if (Test != NULL)
	{
		PlaySound(Sound, Volume, Pitch);
		return;
	}

	FLOAT SoundX = width * 0.5f;
	FLOAT SoundY = height * 0.5f;

	if (clipRect.clipWidth > 0.0f && clipRect.clipHeight > 0.0f)
	{
		SoundX = clipRect.clipX + clipRect.clipWidth * 0.5f;
		SoundY = clipRect.clipY + clipRect.clipHeight * 0.5f;
	}

	if (focusLine != NULL)
		SoundY = (FLOAT)focusLine->index * lineSize + lineSize * 0.5f;

	PlaySound(Sound, Volume, Pitch, SoundX, SoundY);

	unguard;
}

void XListWindow::ParentRequestedPreferredSize(UBOOL bWidthSpecified, FLOAT& PreferredWidth, UBOOL bHeightSpecified, FLOAT& PreferredHeight)
{
	guard(XListWindow::ParentRequestedPreferredSize);

	PreferredWidth = 0.0f;
	for (INT ColIndex = 0; ColIndex < cols.Num(); ColIndex++)
	{
		if (cols(ColIndex).bHide == FALSE)
			PreferredWidth += cols(ColIndex).colWidth;
	}

	PreferredHeight = (FLOAT)rows.Num() * lineSize;

	unguard;
}

void XListWindow::ParentRequestedGranularity(FLOAT& HGranularity, FLOAT& VGranularity)
{
	guard(XListWindow::ParentRequestedGranularity);

	HGranularity = 1.0f;
	VGranularity = lineSize;
	XWindow::ParentRequestedGranularity(HGranularity, VGranularity);

	unguard;
}

void XListWindow::Draw(XGC* GC)
{
	guard(XListWindow::Draw);

	if (GC == NULL)
		return;

	GC->PushGC();

	INT FirstRow = 0;
	INT LastRow = 0;

	if (lineSize > 0.0f)
	{
		FLOAT VisibleTop = 0.0f;
		if (y < 0.0f)
			VisibleTop = -y;
		FirstRow = appFloor((VisibleTop + 0.00001f) / lineSize);
		FLOAT End = (VisibleTop + height) / lineSize;
		LastRow = appCeil(End);

		if (FirstRow < 0)
			FirstRow = 0;

		if (LastRow > rows.Num())
			LastRow = rows.Num();
	}

	GC->SetStyle(STY_Masked);
	GC->EnableWordWrap(FALSE);
	GC->SetAlignments(HALIGN_Left, VALIGN_Top);
	GC->EnableSpecialText(FALSE);

	GC->SetTileColor(highlightColor);
	for (INT RowIndex = FirstRow; RowIndex < LastRow; RowIndex++)
	{
		XListRowData* Row = rows(RowIndex);
		if (Row != NULL && Row->bSelected == TRUE)
		{
			FLOAT RowY = (FLOAT)RowIndex * lineSize;
			GC->DrawPattern(0.0f, RowY, width, lineSize, 0.0f, 0.0f, highlightTexture);
		}
	}

	FLOAT ColX = 0.0f;
	for (INT ColIndex = 0; ColIndex < cols.Num(); ColIndex++)
	{
		XListColData& Col = cols(ColIndex);
		if (Col.bHide == TRUE)
			continue;

		GC->SetFonts(Col.colFont, Col.colFont);
		FLOAT ColWidth = Col.colWidth;
		FLOAT RowY = (FLOAT)FirstRow * lineSize;

		for (INT RowIndex = FirstRow; RowIndex < LastRow; RowIndex++)
		{
			XListRowData* Row = rows(RowIndex);
			if (Row == NULL)
				continue;

			XListFieldData& Field = Row->fieldData(ColIndex);

			if (Row->bSelected == TRUE)
				GC->SetTextColor(inverseColor);
			else
				GC->SetTextColor(Col.colColor);

			XClipRect SavedClip = GC->clipRect;

			FLOAT InnerX = ColX + colMargin;
			FLOAT InnerY = RowY + rowMargin;
			FLOAT InnerW = ColWidth - colMargin - colMargin;
			FLOAT InnerH = lineSize - rowMargin - rowMargin;

			GC->Intersect(InnerX, InnerY, InnerW, InnerH);

			FLOAT TextX = InnerX;
			if (Col.alignment == HALIGN_Center)
				TextX = ColX + (ColWidth - Field.fieldWidth) * 0.5f;
			else if (Col.alignment == HALIGN_Right)
				TextX = ColX + ColWidth - Field.fieldWidth - colMargin;

			FLOAT TextY = RowY + lineSize - Field.fieldHeight - rowMargin + 1.0f;
			GC->DrawText(TextX, TextY, Field.fieldWidth, Field.fieldHeight, ReconSafeString(Field.field));

			GC->SetClipRect(SavedClip);
			RowY += lineSize;
		}

		ColX += ColWidth;
	}

	if (IsFocusWindow() == TRUE && focusLine != NULL)
	{
		if (focusLine->index >= FirstRow && focusLine->index < LastRow)
		{
			GC->SetTileColor(focusColor);
			FLOAT FocusY = (FLOAT)focusLine->index * lineSize;
			GC->DrawBox(0.0f, FocusY, width, lineSize, 0.0f, 0.0f, focusThickness, focusTexture);
		}
	}

	GC->PopGC();
	XWindow::Draw(GC);

	unguard;
}

UBOOL XListWindow::MouseButtonPressed(FLOAT PointX, FLOAT PointY, EInputKey Button, INT NumClicks)
{
	guard(XListWindow::MouseButtonPressed);

	struct
	{
		FLOAT pointX;
		FLOAT pointY;
		EInputKey button;
		INT numClicks;
		UBOOL bResult;
	} Params;

	Params.pointX = PointX;
	Params.pointY = PointY;
	Params.button = Button;
	Params.numClicks = NumClicks;
	Params.bResult = FALSE;
	ProcessScript(EXTENSION_MouseButtonPressed, &Params, FALSE);

	if (Button == IK_LeftMouse)
	{
		if (lineSize > 0.0f && rows.Num() > 0)
		{
			INT RowIndex = appFloor(PointY / lineSize);
			if (RowIndex > rows.Num() - 1)
				RowIndex = rows.Num() - 1;

			if (RowIndex < 0)
				RowIndex = 0;

			UBOOL bShift = IsKeyDown(IK_Shift);
			UBOOL bCtrl = IsKeyDown(IK_Ctrl);

			MoveToRow(rows(RowIndex), TRUE, bCtrl == FALSE, bCtrl, bShift, TRUE);

			bDragging = TRUE;
			lastIndex = RowIndex;
			remainingDelay = GetTickOffset() + 0.1f;
		}

		return TRUE;
	}

	return Params.bResult;

	unguard;
}

void XListWindow::MouseMoved(FLOAT PointX, FLOAT PointY)
{
	guard(XListWindow::MouseMoved);

	struct
	{
		FLOAT pointX;
		FLOAT pointY;
	} Params;

	Params.pointX = PointX;
	Params.pointY = PointY;
	ProcessScript(EXTENSION_MouseMoved, &Params, FALSE);

	if (bDragging == TRUE && lineSize > 0.0f && rows.Num() > 0)
	{
		INT RowIndex = appFloor(PointY / lineSize);
		if (RowIndex > rows.Num() - 1)
			RowIndex = rows.Num() - 1;

		if (RowIndex < 0)
			RowIndex = 0;

		if (RowIndex != lastIndex)
		{
			lastIndex = RowIndex;
			MoveToRow(rows(RowIndex), TRUE, FALSE, FALSE, TRUE, TRUE);
		}
	}

	unguard;
}

UBOOL XListWindow::MouseButtonReleased(FLOAT PointX, FLOAT PointY, EInputKey Button, INT NumClicks)
{
	guard(XListWindow::MouseButtonReleased);

	struct
	{
		FLOAT pointX;
		FLOAT pointY;
		EInputKey button;
		INT numClicks;
		UBOOL bResult;
	} Params;

	Params.pointX = PointX;
	Params.pointY = PointY;
	Params.button = Button;
	Params.numClicks = NumClicks;
	Params.bResult = FALSE;
	ProcessScript(EXTENSION_MouseButtonReleased, &Params, FALSE);

	if (Button == IK_LeftMouse)
	{
		bDragging = FALSE;

		if (NumClicks > 1 && lineSize > 0.0f && rows.Num() > 0)
		{
			INT RowIndex = appFloor(PointY / lineSize);
			if (RowIndex >= 0 && RowIndex < rows.Num())
			{
				SetFocusRow((INT)rows(RowIndex), TRUE, TRUE);
				PlayListSound(activateSound, -1.0f, 1.0f);
				ActivateListRow();
			}
		}

		return TRUE;
	}

	return Params.bResult;

	unguard;
}

UBOOL XListWindow::KeyPressed(TCHAR Key)
{
	guard(XListWindow::KeyPressed);

	XReconListKeyPressedParams Params;

	Params.key = FString::Printf(TEXT("%c"), Key);
	Params.bResult = FALSE;
	ProcessScript(EXTENSION_KeyPressed, &Params, FALSE);

	if (ReconIsHotKeyChar(Key) == TRUE && bHotKeys == TRUE)
	{
		INT RowId = FindRowByKey(Key);
		if (RowId != 0)
			SetRow(RowId, TRUE, TRUE, FALSE);

		return TRUE;
	}

	return Params.bResult;

	unguard;
}

UBOOL XListWindow::VirtualKeyPressed(EInputKey Key, UBOOL bRepeat)
{
	guard(XListWindow::VirtualKeyPressed);

	struct
	{
		EInputKey key;
		UBOOL bRepeat;
		UBOOL bResult;
	} Params;

	Params.key = Key;
	Params.bRepeat = bRepeat;
	Params.bResult = FALSE;
	ProcessScript(EXTENSION_VirtualKeyPressed, &Params, FALSE);

	if (Key == IK_Left || Key == IK_Right || Key == IK_Up || Key == IK_Down || Key == IK_Escape)
		ClearHotKeyString();

	if (Key == IK_Enter)
	{
		PlayListSound(activateSound, -1.0f, 1.0f);
		ActivateListRow();
		return TRUE;
	}

	return Params.bResult;

	unguard;
}

void XListWindow::VisibilityChanged(UBOOL bVisible)
{
	guard(XListWindow::VisibilityChanged);

	ProcessScript(EXTENSION_VisibilityChanged, &bVisible, FALSE);

	if (bVisible == TRUE && focusLine == NULL && rows.Num() > 0)
		SetRow((INT)rows(0), TRUE, TRUE, FALSE);

	unguard;
}

void XListWindow::Tick(FLOAT DeltaSeconds)
{
	guard(XListWindow::Tick);

	if (bDragging == TRUE)
	{
		remainingDelay -= DeltaSeconds;
		if (remainingDelay < 0.0f)
		{
			remainingDelay = 0.1f;
			FLOAT MouseX = 0.0f;
			FLOAT MouseY = 0.0f;
			GetCursorPos(&MouseX, &MouseY);
			MouseMoved(MouseX, MouseY);
		}
	}

	if (hotKeyTimer > 0.0f)
	{
		hotKeyTimer -= DeltaSeconds;
		if (hotKeyTimer <= 0.0f)
			ClearHotKeyString();
	}

	unguard;
}

void XListWindow::InitColumn(XListColData& Col, INT Index)
{
	guard(XListWindow::InitColumn);

	Col.index = Index;
	Col.title = TEXT("");
	Col.sortIndex = 0;
	Col.bReverse = FALSE;
	Col.bCaseSensitive = FALSE;
	Col.bHide = FALSE;
	Col.colWidth = colMargin + colMargin + 20.0f;
	Col.alignment = HALIGN_Left;
	Col.colColor = textColor;
	Col.colFont = normalFont;
	Col.colType = COLTYPE_String;
	Col.colFmt = TEXT("");

	unguard;
}

void XListWindow::RecomputeIndices()
{
	guard(XListWindow::RecomputeIndices);

	for (INT i = 0; i < rows.Num(); i++)
		rows(i)->index = i;

	unguard;
}

UBOOL XListWindow::IsHotKeyValid(TCHAR Key)
{
	guard(XListWindow::IsHotKeyValid);
	return ReconIsHotKeyChar(Key);
	unguard;
}

void XListWindow::ClearHotKeyString()
{
	guard(XListWindow::ClearHotKeyString);
	hotKeyString = TEXT("");
	hotKeyTimer = 0.0f;
	unguard;
}

FLOAT XListWindow::StringToFloat(const TCHAR* Text)
{
	guard(XListWindow::StringToFloat);

	// Original accepts decimal, hexadecimal after 0x, octal-like zero-prefix,
	// signs, whitespace, and time/angle separators. A single quote contributes
	// hours*3600; colon or double quote contributes minutes*60.
	if (Text == NULL)
		return 0.0f;

	FLOAT Total = 0.0f;
	FLOAT Current = 0.0f;
	FLOAT Sign = 1.0f;
	FLOAT Base = 10.0f;
	FLOAT FractionScale = 1.0f;
	UBOOL bFraction = FALSE;
	UBOOL bAllowSign = TRUE;
	UBOOL bAllowBasePrefix = TRUE;
	UBOOL bAllowWhitespace = TRUE;

	const TCHAR* Ch = Text;
	while (*Ch != 0)
	{
		TCHAR C = *Ch;
		FLOAT Digit = -1.0f;

		if (C >= TEXT('0') && C <= TEXT('9'))
			Digit = (FLOAT)(C - TEXT('0'));
		else if (C >= TEXT('a') && C <= TEXT('z'))
			Digit = (FLOAT)(C - TEXT('a') + 10);
		else if (C >= TEXT('A') && C <= TEXT('Z'))
			Digit = (FLOAT)(C - TEXT('A') + 10);

		if (Digit >= 0.0f)
		{
			if (C == TEXT('0') && bAllowBasePrefix == TRUE)
			{
				Base = 8.0f;
				if (Ch[1] == TEXT('x') || Ch[1] == TEXT('X'))
				{
					Ch++;
					Base = 16.0f;
				}
			}
			else
			{
				if (Digit >= Base)
					break;

				if (bFraction == TRUE)
				{
					FractionScale *= Base;
					Current += Digit / FractionScale;
				}
				else
				{
					Current = Current * Base + Digit;
				}
			}

			bAllowSign = FALSE;
			bAllowBasePrefix = FALSE;
			bAllowWhitespace = FALSE;
		}
		else if ((C == TEXT('+') || C == TEXT('-')) && bAllowSign == TRUE)
		{
			if (C == TEXT('-'))
				Sign = -1.0f;

			bAllowSign = FALSE;
			bAllowBasePrefix = FALSE;
		}
		else if ((C == TEXT('.') || C == TEXT(',')))
		{
			bFraction = TRUE;
			bAllowSign = FALSE;
			bAllowBasePrefix = FALSE;
			bAllowWhitespace = FALSE;
		}
		else if (C == TEXT('\''))
		{
			Total += Current * 3600.0f * Sign;
			Current = 0.0f;
			bFraction = FALSE;
			FractionScale = 1.0f;
			bAllowSign = FALSE;
			bAllowBasePrefix = FALSE;
			bAllowWhitespace = TRUE;
		}
		else if (C == TEXT(':') || C == TEXT('"'))
		{
			Total += Current * 60.0f * Sign;
			Current = 0.0f;
			bFraction = FALSE;
			FractionScale = 1.0f;
			bAllowSign = FALSE;
			bAllowBasePrefix = FALSE;
			bAllowWhitespace = TRUE;
		}
		else if ((C == TEXT(' ') || C == TEXT('\t') || C == TEXT('\r') || C == TEXT('\n')) && bAllowWhitespace == TRUE)
		{
			// eat leading or separator whitespace
		}
		else
		{
			break;
		}

		Ch++;
	}

	return Total + Current * Sign;

	unguard;
}

FLOAT XListWindow::FieldConvertToValue(BYTE ColType, const TCHAR* Text)
{
	guard(XListWindow::FieldConvertToValue);

	if (ColType == COLTYPE_Float || ColType == COLTYPE_Time)
		return StringToFloat(Text);

	return 0.0f;

	unguard;
}

const TCHAR* XListWindow::FieldConvertToString(BYTE ColType, const TCHAR* Fmt, FLOAT Value)
{
	guard(XListWindow::FieldConvertToString);

	if (Fmt == NULL)
		Fmt = TEXT("");

	if (ColType == COLTYPE_Time)
	{
		INT TotalSeconds = appFloor(Value);
		INT Hours = TotalSeconds / 3600;
		INT Minutes = (TotalSeconds / 60) % 60;
		GReconFieldConvertStringBuffer = FString::Printf(Fmt, Hours, Minutes);
		return *GReconFieldConvertStringBuffer;
	}

	if (Fmt[0] != 0)
	{
		GReconFieldConvertStringBuffer = FString::Printf(Fmt, Value);
		return *GReconFieldConvertStringBuffer;
	}

	GReconFieldConvertStringBuffer = FString::Printf(TEXT("%f"), Value);
	return *GReconFieldConvertStringBuffer;

	unguard;
}

void XListWindow::SetFieldByString(XListRowData* Row, INT ColIndex, const TCHAR* Text)
{
	guard(XListWindow::SetFieldByString);

	if (Text == NULL)
		Text = TEXT("");

	XListColData& Col = cols(ColIndex);
	XListFieldData& Field = Row->fieldData(ColIndex);

	if (Col.colType != COLTYPE_String)
	{
		Field.fieldValue = FieldConvertToValue(Col.colType, Text);
		Field.field = FieldConvertToString(Col.colType, ReconSafeString(Col.colFmt), Field.fieldValue);
	}
	else
	{
		Field.fieldValue = 0.0f;
		Field.field = Text;
	}

	unguard;
}

void XListWindow::SetFieldByValue(XListRowData* Row, INT ColIndex, FLOAT Value)
{
	guard(XListWindow::SetFieldByValue);

	XListColData& Col = cols(ColIndex);
	XListFieldData& Field = Row->fieldData(ColIndex);

	Field.field = FieldConvertToString(Col.colType, ReconSafeString(Col.colFmt), Value);

	if (Col.colType != COLTYPE_String)
		Field.fieldValue = Value;
	else
		Field.fieldValue = 0.0f;

	unguard;
}

void XListWindow::ComputeFieldSize(XGC* GC, XListRowData* Row, INT ColIndex)
{
	guard(XListWindow::ComputeFieldSize);

	if (Row == NULL)
		return;

	if (GC == NULL)
	{
		Row->fieldData(ColIndex).fieldWidth = 0.0f;
		Row->fieldData(ColIndex).fieldHeight = 0.0f;
		return;
	}

	XListColData& Col = cols(ColIndex);
	XListFieldData& Field = Row->fieldData(ColIndex);

	GC->EnableSpecialText(FALSE);

	UFont* OldNormal = GC->normalFont;
	UFont* OldBold = GC->boldFont;

	if (Col.colFont != NULL)
		GC->SetFonts(Col.colFont, Col.colFont);

	GC->GetTextExtent(0.0f, Field.fieldWidth, Field.fieldHeight, ReconSafeString(Field.field));

	GC->SetFonts(OldNormal, OldBold);

	unguard;
}

UBOOL XListWindow::ExpandColumnByField(XListRowData* Row, INT ColIndex)
{
	guard(XListWindow::ExpandColumnByField);

	if (Row == NULL)
		return FALSE;

	FLOAT NewWidth = colMargin + colMargin + Row->fieldData(ColIndex).fieldWidth;
	if (NewWidth <= cols(ColIndex).colWidth)
		return FALSE;

	cols(ColIndex).colWidth = NewWidth;
	return TRUE;

	unguard;
}

void XListWindow::ComputeRowSize()
{
	guard(XListWindow::ComputeRowSize);

	lineSize = 0.0f;

	for (INT ColIndex = 0; ColIndex < cols.Num(); ColIndex++)
	{
		UFont* Font = cols(ColIndex).colFont;
		if (Font == NULL)
			continue;

		INT CharW = 0;
		INT CharH = 0;
		XGC::GetCharSize(Font, TEXT(' '), &CharW, &CharH);

		if ((FLOAT)CharH > lineSize)
			lineSize = (FLOAT)CharH;
	}

	lineSize = rowMargin + rowMargin + lineSize;

	unguard;
}

void XListWindow::ExtendRow(XListRowData* Row)
{
	guard(XListWindow::ExtendRow);

	if (Row == NULL)
		return;

	if (Row->fieldData.Num() < cols.Num())
	{
		INT AddCount = cols.Num() - Row->fieldData.Num();
		Row->fieldData.AddZeroed(AddCount);
	}
	else if (Row->fieldData.Num() > cols.Num())
	{
		if (cols.Num() == 0)
			Row->fieldData.Empty();
		else
			Row->fieldData.Remove(cols.Num(), Row->fieldData.Num() - cols.Num());
	}

	unguard;
}

UBOOL XListWindow::FillRow(XListRowData* Row, const TCHAR* LineStr)
{
	guard(XListWindow::FillRow);

	UBOOL bExpanded = FALSE;
	XGC* GC = GetGC();

	ExtendRow(Row);

	const TCHAR* Cursor = LineStr;
	if (Cursor == NULL)
		Cursor = TEXT("");

	for (INT ColIndex = 0; ColIndex < cols.Num(); ColIndex++)
	{
		TCHAR FieldBuffer[2048];
		INT CharCount = 0;

		while (*Cursor != 0)
		{
			TCHAR Delim = 0;
			if (delimiter.Len() > 0)
				Delim = (*delimiter)[0];

			if (*Cursor == Delim)
				break;

			if (CharCount < 2047)
			{
				FieldBuffer[CharCount] = *Cursor;
				CharCount++;
			}

			Cursor++;
		}

		if (*Cursor != 0)
			Cursor++;

		FieldBuffer[CharCount] = 0;

		SetFieldByString(Row, ColIndex, FieldBuffer);
		ComputeFieldSize(GC, Row, ColIndex);

		if (bAutoExpandColumns == TRUE)
		{
			if (ExpandColumnByField(Row, ColIndex) == TRUE)
				bExpanded = TRUE;
		}
	}

	ReleaseGC(GC);
	return bExpanded;

	unguard;
}

XListRowData* XListWindow::CreateRow(const TCHAR* LineStr, INT ClientData)
{
	guard(XListWindow::CreateRow);

	XListRowData* Row = new XListRowData;
	Row->index = -1;
	Row->bSelected = FALSE;
	Row->clientData = ClientData;
	Row->fieldData.Empty();
	Row->fieldData.AddZeroed(cols.Num());

	FillRow(Row, LineStr);
	return Row;

	unguard;
}

void XListWindow::SetupSortedColData()
{
	guard(XListWindow::SetupSortedColData);

	GReconSortColumnCount = 0;

	for (INT ColIndex = 0; ColIndex < cols.Num(); ColIndex++)
	{
		if (cols(ColIndex).sortIndex >= 0)
		{
			INT InsertAt = GReconSortColumnCount;
			while (InsertAt > 0)
			{
				if (cols(ColIndex).sortIndex <= GReconSortColumns[InsertAt - 1]->sortIndex)
					break;

				GReconSortColumns[InsertAt] = GReconSortColumns[InsertAt - 1];
				InsertAt--;
			}

			GReconSortColumns[InsertAt] = &cols(ColIndex);
			GReconSortColumnCount++;

			if (GReconSortColumnCount >= RECON_MAX_SORT_COLS)
				break;
		}
	}

	unguard;
}

UBOOL XListWindow::FindRowData(XListRowData* Row, INT* InsertIndex)
{
	guard(XListWindow::FindRowData);

	SetupSortedColData();

	INT Low = 0;
	INT High = rows.Num();
	UBOOL bFound = FALSE;

	while (Low != High)
	{
		INT Mid = (Low + High) >> 1;
		INT Compare = ReconCompareRows(rows(Mid), Row);

		if (Compare == 0)
		{
			bFound = TRUE;
			break;
		}

		if (Compare <= 0)
			Low = Mid + 1;
		else
			High = Mid;
	}

	if (InsertIndex != NULL)
		*InsertIndex = (Low + High) >> 1;

	return bFound;

	unguard;
}

void XListWindow::SortRows()
{
	guard(XListWindow::SortRows);

	SetupSortedColData();

	if (rows.Num() > 1)
		appQsort(&rows(0), rows.Num(), sizeof(XListRowData*), ReconCompareRowPointers);

	RecomputeIndices();

	unguard;
}

void XListWindow::CheckPosition(XListRowData* Row)
{
	guard(XListWindow::CheckPosition);

	if (Row == NULL)
		return;

	if (bAutoSort == FALSE)
		return;

	SetupSortedColData();

	UBOOL bNeedsMove = FALSE;

	if (Row->index > 0)
	{
		if (ReconCompareRows(rows(Row->index - 1), Row) >= 0)
			bNeedsMove = TRUE;
	}

	if (Row->index < rows.Num() - 1)
	{
		if (ReconCompareRows(Row, rows(Row->index + 1)) <= 0)
			bNeedsMove = TRUE;
	}

	if (bNeedsMove == TRUE)
	{
		RemoveRow(Row);
		InsertRow(Row);
	}

	unguard;
}

void XListWindow::InsertRow(XListRowData* Row)
{
	guard(XListWindow::InsertRow);

	if (Row == NULL)
		return;

	if (bAutoSort == TRUE)
	{
		INT InsertAt = 0;
		UBOOL bFound = FindRowData(Row, &InsertAt);
		if (bFound == TRUE)
			InsertAt++;

		rows.Insert(InsertAt, 1);
		rows(InsertAt) = Row;

		for (INT i = InsertAt; i < rows.Num(); i++)
			rows(i)->index = i;
	}
	else
	{
		Row->index = rows.Num();
		rows.AddItem(Row);
	}

	unguard;
}

void XListWindow::RemoveRow(XListRowData* Row)
{
	guard(XListWindow::RemoveRow);

	if (Row == NULL)
		return;

	INT Index = Row->index;
	if (Index < 0)
		return;

	if (Index >= rows.Num())
		return;

	rows.Remove(Index, 1);

	for (INT i = Index; i < rows.Num(); i++)
		rows(i)->index = i;

	Row->index = -1;

	unguard;
}

void XListWindow::ChangeSelectRow(INT Index, UBOOL bSelected, UBOOL& bChanged)
{
	guard(XListWindow::ChangeSelectRow);

	if (Index < 0 || Index >= rows.Num())
		return;

	XListRowData* Row = rows(Index);
	if (Row->bSelected != bSelected)
	{
		Row->bSelected = bSelected;

		if (bSelected == TRUE)
			numSelected++;
		else
			numSelected--;

		bChanged = TRUE;
	}

	unguard;
}

void XListWindow::MoveToRow(XListRowData* Row, UBOOL bSelect, UBOOL bClearRows, UBOOL bInvert, UBOOL bDrag, UBOOL bMoveFocus)
{
	guard(XListWindow::MoveToRow);

	XListRowData* OldFocus = focusLine;
	UBOOL bSelectionChanged = FALSE;

	if (bMultiSelect == FALSE)
	{
		bClearRows = TRUE;
		bDrag = FALSE;
	}

	INT TargetIndex = 0;
	if (Row != NULL)
		TargetIndex = Row->index;

	if (bSelect == TRUE)
	{
		INT AnchorIndex = 0;
		if (focusLine != NULL)
			AnchorIndex = focusLine->index;

		if (anchorLine != NULL)
			AnchorIndex = anchorLine->index;

		if (bClearRows == TRUE)
		{
			for (INT i = 0; i < rows.Num(); i++)
				ChangeSelectRow(i, FALSE, bSelectionChanged);
		}
		else if (bDrag == TRUE)
		{
			INT OldIndex = 0;
			if (OldFocus != NULL)
				OldIndex = OldFocus->index;

			if (AnchorIndex >= OldIndex)
			{
				for (INT i = OldIndex; i <= AnchorIndex; i++)
					ChangeSelectRow(i, FALSE, bSelectionChanged);
			}
			else
			{
				for (INT i = AnchorIndex; i <= OldIndex; i++)
					ChangeSelectRow(i, FALSE, bSelectionChanged);
			}
		}

		if (bDrag == FALSE)
		{
			if (bInvert == TRUE)
			{
				if (Row != NULL)
					ChangeSelectRow(TargetIndex, Row->bSelected == FALSE, bSelectionChanged);
			}
			else
			{
				if (Row != NULL)
					ChangeSelectRow(TargetIndex, TRUE, bSelectionChanged);
			}
		}
		else
		{
			if (AnchorIndex >= TargetIndex)
			{
				for (INT i = TargetIndex; i <= AnchorIndex; i++)
					ChangeSelectRow(i, TRUE, bSelectionChanged);
			}
			else
			{
				for (INT i = AnchorIndex; i <= TargetIndex; i++)
					ChangeSelectRow(i, TRUE, bSelectionChanged);
			}
		}
	}

	if (bMoveFocus == TRUE && TargetIndex >= 0 && TargetIndex < rows.Num())
		SetFocusRow((INT)rows(TargetIndex), TRUE, bDrag == FALSE);

	if (bSelectionChanged == TRUE)
		ChangeListSelection();

	if (focusLine != OldFocus && focusLine != NULL)
		PlayListSound(moveSound, -1.0f, 1.0f);

	unguard;
}

void XListWindow::ActivateListRow()
{
	guard(XListWindow::ActivateListRow);

	XWindow* Locked = this;
	while (Locked != NULL && Locked->IsLocked() == FALSE)
		Locked = Locked->GetParent();

	if (Locked != NULL)
		return;

	for (XWindow* Test = this; Test != NULL; Test = Test->GetParent())
	{
		if (Test->ListRowActivated(this, (INT)focusLine) == TRUE)
			break;
	}

	unguard;
}

void XListWindow::ChangeListSelection()
{
	guard(XListWindow::ChangeListSelection);

	for (XWindow* Test = this; Test != NULL; Test = Test->GetParent())
	{
		if (Test->ListSelectionChanged(this, numSelected, (INT)focusLine) == TRUE)
			break;
	}

	unguard;
}

UBOOL XListWindow::IsColumnValid(INT ColIndex)
{
	guard(XListWindow::IsColumnValid);
	return ColIndex >= 0 && ColIndex < cols.Num();
	unguard;
}

/*
Native wrappers in the original are thin P_GET_* bridges for native ids 1720..1812.
They do not contain additional behavior beyond parameter coercion and optional
parameter defaults. Reconstruction keeps the behavioral layer above, and the
compile pass should regenerate wrappers mechanically from ListWindow.uc.
*/

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
