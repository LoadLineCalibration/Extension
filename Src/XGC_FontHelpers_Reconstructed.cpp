/*=============================================================================
	XGC_FontHelpers_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 02.

	This file documents the two small but very important private helpers which
	Hex-Rays names sub_10026880 and sub_10026950.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

/*-----------------------------------------------------------------------------
	Font glyph lookup.

	Original layout from Deus Ex Engine/Inc/UnTex.h:
		UFont::CharactersPerPage
		UFont::Pages[PageIndex].Texture
		UFont::Pages[PageIndex].Characters[CharIndex]

	The original native returns a pointer to a static empty glyph if the character
	is missing and optionally returns the page texture.
-----------------------------------------------------------------------------*/

static FFontCharacter GReconMissingFontCharacter;

static UBOOL XReconResolveFontCharacter(UFont* Font, TCHAR Ch, INT& PageIndex, INT& CharIndex)
{
	guard(XReconResolveFontCharacter);

	PageIndex = 0;
	CharIndex = 0;

	if (Font == NULL)
		return FALSE;

	if (Font->CharactersPerPage <= 0)
		return FALSE;

	PageIndex = INT(Ch) / Font->CharactersPerPage;
	CharIndex = INT(Ch) % Font->CharactersPerPage;

	if (PageIndex < 0)
		return FALSE;

	if (PageIndex >= Font->Pages.Num())
		return FALSE;

	if (CharIndex < 0)
		return FALSE;

	if (CharIndex >= Font->Pages(PageIndex).Characters.Num())
		return FALSE;

	return TRUE;

	unguard;
}

static FFontCharacter* XReconGetFontCharacter(UFont* Font, TCHAR Ch, UTexture** OutTexture)
{
	guard(XReconGetFontCharacter);

	INT PageIndex = 0;
	INT CharIndex = 0;

	if (XReconResolveFontCharacter(Font, Ch, PageIndex, CharIndex) == TRUE)
	{
		if (OutTexture != NULL)
			*OutTexture = Font->Pages(PageIndex).Texture;

		return &Font->Pages(PageIndex).Characters(CharIndex);
	}

	if (OutTexture != NULL)
		*OutTexture = NULL;

	GReconMissingFontCharacter.StartU = 0;
	GReconMissingFontCharacter.StartV = 0;
	GReconMissingFontCharacter.USize = 0;
	GReconMissingFontCharacter.VSize = 0;

	return &GReconMissingFontCharacter;

	unguard;
}

void XGC::GetCharSize(UFont* Font, TCHAR Ch, INT* pWidth, INT* pHeight)
{
	guard(XGC::GetCharSize);

	UTexture* Texture = NULL;
	FFontCharacter* CharInfo = XReconGetFontCharacter(Font, Ch, &Texture);

	if (pWidth != NULL)
		*pWidth = CharInfo->USize;

	if (pHeight != NULL)
		*pHeight = CharInfo->VSize;

	unguard;
}

