/*=============================================================================
	XEditWindow_Reconstructed.cpp
	Deus Ex Extension.dll reconstruction notes/source pass.

	This file is reconstruction-first, not a final compile-ready replacement.
	The goal is to preserve the original Extension.dll behaviour model closely:
	caret movement, selection, word navigation, filtering, undo/redo, text buffer
	mutation, drawing and input routing.

	Original evidence used in this pass:
	- ExtEdit.h / EditWindow.uc
	- Extension.dll.c XEditWindow block, especially 1001CEA0..10021A30
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XEditWindow);

/*-----------------------------------------------------------------------------
	Local reconstruction helpers.
-----------------------------------------------------------------------------*/

static const TCHAR* ReconEmptyText()
{
	return TEXT("");
}

static FString GExtensionTextBuffer;


struct XReconEditCursorRequestedParms
{
	XWindow* win;
	FLOAT pointX;
	FLOAT pointY;
	FLOAT hotX;
	FLOAT hotY;
	FColor newColor;
	UTexture** pCursorShadow;
	XCursor* returnValue;
};


static void ReconAssignSubstring(FString& Dest, const TCHAR* Source, INT Count)
{
	guard(ReconAssignSubstring);

	if (Source == NULL)
		Source = ReconEmptyText();

	if (Count <= 0)
	{
		Dest = TEXT("");
		return;
	}

	TCHAR* Buffer = new TCHAR[Count + 1];
	for (INT Index = 0; Index < Count; Index++)
		Buffer[Index] = Source[Index];
	Buffer[Count] = 0;

	Dest = Buffer;
	delete [] Buffer;

	unguard;
}

static UBOOL ReconIsSpaceForEditWord(TCHAR ch)
{
	UBOOL bResult = FALSE;

	if (ch == TEXT(' '))
		bResult = TRUE;

	if ((ch >= 9) && (ch <= 13))
		bResult = TRUE;

	return bResult;
}

static INT ReconClampInt(INT value, INT minValue, INT maxValue)
{
	INT result = value;

	if (result < minValue)
		result = minValue;

	if (result > maxValue)
		result = maxValue;

	return result;
}




/*-----------------------------------------------------------------------------
	Construction / destruction.
-----------------------------------------------------------------------------*/

XEditWindow::XEditWindow(XWindow* parent)
: XLargeTextWindow(parent)
{
	guard(XEditWindow::XEditWindow_Reconstructed);
	unguard;
}

void XEditWindow::Init(XWindow* parent)
{
	guard(XEditWindow::Init_Reconstructed);

	XLargeTextWindow::Init(parent);

	// Original default bitfield is 0b001:
	// editable on, single-line off, uppercase-only off.
	bEditable      = TRUE;
	bSingleLine    = FALSE;
	bUppercaseOnly = FALSE;

	insertPos     = 0;
	insertHookPos = 0;
	insertType    = INSTYPE_Insert;

	selectStart = -1;
	selectEnd   = -1;
	maxSize     = 200000;

	// Defaults seen in constructor/decompiled Init and defaultproperties.
	insertTexture = Cast<UTexture>(StaticLoadObject(UTexture::StaticClass(), NULL, TEXT("Extension.Solid"), NULL, LOAD_NoWarn, NULL));
	selectTexture = Cast<UTexture>(StaticLoadObject(UTexture::StaticClass(), NULL, TEXT("Extension.Solid"), NULL, LOAD_NoWarn, NULL));

	insertColor  = FColor(255, 255, 255);
	selectColor  = FColor(192, 192, 192);
	inverseColor = FColor(0, 0, 0);

	editCursor       = NULL;
	editCursorShadow = NULL;
	editCursorColor  = FColor(255, 255, 255);

	typeSound   = NULL;
	deleteSound = NULL;
	enterSound  = NULL;
	moveSound   = NULL;

	bufferList.Empty();
	currentUndo   = 0;
	maxUndos      = -1;
	unchangedUndo = 0;

	dragDelay    = 0.0f;
	blinkStart   = 0.75f;
	blinkPeriod  = 1.0f;
	blinkDelay   = XWindow::GetTickOffset() + blinkStart;
	bInsertShowing = TRUE;

	bDragging    = FALSE;
	bSelectWords = FALSE;

	insertX          = 0.0f;
	insertY          = 0.0f;
	insertWidth      = 2.0f;
	insertHeight     = lineHeight;
	insertPrefWidth  = 0.0f;
	insertPrefHeight = 0.0f;

	showAreaX      = 0.0f;
	showAreaY      = 0.0f;
	showAreaWidth  = 0.0f;
	showAreaHeight = 0.0f;

	insertPreferredCol = 0.0f;

	lastConfigWidth  = 0.0f;
	lastConfigHeight = 0.0f;

	selectStartRow = -1;
	selectEndRow   = -1;
	selectStartX   = 0.0f;
	selectEndX     = 0.0f;

	// Text editing disables GC special text parsing. Otherwise the user could
	// type |c, |b, |n style control runs and the edit geometry would no longer
	// match the raw text buffer.
	EnableSpecialText(FALSE);
	SetSelectability(TRUE);

	// Edit controls in Deus Ex are usually hosted by tiny ClipWindow viewports
	// (MenuUIEditWindow has maxHeight=11). The base TextWindow margins are too
	// large for this path, so native EditWindow starts with a raw text client rect.
	hAlign = HALIGN_Left;
	vAlign = VALIGN_Top;
	hMargin = 0.0f;
	vMargin = 0.0f;

	ComputeCursorConfig(TRUE);

	unguard;
}

void XEditWindow::CleanUp(void)
{
	guard(XEditWindow::CleanUp_Reconstructed);

	for (INT iUndo = 0; iUndo < bufferList.Num(); iUndo++)
	{
		bufferList(iUndo).oldString.Empty();
		bufferList(iUndo).newString.Empty();
	}

	bufferList.Empty();
	currentUndo   = 0;
	unchangedUndo = 0;

	XLargeTextWindow::CleanUp();

	unguard;
}

void XEditWindow::Serialize(FArchive& Ar)
{
	guard(XEditWindow::Serialize_Reconstructed);
	XLargeTextWindow::Serialize(Ar);
	unguard;
}

/*-----------------------------------------------------------------------------
	Public edit state.
-----------------------------------------------------------------------------*/

void XEditWindow::EnableEditing(UBOOL bEdit)
{
	guard(XEditWindow::EnableEditing_Reconstructed);
	bEditable = bEdit;
	unguard;
}

void XEditWindow::EnableSingleLineEditing(UBOOL bNewSingleLine)
{
	guard(XEditWindow::EnableSingleLineEditing_Reconstructed);

	if (bSingleLine != (BITFIELD)bNewSingleLine)
	{
		bSingleLine = bNewSingleLine;

		if (bSingleLine == TRUE)
		{
			// Original rewrites the current text through SetText() and disables
			// word wrapping. The rewrite is important because FilterString()
			// removes newline characters while single-line mode is enabled.
			FString currentText = text;
			SetText(*currentText);
			SetWordWrap(FALSE);
		}
		else
		{
			SetWordWrap(TRUE);
		}
	}

	unguard;
}

void XEditWindow::EnableUppercaseOnly(UBOOL bNewUppercaseOnly)
{
	guard(XEditWindow::EnableUppercaseOnly_Reconstructed);

	if (bUppercaseOnly != (BITFIELD)bNewUppercaseOnly)
	{
		bUppercaseOnly = bNewUppercaseOnly;

		if (bUppercaseOnly == TRUE)
		{
			FString currentText = text;
			SetText(*currentText);
		}
	}

	unguard;
}

void XEditWindow::ClearTextChangedFlag(void)
{
	guard(XEditWindow::ClearTextChangedFlag_Reconstructed);
	SetTextChangedFlag(FALSE);
	unguard;
}

void XEditWindow::SetTextChangedFlag(UBOOL bTextChanged)
{
	guard(XEditWindow::SetTextChangedFlag_Reconstructed);

	if (bTextChanged == TRUE)
		unchangedUndo = -1;
	else
		unchangedUndo = currentUndo;

	unguard;
}

void XEditWindow::SetMaxSize(INT newMaxSize)
{
	guard(XEditWindow::SetMaxSize_Reconstructed);

	if (maxSize != newMaxSize)
	{
		maxSize = newMaxSize;

		if (maxSize > 0)
		{
			INT visibleLen = text.Len();

			if (visibleLen > maxSize)
			{
				selectStart = maxSize;
				selectEnd   = visibleLen;
				insertPos   = visibleLen;

				ReplaceText(NULL, FALSE, FALSE);
				SetInsertionPoint(0, FALSE);
			}
		}
	}

	unguard;
}

void XEditWindow::SetMaxUndos(INT newMaxUndos)
{
	guard(XEditWindow::SetMaxUndos_Reconstructed);

	if (newMaxUndos < 0)
		newMaxUndos = -1;

	if (maxUndos != newMaxUndos)
	{
		maxUndos = newMaxUndos;
		ClampUndo();
	}

	unguard;
}

void XEditWindow::SetInsertionPointBlinkRate(FLOAT newBlinkStart, FLOAT newBlinkPeriod)
{
	guard(XEditWindow::SetInsertionPointBlinkRate_Reconstructed);
	blinkStart  = newBlinkStart;
	blinkPeriod = newBlinkPeriod;
	unguard;
}

void XEditWindow::SetInsertionPointTexture(UTexture* newTexture, FColor newColor)
{
	guard(XEditWindow::SetInsertionPointTexture_Reconstructed);
	insertTexture = newTexture;
	insertColor   = newColor;
	ComputeCursorConfig(FALSE);
	unguard;
}

void XEditWindow::SetInsertionPointType(EInsertionPointType newType, FLOAT prefWidth, FLOAT prefHeight)
{
	guard(XEditWindow::SetInsertionPointType_Reconstructed);
	insertType       = newType;
	insertPrefWidth  = prefWidth;
	insertPrefHeight = prefHeight;
	ComputeCursorConfig(FALSE);
	unguard;
}

void XEditWindow::SetSelectedAreaTexture(UTexture* newTexture, FColor newColor)
{
	guard(XEditWindow::SetSelectedAreaTexture_Reconstructed);
	selectTexture = newTexture;
	selectColor   = newColor;
	unguard;
}

void XEditWindow::SetSelectedAreaTextColor(FColor newColor)
{
	guard(XEditWindow::SetSelectedAreaTextColor_Reconstructed);
	inverseColor = newColor;
	unguard;
}

void XEditWindow::SetEditCursor(XCursor* newCursor, UTexture* newCursorShadow, FColor newColor)
{
	guard(XEditWindow::SetEditCursor_Reconstructed);
	editCursor       = newCursor;
	editCursorShadow = newCursorShadow;
	editCursorColor  = newColor;
	unguard;
}

void XEditWindow::SetEditSounds(USound* newTypeSound, USound* newDeleteSound, USound* newEnterSound, USound* newMoveSound)
{
	guard(XEditWindow::SetEditSounds_Reconstructed);
	typeSound   = newTypeSound;
	deleteSound = newDeleteSound;
	enterSound  = newEnterSound;
	moveSound   = newMoveSound;
	unguard;
}

void XEditWindow::PlayEditSound(USound* sound, FLOAT volume, FLOAT pitch)
{
	guard(XEditWindow::PlayEditSound_Reconstructed);

	// Original plays from the centre of the current caret show-area, not from
	// the window centre. That makes long scrolling edit controls sound like the
	// active cursor, not like a generic widget.
	FLOAT soundX = showAreaX + showAreaWidth * 0.5f;
	FLOAT soundY = showAreaY + showAreaHeight * 0.5f;
	PlaySound(sound, volume, pitch, soundX, soundY);

	unguard;
}

/*-----------------------------------------------------------------------------
	Caret and selection.
-----------------------------------------------------------------------------*/

void XEditWindow::MoveInsertionPoint(EMoveInsert moveInsert, UBOOL bDrag)
{
	guard(XEditWindow::MoveInsertionPoint_Reconstructed);

	INT currentRow = 0;
	INT currentCol = 0;
	INT pageRows   = 1;

	if (rowData.Num() > 0)
	{
		PosToRowCol(insertPos, &currentRow, &currentCol);

		FLOAT rowStep = lineHeight + vSpace;
		if (rowStep > 0.0f)
		{
			pageRows = appFloor(height / rowStep);
			if (pageRows < 1)
				pageRows = 1;
		}
	}

	switch (moveInsert)
	{
		case MOVEINSERT_Up:
			if (rowData.Num() > 0)
				SetCursorRow(currentRow - 1, bDrag);
			break;

		case MOVEINSERT_Down:
			if (rowData.Num() > 0)
				SetCursorRow(currentRow + 1, bDrag);
			break;

		case MOVEINSERT_Left:
			SetCursorPoint(insertPos - 1, bDrag);
			break;

		case MOVEINSERT_Right:
			SetCursorPoint(insertPos + 1, bDrag);
			break;

		case MOVEINSERT_WordLeft:
			if (rowData.Num() > 0)
				SetCursorPoint(PrevWord(insertPos), bDrag);
			break;

		case MOVEINSERT_WordRight:
			if (rowData.Num() > 0)
				SetCursorPoint(NextWord(insertPos), bDrag);
			break;

		case MOVEINSERT_StartOfLine:
			if (rowData.Num() > 0)
				SetCursorPoint(rowData(currentRow).startPos, bDrag);
			break;

		case MOVEINSERT_EndOfLine:
			if (rowData.Num() > 0)
				SetCursorPoint(rowData(currentRow).startPos + rowData(currentRow).totalCount - 1, bDrag);
			break;

		case MOVEINSERT_PageUp:
			if (rowData.Num() > 0)
				SetCursorRow(currentRow - pageRows, bDrag);
			break;

		case MOVEINSERT_PageDown:
			if (rowData.Num() > 0)
				SetCursorRow(currentRow + pageRows, bDrag);
			break;

		case MOVEINSERT_Home:
			SetCursorPoint(0, bDrag);
			break;

		case MOVEINSERT_End:
			{
				INT endPos = text.Len();
				SetCursorPoint(endPos, bDrag);
			}
			break;
	}

	unguard;
}

void XEditWindow::SetInsertionPoint(INT newInsertionPoint, UBOOL bDrag)
{
	guard(XEditWindow::SetInsertionPoint_Reconstructed);
	SetCursorPoint(newInsertionPoint, bDrag);
	unguard;
}

void XEditWindow::SetSelectedArea(INT startPos, INT count)
{
	guard(XEditWindow::SetSelectedArea_Reconstructed);

	bSelectWords = FALSE;
	SetCursorPoint(startPos, FALSE);
	SetCursorPoint(startPos + count, TRUE);

	unguard;
}

void XEditWindow::GetSelectedArea(INT* pStartPos, INT* pCount)
{
	guard(XEditWindow::GetSelectedArea_Reconstructed);

	INT outStart = selectStart;
	INT outCount = selectEnd - selectStart;

	if (outStart < 0)
		outStart = 0;

	if (outCount < 0)
		outCount = 0;

	if (pStartPos != NULL)
		*pStartPos = outStart;

	if (pCount != NULL)
		*pCount = outCount;

	unguard;
}

void XEditWindow::SetCursorPoint(INT pos, UBOOL bDrag)
{
	guard(XEditWindow::SetCursorPoint_Reconstructed);

	INT maxPos = text.Len();
	if (maxPos < 0)
		maxPos = 0;

	insertPos = ReconClampInt(pos, 0, maxPos);

	if (bDrag == FALSE)
		insertHookPos = insertPos;

	ComputeSelectArea();
	ComputeCursorConfig(TRUE);
	ShowCursor();

	unguard;
}

void XEditWindow::SetCursorRow(INT row, UBOOL bDrag)
{
	guard(XEditWindow::SetCursorRow_Reconstructed);

	INT maxPos = text.Len();
	if (maxPos < 0)
		maxPos = 0;

	INT clampedRow = row;

	if (clampedRow > rowData.Num() - 1)
		clampedRow = rowData.Num() - 1;

	if (clampedRow < 0)
		clampedRow = 0;

	INT newPos = 0;

	if ((clampedRow >= 0) && (clampedRow < rowData.Num()))
	{
		INT col = XToCol(clampedRow, insertPreferredCol);
		newPos = rowData(clampedRow).startPos + col;
	}

	if (newPos > maxPos)
		newPos = maxPos;

	if (newPos < 0)
		newPos = 0;

	insertPos = newPos;

	if (bDrag == FALSE)
		insertHookPos = newPos;

	ComputeSelectArea();
	ComputeCursorConfig(FALSE);
	ShowCursor();

	unguard;
}

void XEditWindow::ComputeSelectArea(void)
{
	guard(XEditWindow::ComputeSelectArea_Reconstructed);

	if ((insertPos != insertHookPos) || (bSelectWords == TRUE))
	{
		UBOOL bSelectionRunsBackward = FALSE;

		if (insertHookPos > insertPos)
		{
			selectStart = insertPos;
			selectEnd   = insertHookPos;
		}
		else
		{
			selectStart = insertHookPos;
			selectEnd   = insertPos;
			bSelectionRunsBackward = TRUE;
		}

		if (bSelectWords == TRUE)
		{
			selectStart = StartOfWord(selectStart);
			selectEnd   = EndOfWord(selectEnd);

			if (bSelectionRunsBackward == TRUE)
				insertPos = selectEnd;
			else
				insertPos = selectStart;
		}
	}
	else
	{
		selectStart = -1;
		selectEnd   = -1;
	}

	unguard;
}

INT XEditWindow::StartOfWord(INT startPos)
{
	guard(XEditWindow::StartOfWord_Reconstructed);

	const TCHAR* source = *text;
	if (source == NULL)
		source = ReconEmptyText();

	INT pos = startPos - 1;

	while (pos >= 0)
	{
		if (ReconIsSpaceForEditWord(source[pos]) == TRUE)
			break;

		pos--;
	}

	return pos + 1;

	unguard;
}

INT XEditWindow::EndOfWord(INT endPos)
{
	guard(XEditWindow::EndOfWord_Reconstructed);

	const TCHAR* source = *text;
	if (source == NULL)
		source = ReconEmptyText();

	INT lastPos = text.Len();
	if (lastPos < 0)
		lastPos = 0;

	INT pos = endPos;

	while (pos < lastPos)
	{
		if (ReconIsSpaceForEditWord(source[pos]) == TRUE)
			break;

		pos++;
	}

	return pos;

	unguard;
}

INT XEditWindow::NextWord(INT startPos)
{
	guard(XEditWindow::NextWord_Reconstructed);

	INT row = 0;
	PosToRowCol(startPos, &row, NULL);

	if ((row < 0) || (row >= rowData.Num()))
		return 0;

	const TCHAR* source = *text;
	if (source == NULL)
		source = ReconEmptyText();

	INT lineStart = rowData(row).startPos;
	INT lineEnd   = rowData(row).startPos + rowData(row).totalCount;
	INT pos       = startPos;

	if (pos >= lineEnd)
		return lineStart + rowData(row).totalCount;

	UBOOL bSawWhitespace = FALSE;

	while (pos < lineEnd)
	{
		if (ReconIsSpaceForEditWord(source[pos]) == TRUE)
		{
			bSawWhitespace = TRUE;
			pos++;
		}
		else
		{
			if (bSawWhitespace == TRUE)
				break;

			pos++;
		}
	}

	if (pos >= lineEnd)
		return rowData(row).startPos + rowData(row).totalCount - 1;

	return pos;

	unguard;
}

INT XEditWindow::PrevWord(INT startPos)
{
	guard(XEditWindow::PrevWord_Reconstructed);

	INT row = 0;
	PosToRowCol(startPos, &row, NULL);

	if ((row < 0) || (row >= rowData.Num()))
		return 0;

	const TCHAR* source = *text;
	if (source == NULL)
		source = ReconEmptyText();

	INT lineStart = rowData(row).startPos;
	INT pos       = startPos;

	if (pos == lineStart)
		return pos - 1;

	UBOOL bSawNonSpace = FALSE;
	pos--;

	while (pos >= lineStart)
	{
		if (ReconIsSpaceForEditWord(source[pos]) == TRUE)
		{
			if (bSawNonSpace == TRUE)
				break;
		}
		else
		{
			bSawNonSpace = TRUE;
		}

		pos--;
	}

	pos++;

	if (pos < 0)
		pos = 0;

	return pos;

	unguard;
}

/*-----------------------------------------------------------------------------
	Text mutation, filtering and undo.
-----------------------------------------------------------------------------*/

void XEditWindow::SetText(const TCHAR* newText)
{
	guard(XEditWindow::SetText_Reconstructed);

	selectStart = 0;
	selectEnd   = text.Len();
	if (selectEnd < 0)
		selectEnd = 0;

	insertPos = selectEnd;
	ReplaceText(newText, FALSE, FALSE);
	SetInsertionPoint(0, FALSE);

	unguard;
}

void XEditWindow::AppendText(const TCHAR* newText)
{
	guard(XEditWindow::AppendText_Reconstructed);

	INT endPos = text.Len();
	if (endPos < 0)
		endPos = 0;

	selectStart = endPos;
	selectEnd   = endPos;
	insertPos   = endPos;

	ReplaceText(newText, FALSE, FALSE);
	SetInsertionPoint(0, FALSE);

	unguard;
}

UBOOL XEditWindow::InsertText(const TCHAR* insertStr, UBOOL bUndo, UBOOL bSelect)
{
	guard(XEditWindow::InsertText_Reconstructed);

	FString filtered;

	if (FilterString(insertStr, filtered) == TRUE)
	{
		if (bUndo == FALSE)
		{
			ClearUndo();
			SetTextChangedFlag(TRUE);
		}

		ReplaceText(*filtered, bUndo, bSelect);
		return TRUE;
	}

	return FALSE;

	unguard;
}

void XEditWindow::DeleteChar(UBOOL bBefore, UBOOL bUndo)
{
	guard(XEditWindow::DeleteChar_Reconstructed);

	if (bUndo == FALSE)
	{
		ClearUndo();
		SetTextChangedFlag(TRUE);
	}

	DeleteText(bBefore, bUndo);

	unguard;
}

void XEditWindow::DeleteText(UBOOL bBefore, UBOOL bUndo)
{
	guard(XEditWindow::DeleteText_Reconstructed);

	if (selectEnd <= selectStart)
	{
		INT pos = insertPos;

		if (bBefore == TRUE)
		{
			if (pos > 0)
			{
				pos--;
				selectStart = pos;
				selectEnd   = pos + 1;
			}
		}
		else
		{
			INT maxPos = text.Len();
			if (maxPos < 0)
				maxPos = 0;

			if (pos < maxPos)
			{
				selectStart = pos;
				selectEnd   = pos + 1;
			}
		}
	}

	ReplaceText(NULL, bUndo, FALSE);

	unguard;
}

void XEditWindow::ReplaceText(const TCHAR* newStr, UBOOL bUndo, UBOOL bSelect)
{
	guard(XEditWindow::ReplaceText_Reconstructed);

	INT oldCount = 0;
	if (selectEnd > selectStart)
		oldCount = selectEnd - selectStart;

	INT newCount = 0;
	if (newStr != NULL)
		newCount = appStrlen(newStr);

	if (maxSize > 0)
	{
		INT visibleLen = text.Len();
		if (visibleLen < 0)
			visibleLen = 0;

		if (visibleLen > maxSize)
		{
			text = text.Left(maxSize);
			ClearUndo();
			SetTextChangedFlag(TRUE);
			AskParentForReconfigure();
			SetInsertionPoint(0, FALSE);
			return;
		}

		if (oldCount < newCount)
		{
			INT overflow = newCount + visibleLen - maxSize - oldCount;
			if (overflow > 0)
			{
				newCount -= overflow;
				if (newCount < 0)
					return;
			}
		}
	}

	if ((oldCount > 0) || (newCount > 0))
	{
		if (selectEnd > selectStart)
			insertPos = selectStart;

		const TCHAR* source = *text;
		if (source == NULL)
			source = ReconEmptyText();

		if (bUndo == TRUE)
			AddUndo(insertPos, source + insertPos, oldCount, newStr, newCount);

		FString leftPart;
		FString rightPart;
		FString insertPart;

		if (insertPos > 0)
			leftPart = text.Left(insertPos);

		INT rightStart = insertPos + oldCount;
		if (rightStart < text.Len())
			rightPart = text.Mid(rightStart);

		if ((newStr != NULL) && (newCount > 0))
			ReconAssignSubstring(insertPart, newStr, newCount);

		text = leftPart + insertPart + rightPart;

		ChangeText();
		MakeAreaDirty(insertPos, oldCount, newCount);
		AskParentForReconfigure();

		insertHookPos = insertPos;
		SetInsertionPoint(insertPos + newCount, bSelect);
	}

	unguard;
}

UBOOL XEditWindow::FilterString(const TCHAR* origStr, FString& newStr)
{
	guard(XEditWindow::FilterString_Reconstructed);

	if (origStr == NULL)
		origStr = ReconEmptyText();

	newStr = TEXT("");

	INT inputLen = appStrlen(origStr);

	for (INT i = 0; i < inputLen; i++)
	{
		TCHAR ch = origStr[i];

		if ((bSingleLine == TRUE) && (ch == TEXT('\n')))
			continue;

		if (FilterChar(ch) == TRUE)
		{
			TCHAR oneChar[2];
			oneChar[0] = ch;
			oneChar[1] = 0;
			newStr += oneChar;
		}
	}

	if (inputLen > 0)
	{
		if (newStr.Len() <= 0)
			return FALSE;
	}

	return TRUE;

	unguard;
}

UBOOL XEditWindow::FilterChar(TCHAR& ch)
{
	guard(XEditWindow::FilterChar_Reconstructed);

	if (bUppercaseOnly == TRUE)
	{
		if ((ch >= TEXT('a')) && (ch <= TEXT('z')))
			ch -= TEXT('a') - TEXT('A');
	}

	UFunction* function = FindFunction(EXTENSION_FilterChar);
	if (function != NULL)
	{
		struct XFilterCharParms
		{
			FString chStr;
			UBOOL   bReturnValue;
		};

		XFilterCharParms parms;
		TCHAR temp[2];
		temp[0] = ch;
		temp[1] = 0;
		parms.chStr = temp;
		parms.bReturnValue = TRUE;

		ProcessEvent(function, &parms, NULL);

		if (parms.bReturnValue == TRUE)
		{
			const TCHAR* resultText = *parms.chStr;
			if (resultText == NULL)
				resultText = ReconEmptyText();
			ch = resultText[0];
		}

		return parms.bReturnValue;
	}

	return TRUE;

	unguard;
}

void XEditWindow::CopyToString(FString& destStr, const TCHAR* srcStr, INT size)
{
	guard(XEditWindow::CopyToString_Reconstructed);

	if (srcStr == NULL)
		srcStr = ReconEmptyText();

	if (size < 0)
		size = appStrlen(srcStr);

	if (size > 0)
		ReconAssignSubstring(destStr, srcStr, size);
	else
		destStr = TEXT("");

	unguard;
}

void XEditWindow::AddUndo(INT pos, const TCHAR* oldStr, INT oldCount, const TCHAR* newStr, INT newCount)
{
	guard(XEditWindow::AddUndo_Reconstructed);

	if (oldStr == NULL)
		oldStr = ReconEmptyText();

	if (newStr == NULL)
		newStr = ReconEmptyText();

	if (oldCount < 0)
		oldCount = appStrlen(oldStr);

	if (newCount < 0)
		newCount = appStrlen(newStr);

	if ((oldCount < 1) && (newCount < 1))
		return;

	// Original has a small coalescing optimisation for sequential typed text:
	// when the previous undo entry is an insertion-only record ending exactly at
	// this position, append the new text into that entry instead of creating a
	// separate undo record for every character.
	UBOOL bCreateNewRecord = TRUE;

	if ((oldCount < 1) && (newCount > 0) && (currentUndo > 0) && (currentUndo != unchangedUndo))
	{
		XUndoBuffer& previous = bufferList(currentUndo - 1);
		INT previousNewLen = previous.newString.Len();
		if (previousNewLen > 0)
			previousNewLen--;

		if ((previous.oldString.Len() <= 1) && (pos == previous.insertPos + previousNewLen))
		{
			FString appendText;
			CopyToString(appendText, newStr, newCount);
			previous.newString += appendText;
			bCreateNewRecord = FALSE;
		}
	}

	if (currentUndo < bufferList.Num())
	{
		for (INT iRemove = currentUndo; iRemove < bufferList.Num(); iRemove++)
		{
			bufferList(iRemove).oldString.Empty();
			bufferList(iRemove).newString.Empty();
		}

		bufferList.Remove(currentUndo, bufferList.Num() - currentUndo);

		if (unchangedUndo > currentUndo)
			unchangedUndo = -1;
	}

	if (bCreateNewRecord == TRUE)
	{
		INT newIndex = bufferList.Num();
		bufferList.AddZeroed(1);

		XUndoBuffer& newBuffer = bufferList(newIndex);
		newBuffer.insertPos = pos;
		CopyToString(newBuffer.oldString, oldStr, oldCount);
		CopyToString(newBuffer.newString, newStr, newCount);

		currentUndo++;
	}

	ClampUndo();

	unguard;
}

void XEditWindow::ClampUndo(void)
{
	guard(XEditWindow::ClampUndo_Reconstructed);

	if (maxUndos >= 0)
	{
		if (bufferList.Num() > maxUndos)
		{
			INT removeCount = bufferList.Num() - maxUndos;

			for (INT iRemove = 0; iRemove < removeCount; iRemove++)
			{
				bufferList(iRemove).oldString.Empty();
				bufferList(iRemove).newString.Empty();
			}

			bufferList.Remove(0, removeCount);

			currentUndo -= removeCount;
			unchangedUndo -= removeCount;

			if (currentUndo < 0)
				currentUndo = 0;

			if (unchangedUndo < 0)
				unchangedUndo = -1;
		}
	}

	unguard;
}

void XEditWindow::ClearUndo(void)
{
	guard(XEditWindow::ClearUndo_Reconstructed);

	UBOOL bWasChanged = FALSE;
	if (currentUndo != unchangedUndo)
		bWasChanged = TRUE;

	for (INT iUndo = 0; iUndo < bufferList.Num(); iUndo++)
	{
		bufferList(iUndo).oldString.Empty();
		bufferList(iUndo).newString.Empty();
	}

	bufferList.Empty();
	currentUndo = 0;

	if (bWasChanged == TRUE)
		unchangedUndo = -1;
	else
		unchangedUndo = 0;

	unguard;
}

void XEditWindow::Undo(void)
{
	guard(XEditWindow::Undo_Reconstructed);

	if (currentUndo > 0)
	{
		currentUndo--;
		XUndoBuffer& buffer = bufferList(currentUndo);

		selectStart = buffer.insertPos;
		selectEnd   = buffer.insertPos + Max(buffer.newString.Len() - 1, 0);
		insertPos   = selectStart;

		ReplaceText(*buffer.oldString, FALSE, TRUE);
	}

	unguard;
}

void XEditWindow::Redo(void)
{
	guard(XEditWindow::Redo_Reconstructed);

	if (currentUndo <= bufferList.Num() - 1)
	{
		XUndoBuffer& buffer = bufferList(currentUndo);

		selectStart = buffer.insertPos;
		selectEnd   = buffer.insertPos + Max(buffer.oldString.Len() - 1, 0);
		insertPos   = selectStart;

		currentUndo++;
		ReplaceText(*buffer.newString, FALSE, TRUE);
	}

	unguard;
}

void XEditWindow::Copy(void)
{
	guard(XEditWindow::Copy_Reconstructed);
	CopyToTextBuffer(FALSE, TRUE);
	unguard;
}

void XEditWindow::Cut(void)
{
	guard(XEditWindow::Cut_Reconstructed);
	CopyToTextBuffer(TRUE, TRUE);
	unguard;
}

void XEditWindow::Paste(void)
{
	guard(XEditWindow::Paste_Reconstructed);
	PasteFromTextBuffer(TRUE, FALSE);
	unguard;
}

void XEditWindow::CopyToTextBuffer(UBOOL bCut, UBOOL bUndo)
{
	guard(XEditWindow::CopyToTextBuffer_Reconstructed);

	if (selectEnd > selectStart)
	{
		const TCHAR* source = *text;
		if (source == NULL)
			source = ReconEmptyText();

		CopyToString(GExtensionTextBuffer, source + selectStart, selectEnd - selectStart);

		if (bCut == TRUE)
			DeleteText(FALSE, bUndo);
	}

	unguard;
}

void XEditWindow::PasteFromTextBuffer(UBOOL bUndo, UBOOL bSelect)
{
	guard(XEditWindow::PasteFromTextBuffer_Reconstructed);
	InsertText(*GExtensionTextBuffer, bUndo, bSelect);
	unguard;
}

/*-----------------------------------------------------------------------------
	Coordinate conversion.
-----------------------------------------------------------------------------*/

INT XEditWindow::YToRow(FLOAT pointY)
{
	guard(XEditWindow::YToRow_Reconstructed);

	INT rowCount = rowData.Num();
	FLOAT totalHeight = rowCount * lineHeight;

	if (rowCount > 1)
		totalHeight += (rowCount - 1) * vSpace;

	FLOAT baseY = vMargin;

	if (vAlign == VALIGN_Bottom)
		baseY = height - totalHeight - vMargin;
	else if (vAlign == VALIGN_Center)
		baseY = appFloor((height - totalHeight) * 0.5f);

	FLOAT rowStep = lineHeight + vSpace;
	INT result = 0;

	if (rowStep > 0.0f)
		result = appFloor((pointY - baseY) / rowStep);

	if (result > rowCount - 1)
		result = rowCount - 1;

	if (result < 0)
		result = 0;

	return result;

	unguard;
}

INT XEditWindow::XToCol(INT row, FLOAT pointX)
{
	guard(XEditWindow::XToCol_Reconstructed);

	if ((row < 0) || (row >= rowData.Num()))
		return 0;

	FLOAT baseX = hMargin;

	if (hAlign == HALIGN_Right)
		baseX = width - rowData(row).xExtent - hMargin;
	else if (hAlign == HALIGN_Center)
		baseX = appFloor((width - rowData(row).xExtent) * 0.5f);

	XGC* gc = GetGC();
	if (gc == NULL)
		return 0;

	const TCHAR* source = *text;
	if (source == NULL)
		source = ReconEmptyText();

	INT col = gc->PixelToChar(source + rowData(row).startPos, pointX - baseX, rowData(row).count, &rowData(row).textState);
	ReleaseGC(gc);

	return col;

	unguard;
}

FLOAT XEditWindow::RowToY(INT row)
{
	guard(XEditWindow::RowToY_Reconstructed);

	INT rowCount = rowData.Num();
	INT rowCountForHeight = rowCount;

	// Original Extension treats an empty edit buffer as one visual row for
	// caret placement. Without this, a VALIGN_Center single-line ToolEditWindow
	// places the empty caret halfway through the whole control instead of on the
	// text baseline used after the first character is entered.
	if (rowCountForHeight < 1)
		rowCountForHeight = 1;

	FLOAT totalHeight = rowCountForHeight * lineHeight;

	if (rowCountForHeight > 1)
		totalHeight += (rowCountForHeight - 1) * vSpace;

	FLOAT baseY = vMargin;

	if (vAlign == VALIGN_Bottom)
		baseY = height - totalHeight - vMargin;
	else if (vAlign == VALIGN_Center)
		baseY = appFloor((height - totalHeight) * 0.5f);

	return baseY + row * (lineHeight + vSpace);

	unguard;
}

FLOAT XEditWindow::ColToX(INT row, INT col)
{
	guard(XEditWindow::ColToX_Reconstructed);

	UBOOL bValidRow = FALSE;
	FLOAT rowExtent = 0.0f;

	if ((row >= 0) && (row < rowData.Num()))
	{
		bValidRow = TRUE;
		rowExtent = rowData(row).xExtent;
	}

	FLOAT baseX = hMargin;

	if (hAlign == HALIGN_Right)
		baseX = width - rowExtent - hMargin;
	else if (hAlign == HALIGN_Center)
		baseX = appFloor((width - rowExtent) * 0.5f);

	FLOAT x = 0.0f;

	if (bValidRow == TRUE)
	{
		XGC* gc = GetGC();
		if (gc != NULL)
		{
			const TCHAR* source = *text;
			if (source == NULL)
				source = ReconEmptyText();

			x = gc->CharToPixel(source + rowData(row).startPos, col, rowData(row).count, &rowData(row).textState);
			ReleaseGC(gc);
		}
	}

	return baseX + x;

	unguard;
}

INT XEditWindow::RowColToPos(INT row, INT col)
{
	guard(XEditWindow::RowColToPos_Reconstructed);

	INT maxPos = text.Len();
	if (maxPos < 0)
		maxPos = 0;

	INT clampedRow = row;
	if (clampedRow > rowData.Num() - 1)
		clampedRow = rowData.Num() - 1;
	if (clampedRow < 0)
		clampedRow = 0;

	INT result = 0;

	if ((clampedRow >= 0) && (clampedRow < rowData.Num()))
	{
		INT clampedCol = col;
		if (clampedCol > rowData(clampedRow).totalCount - 1)
			clampedCol = rowData(clampedRow).totalCount - 1;
		if (clampedCol < 0)
			clampedCol = 0;
		result = rowData(clampedRow).startPos + clampedCol;
	}

	if (result > maxPos)
		result = maxPos;
	if (result < 0)
		result = 0;

	return result;

	unguard;
}

void XEditWindow::PosToRowCol(INT pos, INT* pRow, INT* pCol)
{
	guard(XEditWindow::PosToRowCol_Reconstructed);

	INT maxPos = text.Len();
	if (maxPos < 0)
		maxPos = 0;

	INT clampedPos = ReconClampInt(pos, 0, maxPos);
	INT row = rowData.Num() - 1;

	while (row >= 0)
	{
		if (rowData(row).startPos <= clampedPos)
			break;

		row--;
	}

	INT col = 0;
	if (row < 0)
		row = 0;
	else
		col = clampedPos - rowData(row).startPos;

	if (pRow != NULL)
		*pRow = row;

	if (pCol != NULL)
		*pCol = col;

	unguard;
}

INT XEditWindow::XYToPos(FLOAT pointX, FLOAT pointY)
{
	guard(XEditWindow::XYToPos_Reconstructed);

	if (rowData.Num() <= 0)
		return 0;

	INT row = YToRow(pointY);
	return XToCol(row, pointX) + rowData(row).startPos;

	unguard;
}

void XEditWindow::PosToXY(INT pos, FLOAT* pPointX, FLOAT* pPointY)
{
	guard(XEditWindow::PosToXY_Reconstructed);

	INT row = 0;
	INT col = 0;
	PosToRowCol(pos, &row, &col);

	if (pPointY != NULL)
		*pPointY = RowToY(row);

	if (pPointX != NULL)
		*pPointX = ColToX(row, col);

	unguard;
}

/*-----------------------------------------------------------------------------
	Cursor geometry and draw.
-----------------------------------------------------------------------------*/

void XEditWindow::ComputeCursorConfig(UBOOL bSetPrefCol)
{
	guard(XEditWindow::ComputeCursorConfig_Reconstructed);

	FLOAT charWidth = 8.0f;
	FLOAT charHeight = lineHeight;
	UBOOL bCentreOnChar = TRUE;

	if ((insertType == INSTYPE_Underscore) || (insertType == INSTYPE_Block) || (insertType == INSTYPE_RawOverlay))
		bCentreOnChar = FALSE;

	TCHAR probe[2];
	probe[0] = TEXT(' ');
	probe[1] = 0;

	const TCHAR* source = *text;
	if (source != NULL)
	{
		TCHAR ch = source[insertPos];
		if ((ch != 0) && (ReconIsSpaceForEditWord(ch) == FALSE))
			probe[0] = ch;
	}

	XGC* gc = GetGC();
	if (gc != NULL)
	{
		gc->GetTextExtent(0.0f, charWidth, charHeight, probe);
		ReleaseGC(gc);
	}

	switch (insertType)
	{
		case INSTYPE_Insert:
			insertWidth = insertPrefWidth;
			if (insertWidth < 1.0f)
				insertWidth = 2.0f;
			insertHeight = lineHeight;
			break;

		case INSTYPE_Underscore:
			insertWidth = charWidth;
			if (insertWidth < 1.0f)
				insertWidth = 1.0f;
			insertHeight = insertPrefHeight;
			if (insertHeight < 1.0f)
				insertHeight = 2.0f;
			break;

		case INSTYPE_Block:
			insertWidth = charWidth;
			if (insertWidth < 1.0f)
				insertWidth = 1.0f;
			insertHeight = lineHeight + vSpace - appFloor(vSpace * 0.5f);
			break;

		case INSTYPE_RawInsert:
		case INSTYPE_RawOverlay:
			insertWidth = insertPrefWidth;
			if (insertWidth < 1.0f && insertTexture != NULL)
				insertWidth = insertTexture->USize;
			insertHeight = insertPrefHeight;
			if (insertHeight < 1.0f && insertTexture != NULL)
				insertHeight = insertTexture->VSize;
			break;
	}

	PosToXY(insertPos, &insertX, &insertY);

	if (bSetPrefCol == TRUE)
		insertPreferredCol = insertX;

	showAreaX      = insertX;
	showAreaY      = insertY;
	showAreaWidth  = charWidth;
	showAreaHeight = charHeight;

	if (bCentreOnChar == FALSE)
	{
		insertX += appFloor((charWidth - insertWidth) * 0.5f);
		insertY += appFloor(lineHeight - insertHeight);
	}
	else
	{
		insertX -= appFloor(insertWidth * 0.5f);
		insertY += appFloor((lineHeight - insertHeight) * 0.5f);
	}

	if (showAreaX > insertX)
	{
		showAreaWidth += showAreaX - insertX;
		showAreaX = insertX;
	}

	if (showAreaY > insertY)
	{
		showAreaHeight += showAreaY - insertY;
		showAreaY = insertY;
	}

	if (showAreaX + showAreaWidth < insertX + insertWidth)
		showAreaWidth = insertX + insertWidth - showAreaX;

	if (showAreaY + showAreaHeight < insertY + insertHeight)
		showAreaHeight = insertY + insertHeight - showAreaY;

	selectStartRow = -1;
	selectEndRow   = -1;
	selectStartX   = 0.0f;
	selectEndX     = 0.0f;

	if (selectStart < selectEnd)
	{
		INT col = 0;
		PosToRowCol(selectStart, &selectStartRow, &col);
		selectStartX = ColToX(selectStartRow, col);

		PosToRowCol(selectEnd, &selectEndRow, &col);
		selectEndX = ColToX(selectEndRow, col);
	}

	unguard;
}

void XEditWindow::ShowCursor(void)
{
	guard(XEditWindow::ShowCursor_Reconstructed);

	blinkDelay = XWindow::GetTickOffset() + blinkStart;
	bInsertShowing = TRUE;

	AskParentToShowArea(showAreaX - 1.0f, showAreaY, showAreaWidth + 2.0f, showAreaHeight);

	unguard;
}

void XEditWindow::Draw(XGC* gc)
{
	guard(XEditWindow::Draw_Reconstructed);

	if ((bEditable == TRUE) && (IsFocusWindow() == TRUE))
	{
		// Editable focused draw is not XLargeTextWindow::Draw(). The original draws
		// selection first, then text with per-character inverse colour at partial
		// selection edges, then restores the clip and draws the blinking insertion
		// texture.
		XWindow::Draw(gc);

		XClipRect oldClip = gc->GetClipRect();
		FColor oldTextColor = gc->GetTextColor();

		FLOAT lineStep = lineHeight + vSpace;
		FLOAT halfVSpace = appFloor(vSpace * 0.5f);

		FLOAT totalHeight = rowData.Num() * lineHeight;
		if (rowData.Num() > 1)
			totalHeight += (rowData.Num() - 1) * vSpace;

		FLOAT baseY = vMargin;
		if (vAlign == VALIGN_Bottom)
			baseY = height - totalHeight - vMargin;
		else if (vAlign == VALIGN_Center)
			baseY = appFloor((height - totalHeight) * 0.5f);

		XClipRect drawClip = gc->GetClipRect();
			INT firstRow = appFloor((drawClip.clipY + 0.00001f - baseY) / lineStep);
		INT lastRow = appCeil((drawClip.clipY + drawClip.clipHeight - baseY) / lineStep);

		if (firstRow < 0)
			firstRow = 0;
		if (lastRow > rowData.Num())
			lastRow = rowData.Num();

		gc->SetTileColor(selectColor);
		gc->Intersect(hMargin, vMargin, width - hMargin - hMargin, height - vMargin - vMargin);

		const TCHAR* source = *text;
		if (source == NULL)
			source = ReconEmptyText();

		FLOAT y = baseY + firstRow * lineStep;

		for (INT row = firstRow; row < lastRow; row++)
		{
			XTextRowData& rowInfo = rowData(row);
			FLOAT rowX = hMargin;

			if (hAlign == HALIGN_Right)
				rowX = width - rowInfo.xExtent - hMargin;
			else if (hAlign == HALIGN_Center)
				rowX = appFloor((width - rowInfo.xExtent) * 0.5f);

			FLOAT rowWidth = rowInfo.xExtent;
			if (rowWidth < 4.0f)
				rowWidth = 4.0f;

			if ((row == selectStartRow) || (row == selectEndRow))
			{
				FLOAT selectionX0 = rowX;
				FLOAT selectionX1 = rowX + rowWidth;

				if (row == selectStartRow)
					selectionX0 = selectStartX;

				if (row == selectEndRow)
					selectionX1 = selectEndX;

				gc->DrawPattern(selectionX0, y - halfVSpace, selectionX1 - selectionX0, lineStep, selectionX0, y - halfVSpace, selectTexture);

				// Original toggles inverse colour exactly while the per-character draw
				// position is inside the selected interval. This matters for partial
				// first/last rows: individual glyphs can cross the selection boundary.
				XTextState state = rowInfo.textState;
				const TCHAR* drawText = source + rowInfo.startPos;
				INT drawCount = rowInfo.count;
				FLOAT drawX = rowX;
				FLOAT drawY = y;
				UBOOL bInverse = FALSE;

				while (drawCount > 0)
				{
					if ((drawX >= selectionX0) && (drawX < selectionX1))
					{
						if (bInverse == FALSE)
						{
							gc->SetTextColor(inverseColor);
							bInverse = TRUE;
						}
					}
					else
					{
						if (bInverse == TRUE)
						{
							gc->SetTextColor(oldTextColor);
							bInverse = FALSE;
						}
					}

					if (gc->DrawTextChar(drawX, drawY, &state, drawText, drawCount) == FALSE)
						break;
				}

				if (bInverse == TRUE)
					gc->SetTextColor(oldTextColor);
			}
			else if ((row > selectStartRow) && (row < selectEndRow))
			{
				gc->SetTextColor(inverseColor);
				gc->DrawPattern(rowX, y - halfVSpace, rowWidth, lineStep, rowX, y - halfVSpace, selectTexture);
				gc->DrawTextLine(rowX, y, &rowInfo.textState, source + rowInfo.startPos, rowInfo.count);
				gc->SetTextColor(oldTextColor);
			}
			else
			{
				gc->DrawTextLine(rowX, y, &rowInfo.textState, source + rowInfo.startPos, rowInfo.count);
			}

			y += lineStep;
		}

		gc->SetClipRect(oldClip);

		if ((bInsertShowing == TRUE) && (insertTexture != NULL))
		{
			gc->SetStyle(STY_Masked);
			gc->SetTileColor(insertColor);
			gc->DrawPattern(insertX, insertY, insertWidth, insertHeight, 0.0f, 0.0f, insertTexture);
		}
	}
	else
	{
		XLargeTextWindow::Draw(gc);
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Parent callbacks and timers.
-----------------------------------------------------------------------------*/

void XEditWindow::ChangeText(void)
{
	guard(XEditWindow::ChangeText_Reconstructed);

	for (XWindow* test = this; test != NULL; test = test->parent)
	{
		if (test->TextChanged(this, currentUndo != unchangedUndo) == TRUE)
			break;
	}

	unguard;
}

void XEditWindow::ActivateEdit(void)
{
	guard(XEditWindow::ActivateEdit_Reconstructed);

	for (XWindow* test = this; test != NULL; test = test->parent)
	{
		if (test->EditActivated(this, currentUndo != unchangedUndo) == TRUE)
			break;
	}

	unguard;
}

void XEditWindow::Tick(FLOAT deltaSeconds)
{
	guard(XEditWindow::Tick_Reconstructed);

	if (bEditable == TRUE)
	{
		blinkDelay -= deltaSeconds;

		if (blinkDelay < 0.0f)
		{
			FLOAT halfPeriod = blinkPeriod * 0.5f;
			blinkDelay += halfPeriod;

			if (blinkDelay < 0.0f)
				blinkDelay = halfPeriod;

			if (bInsertShowing == TRUE)
				bInsertShowing = FALSE;
			else
				bInsertShowing = TRUE;
		}
	}

	if (bDragging == TRUE)
	{
		dragDelay -= deltaSeconds;

		if (dragDelay < 0.0f)
		{
			dragDelay += 0.1f;
			if (dragDelay < 0.0f)
				dragDelay = 0.1f;

			FLOAT cursorX = 0.0f;
			FLOAT cursorY = 0.0f;
			GetCursorPos(&cursorX, &cursorY);
			MouseMoved(cursorX, cursorY);
		}
	}

	unguard;
}

void XEditWindow::ParentRequestedPreferredSize(UBOOL bWidthSpecified, FLOAT& preferredWidth, UBOOL bHeightSpecified, FLOAT& preferredHeight)
{
	guard(XEditWindow::ParentRequestedPreferredSize_Reconstructed);

	XLargeTextWindow::ParentRequestedPreferredSize(bWidthSpecified, preferredWidth, bHeightSpecified, preferredHeight);

	if ((bSingleLine == TRUE) && (bHeightSpecified == FALSE))
		preferredHeight = vMargin + vMargin + lineHeight;

	if (bWidthSpecified == FALSE)
	{
		XGC* gc = GetGC();
		if (gc != NULL)
		{
			FLOAT cursorWidth = 0.0f;
			FLOAT cursorHeight = 0.0f;
			gc->GetTextExtent(0.0f, cursorWidth, cursorHeight, TEXT(" "));
			preferredWidth += cursorWidth;
			ReleaseGC(gc);
		}
	}

	unguard;
}

void XEditWindow::ConfigurationChanged(void)
{
	guard(XEditWindow::ConfigurationChanged_Reconstructed);

	XLargeTextWindow::ConfigurationChanged();

	if ((lastConfigWidth == width) && (lastConfigHeight == height))
		ComputeCursorConfig(FALSE);
	else
		ComputeCursorConfig(TRUE);

	lastConfigWidth = width;
	lastConfigHeight = height;

	unguard;
}

void XEditWindow::VisibilityChanged(UBOOL bNewVisibility)
{
	guard(XEditWindow::VisibilityChanged_Reconstructed);

	ProcessScript(EXTENSION_VisibilityChanged, &bNewVisibility, FALSE);

	if (bNewVisibility == TRUE)
	{
		AskParentForReconfigure();
		ShowCursor();
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Input routing.
-----------------------------------------------------------------------------*/

XCursor* XEditWindow::CursorRequested(XWindow* win, FLOAT pointX, FLOAT pointY, FLOAT& hotX, FLOAT& hotY, FColor& newColor, UTexture** pCursorShadow)
{
	guard(XEditWindow::CursorRequested_Reconstructed);

	if (bEditable == TRUE)
	{
		XRootWindow* root = GetRootWindow();

		*pCursorShadow = editCursorShadow;
		newColor = editCursorColor;

		if (editCursor != NULL)
			return editCursor;

		if (root != NULL && root->defaultEditCursor != NULL)
			return root->defaultEditCursor;
	}

	XReconEditCursorRequestedParms parms;
	parms.win = win;
	parms.pointX = pointX;
	parms.pointY = pointY;
	parms.hotX = hotX;
	parms.hotY = hotY;
	parms.newColor = newColor;
	parms.pCursorShadow = pCursorShadow;
	parms.returnValue = NULL;

	ProcessScript(EXTENSION_CursorRequested, &parms, FALSE);

	hotX = parms.hotX;
	hotY = parms.hotY;
	newColor = parms.newColor;

	return parms.returnValue;

	unguard;
}

UBOOL XEditWindow::KeyPressed(TCHAR key)
{
	guard(XEditWindow::KeyPressed_Reconstructed);

	struct XKeyPressedParms
	{
		FString key;
		UBOOL   returnValue;
	};

	XKeyPressedParms parms;
	TCHAR keyText[2];
	keyText[0] = key;
	keyText[1] = 0;
	parms.key = keyText;
	parms.returnValue = FALSE;

	ProcessScript(EXTENSION_KeyPressed, &parms, FALSE);

	if ((bEditable == TRUE) && (key != TEXT('`')) && (key != TEXT('~')) && (key >= 0x20))
	{
		TCHAR insertText[2];
		insertText[0] = key;
		insertText[1] = 0;

		if (InsertText(insertText, TRUE, FALSE) == TRUE)
		{
			PlayEditSound(typeSound, soundVolume, 1.0f);
			return TRUE;
		}
	}

	return parms.returnValue;

	unguard;
}

UBOOL XEditWindow::VirtualKeyPressed(EInputKey key, UBOOL bRepeat)
{
	guard(XEditWindow::VirtualKeyPressed_Reconstructed);

	if (key == IK_Enter)
	{
		PlayEditSound(enterSound, soundVolume, 1.0f);

		if (bEditable == TRUE)
		{
			if (bSingleLine == TRUE)
			{
				ActivateEdit();
				return TRUE;
			}

			InsertText(TEXT("\n"), TRUE, FALSE);
			return TRUE;
		}
	}

	struct XVKeyParms
	{
		EInputKey key;
		UBOOL     bRepeat;
		UBOOL     returnValue;
	};

	XVKeyParms parms;
	parms.key = key;
	parms.bRepeat = bRepeat;
	parms.returnValue = FALSE;

	ProcessScript(EXTENSION_VirtualKeyPressed, &parms, FALSE);
	return parms.returnValue;

	unguard;
}

UBOOL XEditWindow::MouseButtonPressed(FLOAT pointX, FLOAT pointY, EInputKey button, INT numClicks)
{
	guard(XEditWindow::MouseButtonPressed_Reconstructed);

	struct XMouseParms
	{
		FLOAT pointX;
		FLOAT pointY;
		EInputKey button;
		INT numClicks;
		UBOOL returnValue;
	};

	XMouseParms parms;
	parms.pointX = pointX;
	parms.pointY = pointY;
	parms.button = button;
	parms.numClicks = numClicks;
	parms.returnValue = FALSE;

	ProcessScript(EXTENSION_MouseButtonPressed, &parms, FALSE);

	if ((button == IK_LeftMouse) && (bEditable == TRUE))
	{
		bSelectWords = FALSE;

		if (IsKeyDown(IK_Ctrl) == TRUE)
			bSelectWords = TRUE;

		if (((numClicks - 1) % 2) == 1)
			bSelectWords = TRUE;

		bDragging = TRUE;
		dragDelay = XWindow::GetTickOffset() + 0.1f;

		INT pos = XYToPos(pointX, pointY);
		SetInsertionPoint(pos, IsKeyDown(IK_Shift));
		return TRUE;
	}

	return parms.returnValue;

	unguard;
}

void XEditWindow::MouseMoved(FLOAT pointX, FLOAT pointY)
{
	guard(XEditWindow::MouseMoved_Reconstructed);

	struct XMouseMovedParms
	{
		FLOAT pointX;
		FLOAT pointY;
	};

	XMouseMovedParms parms;
	parms.pointX = pointX;
	parms.pointY = pointY;
	ProcessScript(EXTENSION_MouseMoved, &parms, FALSE);

	if (bDragging == TRUE)
	{
		INT pos = XYToPos(pointX, pointY);
		SetInsertionPoint(pos, TRUE);
	}

	unguard;
}

UBOOL XEditWindow::MouseButtonReleased(FLOAT pointX, FLOAT pointY, EInputKey button, INT numClicks)
{
	guard(XEditWindow::MouseButtonReleased_Reconstructed);

	struct XMouseParms
	{
		FLOAT pointX;
		FLOAT pointY;
		EInputKey button;
		INT numClicks;
		UBOOL returnValue;
	};

	XMouseParms parms;
	parms.pointX = pointX;
	parms.pointY = pointY;
	parms.button = button;
	parms.numClicks = numClicks;
	parms.returnValue = FALSE;

	ProcessScript(EXTENSION_MouseButtonReleased, &parms, FALSE);

	if (button == IK_LeftMouse)
	{
		bDragging = FALSE;

		if (bEditable == TRUE)
			return TRUE;
	}

	return parms.returnValue;

	unguard;
}

/*-----------------------------------------------------------------------------
	Native wrappers are intentionally left for the compile-pass.

	The 1880..1921 wrappers are thin P_GET_* bridges to the methods above. For
	reconstruction they add noise, not behaviour. The method-level behaviour is the
	important part for understanding original UI semantics.
-----------------------------------------------------------------------------*/
