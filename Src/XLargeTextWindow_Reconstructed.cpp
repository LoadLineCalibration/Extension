/*=============================================================================
    XLargeTextWindow_Reconstructed.cpp
    Reconstructed from Extension.dll.c, pass 08.

    Focus: LargeTextWindow row generation, dirty-range invalidation, preferred
    size queries, visible-row drawing and exact relationship to XGC::GetTextLine.

    This file is intentionally reconstruction-first.  It explains the native
    behaviour and keeps names close to ExtLargeText.h instead of flattening the
    decompiled control flow into anonymous offsets.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XLargeTextWindow);

/*-----------------------------------------------------------------------------
    Local helpers.
-----------------------------------------------------------------------------*/

static const TCHAR* XReconLargeGetTextData(const FStringNoInit& Text)
{
    const TCHAR* Result = *Text;
    if (Result == NULL)
        Result = TEXT("");
    return Result;
}

static UBOOL XReconTextStatesMatch(const XTextState& Left, const XTextState& Right)
{
    return (Left == Right);
}

static void XReconInitTextState(XTextState& State)
{
    State = XTextState();
}

static void XReconInitTextParams(XTextParams& Params)
{
    appMemzero(&Params, sizeof(Params));
    Params.bDirty = TRUE;
    Params.bSpecialText = FALSE;
    Params.dirtyStart = 0;
    Params.dirtyCount = 1;
    Params.normalFont = NULL;
    Params.boldFont = NULL;
    Params.destWidth = 0.0f;
}

static void XReconClearRowArray(TArray<XTextRowData>& Rows)
{
    Rows.Empty();
}

static void XReconAppendRow(TArray<XTextRowData>& Rows, const XTextRowData& Row)
{
    new(Rows) XTextRowData(Row);
}

static UBOOL XReconTextParamsCompatible(const XTextParams& OldParams, const XTextParams& NewParams)
{
    if (OldParams.bSpecialText != NewParams.bSpecialText)
        return FALSE;

    if (OldParams.destWidth != NewParams.destWidth)
        return FALSE;

    if (OldParams.normalFont != NewParams.normalFont)
        return FALSE;

    if (OldParams.boldFont != NewParams.boldFont)
        return FALSE;

    return TRUE;
}

/*-----------------------------------------------------------------------------
    Construction / destruction.
-----------------------------------------------------------------------------*/

XLargeTextWindow::XLargeTextWindow(XWindow* Parent)
    : XTextWindow(Parent)
{
    XReconInitTextParams(textParams);
    XReconInitTextParams(queryTextParams);
}

void XLargeTextWindow::Init(XWindow* Parent)
{
    guard(XLargeTextWindow::Init);

    XTextWindow::Init(Parent);

    vSpace = 1.0f;
    lineHeight = 0.0f;

    XReconInitTextParams(textParams);
    XReconInitTextParams(queryTextParams);
    XReconClearRowArray(rowData);
    XReconClearRowArray(queryRowData);
    XReconClearRowArray(tempRowData);

    XGC* gc = GetGC();
    if (gc != NULL)
    {
        lineHeight = ComputeLineHeight(gc);
        ReleaseGC(gc);
    }

    MakeLinesDirty();

    // LargeTextWindow deliberately disables TextWindow accelerator behaviour.
    // Long wrapped text must not register the whole string as a hotkey label.
    EnableTextAsAccelerator(FALSE);

    unguard;
}

void XLargeTextWindow::CleanUp(void)
{
    guard(XLargeTextWindow::CleanUp);

    XReconClearRowArray(rowData);
    XReconClearRowArray(queryRowData);
    XReconClearRowArray(tempRowData);

    XTextWindow::CleanUp();

    unguard;
}

void XLargeTextWindow::Serialize(FArchive& Ar)
{
    guard(XLargeTextWindow::Serialize);

    XTextWindow::Serialize(Ar);

    unguard;
}

/*-----------------------------------------------------------------------------
    Text mutations and invalidation.
-----------------------------------------------------------------------------*/

void XLargeTextWindow::SetText(const TCHAR* NewText)
{
    guard(XLargeTextWindow::SetText);

    // The original invalidates before delegating to XTextWindow::SetText().
    // This means even assigning the same text leaves the row caches dirty if the
    // caller reached this native method.
    MakeLinesDirty();
    XTextWindow::SetText(NewText);

    unguard;
}

void XLargeTextWindow::AppendText(const TCHAR* NewText)
{
    guard(XLargeTextWindow::AppendText);

    const TCHAR* CurrentText = XReconLargeGetTextData(text);
    INT startPos = 0;

    if (text.Len() > 0)
        startPos = text.Len();

    if (NewText != NULL && *NewText != 0)
    {
        INT newCount = appStrlen(NewText);
        if (newCount > 0)
            MakeAreaDirty(startPos, 0, newCount);
    }

    XTextWindow::AppendText(NewText);

    unguard;
}

void XLargeTextWindow::SetVerticalSpacing(FLOAT NewVSpace)
{
    guard(XLargeTextWindow::SetVerticalSpacing);

    if (NewVSpace < 0.0f)
        NewVSpace = 0.0f;

    if (vSpace != NewVSpace)
    {
        vSpace = NewVSpace;
        AskParentForReconfigure();
    }

    unguard;
}

void XLargeTextWindow::MakeLinesDirty(void)
{
    guard(XLargeTextWindow::MakeLinesDirty);

    textParams.bDirty = TRUE;
    queryTextParams.bDirty = TRUE;

    unguard;
}

void XLargeTextWindow::MakeAreaDirty(INT StartPos, INT OldSize, INT NewSize)
{
    guard(XLargeTextWindow::MakeAreaDirty);

    const INT deltaSize = NewSize - OldSize;

    if (textParams.dirtyCount <= 0)
    {
        textParams.dirtyStart = StartPos;
        textParams.dirtyCount = NewSize;

        if (textParams.dirtyCount < 1)
            textParams.dirtyCount = 1;

        for (INT rowIndex = 0; rowIndex < rowData.Num(); rowIndex++)
        {
            XTextRowData& row = rowData(rowIndex);

            if (row.startPos > StartPos)
            {
                row.startPos += deltaSize;

                // Original clamp: if shifting crosses the edit start, row start
                // is forced just after StartPos so the incremental rebuild will
                // not accidentally attach to stale text before the changed span.
                if (row.startPos <= StartPos)
                    row.startPos = StartPos + 1;
            }
        }
    }
    else
    {
        textParams.bDirty = TRUE;
    }

    if (queryTextParams.dirtyCount <= 0)
    {
        queryTextParams.dirtyStart = StartPos;
        queryTextParams.dirtyCount = NewSize;

        if (queryTextParams.dirtyCount < 1)
            queryTextParams.dirtyCount = 1;

        for (INT rowIndex = 0; rowIndex < queryRowData.Num(); rowIndex++)
        {
            XTextRowData& row = queryRowData(rowIndex);

            if (row.startPos > StartPos)
            {
                row.startPos += deltaSize;

                if (row.startPos <= StartPos)
                    row.startPos = StartPos + 1;
            }
        }
    }
    else
    {
        queryTextParams.bDirty = TRUE;
    }

    unguard;
}

/*-----------------------------------------------------------------------------
    Line generation.
-----------------------------------------------------------------------------*/

FLOAT XLargeTextWindow::ComputeLineHeight(XGC* gc)
{
    guard(XLargeTextWindow::ComputeLineHeight);

    if (gc == NULL)
        return 1.0f;

    FLOAT computedHeight = gc->GetFontHeight(FALSE);
    if (computedHeight < 1.0f)
        computedHeight = 1.0f;

    return computedHeight;

    unguard;
}

static FLOAT XReconLargeGetWrapWidth(XLargeTextWindow* Window, FLOAT HMargin)
{
    guard(XReconLargeGetWrapWidth);

    if (Window == NULL)
        return 0.0f;

    FLOAT WrapWidth = Window->width - (HMargin + HMargin);
    if (WrapWidth < 0.0f)
        WrapWidth = 0.0f;

    return WrapWidth;

    unguard;
}

void XLargeTextWindow::GenerateLines(
    XGC* gc,
    XTextParams& params,
    TArray<XTextRowData>& rows,
    FLOAT& outLineHeight,
    FLOAT destWidth)
{
    guard(XLargeTextWindow::GenerateLines);

    outLineHeight = ComputeLineHeight(gc);

    XTextParams newParams;
    XReconInitTextParams(newParams);
    newParams.bDirty = FALSE;
    newParams.dirtyStart = 0;
    newParams.dirtyCount = 0;
    newParams.bSpecialText = FALSE;

    if (gc != NULL && gc->IsSpecialTextEnabled() == TRUE)
        newParams.bSpecialText = TRUE;

    if (gc != NULL)
    {
        newParams.normalFont = gc->normalFont;
        newParams.boldFont = gc->boldFont;
    }

    newParams.destWidth = destWidth;

    const TCHAR* textStr = XReconLargeGetTextData(text);
    INT textLength = 0;

    if (textStr != NULL && *textStr != 0)
        textLength = appStrlen(textStr);

    UBOOL bUseIncremental = FALSE;
    if ((params.bDirty == FALSE) &&
        (params.dirtyCount > 0) &&
        (XReconTextParamsCompatible(params, newParams) == TRUE))
    {
        bUseIncremental = TRUE;
    }

    if (bUseIncremental == TRUE)
    {
        const INT dirtyStart = params.dirtyStart;
        const INT dirtyEnd = params.dirtyStart + params.dirtyCount;

        INT rebuildStartRow = 0;
        for (INT rowIndex = 0; rowIndex < rows.Num(); rowIndex++)
        {
            if (dirtyStart < rows(rowIndex).startPos)
                break;

            rebuildStartRow = rowIndex;
        }

        rebuildStartRow -= 2;
        if (rebuildStartRow < 0)
            rebuildStartRow = 0;

        INT replaceEndRow = rebuildStartRow;
        while (replaceEndRow < rows.Num() && dirtyEnd >= rows(replaceEndRow).startPos)
            replaceEndRow++;

        replaceEndRow++;
        if (replaceEndRow > rows.Num())
            replaceEndRow = rows.Num();

        XReconClearRowArray(tempRowData);

        XTextState currentState;
        XReconInitTextState(currentState);

        const TCHAR* scan = textStr;
        if (rows.Num() > 0)
        {
            scan = textStr + rows(rebuildStartRow).startPos;
            currentState = rows(rebuildStartRow).textState;
        }

        const TCHAR* textEnd = textStr + textLength;
        INT joinRow = replaceEndRow;

        while ((scan < textEnd) || (gc->HasPendingCarriageReturn(currentState) == TRUE))
        {
            const TCHAR* nextLine = NULL;
            XTextState newState;
            INT lineCount = 0;
            FLOAT xExtent = 0.0f;

            UBOOL bGotLine = gc->GetTextLine(
                scan,
                &currentState,
                INT(textEnd - scan),
                destWidth,
                &nextLine,
                &newState,
                &lineCount,
                &xExtent);

            if (bGotLine == FALSE)
                break;

            XTextRowData row;
            row.xExtent = xExtent;
            row.startPos = INT(scan - textStr);
            row.count = lineCount;
            row.totalCount = INT(nextLine - scan);
            row.textState = currentState;
            XReconAppendRow(tempRowData, row);

            scan = nextLine;
            currentState = newState;

            while (joinRow < rows.Num())
            {
                if (rows(joinRow).startPos < INT(scan - textStr))
                {
                    joinRow++;
                    continue;
                }

                if (rows(joinRow).startPos != INT(scan - textStr))
                    break;

                if (XReconTextStatesMatch(rows(joinRow).textState, currentState) == FALSE)
                {
                    joinRow++;
                    break;
                }

                goto ReconnectOldRows;
            }
        }

ReconnectOldRows:
        const INT oldCount = replaceEndRow - rebuildStartRow;
        if (oldCount > 0)
            rows.Remove(rebuildStartRow, oldCount);

        if (tempRowData.Num() > 0)
        {
            rows.Insert(rebuildStartRow, tempRowData.Num());
            for (INT tempIndex = 0; tempIndex < tempRowData.Num(); tempIndex++)
                rows(rebuildStartRow + tempIndex) = tempRowData(tempIndex);
        }

        if (rows.Num() > 0)
            rows(rows.Num() - 1).totalCount++;
    }
    else
    {
        XReconClearRowArray(rows);

        XTextState currentState;
        XReconInitTextState(currentState);

        const TCHAR* scan = textStr;
        const TCHAR* textEnd = textStr + textLength;

        while ((scan < textEnd) || (gc->HasPendingCarriageReturn(currentState) == TRUE))
        {
            const TCHAR* nextLine = NULL;
            XTextState newState;
            INT lineCount = 0;
            FLOAT xExtent = 0.0f;

            UBOOL bGotLine = gc->GetTextLine(
                scan,
                &currentState,
                INT(textEnd - scan),
                destWidth,
                &nextLine,
                &newState,
                &lineCount,
                &xExtent);

            if (bGotLine == FALSE)
                break;

            XTextRowData row;
            row.xExtent = xExtent;
            row.startPos = INT(scan - textStr);
            row.count = lineCount;
            row.totalCount = INT(nextLine - scan);
            row.textState = currentState;
            XReconAppendRow(rows, row);

            scan = nextLine;
            currentState = newState;
        }

        if (rows.Num() > 0)
            rows(rows.Num() - 1).totalCount++;
    }

    params = newParams;

    unguard;
}

void XLargeTextWindow::GetTextExtent(
    TArray<XTextRowData>& rows,
    FLOAT& inLineHeight,
    FLOAT& xExtent,
    FLOAT& yExtent)
{
    guard(XLargeTextWindow::GetTextExtent);

    xExtent = 0.0f;
    yExtent = 0.0f;

    if (rows.Num() <= 0)
    {
        yExtent = inLineHeight;
        return;
    }

    for (INT rowIndex = 0; rowIndex < rows.Num(); rowIndex++)
    {
        if (xExtent < rows(rowIndex).xExtent)
            xExtent = rows(rowIndex).xExtent;
    }

    yExtent = FLOAT(rows.Num()) * inLineHeight;
    if (rows.Num() > 1)
        yExtent += FLOAT(rows.Num() - 1) * vSpace;

    unguard;
}

/*-----------------------------------------------------------------------------
    XWindow callbacks.
-----------------------------------------------------------------------------*/

void XLargeTextWindow::ParentRequestedPreferredSize(
    UBOOL bWidthSpecified,
    FLOAT& preferredWidth,
    UBOOL bHeightSpecified,
    FLOAT& preferredHeight)
{
    guard(XLargeTextWindow::ParentRequestedPreferredSize);

    XGC* gc = GetGC();
    if (gc != NULL)
    {
        FLOAT usableWidth = 0.0f;

        if (bWidthSpecified == TRUE && bWordWrap == TRUE)
        {
            usableWidth = preferredWidth - (hMargin + hMargin);
            if (usableWidth < 0.0f)
                usableWidth = 0.0f;
        }

        FLOAT queryLineHeight = 0.0f;
        GenerateLines(gc, queryTextParams, queryRowData, queryLineHeight, usableWidth);
        GetTextExtent(queryRowData, queryLineHeight, preferredWidth, preferredHeight);

        if (bHeightSpecified == FALSE)
        {
            if (minLines >= 0 || maxLines >= 0)
            {
                FLOAT baseHeight = gc->GetFontHeight(TRUE);

                if (minLines >= 0 && minLines > queryRowData.Num())
                    preferredHeight = FLOAT(minLines) * baseHeight + FLOAT(minLines - 1) * vSpace;

                if (maxLines >= 0 && queryRowData.Num() > maxLines)
                    preferredHeight = FLOAT(maxLines) * baseHeight + FLOAT(maxLines - 1) * vSpace;
            }
        }

        preferredWidth += hMargin + hMargin;
        preferredHeight += vMargin + vMargin;

        if (bWidthSpecified == FALSE && preferredWidth < minWidth)
            preferredWidth = minWidth;

        ReleaseGC(gc);
    }

    unguard;
}

void XLargeTextWindow::ParentRequestedGranularity(FLOAT& hGranularity, FLOAT& vGranularity)
{
    guard(XLargeTextWindow::ParentRequestedGranularity);

    XGC* gc = GetGC();
    if (gc != NULL)
    {
        FLOAT computedHeight = ComputeLineHeight(gc);
        vGranularity = computedHeight + vSpace;
        hGranularity = 1.0f;
        ReleaseGC(gc);
    }

    XWindow::ParentRequestedGranularity(hGranularity, vGranularity);

    unguard;
}

void XLargeTextWindow::ConfigurationChanged(void)
{
    guard(XLargeTextWindow::ConfigurationChanged);

    XGC* gc = GetGC();
    if (gc != NULL)
    {
        FLOAT usableWidth = 0.0f;

        if (bWordWrap == TRUE)
            usableWidth = XReconLargeGetWrapWidth(this, hMargin);

        GenerateLines(gc, textParams, rowData, lineHeight, usableWidth);
        ReleaseGC(gc);
    }

    unguard;
}

void XLargeTextWindow::Draw(XGC* gc)
{
    guard(XLargeTextWindow::Draw);

    XWindow::Draw(gc);

    if (gc != NULL)
    {
        FLOAT usableWidth = 0.0f;
        if (bWordWrap == TRUE)
            usableWidth = XReconLargeGetWrapWidth(this, hMargin);

        UBOOL bNeedRows = FALSE;
        if (textParams.bDirty == TRUE)
            bNeedRows = TRUE;
        else if (textParams.destWidth != usableWidth)
            bNeedRows = TRUE;
        else if (textParams.normalFont != gc->normalFont)
            bNeedRows = TRUE;
        else if (textParams.boldFont != gc->boldFont)
            bNeedRows = TRUE;
        else if (textParams.bSpecialText != gc->IsSpecialTextEnabled())
            bNeedRows = TRUE;

        if (bNeedRows == TRUE)
            GenerateLines(gc, textParams, rowData, lineHeight, usableWidth);
    }

    const TCHAR* textStr = XReconLargeGetTextData(text);
    if (textStr == NULL)
        return;

    const INT rowCount = rowData.Num();
    FLOAT fullTextHeight = FLOAT(rowCount) * lineHeight;
    if (rowCount > 1)
        fullTextHeight += FLOAT(rowCount - 1) * vSpace;

    FLOAT textY = vMargin;
    if (vAlign == VALIGN_Bottom)
    {
        textY = height - fullTextHeight - vMargin;
    }
    else if (vAlign == VALIGN_Center)
    {
        textY = FLOAT(INT((height - fullTextHeight) * 0.5f));
    }

    const FLOAT rowStep = lineHeight + vSpace;
    INT firstRow = INT(((clipRect.clipY + 0.00001f) - textY) / rowStep);
    if (firstRow < 0)
        firstRow = 0;

    INT endRow = appCeil((clipRect.clipY + clipRect.clipHeight - textY) / rowStep);
    if (endRow > rowCount)
        endRow = rowCount;

    XClipRect oldClip = gc->clipRect;
    gc->Intersect(hMargin, vMargin, width - (hMargin + hMargin), height - (vMargin + vMargin));

    FLOAT drawY = textY + FLOAT(firstRow) * rowStep;

    for (INT rowIndex = firstRow; rowIndex < endRow; rowIndex++)
    {
        FLOAT drawX = hMargin;

        if (hAlign == HALIGN_Right)
            drawX = width - rowData(rowIndex).xExtent - hMargin;
        else if (hAlign == HALIGN_Center)
            drawX = FLOAT(INT((width - rowData(rowIndex).xExtent) * 0.5f));

        gc->DrawTextLine(
            drawX,
            drawY,
            &rowData(rowIndex).textState,
            textStr + rowData(rowIndex).startPos,
            rowData(rowIndex).count);

        drawY += rowStep;
    }

    gc->SetClipRect(oldClip);

    unguard;
}

/*-----------------------------------------------------------------------------
    Intrinsics.
-----------------------------------------------------------------------------*/

void XLargeTextWindow::execSetVerticalSpacing(FFrame& Stack, RESULT_DECL)
{
    guard(XLargeTextWindow::execSetVerticalSpacing);

    P_GET_FLOAT_OPTX(NewVSpace, 1.0f);
    P_FINISH;

    SetVerticalSpacing(NewVSpace);

    unguardexec;
}
