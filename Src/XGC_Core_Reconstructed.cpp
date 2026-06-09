/*=============================================================================
	XGC_Core_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 01.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XGC);

struct XReconGCTextureInfoCacheEntry
{
	FTextureInfo Info;
	XReconGCTextureInfoCacheEntry* Next;
};

static XReconGCTextureInfoCacheEntry GReconGCTextureInfoCache[5];
static XReconGCTextureInfoCacheEntry* GReconGCTextureInfoMRU = NULL;
static INT GReconGCTextureInfoCount = 0;
static FTextureInfo GReconGCEmptyTextureInfo;

FTextureInfo* XReconGetGCTextureInfo(UCanvas* Canvas, UTexture* Texture)
{
	guard(XReconGetGCTextureInfo);

	if (Texture == NULL)
		return &GReconGCEmptyTextureInfo;

	if (Canvas == NULL || Canvas->Viewport == NULL || Canvas->Viewport->RenDev == NULL)
		return &GReconGCEmptyTextureInfo;

	XReconGCTextureInfoCacheEntry* Entry = GReconGCTextureInfoMRU;
	XReconGCTextureInfoCacheEntry* Previous = NULL;
	INT NewIndex = GReconGCTextureInfoCount;

	while (Entry != NULL)
	{
		XReconGCTextureInfoCacheEntry* Next = Entry->Next;
		if (((Next == NULL) && (GReconGCTextureInfoCount >= 5)) || (Entry->Info.Texture == Texture))
		{
			if (Previous != NULL)
				Previous->Next = Next;
			else
				GReconGCTextureInfoMRU = Next;

			goto FoundEntry;
		}

		Previous = Entry;
		Entry = Next;
	}

	if (GReconGCTextureInfoCount >= 5)
		return &GReconGCEmptyTextureInfo;

	GReconGCTextureInfoCount++;
	Entry = &GReconGCTextureInfoCache[NewIndex];

FoundEntry:
	if (Entry->Info.Texture != Texture)
	{
		if (Entry->Info.Texture != NULL)
			Entry->Info.Texture->Unlock(Entry->Info);

		Texture->Lock(Entry->Info, Canvas->Viewport->CurrentTime, -1, Canvas->Viewport->RenDev);
	}

	Entry->Next = GReconGCTextureInfoMRU;
	GReconGCTextureInfoMRU = Entry;
	return &Entry->Info;

	unguard;
}

void XReconCleanGCTextureInfoCache()
{
	guard(XReconCleanGCTextureInfoCache);

	GReconGCTextureInfoMRU = NULL;
	GReconGCTextureInfoCount = 0;

	for (INT i = 0; i < 5; i++)
	{
		if (GReconGCTextureInfoCache[i].Info.Texture != NULL)
			GReconGCTextureInfoCache[i].Info.Texture->Unlock(GReconGCTextureInfoCache[i].Info);

		GReconGCTextureInfoCache[i].Info.Texture = NULL;
		GReconGCTextureInfoCache[i].Next = NULL;
	}

	unguard;
}

void XGC::CleanUp(void)
{
	guard(XGC::CleanUp);
	XReconCleanGCTextureInfoCache();
	unguard;
}

static inline FPlane XColorToPlane(const FColor& Color)
{
	return FPlane(
		FLOAT(Color.R) * (1.0f / 255.0f),
		FLOAT(Color.G) * (1.0f / 255.0f),
		FLOAT(Color.B) * (1.0f / 255.0f),
		0.0f);
}

XGC::XGC()
	: XExtensionObject()
{
	canvas = NULL;

	clipRect = XClipRect();

	style = STY_Normal;
	bSmoothed = FALSE;
	bDrawEnabled = TRUE;
	bMasked = FALSE;
	bTranslucent = FALSE;
	bModulated = FALSE;

	polyFlags = PF_RenderHint | PF_NoSmooth;
	EnableTranslucentText(FALSE);

	SetTileColor(FColor(255, 255, 255));
	SetTextColor(FColor(0, 255, 0));

	normalFont = NULL;
	boldFont = NULL;
	/*
		Do not clear underlineTexture here. Original XGC::XGC() leaves this
		UTexture* alone so the UnrealScript defaultproperties value
		Texture'Extension.Solid' survives construction. Clearing it breaks
		the |& accelerator underline path.
	*/
	underlineHeight = 1.0f;
	baselineOffset = 2.0f;
	textVSpacing = 1.0f;

	hAlign = HALIGN_Left;
	vAlign = VALIGN_Top;
	bWordWrap = TRUE;
	bParseMetachars = TRUE;

	hMultiplier = 1;
	vMultiplier = 1;

	bFree = FALSE;
	gcCount = 1;
	gcStack = NULL;
	gcFree = NULL;
	gcOwner = NULL;
}

XGC::XGC(XGC& copyGC)
	: XExtensionObject()
{
	clipRect = XClipRect();

	CopyGC(copyGC);

	bFree = FALSE;
	gcStack = NULL;
	gcFree = NULL;
	gcOwner = NULL;
	gcCount = 1;
}

void XGC::Destroy(void)
{
	guard(XGC::Destroy);

	if (gcOwner != NULL)
	{
		XGC* Search = NULL;
		XGC* Prev = NULL;

		if (bFree == TRUE)
		{
			Search = gcOwner->gcFree;
		}
		else
		{
			Search = gcOwner->gcStack;
		}

		while (Search != NULL)
		{
			if (Search == this)
			{
				if (Prev != NULL)
				{
					Prev->gcStack = Search->gcStack;
				}
				else if (bFree == TRUE)
				{
					gcOwner->gcFree = Search->gcStack;
				}
				else
				{
					gcOwner->gcStack = Search->gcStack;
				}
				break;
			}

			Prev = Search;
			Search = Search->gcStack;
		}
	}
	else
	{
		PopGC(0);

		INT FreeCount = 0;
		while (gcFree != NULL)
		{
			XGC* FreeGC = gcFree;
			gcFree = FreeGC->gcStack;
			FreeGC->ConditionalDestroy();
			FreeCount++;
		}

		if (FreeCount > 16 && XReconShouldLogDiagnostics() == TRUE)
		{
			const TCHAR* OuterName = TEXT("None");
			if (GetOuter() != NULL)
				OuterName = GetOuter()->GetName();

			GLog->Logf(TEXT("***WARNING: GC \"%s\" contains an abnormally large GC stack!"), GetName());
			GLog->Logf(TEXT("            GC stack count is %d, parent is \"%s\""), FreeCount, OuterName);
		}
	}

	Super::Destroy();

	unguard;
}

void XGC::SetCanvas(UCanvas* NewCanvas)
{
	canvas = NewCanvas;
}

void XGC::SetMultipliers(INT hMult, INT vMult)
{
	if (hMult < 1)
	{
		hMult = 1;
	}
	if (vMult < 1)
	{
		vMult = 1;
	}

	hMultiplier = hMult;
	vMultiplier = vMult;
}

void XGC::SetClipRect(XClipRect& newClipRect)
{
	clipRect = newClipRect;
}

void XGC::Intersect(XClipRect& intersectClipRect)
{
	FLOAT FromX1 = clipRect.clipX + clipRect.originX;
	FLOAT FromY1 = clipRect.clipY + clipRect.originY;
	FLOAT FromX2 = intersectClipRect.clipX + intersectClipRect.originX;
	FLOAT FromY2 = intersectClipRect.clipY + intersectClipRect.originY;

	FLOAT ToX1 = FromX1 + clipRect.clipWidth;
	FLOAT ToY1 = FromY1 + clipRect.clipHeight;
	FLOAT ToX2 = FromX2 + intersectClipRect.clipWidth;
	FLOAT ToY2 = FromY2 + intersectClipRect.clipHeight;

	if (FromX1 < FromX2)
	{
		FromX1 = FromX2;
	}
	if (FromY1 < FromY2)
	{
		FromY1 = FromY2;
	}
	if (ToX1 > ToX2)
	{
		ToX1 = ToX2;
	}
	if (ToY1 > ToY2)
	{
		ToY1 = ToY2;
	}

	clipRect.clipX = FromX1 - clipRect.originX;
	clipRect.clipY = FromY1 - clipRect.originY;
	clipRect.clipWidth = ToX1 - FromX1;
	clipRect.clipHeight = ToY1 - FromY1;
}

void XGC::Intersect(FLOAT clipX, FLOAT clipY, FLOAT clipWidth, FLOAT clipHeight)
{
	XClipRect TempClip(clipX + clipRect.originX, clipY + clipRect.originY, clipWidth, clipHeight);
	Intersect(TempClip);
}

void XGC::EnableSmoothing(UBOOL bNewSmoothing)
{
	bSmoothed = bNewSmoothing;
	GeneratePolyFlags();
}

void XGC::SetStyle(BYTE newStyle)
{
	style = newStyle;

	if (newStyle != STY_None)
	{
		bDrawEnabled = TRUE;

		if (newStyle == STY_Masked)
		{
			bMasked = TRUE;
			bTranslucent = FALSE;
			bModulated = FALSE;
		}
		else if (newStyle == STY_Translucent)
		{
			bMasked = FALSE;
			bTranslucent = TRUE;
			bModulated = FALSE;
		}
		else if (newStyle == STY_Modulated)
		{
			bMasked = FALSE;
			bTranslucent = FALSE;
			bModulated = TRUE;
		}
		else
		{
			bMasked = FALSE;
			bTranslucent = FALSE;
			bModulated = FALSE;
		}

		GeneratePolyFlags();
	}
	else
	{
		bDrawEnabled = FALSE;
	}
}

void XGC::EnableDrawing(UBOOL bNewDrawEnabled)
{
	bDrawEnabled = bNewDrawEnabled;
}

void XGC::EnableMasking(UBOOL bNewMasking)
{
	bMasked = bNewMasking;
	GenerateStyle();
	GeneratePolyFlags();
}

void XGC::EnableTranslucency(UBOOL bNewTranslucency)
{
	bTranslucent = bNewTranslucency;
	GenerateStyle();
	GeneratePolyFlags();
}

void XGC::EnableModulation(UBOOL bNewModulation)
{
	bModulated = bNewModulation;
	GenerateStyle();
	GeneratePolyFlags();
}

void XGC::SetPolyFlags(DWORD newPolyFlags)
{
	polyFlags = newPolyFlags;

	// Exact relation reconstructed from Extension.dll.c:
	// SetPolyFlags() does NOT force drawing on. It preserves bDrawEnabled and
	// derives the style bits from PF_Masked/PF_Translucent/PF_Modulated/PF_NoSmooth.
	bMasked = FALSE;
	bTranslucent = FALSE;
	bModulated = FALSE;
	bSmoothed = TRUE;

	if ((newPolyFlags & PF_Masked) != 0)
		bMasked = TRUE;

	if ((newPolyFlags & PF_Translucent) != 0)
		bTranslucent = TRUE;

	if ((newPolyFlags & PF_Modulated) != 0)
		bModulated = TRUE;

	if ((newPolyFlags & PF_NoSmooth) != 0)
		bSmoothed = FALSE;

	GenerateStyle();
}

void XGC::SetTileColor(FColor newTileColor)
{
	tileColor = newTileColor;
	tilePlane = XColorToPlane(newTileColor);
}

void XGC::SetTextColor(FColor newTextColor)
{
	textColor = newTextColor;
	textPlane = XColorToPlane(newTextColor);
}

void XGC::EnableTranslucentText(UBOOL bNewTranslucency)
{
	bTextTranslucent = bNewTranslucency;

	if (bTextTranslucent == TRUE)
	{
		/*
			Original Extension sets textPolyFlags to 2052 here:
			PF_Translucent | PF_NoSmooth.  It intentionally does not include
			PF_Masked for translucent text.  Combining PF_Masked and
			PF_Translucent makes some render paths blend/filter font pages
			differently and can soften doubled HUD/ComputerWindow text.
		*/
		textPolyFlags = PF_Translucent | PF_NoSmooth;
	}
	else
	{
		/* Original Extension sets textPolyFlags to 2050: PF_Masked | PF_NoSmooth. */
		textPolyFlags = PF_Masked | PF_NoSmooth;
	}
}

void XGC::SetFonts(UFont* newNormalFont, UFont* newBoldFont)
{
	normalFont = newNormalFont;
	boldFont = newBoldFont;
}

void XGC::SetNormalFont(UFont* newFont)
{
	normalFont = newFont;
}

void XGC::SetBoldFont(UFont* newFont)
{
	boldFont = newFont;
}

void XGC::SetBaselineData(FLOAT newBaselineOffset, FLOAT newUnderlineHeight)
{
	baselineOffset = newBaselineOffset;
	underlineHeight = newUnderlineHeight;
}

void XGC::SetTextVSpacing(FLOAT newVSpacing)
{
	textVSpacing = newVSpacing;
}

void XGC::SetHorizontalAlignment(EHAlign newHAlign)
{
	hAlign = newHAlign;
}

void XGC::SetVerticalAlignment(EVAlign newVAlign)
{
	vAlign = newVAlign;
}

void XGC::SetAlignments(EHAlign newHAlign, EVAlign newVAlign)
{
	hAlign = newHAlign;
	vAlign = newVAlign;
}

void XGC::GetAlignments(EHAlign* pHAlign, EVAlign* pVAlign)
{
	if (pHAlign != NULL)
	{
		*pHAlign = (EHAlign)hAlign;
	}
	if (pVAlign != NULL)
	{
		*pVAlign = (EVAlign)vAlign;
	}
}

void XGC::EnableWordWrap(UBOOL bNewWordWrap)
{
	bWordWrap = bNewWordWrap;
}

void XGC::EnableSpecialText(UBOOL bNewSpecial)
{
	bParseMetachars = bNewSpecial;
}

void XGC::CopyGC(XGC& copyGC)
{
	canvas = copyGC.canvas;
	clipRect = copyGC.clipRect;

	style = copyGC.style;
	bSmoothed = copyGC.bSmoothed;
	bDrawEnabled = copyGC.bDrawEnabled;
	bMasked = copyGC.bMasked;
	bTranslucent = copyGC.bTranslucent;
	bModulated = copyGC.bModulated;
	bTextTranslucent = copyGC.bTextTranslucent;

	polyFlags = copyGC.polyFlags;
	textPolyFlags = copyGC.textPolyFlags;

	tileColor = copyGC.tileColor;
	tilePlane = copyGC.tilePlane;
	textColor = copyGC.textColor;
	textPlane = copyGC.textPlane;

	normalFont = copyGC.normalFont;
	boldFont = copyGC.boldFont;
	underlineTexture = copyGC.underlineTexture;
	underlineHeight = copyGC.underlineHeight;
	baselineOffset = copyGC.baselineOffset;
	textVSpacing = copyGC.textVSpacing;
	hAlign = copyGC.hAlign;
	vAlign = copyGC.vAlign;
	bWordWrap = copyGC.bWordWrap;
	bParseMetachars = copyGC.bParseMetachars;

	hMultiplier = copyGC.hMultiplier;
	vMultiplier = copyGC.vMultiplier;
}

INT XGC::PushGC(void)
{
	const INT OldCount = gcCount;

	if (gcOwner == NULL)
	{
		XGC* SaveGC = gcFree;

		if (SaveGC != NULL)
		{
			gcFree = SaveGC->gcStack;
			SaveGC->CopyGC(*this);
		}
		else
		{
			SaveGC = new(this) XGC(*this);
		}

		if (SaveGC != NULL)
		{
			SaveGC->gcOwner = this;
			SaveGC->bFree = FALSE;
			SaveGC->gcFree = NULL;
			SaveGC->gcCount = gcCount;
			SaveGC->gcStack = gcStack;

			gcStack = SaveGC;
			gcCount = OldCount + 1;
		}
	}

	return OldCount;
}

void XGC::PopGC(void)
{
	if (gcStack != NULL)
	{
		PopGC(gcStack->gcCount);
	}
	else
	{
		PopGC(gcCount - 1);
	}
}

void XGC::PopGC(INT gcNum)
{
	if (gcNum < gcCount && gcOwner == NULL)
	{
		UBOOL bFoundTarget = FALSE;
		XGC* LastPopped = NULL;

		while (bFoundTarget == FALSE)
		{
			XGC* SaveGC = gcStack;
			if (SaveGC == NULL)
			{
				break;
			}
			if (SaveGC->gcCount < gcNum)
			{
				break;
			}
			if (SaveGC->gcCount == gcNum)
			{
				bFoundTarget = TRUE;
			}

			LastPopped = SaveGC;
			gcStack = SaveGC->gcStack;
			gcCount = SaveGC->gcCount;

			if (gcFree != NULL)
			{
				SaveGC->gcCount = gcFree->gcCount + 1;
			}
			else
			{
				SaveGC->gcCount = 1;
			}

			SaveGC->gcOwner = this;
			SaveGC->bFree = TRUE;
			SaveGC->gcStack = gcFree;
			SaveGC->gcFree = NULL;
			gcFree = SaveGC;
		}

		if (LastPopped != NULL)
		{
			CopyGC(*LastPopped);
		}
	}
}

void XGC::GenerateStyle(void)
{
	if (bDrawEnabled == FALSE)
	{
		style = STY_None;
	}
	else if (bMasked == TRUE)
	{
		style = STY_Masked;
	}
	else if (bTranslucent == TRUE)
	{
		style = STY_Translucent;
	}
	else if (bModulated == TRUE)
	{
		style = STY_Modulated;
	}
	else
	{
		style = STY_Normal;
	}
}

void XGC::GeneratePolyFlags(void)
{
	polyFlags = PF_RenderHint;

	if (bMasked == TRUE)
	{
		polyFlags |= PF_Masked;
	}
	if (bTranslucent == TRUE)
	{
		polyFlags |= PF_Translucent;
	}
	if (bModulated == TRUE)
	{
		polyFlags |= PF_Modulated;
	}
	if (bSmoothed == FALSE)
	{
		polyFlags |= PF_NoSmooth;
	}
}
