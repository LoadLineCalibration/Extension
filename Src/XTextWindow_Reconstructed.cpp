/*=============================================================================
	XTextWindow_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 03.

	Focus: native TextWindow behaviour, preferred-size negotiation, accelerator
	binding and draw-time GC setup.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XTextWindow);

/*-----------------------------------------------------------------------------
	Small helpers.
-----------------------------------------------------------------------------*/

static const TCHAR* XReconGetTextData(const FStringNoInit& Text)
{
	const TCHAR* Result = *Text;
	if (Result == NULL)
		Result = TEXT("");
	return Result;
}

static void XReconAssignString(FStringNoInit& Dest, const TCHAR* Source)
{
	if (Source == NULL)
		Source = TEXT("");
	Dest = Source;
}

struct XReconRawStringArray
{
	void* Data;
	INT ArrayNum;
	INT ArrayMax;
};

static XReconRawStringArray* XReconGetRawStringArray(FStringNoInit& Text)
{
	return (XReconRawStringArray*)((void*)&Text);
}

/*-----------------------------------------------------------------------------
	Construction / destruction.
-----------------------------------------------------------------------------*/

XTextWindow::XTextWindow(XWindow* Parent)
	: XWindow(Parent)
{
}

void XTextWindow::Init(XWindow* Parent)
{
	guard(XTextWindow::Init);

	XWindow::Init(Parent);

	// Original Extension.dll initializes TextWindow drawing alignment to center/center.
	// ToolButtonWindow.DrawWindow() relies on this GC state and passes the
	// whole button rectangle to GC.DrawText(), without doing its own centering.
	hAlign = HALIGN_Center;
	vAlign = VALIGN_Center;

	bWordWrap = TRUE;
	bTextIsAccelerator = FALSE;

	hMargin = 3.0f;
	vMargin = 3.0f;

	minLines = -1;
	maxLines = -1;
	minWidth = 0.0f;

	XReconAssignString(text, TEXT(""));
	EnableTextAsAccelerator(TRUE);

	unguard;
}

void XTextWindow::CleanUp(void)
{
	guard(XTextWindow::CleanUp);

	text.Empty();

	XWindow::CleanUp();

	unguard;
}

void XTextWindow::Serialize(FArchive& Ar)
{
	guard(XTextWindow::Serialize);

	XWindow::Serialize(Ar);
	Ar << text;

	unguard;
}

/*-----------------------------------------------------------------------------
	Text state.
-----------------------------------------------------------------------------*/

void XTextWindow::SetText(const TCHAR* NewText)
{
	guard(XTextWindow::SetText);

	if (NewText == NULL)
		NewText = TEXT("");

	const TCHAR* OldText = XReconGetTextData(text);
	if (appStricmp(OldText, NewText) != 0)
	{
		if (OldText != NewText)
			XReconAssignString(text, NewText);

		if (bTextIsAccelerator == TRUE)
			SetAcceleratorText(NewText);

		AskParentForReconfigure();
	}

	unguard;
}

void XTextWindow::AppendText(const TCHAR* NewText)
{
	guard(XTextWindow::AppendText);

	if (NewText == NULL)
		NewText = TEXT("");

	const INT AddLen = appStrlen(NewText);
	XReconRawStringArray* TextArray = XReconGetRawStringArray(text);
	INT OldNum = TextArray->ArrayNum;

	if (OldNum > 0 && TextArray->Data != NULL)
	{
		const INT NewNum = OldNum + AddLen;

		if (NewNum > TextArray->ArrayMax)
		{
			const INT NewMax = 3 * NewNum / 8 + NewNum + 32;
			void* NewData = appMalloc(NewMax * sizeof(TCHAR), TEXT("XTextWindowText"));

			if (NewData != NULL)
			{
				appMemcpy(NewData, TextArray->Data, OldNum * sizeof(TCHAR));
				appFree(TextArray->Data);
				TextArray->Data = NewData;
				TextArray->ArrayMax = NewMax;
			}
		}

		TextArray->ArrayNum = NewNum;

		if (TextArray->Data != NULL)
			appStrcpy(((TCHAR*)TextArray->Data) + OldNum - 1, NewText);
	}
	else if (*NewText)
	{
		const INT NewNum = AddLen + 1;
		const INT NewMax = 3 * NewNum / 8 + NewNum + 32;

		TextArray->Data = appMalloc(NewMax * sizeof(TCHAR), TEXT("XTextWindowText"));
		TextArray->ArrayNum = NewNum;
		TextArray->ArrayMax = NewMax;

		if (TextArray->Data != NULL)
			appStrcpy((TCHAR*)TextArray->Data, NewText);
	}

	if (bTextIsAccelerator == TRUE)
		SetAcceleratorText(XReconGetTextData(text));

	AskParentForReconfigure();

	unguard;
}

void XTextWindow::SetWordWrap(UBOOL bNewWordWrap)
{
	guard(XTextWindow::SetWordWrap);

	if ((UBOOL)bWordWrap != bNewWordWrap)
	{
		bWordWrap = bNewWordWrap;
		AskParentForReconfigure();
	}

	unguard;
}

void XTextWindow::SetTextAlignments(EHAlign NewHAlign, EVAlign NewVAlign)
{
	hAlign = NewHAlign;
	vAlign = NewVAlign;
}

void XTextWindow::SetTextMargins(FLOAT NewHMargin, FLOAT NewVMargin)
{
	guard(XTextWindow::SetTextMargins);

	if (hMargin != NewHMargin || vMargin != NewVMargin)
	{
		hMargin = NewHMargin;
		vMargin = NewVMargin;
		AskParentForReconfigure();
	}

	unguard;
}

void XTextWindow::SetLines(INT NewMinLines, INT NewMaxLines)
{
	guard(XTextWindow::SetLines);

	if (NewMinLines < 0)
		NewMinLines = 0;

	if (NewMaxLines < 0)
		NewMaxLines = 0;

	if (NewMinLines > NewMaxLines)
		NewMinLines = NewMaxLines;

	if (minLines != NewMinLines || maxLines != NewMaxLines)
	{
		minLines = NewMinLines;
		maxLines = NewMaxLines;
		AskParentForReconfigure();
	}

	unguard;
}

void XTextWindow::SetMinLines(INT NewMinLines)
{
	guard(XTextWindow::SetMinLines);

	if (NewMinLines < 0)
		NewMinLines = 0;

	if (NewMinLines > maxLines)
		NewMinLines = maxLines;

	if (minLines != NewMinLines)
	{
		minLines = NewMinLines;
		AskParentForReconfigure();
	}

	unguard;
}

void XTextWindow::SetMaxLines(INT NewMaxLines)
{
	guard(XTextWindow::SetMaxLines);

	if (NewMaxLines < 0)
		NewMaxLines = 0;

	if (NewMaxLines < minLines)
		NewMaxLines = minLines;

	if (maxLines != NewMaxLines)
	{
		maxLines = NewMaxLines;
		AskParentForReconfigure();
	}

	unguard;
}

void XTextWindow::ResetLines(void)
{
	guard(XTextWindow::ResetLines);

	if (minLines >= 0 || maxLines >= 0)
	{
		minLines = -1;
		maxLines = -1;
		AskParentForReconfigure();
	}

	unguard;
}

void XTextWindow::SetMinWidth(FLOAT NewMinWidth)
{
	guard(XTextWindow::SetMinWidth);

	if (NewMinWidth < 0.0f)
		NewMinWidth = 0.0f;

	if (minWidth != NewMinWidth)
	{
		minWidth = NewMinWidth;
		AskParentForReconfigure();
	}

	unguard;
}

void XTextWindow::ResetMinWidth(void)
{
	guard(XTextWindow::ResetMinWidth);

	if (minWidth > 0.0f)
	{
		minWidth = 0.0f;
		AskParentForReconfigure();
	}

	unguard;
}

void XTextWindow::EnableTextAsAccelerator(UBOOL bEnable)
{
	guard(XTextWindow::EnableTextAsAccelerator);

	if ((UBOOL)bTextIsAccelerator != bEnable)
	{
		bTextIsAccelerator = bEnable;

		if (bTextIsAccelerator == TRUE)
			SetAcceleratorText(XReconGetTextData(text));
		else
			SetAcceleratorText(NULL);
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Layout negotiation.
-----------------------------------------------------------------------------*/

void XTextWindow::ParentRequestedPreferredSize(
	UBOOL bWidthSpecified,
	FLOAT& PreferredWidth,
	UBOOL bHeightSpecified,
	FLOAT& PreferredHeight)
{
	guard(XTextWindow::ParentRequestedPreferredSize);

	XGC* GC = GetGC();
	if (GC == NULL)
		return;

	const TCHAR* CurrentText = XReconGetTextData(text);
	const UBOOL bHasText = appStricmp(CurrentText, TEXT("")) != 0;

	if (bHasText == TRUE)
	{
		if (bHeightSpecified == TRUE || (minLines < 0 && maxLines < 0))
		{
			if (bWidthSpecified == TRUE && bWordWrap == TRUE)
			{
				FLOAT TextWidth = PreferredWidth - hMargin - hMargin;
				GC->GetTextExtent(TextWidth, PreferredWidth, PreferredHeight, CurrentText);
			}
			else
			{
				GC->GetTextExtent(0.0f, PreferredWidth, PreferredHeight, CurrentText);
			}
		}
		else
		{
			if (bWidthSpecified == TRUE && bWordWrap == TRUE)
			{
				FLOAT QueryWidth = PreferredWidth - hMargin - hMargin;
				GC->GetTextExtent(QueryWidth, PreferredWidth, PreferredHeight, CurrentText);
			}
			else
			{
				GC->GetTextExtent(0.0f, PreferredWidth, PreferredHeight, CurrentText);
			}

			FLOAT FontHeight = GC->GetFontHeight(TRUE);
			FLOAT NewHeight = PreferredHeight;

			if (minLines >= 0)
			{
				FLOAT MinHeight = FLOAT(minLines) * FontHeight;
				if (MinHeight > NewHeight)
					NewHeight = MinHeight;
			}

			if (maxLines >= 0)
			{
				FLOAT MaxHeight = FLOAT(maxLines) * FontHeight;
				if (NewHeight > MaxHeight)
					NewHeight = MaxHeight;
			}

			PreferredHeight = NewHeight;
		}

		PreferredWidth += hMargin + hMargin;
		PreferredHeight += vMargin + vMargin;
	}
	else
	{
		if (background != NULL)
		{
			PreferredWidth = FLOAT(background->USize);
			PreferredHeight = FLOAT(background->VSize);

			if (bHeightSpecified == FALSE)
			{
				if (minLines >= 0)
					PreferredHeight = FLOAT(minLines) * GC->GetFontHeight(TRUE);
				else if (maxLines >= 0)
					PreferredHeight = FLOAT(maxLines) * GC->GetFontHeight(TRUE);
			}
		}
		else
		{
			XWindow::ParentRequestedPreferredSize(
				bWidthSpecified,
				PreferredWidth,
				bHeightSpecified,
				PreferredHeight);
		}
	}

	if (bWidthSpecified == FALSE && PreferredWidth < minWidth)
		PreferredWidth = minWidth;

	ReleaseGC(GC);

	unguard;
}

void XTextWindow::ParentRequestedGranularity(FLOAT& HGranularity, FLOAT& VGranularity)
{
	guard(XTextWindow::ParentRequestedGranularity);

	XGC* GC = GetGC();
	if (GC != NULL)
	{
		VGranularity = GC->GetFontHeight(TRUE);
		HGranularity = 1.0f;
		ReleaseGC(GC);
	}

	XWindow::ParentRequestedGranularity(HGranularity, VGranularity);

	unguard;
}

/*-----------------------------------------------------------------------------
	Drawing.
-----------------------------------------------------------------------------*/

void XTextWindow::Draw(XGC* GC)
{
	guard(XTextWindow::Draw);

	GC->SetAlignments((EHAlign)hAlign, (EVAlign)vAlign);
	GC->EnableWordWrap(bWordWrap);

	XWindow::Draw(GC);

	GC->DrawText(
		hMargin,
		vMargin,
		width - hMargin - hMargin,
		height - vMargin - vMargin,
		XReconGetTextData(text));

	unguard;
}

/*-----------------------------------------------------------------------------
	Native wrappers.

	The decompile shows these ids:
	1550 SetText, 1551 AppendText, 1552 GetText, 1553 GetTextLength,
	1554 GetTextPart, 1555 SetWordWrap, 1556 SetTextAlignments,
	1557 SetTextMargins, 1558 SetLines, 1559 SetMinLines, 1560 SetMaxLines,
	1561 ResetLines, 1562 SetMinWidth, 1563 ResetMinWidth,
	1564 EnableTextAsAccelerator.
-----------------------------------------------------------------------------*/

