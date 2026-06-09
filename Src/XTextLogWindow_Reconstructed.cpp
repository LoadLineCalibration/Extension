/*=============================================================================
    XTextLogWindow_Reconstructed.cpp
    Reconstructed from Extension.dll.c, pass 08.

    Focus: TextLogWindow log storage, timeout removal, regenerated combined text,
    auto-bottom drawing and line-by-line color handling.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XTextLogWindow);

/*-----------------------------------------------------------------------------
    Local helpers.
-----------------------------------------------------------------------------*/

static const TCHAR* XReconTextLogGetString(const FString& Text)
{
    const TCHAR* Result = *Text;
    if (Result == NULL)
        Result = TEXT("");
    return Result;
}

static void XReconTextLogAssignString(FString& Dest, const TCHAR* Source)
{
    if (Source == NULL)
        Source = TEXT("");
    Dest = Source;
}

static void XReconTextLogAppendString(FString& Dest, const TCHAR* Source)
{
    if (Source == NULL)
        Source = TEXT("");

    if (*Source == 0)
        return;

    Dest += Source;
}

static void XReconTextLogDestroyLines(TArray<XTextLogLine>& Lines)
{
    Lines.Empty();
}

/*-----------------------------------------------------------------------------
    Construction / destruction.
-----------------------------------------------------------------------------*/

XTextLogWindow::XTextLogWindow(XWindow* Parent)
    : XTextWindow(Parent)
{
}

void XTextLogWindow::Init(XWindow* Parent)
{
    guard(XTextLogWindow::Init);

    XTextWindow::Init(Parent);

    bTooTall = FALSE;
    bAutoVertSize = FALSE;
    bPaused = FALSE;

    textTimeout = 5.0f;
    XReconTextLogDestroyLines(lines);

    // The native Init clears one inherited draw/background bit.  In practice it
    // prevents the log from behaving like a plain TextWindow background block.
    bDrawRawBackground = FALSE;

    unguard;
}

void XTextLogWindow::CleanUp(void)
{
    guard(XTextLogWindow::CleanUp);

    // Original Extension.dll does not clear the log line array in CleanUp();
    // line storage is owned by normal UObject/member destruction.  Keeping the
    // cached lines here matters if CleanUp() is called before final destruction.
    XTextWindow::CleanUp();

    unguard;
}

void XTextLogWindow::Serialize(FArchive& Ar)
{
    guard(XTextLogWindow::Serialize);

    XTextWindow::Serialize(Ar);

    unguard;
}

/*-----------------------------------------------------------------------------
    Public interface.
-----------------------------------------------------------------------------*/

void XTextLogWindow::AddLog(const TCHAR* NewText, FColor lineCol)
{
    guard(XTextLogWindow::AddLog);

    if (NewText == NULL)
        NewText = TEXT("");

    XTextLogLine line;
    line.timeLeft = GetTickOffset() + textTimeout;
    XReconTextLogAssignString(line.lineStr, NewText);
    line.lineCol = lineCol;

    new(lines) XTextLogLine(line);

    RegenerateText();

    unguard;
}

void XTextLogWindow::ClearLog(void)
{
    guard(XTextLogWindow::ClearLog);

    XReconTextLogDestroyLines(lines);
    RegenerateText();

    unguard;
}

void XTextLogWindow::SetTextTimeout(FLOAT TimeoutSecs)
{
    textTimeout = TimeoutSecs;
}

void XTextLogWindow::PauseLog(UBOOL bNewPauseState)
{
    bPaused = bNewPauseState;
}

/*-----------------------------------------------------------------------------
    XWindow callbacks.
-----------------------------------------------------------------------------*/

void XTextLogWindow::ConfigurationChanged(void)
{
    guard(XTextLogWindow::ConfigurationChanged);

    XGC* gc = GetGC();
    if (gc != NULL)
    {
        const TCHAR* textStr = XReconTextLogGetString(text);
        FLOAT textWidth = 0.0f;
        FLOAT textHeight = 0.0f;
        FLOAT usableWidth = width - (hMargin + hMargin);

        gc->GetTextExtent(usableWidth, textWidth, textHeight, textStr);

        if (height - (vMargin + vMargin) >= textHeight)
            bTooTall = FALSE;
        else
            bTooTall = TRUE;

        ReleaseGC(gc);
    }

    ProcessScript(EXTENSION_ConfigurationChanged, NULL, FALSE);

    unguard;
}

void XTextLogWindow::Draw(XGC* gc)
{
    guard(XTextLogWindow::Draw);

    gc->SetHorizontalAlignment(EHAlign(hAlign));

    if (bTooTall == TRUE)
        gc->SetVerticalAlignment(VALIGN_Bottom);
    else
        gc->SetVerticalAlignment(EVAlign(vAlign));

    gc->EnableWordWrap(TRUE);

    FLOAT drawY = vMargin;
    INT startIndex = 0;
    INT step = 1;

    if (bTooTall == TRUE)
    {
        startIndex = lines.Num() - 1;
        step = -1;
    }

    INT lineIndex = startIndex;
    while (lineIndex < lines.Num() && lineIndex >= 0)
    {
        // Original copies each log line into inherited text storage before
        // measuring/drawing it.  This is weird, but important: drawing uses the
        // exact same GC text routines as TextWindow and therefore keeps all
        // metachar/wrap behaviour.
        XReconTextLogAssignString(text, XReconTextLogGetString(lines(lineIndex).lineStr));

        const TCHAR* lineText = XReconTextLogGetString(text);
        FLOAT xExtent = 0.0f;
        FLOAT yExtent = 0.0f;
        FLOAT usableWidth = width - (hMargin + hMargin);
        FLOAT usableHeight = height - (vMargin + vMargin);

        gc->GetTextExtent(usableWidth, xExtent, yExtent, lineText);
        gc->SetTextColor(lines(lineIndex).lineCol);
        gc->DrawText(hMargin, drawY, usableWidth, usableHeight, lineText);

        if (bTooTall == TRUE)
            drawY -= yExtent;
        else
            drawY += yExtent;

        lineIndex += step;
    }

    unguard;
}

void XTextLogWindow::Tick(FLOAT DeltaSeconds)
{
    guard(XTextLogWindow::Tick);

    if (bPaused == FALSE)
    {
        for (INT lineIndex = 0; lineIndex < lines.Num(); lineIndex++)
            lines(lineIndex).timeLeft -= DeltaSeconds;

        INT removeCount = 0;
        while (removeCount < lines.Num())
        {
            if (lines(removeCount).timeLeft > 0.0f)
                break;

            removeCount++;
        }

        if (removeCount > 0)
        {
            lines.Remove(0, removeCount);
            RegenerateText();
        }
    }

    unguard;
}

/*-----------------------------------------------------------------------------
    Internal regeneration.
-----------------------------------------------------------------------------*/

void XTextLogWindow::RegenerateText(void)
{
    guard(XTextLogWindow::RegenerateText);

    XReconTextLogAssignString(text, TEXT(""));

    for (INT lineIndex = 0; lineIndex < lines.Num(); lineIndex++)
    {
        if (lineIndex > 0)
            XReconTextLogAppendString(text, TEXT("\n"));

        XReconTextLogAppendString(text, XReconTextLogGetString(lines(lineIndex).lineStr));
    }

    AskParentForReconfigure();

    unguard;
}

/*-----------------------------------------------------------------------------
    Intrinsics.
-----------------------------------------------------------------------------*/

void XTextLogWindow::execAddLog(FFrame& Stack, RESULT_DECL)
{
    guard(XTextLogWindow::execAddLog);

    P_GET_STR(NewText);
    P_GET_STRUCT(FColor, LineColor);
    P_FINISH;

    AddLog(*NewText, LineColor);

    unguardexec;
}

void XTextLogWindow::execClearLog(FFrame& Stack, RESULT_DECL)
{
    guard(XTextLogWindow::execClearLog);

    P_FINISH;
    ClearLog();

    unguardexec;
}

void XTextLogWindow::execSetTextTimeout(FFrame& Stack, RESULT_DECL)
{
    guard(XTextLogWindow::execSetTextTimeout);

    P_GET_FLOAT(NewTimeout);
    P_FINISH;

    SetTextTimeout(NewTimeout);

    unguardexec;
}

void XTextLogWindow::execPauseLog(FFrame& Stack, RESULT_DECL)
{
    guard(XTextLogWindow::execPauseLog);

    P_GET_UBOOL(bNewPauseState);
    P_FINISH;

    PauseLog(bNewPauseState);

    unguardexec;
}
