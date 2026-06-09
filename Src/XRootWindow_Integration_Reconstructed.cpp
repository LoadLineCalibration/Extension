/*=============================================================================
	XRootWindow_Integration_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 09.

	Focus: the root window and the frame/input bridge. This is the layer which
	turns the window tree reconstructed in earlier passes into a live Deus Ex UI:
	root ownership, render-area clipping, raw background fill, paint order,
	cursor drawing, mouse/key routing, multi-click tracking, window-ready dispatch,
	and snapshot creation.

	This is reconstruction-first source. It preserves behaviour and naming where
	possible, but it is not yet a final VC98 drop-in translation unit.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XRootWindow);

extern DLL_IMPORT INT HeckbertQuantize(void* SourceData, BYTE* DestData, unsigned short* PaletteData, INT SourceFormat, INT PaletteEntries, INT Width, INT Height, INT bHasColorKey, unsigned long ColorKey, INT bGenerateMipmaps);

static const INT XReconSnapshotSourceFormatRGB888 = 2;

static XRootWindow* GReconFirstRootWindow = NULL;
static DOUBLE GReconLastRootTickSeconds = 0.0;

static INT GReconStoredFrameCount = 0;
static INT GReconMaxStoredFrames = 8;
static FSceneNode GReconStoredFrames[8];

static TArray<XClipRect> GReconRawBackgroundClipList;

static INT ReconRoundDownToInt(FLOAT Value)
{
	return (INT)Value;
}

static FLOAT GReconInterfaceMouseSensitivity = 1.0f;
static UBOOL GReconInterfaceMouseSensitivityLoaded = FALSE;

static FLOAT ReconClampInterfaceMouseSensitivity(FLOAT Value)
{
	if (Value < 0.01f)
		return 0.01f;

	if (Value > 10.0f)
		return 10.0f;

	return Value;
}

static FLOAT ReconGetInterfaceMouseSensitivity()
{
	guard(ReconGetInterfaceMouseSensitivity);

	if (GReconInterfaceMouseSensitivityLoaded == FALSE)
	{
		FLOAT NewSensitivity = 1.0f;
		const TCHAR* ConfigFilename = XReconGetInterfaceConfigFilename();

		if (GConfig != NULL)
		{
			if (GConfig->GetFloat(TEXT("Extension.UI"), TEXT("MouseSensitivity"), NewSensitivity, ConfigFilename) == FALSE)
				GConfig->SetFloat(TEXT("Extension.UI"), TEXT("MouseSensitivity"), NewSensitivity, ConfigFilename);
		}

		GReconInterfaceMouseSensitivity = ReconClampInterfaceMouseSensitivity(NewSensitivity);
		GReconInterfaceMouseSensitivityLoaded = TRUE;
	}

	return GReconInterfaceMouseSensitivity;

	unguard;
}

static void BuildPaletteFromFramebuffer(UTexture* Texture, const FColor* SourcePixels, INT SourceWidth, INT SourceHeight)
{
	guard(BuildPaletteFromFramebuffer);

	if (Texture == NULL || Texture->Palette == NULL || SourcePixels == NULL)
		return;

	INT SourceCount = SourceWidth * SourceHeight;
	if (SourceCount <= 0)
		return;

	TArray<BYTE> SourceRgb;
	SourceRgb.Add(SourceCount * 3);

	BYTE* RgbData = &SourceRgb(0);
	for (INT i = 0; i < SourceCount; i++)
	{
		const FColor& SourceColor = SourcePixels[i];
		RgbData[i * 3 + 0] = SourceColor.R;
		RgbData[i * 3 + 1] = SourceColor.G;
		RgbData[i * 3 + 2] = SourceColor.B;
	}

	TArray<BYTE> IndexedData;
	IndexedData.Add(SourceCount);

	unsigned short Palette565[256];
	appMemzero(Palette565, sizeof(Palette565));

	INT PaletteCount = HeckbertQuantize(
		RgbData,
		&IndexedData(0),
		Palette565,
		XReconSnapshotSourceFormatRGB888,
		256,
		SourceWidth,
		SourceHeight,
		0,
		0,
		0);

	Texture->Palette->Colors.Empty();
	Texture->Palette->Colors.Add(256);

	if (PaletteCount < 0)
		PaletteCount = 0;

	if (PaletteCount > 256)
		PaletteCount = 256;

	for (INT iPalette = 0; iPalette < PaletteCount; iPalette++)
	{
		unsigned short PackedColor = Palette565[iPalette];
		BYTE Red = BYTE((PackedColor >> 8) & 0xF8);
		BYTE Green = BYTE((PackedColor >> 3) & 0xFC);
		BYTE Blue = BYTE((PackedColor << 3) & 0xF8);
		Texture->Palette->Colors(iPalette) = FColor(Red, Green, Blue, 255);
	}

	for (INT iFill = PaletteCount; iFill < 256; iFill++)
		Texture->Palette->Colors(iFill) = FColor(0, 0, 0, 255);

	unguard;
}

static BYTE ReconClampByte(FLOAT Value)
{
	INT IntValue = (INT)(Value - 0.5f);

	if (IntValue < 0)
		return 0;

	if (IntValue > 255)
		return 255;

	return (BYTE)IntValue;
}

static INT ReconNextPowerOfTwo(INT Value)
{
	INT Result = 1;

	if (Value <= 0)
		return 0;

	while (Result < Value)
		Result <<= 1;

	return Result;
}

static XWindow* ReconGetTopModalOrRoot(XRootWindow* Root)
{
	guard(ReconGetTopModalOrRoot);

	if (Root == NULL)
		return NULL;

	for (XWindow* Child = Root->GetTopChild(); Child != NULL; Child = Child->GetLowerSibling())
	{
		if (Child->windowType >= WIN_Modal)
			return Child;
	}

	return Root;

	unguard;
}

XRootWindow::XRootWindow(APlayerPawnExt* InParentPawn)
: XModalWindow(NULL)
{
	guard(XRootWindow::XRootWindow);
	unguard;
}

void XRootWindow::Init(XWindow*)
{
	guard(XRootWindow::Init_InvalidParent);

	if (XReconShouldLogDiagnostics() == TRUE)
		GLog->Logf(TEXT("Root cannot have a parent window!"));

	unguard;
}

void XRootWindow::Init(APlayerPawnExt* InParentPawn)
{
	guard(XRootWindow::Init);

	XModalWindow::Init(NULL);

	AddToRoot();

	windowType = WIN_Root;
	initCount = 0;

	mouseX = 0.0f;
	mouseY = 0.0f;
	prevMouseX = -1.0f;
	prevMouseY = -1.0f;
	lastMouseWindow = NULL;

	bMouseMoved = TRUE;
	bMouseMoveLocked = FALSE;
	bMouseButtonLocked = FALSE;
	bCursorVisible = TRUE;

	defaultEditCursor = NULL;
	defaultMoveCursor = NULL;
	defaultHorizontalMoveCursor = NULL;
	defaultVerticalMoveCursor = NULL;
	defaultTopLeftMoveCursor = NULL;
	defaultTopRightMoveCursor = NULL;

	bPositionalSound = TRUE;

	grabbedWindow = NULL;
	focusWindow = NULL;

	handleKeyboardRef = 0;
	handleMouseRef = 0;

	bRender = TRUE;
	bClipRender = FALSE;
	bStretchRawBackground = FALSE;

	renderX = 0.0f;
	renderY = 0.0f;
	renderWidth = 0.0f;
	renderHeight = 0.0f;

	rawColor = FColor(0, 0, 0);

	bTickEnabled = TRUE;
	tickCycles = 0;
	paintCycles = 0;
	bShowStats = FALSE;
	bShowFrames = FALSE;
	frameTimer = 0.0f;

	multiClickTimeout = 0.5f;
	maxMouseDist = 10.0f;
	clickCount = 0;
	lastButtonType = 0;
	lastButtonPress = 0.0f;
	lastButtonWindow = NULL;
	firstButtonMouseX = -1.0f;
	firstButtonMouseY = -1.0f;

	for (INT i = 0; i < IK_MAX; i++)
		keyDownMap[i] = 0;

	hMultiplier = 1;
	vMultiplier = 1;
	snapshotWidth = 0;
	snapshotHeight = 0;
	rootFrame = NULL;

	parentPawn = InParentPawn;
	if (parentPawn != NULL)
		parentPawn->rootWindow = this;

	nextRootWindow = GReconFirstRootWindow;
	GReconFirstRootWindow = this;

	unguard;
}

void XRootWindow::CleanUp()
{
	guard(XRootWindow::CleanUp);

	XRootWindow* PrevRoot = NULL;
	XRootWindow* TestRoot = GReconFirstRootWindow;

	while (TestRoot != NULL)
	{
		if (TestRoot == this)
		{
			if (PrevRoot != NULL)
				PrevRoot->nextRootWindow = nextRootWindow;
			else
				GReconFirstRootWindow = nextRootWindow;

			break;
		}

		PrevRoot = TestRoot;
		TestRoot = TestRoot->nextRootWindow;
	}

	if (parentPawn != NULL && parentPawn->rootWindow == this)
		parentPawn->rootWindow = NULL;

	RemoveFromRoot();
	XModalWindow::CleanUp();

	unguard;
}

void XRootWindow::Destroy()
{
	guard(XRootWindow::Destroy);
	XWindow::Destroy();
	unguard;
}

void XRootWindow::SetDefaultEditCursor(XCursor* NewEditCursor)
{
	defaultEditCursor = NewEditCursor;
}

void XRootWindow::SetDefaultMovementCursors(XCursor* NewMoveCursor, XCursor* NewHorizontalCursor, XCursor* NewVerticalCursor, XCursor* NewTopLeftCursor, XCursor* NewTopRightCursor)
{
	defaultMoveCursor = NewMoveCursor;
	defaultHorizontalMoveCursor = NewHorizontalCursor;
	defaultVerticalMoveCursor = NewVerticalCursor;
	defaultTopLeftMoveCursor = NewTopLeftCursor;
	defaultTopRightMoveCursor = NewTopRightCursor;
}

void XRootWindow::GetRootCursorPos(FLOAT* PointX, FLOAT* PointY)
{
	if (PointX != NULL)
		*PointX = mouseX;

	if (PointY != NULL)
		*PointY = mouseY;
}

void XRootWindow::SetRootCursorPos(FLOAT NewPointX, FLOAT NewPointY)
{
	guard(XRootWindow::SetRootCursorPos);

	mouseX = NewPointX;
	mouseY = NewPointY;
	bMouseMoved = TRUE;

	ClampMousePosition();
	ProcessMouseMove();

	unguard;
}

void XRootWindow::EnableRendering(UBOOL bNewRender)
{
	bRender = bNewRender;
}

UBOOL XRootWindow::IsRenderingEnabled()
{
	return bRender;
}

void XRootWindow::SetRenderViewport(FLOAT NewX, FLOAT NewY, FLOAT NewWidth, FLOAT NewHeight)
{
	bClipRender = TRUE;
	renderX = NewX;
	renderY = NewY;
	renderWidth = NewWidth;
	renderHeight = NewHeight;
}

void XRootWindow::ResetRenderViewport()
{
	bClipRender = FALSE;
	renderX = 0.0f;
	renderY = 0.0f;
}

void XRootWindow::SetRawBackground(UTexture* Texture, FColor NewColor)
{
	rawBackground = Texture;
	rawColor = NewColor;
}

void XRootWindow::SetRawBackgroundSize(FLOAT NewWidth, FLOAT NewHeight)
{
	rawBackgroundWidth = NewWidth;
	rawBackgroundHeight = NewHeight;
}

void XRootWindow::StretchRawBackground(UBOOL bStretch)
{
	bStretchRawBackground = bStretch;
}

void XRootWindow::EnablePositionalSound(UBOOL bEnabled)
{
	bPositionalSound = bEnabled;
}

void XRootWindow::LockMouse(UBOOL bLockMove, UBOOL bLockButton)
{
	bMouseMoveLocked = bLockMove;
	bMouseButtonLocked = bLockButton;
}

void XRootWindow::ShowCursor(UBOOL bShow)
{
	bCursorVisible = bShow;
}

UBOOL XRootWindow::IsMouseButton(EInputKey Key)
{
	if (Key == IK_MouseX)
		return TRUE;

	if (Key == IK_MouseY)
		return TRUE;

	if (Key == IK_MouseZ)
		return TRUE;

	if (Key == IK_MouseW)
		return TRUE;

	if (Key == IK_LeftMouse)
		return TRUE;

	if (Key == IK_RightMouse)
		return TRUE;

	if (Key == IK_MiddleMouse)
		return TRUE;

	if (Key == IK_MouseWheelUp)
		return TRUE;

	if (Key == IK_MouseWheelDown)
		return TRUE;

	return FALSE;
}

void XRootWindow::GrabKeyboardEvents()
{
	guard(XRootWindow::GrabKeyboardEvents);

	if (handleKeyboardRef <= 0)
	{
		for (INT i = 0; i < IK_MAX; i++)
		{
			if (IsMouseButton((EInputKey)i) == FALSE)
				keyDownMap[i] = 0;
		}
	}

	handleKeyboardRef++;

	unguard;
}

void XRootWindow::UngrabKeyboardEvents()
{
	if (handleKeyboardRef > 0)
		handleKeyboardRef--;
}

void XRootWindow::GrabMouseEvents()
{
	guard(XRootWindow::GrabMouseEvents);

	if (handleMouseRef <= 0)
	{
		for (INT i = 0; i < IK_MAX; i++)
		{
			if (IsMouseButton((EInputKey)i) == TRUE)
				keyDownMap[i] = 0;
		}
	}

	handleMouseRef++;

	unguard;
}

void XRootWindow::UngrabMouseEvents()
{
	if (handleMouseRef > 0)
		handleMouseRef--;
}

void XRootWindow::SetSnapshotSize(FLOAT NewWidth, FLOAT NewHeight)
{
	snapshotWidth = (INT)NewWidth;
	snapshotHeight = (INT)NewHeight;
}

void XRootWindow::ResizeRoot(UCanvas* Canvas)
{
	guard(XRootWindow::ResizeRoot);

	if (Canvas == NULL)
		return;

	FLOAT CanvasWidth = Canvas->ClipX;
	FLOAT CanvasHeight = Canvas->ClipY;

	INT NewHMultiplier = (INT)(CanvasWidth * (1.0f / 640.0f));
	INT NewVMultiplier = (INT)(CanvasHeight * (1.0f / 480.0f));

	if (NewHMultiplier < 1)
		NewHMultiplier = 1;

	if (NewVMultiplier < 1)
		NewVMultiplier = 1;

	if (NewVMultiplier < NewHMultiplier)
		NewHMultiplier = NewVMultiplier;
	else if (NewVMultiplier > NewHMultiplier)
		NewVMultiplier = NewHMultiplier;

	hMultiplier = NewHMultiplier;
	vMultiplier = NewVMultiplier;

	x = 0.0f;
	y = 0.0f;
	winHAlign = HALIGN_Left;
	winVAlign = VALIGN_Top;
	hMargin0 = 0.0f;
	hMargin1 = 0.0f;
	vMargin0 = 0.0f;
	vMargin1 = 0.0f;

	FLOAT NewWidth = CanvasWidth / (FLOAT)hMultiplier;
	FLOAT NewHeight = CanvasHeight / (FLOAT)vMultiplier;

	hardcodedWidth = NewWidth;
	hardcodedHeight = NewHeight;

	if (width != NewWidth || height != NewHeight)
		ConfigureChild(0.0f, 0.0f, NewWidth, NewHeight);

	ClampMousePosition();

	unguard;
}

void XRootWindow::PreRender(UCanvas* Canvas)
{
	guard(XRootWindow::PreRender);

	ResizeRoot(Canvas);

	if (Canvas == NULL)
		return;

	FSceneNode* Frame = Canvas->Frame;
	if (Frame == NULL)
		return;

	if (GReconStoredFrameCount < GReconMaxStoredFrames)
		appMemcpy(&GReconStoredFrames[GReconStoredFrameCount], Frame, sizeof(FSceneNode));

	GReconStoredFrameCount++;

	if (bRender == TRUE)
	{
		if (bClipRender == TRUE)
		{
			INT NewXB = Frame->XB + (INT)((FLOAT)hMultiplier * renderX);
			INT NewYB = Frame->YB + (INT)((FLOAT)vMultiplier * renderY);
			INT NewXEnd = NewXB + (INT)((FLOAT)hMultiplier * renderWidth);
			INT NewYEnd = NewYB + (INT)((FLOAT)vMultiplier * renderHeight);

			if (NewXB < Frame->XB)
				NewXB = Frame->XB;

			if (NewYB < Frame->YB)
				NewYB = Frame->YB;

			if (NewXEnd > Frame->XB + Frame->X)
				NewXEnd = Frame->XB + Frame->X;

			if (NewYEnd > Frame->YB + Frame->Y)
				NewYEnd = Frame->YB + Frame->Y;

			Frame->XB = NewXB;
			Frame->YB = NewYB;
			Frame->X = NewXEnd - NewXB;
			Frame->Y = NewYEnd - NewYB;
		}
	}
	else
	{
		Frame->X = 0;
		Frame->Y = 0;
	}

	Frame->ComputeRenderSize();

	Canvas->X = Frame->X;
	Canvas->Y = Frame->Y;
	Canvas->ClipX = (FLOAT)Frame->X;
	Canvas->ClipY = (FLOAT)Frame->Y;
	Canvas->SetClip(Frame->XB, Frame->YB, Frame->X, Frame->Y);

	unguard;
}

void XRootWindow::PostRender(UCanvas* Canvas)
{
	guard(XRootWindow::PostRender);

	InitializeWindows();

	FSceneNode LocalFrame;
	appMemzero(&LocalFrame, sizeof(LocalFrame));

	if (Canvas != NULL && Canvas->Frame != NULL)
	{
		appMemcpy(&LocalFrame, Canvas->Frame, sizeof(FSceneNode));
		rootFrame = &LocalFrame;

		GReconStoredFrameCount--;
		if (GReconStoredFrameCount >= 0)
			appMemcpy(Canvas->Frame, &GReconStoredFrames[GReconStoredFrameCount], sizeof(FSceneNode));

		Canvas->SetClip(0, 0, Canvas->Frame->X, Canvas->Frame->Y);
	}

	PaintWindows(Canvas);
	rootFrame = NULL;

	unguard;
}

void XRootWindow::ClipListInit(XClipRect& Clip, TArray<XClipRect>& ClipList)
{
	guard(XRootWindow::ClipListInit);

	if (ClipList.Num() > 0)
		ClipList.Remove(0, ClipList.Num());

	INT Index = ClipList.Add();
	ClipList(Index) = Clip;

	unguard;
}

static UBOOL ReconClipRectHasArea(const XClipRect& Clip)
{
	if (Clip.clipWidth <= 0.0f)
		return FALSE;

	if (Clip.clipHeight <= 0.0f)
		return FALSE;

	return TRUE;
}

void XRootWindow::ClipListSubtract(XClipRect& CutOut, TArray<XClipRect>& ClipList)
{
	guard(XRootWindow::ClipListSubtract);

	INT InitialCount = ClipList.Num();

	for (INT Index = 0; Index < InitialCount; Index++)
	{
		XClipRect& Current = ClipList(Index);
		XClipRect Cut = Current;
		Cut.Intersect(CutOut);

		if (Cut.HasArea() == TRUE)
		{
			XClipRect Pieces[4];

			// Top strip.
			Pieces[0].originX = Cut.originX;
			Pieces[0].originY = Cut.originY;
			Pieces[0].clipX = Current.clipX;
			Pieces[0].clipY = Current.clipY;
			Pieces[0].clipWidth = Current.clipWidth;
			Pieces[0].clipHeight = Cut.clipY - Current.clipY;

			// Bottom strip.
			Pieces[1].originX = Cut.originX;
			Pieces[1].originY = Cut.originY;
			Pieces[1].clipX = Current.clipX;
			Pieces[1].clipY = Cut.clipY + Cut.clipHeight;
			Pieces[1].clipWidth = Current.clipWidth;
			Pieces[1].clipHeight = Current.clipY + Current.clipHeight - Pieces[1].clipY;

			// Left strip.
			Pieces[2].originX = Cut.originX;
			Pieces[2].originY = Cut.originY;
			Pieces[2].clipX = Current.clipX;
			Pieces[2].clipY = Cut.clipY;
			Pieces[2].clipWidth = Cut.clipX - Current.clipX;
			Pieces[2].clipHeight = Cut.clipHeight;

			// Right strip.
			Pieces[3].originX = Cut.originX;
			Pieces[3].originY = Cut.originY;
			Pieces[3].clipX = Cut.clipX + Cut.clipWidth;
			Pieces[3].clipY = Cut.clipY;
			Pieces[3].clipWidth = Current.clipX + Current.clipWidth - Pieces[3].clipX;
			Pieces[3].clipHeight = Cut.clipHeight;

			UBOOL bUsedCurrentSlot = FALSE;

			for (INT PieceIndex = 0; PieceIndex < 4; PieceIndex++)
			{
				if (ReconClipRectHasArea(Pieces[PieceIndex]) == TRUE)
				{
					if (bUsedCurrentSlot == FALSE)
					{
						Current = Pieces[PieceIndex];
						bUsedCurrentSlot = TRUE;
					}
					else
					{
						INT NewIndex = ClipList.Add();
						ClipList(NewIndex) = Pieces[PieceIndex];
					}
				}
			}

			if (bUsedCurrentSlot == FALSE)
				Current.clipHeight = 0.0f;
		}
	}

	INT WriteIndex = 0;
	for (INT ReadIndex = 0; ReadIndex < ClipList.Num(); ReadIndex++)
	{
		if (ReconClipRectHasArea(ClipList(ReadIndex)) == TRUE)
		{
			if (WriteIndex != ReadIndex)
				ClipList(WriteIndex) = ClipList(ReadIndex);

			WriteIndex++;
		}
	}

	if (WriteIndex < ClipList.Num())
		ClipList.Remove(WriteIndex, ClipList.Num() - WriteIndex);

	unguard;
}

void XRootWindow::ClipListGenerate(XWindow* Window, TArray<XClipRect>& ClipList)
{
	guard(XRootWindow::ClipListGenerate);

	XWindow* Current = Window;

	if (Current == NULL)
	{
		ClipListInit(clipRect, ClipList);

		if (bRender == TRUE)
		{
			if (bClipRender == TRUE)
			{
				XClipRect RenderClip = clipRect;
				RenderClip.clipX = renderX;
				RenderClip.clipY = renderY;
				RenderClip.clipWidth = renderWidth;
				RenderClip.clipHeight = renderHeight;
				ClipListSubtract(RenderClip, ClipList);
			}
			else
			{
				ClipListSubtract(clipRect, ClipList);
			}
		}

		Current = this;
	}

	if (Current->bDrawRawBackground == TRUE)
	{
		for (XWindow* Child = Current->GetBottomChild(TRUE); Child != NULL; Child = Child->GetHigherSibling(TRUE))
			ClipListGenerate(Child, ClipList);
	}
	else
	{
		ClipListSubtract(Current->clipRect, ClipList);
	}

	unguard;
}

void XRootWindow::DrawRawBackground(XGC* GC, UTexture* Texture, FColor Color)
{
	guard(XRootWindow::DrawRawBackground);

	if (GC == NULL)
		return;

	if (Texture == NULL)
		return;

	GC->PushGC();
	GC->EnableDrawing(TRUE);
	GC->SetStyle(STY_Normal);
	GC->SetTileColor(Color);

	ClipListGenerate(NULL, GReconRawBackgroundClipList);

	for (INT Index = 0; Index < GReconRawBackgroundClipList.Num(); Index++)
	{
		XClipRect& DrawClip = GReconRawBackgroundClipList(Index);

		FLOAT DrawX = DrawClip.originX + DrawClip.clipX;
		FLOAT DrawY = DrawClip.originY + DrawClip.clipY;

		if (bStretchRawBackground == TRUE)
		{
			GC->DrawStretchedTexture(DrawX,
				DrawY,
				DrawClip.clipWidth,
				DrawClip.clipHeight,
				0.0f,
				0.0f,
				rawBackgroundWidth,
				rawBackgroundHeight,
				Texture);
		}
		else
		{
			GC->DrawPattern(DrawX,
				DrawY,
				DrawClip.clipWidth,
				DrawClip.clipHeight,
				DrawX,
				DrawY,
				Texture);
		}
	}

	GC->PopGC();

	unguard;
}

void XRootWindow::PaintWindows(UCanvas* Canvas)
{
	guard(XRootWindow::PaintWindows);

	if (Canvas == NULL)
		return;

	if (winGC == NULL)
		return;

	FLOAT MouseRelX = 0.0f;
	FLOAT MouseRelY = 0.0f;
	XWindow* MouseWindow = GetMouseWindow(NULL, &MouseRelX, &MouseRelY);

	paintCycles = 0;
	DWORD StartCycles = 0;
	StartCycles = appCycles();

	INT SaveIndex = winGC->PushGC();
	winGC->SetCanvas(Canvas);
	winGC->SetMultipliers(hMultiplier, vMultiplier);

	DrawRawBackground(winGC, rawBackground, rawColor);

	UTexture* DebugTexture = NULL;
	if (bShowFrames == TRUE)
		DebugTexture = debugTexture;

	FLOAT DebugPhase = frameTimer / 1.0f;
	DrawTree(Canvas, DebugTexture, DebugPhase, hMultiplier, vMultiplier);

	winGC->PopGC(SaveIndex);

	SaveIndex = winGC->PushGC();
	winGC->EnableDrawing(TRUE);
	winGC->SetCanvas(Canvas);
	winGC->SetMultipliers(hMultiplier, vMultiplier);

	if (handleMouseRef > 0 && bCursorVisible == TRUE)
	{
		FLOAT HotX = 0.0f;
		FLOAT HotY = 0.0f;
		FColor CursorColor(255, 255, 255);
		UTexture* ShadowTexture = NULL;
		UTexture* CursorTexture = NULL;

		if (MouseWindow != NULL)
			CursorTexture = MouseWindow->QueryCursor(MouseRelX, MouseRelY, &HotX, &HotY, &CursorColor, &ShadowTexture);

		if (CursorTexture != NULL)
		{
			FLOAT DrawX = mouseX - HotX;
			FLOAT DrawY = mouseY - HotY;

			if (ShadowTexture != NULL)
			{
				winGC->SetStyle(STY_Modulated);
				winGC->DrawIcon(DrawX, DrawY, ShadowTexture);
			}

			winGC->SetStyle(STY_Masked);
			winGC->SetTileColor(CursorColor);
			winGC->DrawIcon(DrawX, DrawY, CursorTexture);
		}
	}

	paintCycles = appCycles() - StartCycles;

	if (bShowStats == TRUE)
	{
		winGC->SetFonts(Canvas->SmallFont, Canvas->SmallFont);
		winGC->EnableWordWrap(TRUE);
		winGC->SetTextColor(FColor(255, 255, 255));
		winGC->SetAlignments(HALIGN_Center, VALIGN_Top);
		winGC->Printf(0.0f, 0.0f, width, height, TEXT("WinTick: %5.1f ms, Paint: %5.1f ms"), (FLOAT)tickCycles * GSecondsPerCycle * 1000.0f, (FLOAT)paintCycles * GSecondsPerCycle * 1000.0f);
	}

	winGC->PopGC(SaveIndex);
	XGC::CleanUp();

	unguard;
}

void XRootWindow::ClampMousePosition()
{
	guard(XRootWindow::ClampMousePosition);

	FLOAT MaxX = x + width - 1.0f;
	FLOAT MaxY = y + height - 1.0f;

	if (mouseX < x)
		mouseX = x;
	else if (mouseX >= MaxX)
		mouseX = MaxX;

	if (mouseY < y)
		mouseY = y;
	else if (mouseY >= MaxY)
		mouseY = MaxY;

	unguard;
}

void XRootWindow::ProcessMouseMove()
{
	guard(XRootWindow::ProcessMouseMove);

	if (bMouseMoved == FALSE)
		return;

	XModalWindow* ModalWindow = (XModalWindow*)ReconGetTopModalOrRoot(this);
	FLOAT RelX = 0.0f;
	FLOAT RelY = 0.0f;
	XWindow* MouseWindow = GetMouseWindow(ModalWindow, &RelX, &RelY);

	if (MouseWindow != NULL && MouseWindow->bBeingDestroyed == TRUE)
		MouseWindow = NULL;

	if (lastMouseWindow != MouseWindow)
	{
		XWindow* OldMouseWindow = lastMouseWindow;

		if (OldMouseWindow != NULL)
		{
			OldMouseWindow->LockWindow();
			OldMouseWindow->MouseLeftWindow();

			if (ModalWindow != NULL && ModalWindow->focusMode >= MFOCUS_EnterLeave)
				SetFocusWindow(NULL);

			OldMouseWindow->UnlockWindow();
		}

		lastMouseWindow = NULL;

		if (MouseWindow != NULL)
		{
			MouseWindow->LockWindow();
			MouseWindow->MouseEnteredWindow();

			if (MouseWindow->bBeingDestroyed == FALSE)
			{
				if (ModalWindow != NULL && ModalWindow->focusMode >= MFOCUS_Enter)
					SetFocusWindow(MouseWindow);

				lastMouseWindow = MouseWindow;
			}

			MouseWindow->UnlockWindow();
		}
	}

	if (prevMouseX != mouseX || prevMouseY != mouseY)
	{
		MouseWindow = lastMouseWindow;

		if (MouseWindow != NULL && MouseWindow->bBeingDestroyed == FALSE)
		{
			MouseWindow->LockWindow();

			if (MouseWindow->bBeingDestroyed == FALSE)
				MouseWindow->MouseMoved(mouseX - MouseWindow->clipRect.originX, mouseY - MouseWindow->clipRect.originY);

			MouseWindow->UnlockWindow();
		}

		prevMouseX = mouseX;
		prevMouseY = mouseY;
	}

	bMouseMoved = FALSE;

	unguard;
}

XWindow* XRootWindow::GetMouseWindow(XModalWindow* Modal, FLOAT* RelX, FLOAT* RelY)
{
	guard(XRootWindow::GetMouseWindow);

	XWindow* SearchRoot = Modal;
	if (SearchRoot == NULL)
		SearchRoot = ReconGetTopModalOrRoot(this);

	XWindow* Result = grabbedWindow;

	if (Result == NULL && SearchRoot != NULL)
	{
		FLOAT SearchX = mouseX - SearchRoot->clipRect.originX;
		FLOAT SearchY = mouseY - SearchRoot->clipRect.originY;
		Result = SearchRoot->FindWindowByPoint(SearchX, SearchY, NULL, NULL);

		if (Result == NULL)
			Result = SearchRoot;
	}

	FLOAT LocalX = mouseX;
	FLOAT LocalY = mouseY;

	if (Result != NULL)
	{
		LocalX = mouseX - Result->clipRect.originX;
		LocalY = mouseY - Result->clipRect.originY;
	}

	if (RelX != NULL)
		*RelX = LocalX;

	if (RelY != NULL)
		*RelY = LocalY;

	return Result;

	unguard;
}

UBOOL XRootWindow::SetMousePosition(FLOAT NewMouseX, FLOAT NewMouseY)
{
	guard(XRootWindow::SetMousePosition);

	if (handleMouseRef <= 0)
		return FALSE;

	FLOAT RootX = (FLOAT)(INT)(NewMouseX / (FLOAT)hMultiplier);
	FLOAT RootY = (FLOAT)(INT)(NewMouseY / (FLOAT)vMultiplier);

	return HandleMouse(RootX, RootY);

	unguard;
}

UBOOL XRootWindow::SetMouseDelta(FLOAT DeltaX, FLOAT DeltaY)
{
	guard(XRootWindow::SetMouseDelta);

	FLOAT AbsX = DeltaX;
	FLOAT AbsY = DeltaY;

	if (AbsX < 0.0f)
		AbsX = -AbsX;

	if (AbsY < 0.0f)
		AbsY = -AbsY;

	if (AbsX > 2.0f || AbsY > 2.0f)
	{
		DeltaX += DeltaX;
		DeltaY += DeltaY;
	}

	if (XReconGetCompatibilityBool(TEXT("bCompensateMouseDeltaForInterfaceScale"), TRUE) == TRUE)
	{
		if (hMultiplier > 0)
			DeltaX /= (FLOAT)hMultiplier;

		if (vMultiplier > 0)
			DeltaY /= (FLOAT)vMultiplier;
	}

	FLOAT MouseSensitivity = ReconGetInterfaceMouseSensitivity();
	DeltaX *= MouseSensitivity;
	DeltaY *= MouseSensitivity;

	if (handleMouseRef <= 0)
		return FALSE;

	return HandleMouse(mouseX + DeltaX, mouseY + DeltaY);

	unguard;
}

UBOOL XRootWindow::HandleMouse(FLOAT NewX, FLOAT NewY)
{
	guard(XRootWindow::HandleMouse);

	if (bMouseMoveLocked == FALSE)
	{
		mouseX = NewX;
		mouseY = NewY;
		bMouseMoved = TRUE;

		ClampMousePosition();
		ProcessMouseMove();
	}

	return TRUE;

	unguard;
}

UBOOL XRootWindow::Process(EInputKey Key, EInputAction Action, FLOAT Delta)
{
	guard(XRootWindow::Process);

	if ((INT)Key < 0 || (INT)Key >= IK_MAX)
		return FALSE;

	UBOOL bRepeat = FALSE;
	UBOOL bReleaseWithoutPress = FALSE;

	if (Action == IST_Press)
	{
		if (keyDownMap[Key] != 0)
			bRepeat = TRUE;

		keyDownMap[Key] = 1;
	}
	else if (Action == IST_Release)
	{
		if (keyDownMap[Key] == 0)
			bReleaseWithoutPress = TRUE;

		keyDownMap[Key] = 0;
	}

	if (IsMouseButton(Key) == TRUE)
	{
		if (handleMouseRef > 0)
		{
			if (parentPawn != NULL)
			{
				parentPawn->bFire = FALSE;
				parentPawn->bAltFire = FALSE;
			}

			if (bReleaseWithoutPress == FALSE)
				HandleButtons(Key, Action);

			return TRUE;
		}
	}
	else if (handleKeyboardRef > 0)
	{
		if (bReleaseWithoutPress == FALSE)
			HandleVirtualKeyboard(Key, Action, bRepeat);

		return TRUE;
	}

	return FALSE;

	unguard;
}

UBOOL XRootWindow::Key(EInputKey Key)
{
	guard(XRootWindow::Key);

	if (handleKeyboardRef <= 0)
		return FALSE;

	HandleKeyboard(Key);
	return TRUE;

	unguard;
}

UBOOL XRootWindow::HandleKeyboard(TCHAR Ch)
{
	guard(XRootWindow::HandleKeyboard);

	XWindow* Target = focusWindow;
	XWindow* RootAsWindow = this;

	if (Target == NULL)
		Target = ReconGetTopModalOrRoot(this);

	UBOOL bHandled = FALSE;

	while (Target != NULL)
	{
		if (Target->windowType >= WIN_Modal || RootAsWindow->IsKeyDown(IK_Alt) == FALSE)
			bHandled = Target->KeyPressed(Ch);

		if (bHandled == TRUE)
			break;

		check((Target->bBeingDestroyed == FALSE) && "Handled window destroyed itself without reporting handled");
		Target = Target->parent;
	}

	return bHandled;

	unguard;
}

UBOOL XRootWindow::HandleVirtualKeyboard(EInputKey Key, EInputAction Action, UBOOL bRepeat)
{
	guard(XRootWindow::HandleVirtualKeyboard);

	XWindow* Target = focusWindow;
	if (Target == NULL)
		Target = ReconGetTopModalOrRoot(this);

	UBOOL bHandled = FALSE;

	while (Target != NULL)
	{
		if (bRepeat == FALSE)
			bHandled = Target->RawKeyPressed(Key, Action, FALSE);

		if (bHandled == TRUE)
			break;

		check((Target->bBeingDestroyed == FALSE) && "RawKeyPressed destroyed target without consuming event");

		if (Action == IST_Press)
		{
			bHandled = Target->VirtualKeyPressed(Key, bRepeat);
			if (bHandled == TRUE)
				break;
		}

		check((Target->bBeingDestroyed == FALSE) && "VirtualKeyPressed destroyed target without consuming event");
		Target = Target->parent;
	}

	return bHandled;

	unguard;
}

UBOOL XRootWindow::HandleButtons(EInputKey Key, EInputAction Action)
{
	guard(XRootWindow::HandleButtons);

	if (bMouseButtonLocked == TRUE)
		return TRUE;

	XModalWindow* Modal = (XModalWindow*)ReconGetTopModalOrRoot(this);
	XWindow* MouseWindow = GetMouseWindow(Modal, NULL, NULL);

	if (MouseWindow != NULL)
	{
		if (Action == IST_Press)
		{
			MouseWindow->GrabMouse();

			if (Modal->focusMode != MFOCUS_None)
				SetFocusWindow(MouseWindow);
		}
		else if (Action == IST_Release)
		{
			MouseWindow->UngrabMouse();
		}
	}

	FLOAT Now = XWindow::GetTickOffset();
	FLOAT AbsX = mouseX - firstButtonMouseX;
	FLOAT AbsY = mouseY - firstButtonMouseY;

	if (AbsX < 0.0f)
		AbsX = -AbsX;

	if (AbsY < 0.0f)
		AbsY = -AbsY;

	FLOAT RemainingPressTime = lastButtonPress - Now;

	if (RemainingPressTime <= 0.0f || MouseWindow != lastButtonWindow || AbsX > maxMouseDist || AbsY > maxMouseDist || Key != lastButtonType)
	{
		if (XReconShouldLogDiagnostics() == TRUE && MouseWindow != NULL)
			GLog->Logf(TEXT("Assigning Last Button Window to %s"), MouseWindow->GetFullName());

		lastButtonPress = 0.0f;
		lastButtonType = Key;
		lastButtonWindow = MouseWindow;
		clickCount = 0;
		RemainingPressTime = 0.0f;
	}

	if (Action == IST_Press)
	{
		if (RemainingPressTime > 0.0f)
			clickCount++;

		lastButtonPress = Now + multiClickTimeout;

		if (clickCount < 1)
		{
			firstButtonMouseX = mouseX;
			firstButtonMouseY = mouseY;
		}
	}

	UBOOL bHandled = FALSE;

	while (MouseWindow != NULL)
	{
		FLOAT LocalX = 0.0f;
		FLOAT LocalY = 0.0f;
		ConvertCoordinates(this, mouseX, mouseY, MouseWindow, &LocalX, &LocalY);

		bHandled = MouseWindow->RawMouseButtonPressed(LocalX, LocalY, Key, Action);
		if (bHandled == TRUE)
			return TRUE;

		check((MouseWindow->bBeingDestroyed == FALSE) && "RawMouseButtonPressed destroyed target without consuming event");

		INT ReportClickCount = clickCount;
		if (MouseWindow->maxClicks > 0)
			ReportClickCount = clickCount % MouseWindow->maxClicks;

		ReportClickCount++;

		if (Action == IST_Press)
			bHandled = MouseWindow->MouseButtonPressed(LocalX, LocalY, Key, ReportClickCount);
		else if (Action == IST_Release)
			bHandled = MouseWindow->MouseButtonReleased(LocalX, LocalY, Key, ReportClickCount);

		if (bHandled == TRUE)
			return TRUE;

		check((MouseWindow->bBeingDestroyed == FALSE) && "MouseButton handler destroyed target without consuming event");
		MouseWindow = MouseWindow->parent;
	}

	return FALSE;

	unguard;
}

void XRootWindow::InitializeWindows()
{
	guard(XRootWindow::InitializeWindows);

	if (initCount > 0)
		InvokeWindowReady(this);

	check(initCount == 0);

	unguard;
}

void XRootWindow::InvokeWindowReady(XWindow* Window)
{
	guard(XRootWindow::InvokeWindowReady);

	for (XWindow* Child = Window->GetBottomChild(); Child != NULL; Child = Child->GetHigherSibling())
		InvokeWindowReady(Child);

	if (Window->bIsInitialized == FALSE)
	{
		Window->ProcessScript(EXTENSION_WindowReady, NULL, TRUE);
		Window->bIsInitialized = TRUE;
		initCount--;
	}

	unguard;
}

void XRootWindow::RootTick(FLOAT DeltaSeconds)
{
	guard(XRootWindow::RootTick);

	DWORD StartCycles = 0;
	StartCycles = appCycles();

	LockWindow();

	tickCycles = 0;
	TickTree(this, DeltaSeconds);

	if (bShowFrames == TRUE)
	{
		frameTimer += DeltaSeconds;
		while (frameTimer > 1.0f)
			frameTimer -= 1.0f;
	}

	tickCycles = appCycles() - StartCycles;

	UnlockWindow();

	unguard;
}

void XRootWindow::TickTree(XWindow* Window, FLOAT DeltaSeconds)
{
	guard(XRootWindow::TickTree);

	if (Window == NULL)
		return;

	Window->LockWindow();

	XWindow* HigherSibling = Window->GetHigherSibling(FALSE);
	TickTree(HigherSibling, DeltaSeconds);

	if (Window->bBeingDestroyed == FALSE)
	{
		Window->Tick(DeltaSeconds);

		if (Window->bBeingDestroyed == FALSE)
		{
			if (Window->bTickEnabled == TRUE)
			{
				FLOAT ScriptDelta = DeltaSeconds;
				Window->ProcessScript(EXTENSION_Tick, &ScriptDelta, FALSE);
			}

			if (Window->bBeingDestroyed == FALSE)
			{
				Window->InvokeTimers(DeltaSeconds);

				if (Window->bBeingDestroyed == FALSE)
				{
					XWindow* BottomChild = Window->GetBottomChild(FALSE);
					TickTree(BottomChild, DeltaSeconds);
				}
			}
		}
	}

	Window->UnlockWindow();

	unguard;
}

void XRootWindow::Tick(FLOAT DeltaSeconds)
{
	guard(XRootWindow::Tick);

	if (focusWindow == NULL)
	{
		XModalWindow* Modal = (XModalWindow*)ReconGetTopModalOrRoot(this);

		/*
			Original Extension.dll does not blindly focus the top child here.
			It first tries the modal/root preferredFocus saved by SetFocusWindow(),
			unless focusMode is MFOCUS_EnterLeave.  This restores the last selected
			menu button after focus is temporarily cleared by mouse leave / modal
			changes instead of jumping back to the first/top child.
		*/
		if (Modal != NULL && Modal->focusMode < MFOCUS_EnterLeave)
		{
			XWindow* Candidate = Modal->preferredFocus;
			if (Candidate != NULL && Candidate->IsTraversable(TRUE) == TRUE)
				SetFocusWindow(Candidate);
		}

		if (focusWindow == NULL)
			MoveTabGroup(MOVE_Down);
	}

	lastButtonPress -= DeltaSeconds;
	if (lastButtonPress < 0.0f)
		lastButtonPress = 0.0f;

	XModalWindow::Tick(DeltaSeconds);

	unguard;
}

void XRootWindow::ConfigurationChanged()
{
	guard(XRootWindow::ConfigurationChanged);
	XTabGroupWindow::ConfigurationChanged();
	unguard;
}

UBOOL XRootWindow::ChildRequestedReconfiguration(XWindow* Child)
{
	guard(XRootWindow::ChildRequestedReconfiguration);

	struct
	{
		XWindow* Child;
		UBOOL bAccepted;
	} Parms;

	Parms.Child = Child;
	Parms.bAccepted = FALSE;

	ProcessScript(EXTENSION_ChildRequestedReconfiguration, &Parms, FALSE);
	return Parms.bAccepted;

	unguard;
}

void XRootWindow::DescendantRemoved(XWindow* Descendant)
{
	guard(XRootWindow::DescendantRemoved);

	if (Descendant == lastButtonWindow)
	{
		if (XReconShouldLogDiagnostics() == TRUE)
			GLog->Logf(TEXT("Tried to remove lastbuttonwindow descendant. %s"), Descendant->GetFullName());

		lastButtonWindow = NULL;
	}

	if (Descendant == lastMouseWindow)
	{
		lastMouseWindow = NULL;
		prevMouseX = -1.0f;
		prevMouseY = -1.0f;
		bMouseMoved = TRUE;
	}

	if (Descendant == grabbedWindow)
		grabbedWindow = NULL;

	if (Descendant == focusWindow)
		focusWindow = NULL;

	XModalWindow::DescendantRemoved(Descendant);

	unguard;
}

UBOOL XRootWindow::ConvertVectorToCoordinates(FVector Location, FLOAT* RelativeX, FLOAT* RelativeY)
{
	guard(XRootWindow::ConvertVectorToCoordinates);

	check(rootFrame != NULL);

	FVector CameraSpace;
	CameraSpace.X = (Location - rootFrame->Coords.Origin) | rootFrame->Coords.XAxis;
	CameraSpace.Y = (Location - rootFrame->Coords.Origin) | rootFrame->Coords.YAxis;
	CameraSpace.Z = (Location - rootFrame->Coords.Origin) | rootFrame->Coords.ZAxis;

	FLOAT OutX = rootFrame->FX2;
	FLOAT OutY = rootFrame->FY2;
	UBOOL bInFront = FALSE;

	if (CameraSpace.Z > 1.0f)
	{
		FLOAT Scale = rootFrame->Proj.Z / CameraSpace.Z;
		OutX = ((CameraSpace.X * Scale) + rootFrame->FX2) / (FLOAT)hMultiplier + renderX;
		OutY = ((CameraSpace.Y * Scale) + rootFrame->FY2) / (FLOAT)vMultiplier + renderY;
		bInFront = TRUE;
	}

	if (RelativeX != NULL)
		*RelativeX = (FLOAT)(INT)OutX;

	if (RelativeY != NULL)
		*RelativeY = (FLOAT)(INT)OutY;

	return bInFront;

	unguard;
}

void XRootWindow::DestroyAllWindows()
{
	guard(XRootWindow::DestroyAllWindows);

	while (GReconFirstRootWindow != NULL)
		GReconFirstRootWindow->SafeDestroy();

	unguard;
}

FLOAT XRootWindow::GetWindowsTickOffset(UBOOL bReset)
{
	guard(XRootWindow::GetWindowsTickOffset);

	/*
		Original Extension.dll computes this through the full appSeconds() path:
		when GTimestamp is enabled it uses the full 64-bit RDTSC value, not the
		low 32-bit appCycles() helper.  Using appCycles() here wraps every few
		seconds on fast CPUs and can feed negative/large deltas into UI timers.
	*/
	DOUBLE Now = appSeconds();
	FLOAT Result = (FLOAT)(Now - GReconLastRootTickSeconds);

	if (bReset == TRUE)
		GReconLastRootTickSeconds = Now;

	return Result;

	unguard;
}

void XRootWindow::TickWindows(FLOAT DeltaSeconds)
{
	guard(XRootWindow::TickWindows);

	(void)DeltaSeconds;

	FLOAT TickOffset = GetWindowsTickOffset(TRUE);

	for (XRootWindow* Root = GReconFirstRootWindow; Root != NULL; Root = Root->nextRootWindow)
		Root->RootTick(TickOffset);

	unguard;
}

UTexture* XRootWindow::GenerateSnapshot(UTexture* UseTexture, UBOOL bFilter, UBOOL bColor, UBOOL bCreateNewTexture)
{
	guard(XRootWindow::GenerateSnapshot);

	if (GRenderDevice == NULL || GRenderDevice->Viewport == NULL)
		return NULL;

	UViewport* Viewport = GRenderDevice->Viewport;
	INT SourceWidth = Viewport->SizeX;
	INT SourceHeight = Viewport->SizeY;

	if (SourceWidth <= 0 || SourceHeight <= 0)
		return NULL;

	TArray<FColor> SourcePixels;
	SourcePixels.Add(SourceWidth * SourceHeight);
	Viewport->RenDev->ReadPixels(&SourcePixels(0));

	UTexture* Texture = UseTexture;
	if (Texture == NULL || bCreateNewTexture == TRUE)
	{
		Texture = new(GetOuter()) UTexture;
		if (Texture == NULL)
			return NULL;
	}

	Texture->Format = TEXF_P8;
	Texture->PolyFlags |= PF_NoSmooth;

	INT DestWidth = ReconNextPowerOfTwo(snapshotWidth);
	INT DestHeight = ReconNextPowerOfTwo(snapshotHeight);

	Texture->Init(DestWidth, DestHeight);
	Texture->PostLoad();

	if (bColor == TRUE)
	{
		BuildPaletteFromFramebuffer(Texture, &SourcePixels(0), SourceWidth, SourceHeight);
	}
	else
	{
		for (INT i = 0; i < 256; i++)
			Texture->Palette->Colors(i) = FColor(i, i, i, 255);
	}

	Texture->CreateColorRange();
	Texture->MaxColor = FColor(255, 255, 255, 255);

	FLOAT StepX = (FLOAT)SourceWidth / (FLOAT)snapshotWidth;
	FLOAT StepY = (FLOAT)SourceHeight / (FLOAT)snapshotHeight;

	for (INT DestY = 0; DestY < snapshotHeight; DestY++)
	{
		BYTE* DestLine = (BYTE*)Texture->Mips(0).DataArray.GetData() + Texture->Mips(0).USize * DestY;
		FLOAT SourceY = (FLOAT)DestY * StepY;

		for (INT DestX = 0; DestX < snapshotWidth; DestX++)
		{
			FLOAT SourceX = (FLOAT)DestX * StepX;

			FLOAT SumR = 0.0f;
			FLOAT SumG = 0.0f;
			FLOAT SumB = 0.0f;
			INT Count = 0;

			for (INT BoxX = 0; (FLOAT)BoxX < StepX; BoxX++)
			{
				for (INT BoxY = 0; (FLOAT)BoxY < StepY; BoxY++)
				{
					INT ReadX = (INT)SourceX + BoxX;
					INT ReadY = (INT)SourceY + BoxY;

					if (ReadX >= 0 && ReadX < SourceWidth && ReadY >= 0 && ReadY < SourceHeight)
					{
						FColor SourceColor = SourcePixels(ReadY * SourceWidth + ReadX);
						SumR += (FLOAT)SourceColor.R;
						SumG += (FLOAT)SourceColor.G;
						SumB += (FLOAT)SourceColor.B;
						Count++;
					}
				}
			}

			if (Count <= 0)
				Count = 1;

			FColor AvgColor(ReconClampByte(SumR / (FLOAT)Count), ReconClampByte(SumG / (FLOAT)Count), ReconClampByte(SumB / (FLOAT)Count), 255);

			if (bColor == TRUE)
				DestLine[DestX] = Texture->Palette->BestMatch(AvgColor, 0);
			else
				DestLine[DestX] = (BYTE)(((INT)AvgColor.R + (INT)AvgColor.G + (INT)AvgColor.B) / 3);
		}
	}

	return Texture;

	unguard;
}
