/*=============================================================================
	XGC_Text_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 02.

	Focus: text parsing, wrapping, measurement and glyph drawing.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

/*-----------------------------------------------------------------------------
	Local helpers preserved from the native behavior.
-----------------------------------------------------------------------------*/

static FColor GReconTextPalette[8] =
{
	FColor(0,   0,   0),
	FColor(255, 255, 255),
	FColor(255, 0,   0),
	FColor(0,   255, 0),
	FColor(255, 255, 0),
	FColor(0,   0,   255),
	FColor(255, 0,   255),
	FColor(0,   255, 255)
};


static FFontCharacter GReconTextMissingFontCharacter;

static FFontCharacter* XReconGetFontCharacter(UFont* Font, TCHAR Ch, UTexture** OutTexture)
{
	INT PageIndex = 0;
	INT CharIndex = 0;

	if (Font != NULL && Font->CharactersPerPage > 0)
	{
		PageIndex = INT(Ch) / Font->CharactersPerPage;
		CharIndex = INT(Ch) % Font->CharactersPerPage;

		if (PageIndex >= 0 && PageIndex < Font->Pages.Num())
		{
			if (CharIndex >= 0 && CharIndex < Font->Pages(PageIndex).Characters.Num())
			{
				if (OutTexture != NULL)
					*OutTexture = Font->Pages(PageIndex).Texture;

				return &Font->Pages(PageIndex).Characters(CharIndex);
			}
		}
	}

	if (OutTexture != NULL)
		*OutTexture = NULL;

	GReconTextMissingFontCharacter.StartU = 0;
	GReconTextMissingFontCharacter.StartV = 0;
	GReconTextMissingFontCharacter.USize = 0;
	GReconTextMissingFontCharacter.VSize = 0;
	return &GReconTextMissingFontCharacter;
}

static inline FPlane XReconColorToPlane(const FColor& Color)
{
	return FPlane(
		FLOAT(Color.R) * (1.0f / 255.0f),
		FLOAT(Color.G) * (1.0f / 255.0f),
		FLOAT(Color.B) * (1.0f / 255.0f),
		0.0f);
}

static inline UBOOL XReconIsWhitespace(TCHAR Ch)
{
	if (Ch == TEXT(' '))
		return TRUE;

	if (Ch >= 9 && Ch <= 13)
		return TRUE;

	return FALSE;
}

static inline TCHAR XReconLowerAscii(TCHAR Ch)
{
	if (Ch >= TEXT('A') && Ch <= TEXT('Z'))
		return Ch + 32;

	return Ch;
}

static inline INT XReconRoundToCanvas(FLOAT Value)
{
	return INT(Value + 0.1f);
}

/*-----------------------------------------------------------------------------
	Hex color parser for |cRRGGBB.

	Original quirk: first hex digit is stored as 16 * raw character code and the
	second digit ORs the corrected nibble. For valid two-digit bytes the result is
	identical to ordinary hex parsing because the low byte of '0' * 16 is 0.
-----------------------------------------------------------------------------*/

UBOOL XGC::GetColorByte(const TCHAR*& pStr, BYTE& byteVal, const TCHAR*& pEnd)
{
	guard(XGC::GetColorByte);

	byteVal = 0;

	if (pStr >= pEnd)
		return FALSE;

	TCHAR First = *pStr;
	BYTE HighNibble = 0;

	if (First >= TEXT('0') && First <= TEXT('9'))
	{
		HighNibble = BYTE(First - TEXT('0'));
	}
	else if (First >= TEXT('a') && First <= TEXT('f'))
	{
		HighNibble = BYTE(First - TEXT('a') + 10);
	}
	else if (First >= TEXT('A') && First <= TEXT('F'))
	{
		HighNibble = BYTE(First - TEXT('A') + 10);
	}
	else
	{
		return FALSE;
	}

	byteVal = BYTE(HighNibble << 4);
	pStr++;

	if (pStr >= pEnd)
		return FALSE;

	TCHAR Second = *pStr;
	BYTE LowNibble = 0;

	if (Second >= TEXT('0') && Second <= TEXT('9'))
	{
		LowNibble = BYTE(Second - TEXT('0'));
	}
	else if (Second >= TEXT('a') && Second <= TEXT('f'))
	{
		LowNibble = BYTE(Second - TEXT('a') + 10);
	}
	else if (Second >= TEXT('A') && Second <= TEXT('F'))
	{
		LowNibble = BYTE(Second - TEXT('A') + 10);
	}
	else
	{
		return FALSE;
	}

	byteVal = BYTE(byteVal | LowNibble);
	pStr++;

	return TRUE;

	unguard;
}

void XGC::ReadColor(const TCHAR*& pStr, FPlane& plane, const TCHAR*& pEnd)
{
	guard(XGC::ReadColor);

	BYTE Red = 0;
	BYTE Green = 0;
	BYTE Blue = 0;

	if (GetColorByte(pStr, Red, pEnd) == TRUE)
	{
		if (GetColorByte(pStr, Green, pEnd) == TRUE)
		{
			GetColorByte(pStr, Blue, pEnd);
		}
	}

	plane.X = FLOAT(Red) * (1.0f / 255.0f);
	plane.Y = FLOAT(Green) * (1.0f / 255.0f);
	plane.Z = FLOAT(Blue) * (1.0f / 255.0f);

	unguard;
}

/*-----------------------------------------------------------------------------
	Special text parser.

	Meta prefix is '|'. Recognized forms:
		|b      enable bold
		|!b     disable bold
		|cRRGGBB set RGB color
		|!c     reset to GC text color
		|p0..7  set palette color
		|!p     reset to GC text color
		|&      underline next emitted character only

	Important parity detail: original XGC::GetNextChar does not treat |n
	as a newline. Script text conversion is handled by XWindow::ConvertScriptString
	before text reaches widgets that need real line feeds. Here |n is just an
	unknown meta command: the pipe is swallowed and 'n' is emitted.

	Unknown meta commands swallow the pipe and emit the command character.
-----------------------------------------------------------------------------*/

TCHAR XGC::GetNextChar(const TCHAR*& pStr, XTextState& textState, const TCHAR*& pEnd)
{
	guard(XGC::GetNextChar);

	textState.bUnderline = FALSE;

	while (pStr < pEnd)
	{
		if ((bParseMetachars == FALSE) || (*pStr != TEXT('|')))
		{
			TCHAR Result = *pStr;
			pStr++;
			return Result;
		}

		pStr++;

		if (pStr >= pEnd)
			return 0;

		TCHAR Command = XReconLowerAscii(*pStr);
		UBOOL bInverse = FALSE;

		if (Command == TEXT('!'))
		{
			bInverse = TRUE;
			pStr++;

			if (pStr >= pEnd)
				return 0;

			Command = XReconLowerAscii(*pStr);
		}

		if (pStr >= pEnd)
			return 0;

		if (Command == TEXT('b'))
		{
			pStr++;
			if (bInverse == TRUE)
				textState.bBold = FALSE;
			else
				textState.bBold = TRUE;
		}
		else if (Command == TEXT('c'))
		{
			pStr++;
			if (bInverse == TRUE)
			{
				textState.plane = textPlane;
				textState.bPlaneSet = FALSE;
			}
			else
			{
				ReadColor(pStr, textState.plane, pEnd);
				textState.bPlaneSet = TRUE;
			}
		}
		else if (Command == TEXT('p'))
		{
			pStr++;
			if (pStr < pEnd)
			{
				if (bInverse == TRUE)
				{
					textState.plane = textPlane;
					textState.bPlaneSet = FALSE;
				}
				else
				{
					TCHAR PaletteChar = *pStr;
					if (PaletteChar >= TEXT('0') && PaletteChar < TEXT('8'))
					{
						INT PaletteIndex = INT(PaletteChar - TEXT('0'));
						pStr++;
						textState.plane = XReconColorToPlane(GReconTextPalette[PaletteIndex]);
						textState.bPlaneSet = TRUE;
					}
				}
			}
		}
		else if (Command == TEXT('&'))
		{
			pStr++;
			textState.bUnderline = TRUE;
		}
		else
		{
			TCHAR Result = *pStr;
			pStr++;
			return Result;
		}
	}

	return 0;

	unguard;
}

/*-----------------------------------------------------------------------------
	Character/pixel conversion used by edit/list widgets.
-----------------------------------------------------------------------------*/

void XGC::ConvertPixelPos(const TCHAR*& pStr, INT& pos, FLOAT& pixel, UBOOL bDir,
	INT count, XTextState* pTextState)
{
	guard(XGC::ConvertPixelPos);

	if (pStr == NULL)
	{
		if (bDir == TRUE)
			pixel = 0.0f;
		else
			pos = 0;
		return;
	}

	if (count < 0)
		count = appStrlen(pStr);

	const TCHAR* Start = pStr;
	const TCHAR* Scan = pStr;
	const TCHAR* End = pStr + count;

	XTextState State;
	if (pTextState != NULL)
		State = *pTextState;

	if (State.bPlaneSet == FALSE)
		State.plane = textPlane;

	INT LastPos = 0;
	FLOAT Accum = 0.0f;

	if (bDir == TRUE)
	{
		while (TRUE)
		{
			TCHAR Ch = GetNextChar(Scan, State, End);
			if (Ch == 0 || Ch == 10 || LastPos >= pos)
				break;

			LastPos = INT(Scan - Start);

			UFont* Font = normalFont;
			if (State.bBold == TRUE)
				Font = boldFont;

			if (Font != NULL)
			{
				INT CharWidth = 0;
				GetCharSize(Font, Ch, &CharWidth, NULL);
				Accum += FLOAT(CharWidth);
			}
		}

		pixel = Accum;
	}
	else
	{
		while (TRUE)
		{
			TCHAR Ch = GetNextChar(Scan, State, End);
			if (Ch == 0 || Ch == 10)
				break;

			UFont* Font = normalFont;
			if (State.bBold == TRUE)
				Font = boldFont;

			FLOAT CharWidth = 0.0f;
			if (Font != NULL)
			{
				INT IntWidth = 0;
				GetCharSize(Font, Ch, &IntWidth, NULL);
				CharWidth = FLOAT(IntWidth);
			}

			if ((Accum + CharWidth * 0.5f) > pixel)
				break;

			Accum += CharWidth;
			LastPos = INT(Scan - Start);

			if (pixel <= Accum)
				break;
		}

		pos = LastPos;
	}

	unguard;
}

UBOOL XGC::HasPendingCarriageReturn(const XTextState& textState) const
{
	guard(XGC::HasPendingCarriageReturn);
	return (textState.bCarriageReturn == TRUE);
	unguard;
}

FLOAT XGC::CharToPixel(const TCHAR* textStr, INT textPos, INT count, XTextState* textState)
{
	guard(XGC::CharToPixel);

	FLOAT Pixel = 0.0f;
	const TCHAR* Scan = textStr;
	ConvertPixelPos(Scan, textPos, Pixel, TRUE, count, textState);
	return Pixel;

	unguard;
}

INT XGC::PixelToChar(const TCHAR* textStr, FLOAT pixel, INT count, XTextState* textState)
{
	guard(XGC::PixelToChar);

	INT Pos = 0;
	const TCHAR* Scan = textStr;
	ConvertPixelPos(Scan, Pos, pixel, FALSE, count, textState);
	return Pos;

	unguard;
}

/*-----------------------------------------------------------------------------
	Line parser.

	Important: returned count is source-character count, including meta sequences.
	That is why edit widgets can keep script string indices stable while the visual
	width ignores meta control characters.
-----------------------------------------------------------------------------*/

UBOOL XGC::ParseLine(const TCHAR* textStr, XTextState textState,
	const TCHAR* pEnd, FLOAT destWidth,
	const TCHAR** pNextLine, XTextState* pNewState,
	INT* pCount, FLOAT* pXExtent)
{
	guard(XGC::ParseLine);

	if (textStr == NULL)
		return FALSE;

	if (destWidth <= 0.0f)
		destWidth = 500000.0f;

	const TCHAR* Start = textStr;
	const TCHAR* Scan = textStr;
	const TCHAR* LineEnd = textStr;
	const TCHAR* NextLine = textStr;

	const TCHAR* LastWhitespace = NULL;
	const TCHAR* LastWordStart = NULL;

	XTextState StateAtNextLine = textState;
	XTextState StateAtWhitespace = textState;
	XTextState StateAtWordStart = textState;

	FLOAT WidthAtWhitespace = 0.0f;
	FLOAT CurrentWidth = 0.0f;
	FLOAT LineWidth = 0.0f;

	UBOOL bFirstChar = TRUE;
	UBOOL bInWhitespace = TRUE;
	UBOOL bHaveOverflow = FALSE;
	UBOOL bHitCarriageReturn = FALSE;
	UBOOL bLineHadInitialCarriageReturn = textState.bCarriageReturn;

	textState.bCarriageReturn = FALSE;

	UFont* CurrentFont = normalFont;
	if (textState.bBold == TRUE)
		CurrentFont = boldFont;

	while (TRUE)
	{
		const TCHAR* BeforeChar = Scan;
		XTextState BeforeState = textState;

		TCHAR Ch = GetNextChar(Scan, textState, pEnd);
		if (Ch == 0)
		{
			LineEnd = Scan;
			NextLine = Scan;
			StateAtNextLine = textState;
			LineWidth = CurrentWidth;
			break;
		}

		if (Ch == 10)
		{
			bHitCarriageReturn = TRUE;
			LineEnd = BeforeChar;
			NextLine = Scan;
			StateAtNextLine = textState;
			StateAtNextLine.bCarriageReturn = TRUE;
			LineWidth = CurrentWidth;
			break;
		}

		if (XReconIsWhitespace(Ch) == TRUE)
		{
			if (bInWhitespace == FALSE)
			{
				bInWhitespace = TRUE;
				LastWhitespace = BeforeChar;
				StateAtWhitespace = BeforeState;
				WidthAtWhitespace = CurrentWidth;
			}
		}
		else if (bInWhitespace == TRUE)
		{
			if (bHaveOverflow == TRUE)
			{
				LineEnd = LastWhitespace;
				NextLine = BeforeChar;
				StateAtNextLine = BeforeState;
				LineWidth = WidthAtWhitespace;
				break;
			}

			bInWhitespace = FALSE;
			LastWordStart = BeforeChar;
			StateAtWordStart = BeforeState;
		}

		if (BeforeState.bBold != textState.bBold)
		{
			if (textState.bBold == TRUE)
				CurrentFont = boldFont;
			else
				CurrentFont = normalFont;
		}

		if (CurrentFont != NULL)
		{
			INT CharWidth = 0;
			GetCharSize(CurrentFont, Ch, &CharWidth, NULL);
			FLOAT NextWidth = CurrentWidth + FLOAT(CharWidth);

			if (NextWidth > destWidth && bFirstChar == FALSE)
			{
				if (LastWhitespace == NULL)
				{
					LineEnd = BeforeChar;
					NextLine = BeforeChar;
					StateAtNextLine = BeforeState;
					LineWidth = CurrentWidth;
					break;
				}

				if (LastWordStart > LastWhitespace)
				{
					LineEnd = LastWhitespace;
					NextLine = LastWordStart;
					StateAtNextLine = StateAtWordStart;
					LineWidth = WidthAtWhitespace;
					break;
				}

				bHaveOverflow = TRUE;
			}

			CurrentWidth = NextWidth;
		}

		bFirstChar = FALSE;
	}

	if (LineEnd < Start)
		LineEnd = Start;

	if (NextLine < LineEnd)
		NextLine = LineEnd;

	if (pNextLine != NULL)
		*pNextLine = NextLine;

	if (pNewState != NULL)
	{
		*pNewState = StateAtNextLine;
		if (bHitCarriageReturn == TRUE)
			pNewState->bCarriageReturn = TRUE;
	}

	if (pCount != NULL)
		*pCount = INT(LineEnd - Start);

	if (pXExtent != NULL)
		*pXExtent = LineWidth;

	if (LineEnd > Start)
		return TRUE;

	if (NextLine > Start)
		return TRUE;

	if (bLineHadInitialCarriageReturn == TRUE)
		return TRUE;

	return FALSE;

	unguard;
}

const TCHAR* XGC::GetLine(XTextState& textState, FLOAT destWidth,
	const TCHAR*& textStr, const TCHAR* pEnd,
	FLOAT& xExtent, INT& count)
{
	guard(XGC::GetLine);

	if (textStr == NULL)
		return NULL;

	const TCHAR* LineStart = textStr;

	const TCHAR* NextLine = NULL;
	XTextState NewState;
	INT NewCount = 0;
	FLOAT NewExtent = 0.0f;

	UBOOL bGotLine = ParseLine(
		LineStart,
		textState,
		pEnd,
		destWidth,
		&NextLine,
		&NewState,
		&NewCount,
		&NewExtent);

	if (bGotLine == FALSE)
		return NULL;

	textStr = NextLine;
	textState = NewState;
	count = NewCount;
	xExtent = NewExtent;

	return LineStart;

	unguard;
}

UBOOL XGC::GetTextLine(const TCHAR* textStr, XTextState* textState,
	INT count, FLOAT destWidth,
	const TCHAR** pNextLine, XTextState* pNewState,
	INT* pNumChars, FLOAT* pXExtent)
{
	guard(XGC::GetTextLine);

	if (textStr == NULL)
		return FALSE;

	if (count < 0)
		count = appStrlen(textStr);

	const TCHAR* End = textStr + count;
	XTextState State;

	if (textState != NULL)
		State = *textState;

	if (State.bPlaneSet == FALSE)
		State.plane = textPlane;

	return ParseLine(textStr, State, End, destWidth, pNextLine, pNewState, pNumChars, pXExtent);

	unguard;
}

FLOAT XGC::GetFontHeight(UBOOL bIncludeSpace)
{
	guard(XGC::GetFontHeight);

	FLOAT Height = 0.0f;

	if (normalFont != NULL)
	{
		INT NormalHeight = 0;
		GetCharSize(normalFont, TEXT(' '), NULL, &NormalHeight);
		if (NormalHeight > 0)
			Height = FLOAT(NormalHeight);
	}

	if (boldFont != NULL)
	{
		INT BoldHeight = 0;
		GetCharSize(boldFont, TEXT(' '), NULL, &BoldHeight);
		if (Height < FLOAT(BoldHeight))
			Height = FLOAT(BoldHeight);
	}

	if (bIncludeSpace == TRUE)
		Height += textVSpacing;

	return Height;

	unguard;
}

void XGC::GetTextExtent(FLOAT destWidth, FLOAT& xExtent, FLOAT& yExtent, const TCHAR* textStr)
{
	guard(XGC::GetTextExtent);

	XTextState State;
	State.plane = textPlane;

	xExtent = 0.0f;
	yExtent = 0.0f;

	if (textStr == NULL)
		return;

	const TCHAR* Scan = textStr;
	const TCHAR* End = textStr + appStrlen(textStr);

	while (TRUE)
	{
		FLOAT LineExtent = 0.0f;
		INT LineCount = 0;
		const TCHAR* Line = GetLine(State, destWidth, Scan, End, LineExtent, LineCount);

		if (Line == NULL)
			break;

		XTextState DrawState = State;
		const TCHAR* LineScan = Line;
		const TCHAR* LineEnd = Line + LineCount;
		UFont* Font = normalFont;
		FLOAT LineHeight = 0.0f;

		while (TRUE)
		{
			TCHAR Ch = GetNextChar(LineScan, DrawState, LineEnd);
			if (Ch == 0)
				break;

			if (DrawState.bBold == TRUE)
				Font = boldFont;
			else
				Font = normalFont;

			if (Ch != 10 && Font != NULL)
			{
				INT CharHeight = 0;
				GetCharSize(Font, Ch, NULL, &CharHeight);
				if (LineHeight < FLOAT(CharHeight))
					LineHeight = FLOAT(CharHeight);
			}
		}

		if (LineHeight < 1.0f && Font != NULL)
		{
			INT SpaceHeight = 0;
			GetCharSize(Font, TEXT(' '), NULL, &SpaceHeight);
			LineHeight = FLOAT(SpaceHeight);
		}

		if (xExtent < LineExtent)
			xExtent = LineExtent;

		yExtent += LineHeight + textVSpacing;
	}

	unguard;
}

static void XReconDrawCachedTextTile(XGC* GC, UTexture* Texture,
	FLOAT destX, FLOAT destY, FLOAT destWidth, FLOAT destHeight,
	FLOAT srcX, FLOAT srcY, FLOAT srcWidth, FLOAT srcHeight,
	FPlane& Plane, DWORD PolyFlags)
{
	guard(XReconDrawCachedTextTile);

	if (GC == NULL || GC->canvas == NULL || Texture == NULL)
		return;

	UCanvas* Canvas = GC->canvas;
	if (Canvas->Frame == NULL || Canvas->Frame->Viewport == NULL || Canvas->Frame->Viewport->RenDev == NULL)
		return;

	destX += GC->clipRect.originX;
	destY += GC->clipRect.originY;

	FLOAT ClipX = GC->clipRect.clipX + GC->clipRect.originX;
	FLOAT ClipY = GC->clipRect.clipY + GC->clipRect.originY;
	FLOAT ClipRight = ClipX + GC->clipRect.clipWidth;
	FLOAT ClipBottom = ClipY + GC->clipRect.clipHeight;

	/*
		Text glyph drawing uses the cached FTextureInfo path rather than the
		generic XGC::DrawTile() route. It still must clip source and destination
		proportionally. If the right/bottom edge only shrinks destWidth/destHeight
		and leaves srcWidth/srcHeight untouched, a clipped glyph is squeezed into
		the smaller cell. ComputerWindow draws every typewriter character inside a
		fontWidth/fontHeight cell, so that bug visibly distorts HUDInfoLink text
		when the font glyph/DS shadow is wider or taller than the logical cell.
	*/

	if (destWidth > 0.0f && destX < ClipX)
	{
		FLOAT Delta = destX - ClipX;
		FLOAT SourceDelta = Delta * srcWidth / destWidth;
		srcX -= SourceDelta;
		srcWidth += SourceDelta;
		destWidth += Delta;
		destX = ClipX;
	}

	if (destWidth > 0.0f)
	{
		FLOAT DestRight = destX + destWidth;
		if (DestRight > ClipRight)
		{
			srcWidth += (ClipRight - DestRight) * srcWidth / destWidth;
			destWidth = ClipRight - destX;
		}
	}

	if (destHeight > 0.0f && destY < ClipY)
	{
		FLOAT Delta = destY - ClipY;
		FLOAT SourceDelta = Delta * srcHeight / destHeight;
		srcY -= SourceDelta;
		srcHeight += SourceDelta;
		destHeight += Delta;
		destY = ClipY;
	}

	if (destHeight > 0.0f)
	{
		FLOAT DestBottom = destY + destHeight;
		if (DestBottom > ClipBottom)
		{
			srcHeight += (ClipBottom - DestBottom) * srcHeight / destHeight;
			destHeight = ClipBottom - destY;
		}
	}

	if (srcWidth > 0.0f && srcX < 0.0f)
	{
		FLOAT SourceDelta = srcX;
		FLOAT DestDelta = SourceDelta * destWidth / srcWidth;
		destWidth += DestDelta;
		destX -= DestDelta;
		srcWidth += SourceDelta;
		srcX = 0.0f;
	}

	if (srcWidth > 0.0f)
	{
		FLOAT SourceRight = srcX + srcWidth;
		if (SourceRight > FLOAT(Texture->USize))
		{
			destWidth += (FLOAT(Texture->USize) - SourceRight) * destWidth / srcWidth;
			srcWidth = FLOAT(Texture->USize) - srcX;
		}
	}

	if (srcHeight > 0.0f && srcY < 0.0f)
	{
		FLOAT SourceDelta = srcY;
		FLOAT DestDelta = SourceDelta * destHeight / srcHeight;
		destHeight += DestDelta;
		destY -= DestDelta;
		srcHeight += SourceDelta;
		srcY = 0.0f;
	}

	if (srcHeight > 0.0f)
	{
		FLOAT SourceBottom = srcY + srcHeight;
		if (SourceBottom > FLOAT(Texture->VSize))
		{
			destHeight += (FLOAT(Texture->VSize) - SourceBottom) * destHeight / srcHeight;
			srcHeight = FLOAT(Texture->VSize) - srcY;
		}
	}

	if (destWidth <= 0.0f || destHeight <= 0.0f || srcWidth <= 0.0f || srcHeight <= 0.0f)
		return;

	FTextureInfo* Info = XReconGetGCTextureInfo(Canvas, Texture);
	if (Info == NULL || Info->Texture == NULL)
		return;

	FPlane FogPlane(0.0f, 0.0f, 0.0f, 0.0f);
	Canvas->Frame->Viewport->RenDev->DrawTile(
		Canvas->Frame,
		*Info,
		(FLOAT)(GC->hMultiplier * XReconRoundToCanvas(destX)),
		(FLOAT)(GC->vMultiplier * XReconRoundToCanvas(destY)),
		(FLOAT)(GC->hMultiplier * XReconRoundToCanvas(destWidth)),
		(FLOAT)(GC->vMultiplier * XReconRoundToCanvas(destHeight)),
		(FLOAT)XReconRoundToCanvas(srcX),
		(FLOAT)XReconRoundToCanvas(srcY),
		(FLOAT)XReconRoundToCanvas(srcWidth),
		(FLOAT)XReconRoundToCanvas(srcHeight),
		NULL,
		Canvas->Z,
		Plane,
		FogPlane,
		PolyFlags);

	unguard;
}

void XGC::DrawChar(UFont* font, TCHAR ch, FPlane& plane, BYTE& bUnderline,
	FLOAT destX, FLOAT destY, FLOAT* pXOffset, FLOAT* pHeight)
{
	guard(XGC::DrawChar);

	if (canvas == NULL)
		return;

	if (font == NULL)
		return;

	UTexture* PageTexture = NULL;
	FFontCharacter* CharInfo = XReconGetFontCharacter(font, ch, &PageTexture);

	if (pXOffset != NULL)
		*pXOffset += FLOAT(CharInfo->USize);

	if (pHeight != NULL)
		*pHeight = FLOAT(CharInfo->VSize);

	if (PageTexture == NULL)
		return;

	XReconDrawCachedTextTile(this,
		PageTexture,
		destX,
		destY,
		FLOAT(CharInfo->USize),
		FLOAT(CharInfo->VSize),
		FLOAT(CharInfo->StartU),
		FLOAT(CharInfo->StartV),
		FLOAT(CharInfo->USize),
		FLOAT(CharInfo->VSize),
		plane,
		textPolyFlags);

	if (bUnderline == TRUE)
	{
		if (underlineTexture != NULL && underlineHeight > 0.0f)
		{
			FLOAT UnderlineY = destY + FLOAT(CharInfo->VSize) - baselineOffset;
			FLOAT UnderlineWidth = FLOAT(CharInfo->USize - 1);

			XReconDrawCachedTextTile(this,
				underlineTexture,
				destX,
				UnderlineY,
				UnderlineWidth,
				underlineHeight,
				0.0f,
				0.0f,
				UnderlineWidth,
				underlineHeight,
				plane,
				textPolyFlags);
		}
	}

	unguard;
}

void XGC::DrawTextLine(FLOAT destX, FLOAT destY, XTextState* textState,
	const TCHAR* textStr, INT lineSize)
{
	guard(XGC::DrawTextLine);

	if (bDrawEnabled == FALSE || canvas == NULL || textStr == NULL)
		return;

	if (lineSize < 0)
		lineSize = appStrlen(textStr);

	if (lineSize < 1)
		return;

	const TCHAR* Scan = textStr;
	const TCHAR* End = textStr + lineSize;

	XTextState State;
	if (textState != NULL)
		State = *textState;

	if (State.bPlaneSet == FALSE)
		State.plane = textPlane;

	UFont* Font = normalFont;
	if (State.bBold == TRUE)
		Font = boldFont;

	FLOAT XOffset = destX;

	while (TRUE)
	{
		TCHAR Ch = GetNextChar(Scan, State, End);
		if (Ch == 0)
			break;

		if (State.bBold == TRUE)
			Font = boldFont;
		else
			Font = normalFont;

		if (Ch != 10 && Font != NULL)
			DrawChar(Font, Ch, State.plane, State.bUnderline, XOffset, destY, &XOffset, NULL);
	}

	unguard;
}

UBOOL XGC::DrawTextChar(FLOAT& destX, FLOAT& destY, XTextState* textState,
	const TCHAR*& textStr, INT& charCount)
{
	guard(XGC::DrawTextChar);

	if (charCount < 1 || textStr == NULL)
		return FALSE;

	const TCHAR* End = textStr + charCount;
	XTextState State;

	if (textState != NULL)
		State = *textState;

	if (State.bPlaneSet == FALSE)
		State.plane = textPlane;

	TCHAR Ch = GetNextChar(textStr, State, End);
	UBOOL bDrew = FALSE;

	if (Ch != 0)
	{
		UFont* Font = normalFont;
		if (State.bBold == TRUE)
			Font = boldFont;

		if (Ch != 10 && Font != NULL && bDrawEnabled == TRUE && canvas != NULL)
			DrawChar(Font, Ch, State.plane, State.bUnderline, destX, destY, &destX, NULL);

		bDrew = TRUE;
	}

	charCount = INT(End - textStr);
	if (charCount < 0)
		charCount = 0;

	if (textState != NULL)
		*textState = State;

	return bDrew;

	unguard;
}

void XGC::DrawText(FLOAT destX, FLOAT destY, FLOAT destWidth, FLOAT destHeight,
	const TCHAR* textStr)
{
	guard(XGC::DrawText);

	if (bDrawEnabled == FALSE || canvas == NULL || textStr == NULL)
		return;

	XTextState BaseState;
	BaseState.plane = textPlane;
	BaseState.bPlaneSet = TRUE;

	XTextState LineState = BaseState;

	FLOAT WrapWidth = 0.0f;
	if (bWordWrap == TRUE)
		WrapWidth = destWidth;

	INT StackPos = PushGC();
	Intersect(destX, destY, destWidth, destHeight);

	if (vAlign != VALIGN_Top)
	{
		FLOAT TextExtentX = 0.0f;
		FLOAT TextExtentY = 0.0f;
		GetTextExtent(WrapWidth, TextExtentX, TextExtentY, textStr);

		if (vAlign == VALIGN_Center)
			destY += FLOAT(INT((destHeight - TextExtentY) / 2.0f));
		else if (vAlign == VALIGN_Bottom)
			destY += destHeight - TextExtentY;
	}

	const TCHAR* Scan = textStr;
	const TCHAR* End = textStr + appStrlen(textStr);

	while (TRUE)
	{
		XTextState ParseState = LineState;
		FLOAT LineExtent = 0.0f;
		INT LineCount = 0;
		const TCHAR* Line = GetLine(ParseState, WrapWidth, Scan, End, LineExtent, LineCount);

		if (Line == NULL)
			break;

		FLOAT LineX = destX;
		if (hAlign == HALIGN_Center)
			LineX = destX + FLOAT(INT((destWidth - LineExtent) / 2.0f));
		else if (hAlign == HALIGN_Right)
			LineX = destX + destWidth - LineExtent;

		const TCHAR* LineScan = Line;
		const TCHAR* LineEnd = Line + LineCount;
		XTextState DrawState = LineState;
		UFont* Font = normalFont;
		FLOAT MaxHeight = 0.0f;
		FLOAT DrawX = LineX;

		while (TRUE)
		{
			TCHAR Ch = GetNextChar(LineScan, DrawState, LineEnd);
			if (Ch == 0)
				break;

			if (DrawState.bBold == TRUE)
				Font = boldFont;
			else
				Font = normalFont;

			if (Ch != 10 && Font != NULL)
			{
				FLOAT CharHeight = 0.0f;
				DrawChar(Font, Ch, DrawState.plane, DrawState.bUnderline, DrawX, destY, &DrawX, &CharHeight);
				if (MaxHeight < CharHeight)
					MaxHeight = CharHeight;
			}
		}

		if (MaxHeight < 1.0f && Font != NULL)
		{
			INT SpaceHeight = 0;
			GetCharSize(Font, TEXT(' '), NULL, &SpaceHeight);
			MaxHeight = FLOAT(SpaceHeight);
		}

		LineState = ParseState;
		destY += MaxHeight + textVSpacing;
	}

	PopGC(StackPos);

	unguard;
}

void XGC::Printf(FLOAT destX, FLOAT destY, FLOAT destWidth, FLOAT destHeight,
	const TCHAR* fmt, ...)
{
	guard(XGC::Printf);

	TCHAR Temp[8192];
	GET_VARARGS(Temp, ARRAY_COUNT(Temp), fmt);
	DrawText(destX, destY, destWidth, destHeight, Temp);

	unguard;
}

