/*=============================================================================
	XWindow_Core_Reconstructed.cpp
	Reconstruction notes/source for Deus Ex Extension.dll XWindow core.

	This file is intentionally focused on exact behaviour of the original native
	window core: ownership tree, visibility/sensitivity propagation, geometry
	negotiation, GC state access, clipping, timers, focus and child ordering.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XWindow);

/*-----------------------------------------------------------------------------
	Reconstructed internal globals.

	The original Extension.dll keeps several anonymous file-scope counters around
	XWindow.  Their addresses appear in the decompiler as dword_100AEAFC,
	dword_100AEB00, dword_100AEB04, dword_100AEB08 and dword_100AEB0C.
-----------------------------------------------------------------------------*/

static XWindow* GReconConfigAuthority = NULL;

XWindow* XReconPushConfigAuthority(XWindow* NewAuthority)
{
	XWindow* OldAuthority = GReconConfigAuthority;
	GReconConfigAuthority = NewAuthority;
	return OldAuthority;
}

void XReconPopConfigAuthority(XWindow* OldAuthority)
{
	GReconConfigAuthority = OldAuthority;
}
static INT GReconInsideConfigurationChanged = 0;
static INT GReconInsidePreferredSizeQuery = 0;
static INT GReconInsideDraw = 0;
static INT GReconInsideConfigureChild = 0;

static inline FPlane XReconColorToPlane(const FColor& Color)
{
	return FPlane(
		FLOAT(Color.R) / 255.0f,
		FLOAT(Color.G) / 255.0f,
		FLOAT(Color.B) / 255.0f,
		0.0f);
}

static UBOOL XReconIsEffectivelyVisible(XWindow* Window)
{
	for (XWindow* TestWindow = Window; TestWindow != NULL; TestWindow = TestWindow->parent)
	{
		if (TestWindow->bIsVisible == FALSE)
			return FALSE;
	}

	return TRUE;
}

static UBOOL XReconIsEffectivelySensitive(XWindow* Window)
{
	for (XWindow* TestWindow = Window; TestWindow != NULL; TestWindow = TestWindow->parent)
	{
		if (TestWindow->bIsSensitive == FALSE)
			return FALSE;
	}

	return TRUE;
}

/*-----------------------------------------------------------------------------
	Construction / destruction.
-----------------------------------------------------------------------------*/

XWindow::XWindow(XWindow* Parent)
{
	clipRect.originX = 0.0f;
	clipRect.originY = 0.0f;
	clipRect.clipX = 0.0f;
	clipRect.clipY = 0.0f;
	clipRect.clipWidth = 0.0f;
	clipRect.clipHeight = 0.0f;
}

void XWindow::Init(XWindow* Parent)
{
	guard(XWindow::Init);

	windowType = WIN_Normal;
	parentOwner = Parent;

	bIsVisible = FALSE;
	bIsSensitive = TRUE;
	bIsSelectable = FALSE;
	bIsInitialized = FALSE;
	bBeingDestroyed = FALSE;
	lockCount = 0;

	bDrawRawBackground = TRUE;
	bStretchBackground = FALSE;
	bSmoothBackground = FALSE;
	backgroundStyle = STY_Normal;

	textColor = FColor(0, 255, 0);
	textPlane = XReconColorToPlane(textColor);
	normalFont = NULL;
	boldFont = NULL;
	textVSpacing = 1.0f;

	tileColor = FColor(255, 255, 255);
	tilePlane = XReconColorToPlane(tileColor);
	bSpecialText = TRUE;
	bTextTranslucent = FALSE;

	x = 0.0f;
	y = 0.0f;
	width = 10.0f;
	height = 10.0f;

	winHAlign = HALIGN_Left;
	winVAlign = VALIGN_Top;
	hMargin0 = 0.0f;
	hMargin1 = 0.0f;
	vMargin0 = 0.0f;
	vMargin1 = 0.0f;
	hardcodedWidth = -1.0f;
	hardcodedHeight = -1.0f;

	holdX = 0.0f;
	holdY = 0.0f;
	holdWidth = 0.0f;
	holdHeight = 0.0f;

	acceleratorKey = 0;

	bNeedsReconfigure = TRUE;
	bNeedsQuery = TRUE;
	bConfigured = FALSE;
	bLastWidthSpecified = TRUE;
	bLastHeightSpecified = TRUE;
	lastSpecifiedWidth = 0.0f;
	lastSpecifiedHeight = 0.0f;
	lastQueryWidth = 0.0f;
	lastQueryHeight = 0.0f;

	bTickEnabled = FALSE;
	maxClicks = 0;

	background = NULL;
	defaultCursor = NULL;
	defaultCursorShadow = NULL;
	defaultHotX = -1.0f;
	defaultHotY = -1.0f;
	defaultCursorColor = FColor(255, 255, 255);

	focusSound = NULL;
	unfocusSound = NULL;
	visibleSound = NULL;
	invisibleSound = NULL;
	soundVolume = 1.0f;

	clientObject = NULL;
	rowMajorIndex = -1;
	colMajorIndex = -1;
	firstAttribute = NULL;
	firstTimer = NULL;
	freeTimer = NULL;

	winGC = new(this) XGC();
	getGC = new(this) XGC();

	parent = NULL;
	firstChild = NULL;
	lastChild = NULL;
	prevSibling = NULL;
	nextSibling = NULL;

	if (Parent != NULL)
	{
		Parent->AddChild(this, TRUE);
		Parent->ChildAdded(this);

		for (XWindow* NotifyWindow = Parent; NotifyWindow != NULL; NotifyWindow = NotifyWindow->parent)
			NotifyWindow->DescendantAdded(this);
	}

	if (parent != NULL)
	{
		clipRect.originX = parent->clipRect.originX;
		clipRect.originY = parent->clipRect.originY;
		clipRect.clipX = 0.0f;
		clipRect.clipY = 0.0f;
		clipRect.clipWidth = width;
		clipRect.clipHeight = height;
	}
	else
	{
		clipRect.originX = 0.0f;
		clipRect.originY = 0.0f;
		clipRect.clipX = 0.0f;
		clipRect.clipY = 0.0f;
		clipRect.clipWidth = width;
		clipRect.clipHeight = height;
	}

	for (XWindow* FontWindow = parent; FontWindow != NULL; FontWindow = FontWindow->parent)
	{
		if ((FontWindow->normalFont != NULL) || (FontWindow->boldFont != NULL))
		{
			normalFont = FontWindow->normalFont;
			boldFont = FontWindow->boldFont;
			break;
		}
	}

	if (winGC != NULL)
		winGC->SetFonts(normalFont, boldFont);

	unguard;
}

void XWindow::CleanUp(void)
{
	guard(XWindow::CleanUp);

	XRootWindow* RootWindow = GetRootWindow();

	if (parent != NULL)
	{
		parent->ChildRemoved(this);
		for (XWindow* NotifyWindow = parent; NotifyWindow != NULL; NotifyWindow = NotifyWindow->parent)
			NotifyWindow->DescendantRemoved(this);
	}

	if (RootWindow != NULL)
	{
		if (RootWindow->grabbedWindow == this)
			RootWindow->grabbedWindow = NULL;

		if (RootWindow->focusWindow == this)
			RootWindow->focusWindow = NULL;
	}

	while (firstTimer != NULL)
	{
		XTimerStruct* NextTimer = firstTimer->nextTimer;
		GMalloc->Free(firstTimer);
		firstTimer = NextTimer;
	}

	while (freeTimer != NULL)
	{
		XTimerStruct* NextTimer = freeTimer->nextTimer;
		GMalloc->Free(freeTimer);
		freeTimer = NextTimer;
	}

	if (winGC != NULL)
		delete winGC;
	winGC = NULL;

	if (getGC != NULL)
		delete getGC;
	getGC = NULL;

	if (parent != NULL)
		parent->RemoveChild(this);

	unguard;
}

void XWindow::Destroy(void)
{
	guard(XWindow::Destroy);
	PreDestroy();
	check(lockCount <= 0);
	Super::Destroy();
	unguard;
}

UBOOL XWindow::SafeDestroy(void)
{
	guard(XWindow::SafeDestroy);
	PreDestroy();

	if (lockCount <= 0)
	{
		delete this;
		return TRUE;
	}

	return FALSE;
	unguard;
}

void XWindow::LockWindow(void)
{
	lockCount++;
}

void XWindow::UnlockWindow(void)
{
	guard(XWindow::UnlockWindow);

	if (lockCount > 0)
	{
		lockCount--;
		if (bBeingDestroyed == TRUE)
			SafeDestroy();
	}

	unguard;
}

void XWindow::PreDestroy(void)
{
	guard(XWindow::PreDestroy);

	if (bBeingDestroyed == FALSE)
	{
		if (XReconIsEffectivelyVisible(this) == TRUE)
			SetVisibility(FALSE);

		SetSelectability(FALSE);
		KillAllChildren();
		DestroyWindow();
		CleanUp();
		bBeingDestroyed = TRUE;
	}

	unguard;
}

void XWindow::KillAllChildren(void)
{
	guard(XWindow::KillAllChildren);

	XWindow* Child = firstChild;
	while (Child != NULL)
	{
		XWindow* NextChild = Child->nextSibling;
		Child->SafeDestroy();
		Child = NextChild;
	}

	unguard;
}

XWindowAttributes* XWindow::GetWindowAttributes(UClass* ParentClass)
{
	guard(XWindow::GetWindowAttributes);

	XWindowAttributes* Result = NULL;
	if (this != NULL)
	{
		for (Result = firstAttribute; Result != NULL; Result = Result->nextAttribute)
		{
			if (Result->parentClass == ParentClass)
				break;
		}
	}

	return Result;
	unguard;
}

void XWindow::DeleteWindowAttributes(UClass* ParentClass)
{
	guard(XWindow::DeleteWindowAttributes);

	XWindowAttributes* Attributes = GetWindowAttributes(ParentClass);
	if (Attributes != NULL)
		delete Attributes;

	unguard;
}

/*-----------------------------------------------------------------------------
	Tree and ancestry.
-----------------------------------------------------------------------------*/

XWindow* XWindow::GetParent(void)
{
	return parent;
}

XTabGroupWindow* XWindow::GetTabGroupWindow(void)
{
	for (XWindow* TestWindow = this; TestWindow != NULL; TestWindow = TestWindow->parent)
	{
		if (TestWindow->windowType != WIN_Normal)
			return (XTabGroupWindow*)TestWindow;
	}

	return NULL;
}

XModalWindow* XWindow::GetModalWindow(void)
{
	for (XWindow* TestWindow = this; TestWindow != NULL; TestWindow = TestWindow->parent)
	{
		if (TestWindow->windowType >= WIN_Modal)
			return (XModalWindow*)TestWindow;
	}

	return NULL;
}

XRootWindow* XWindow::GetRootWindow(void)
{
	XWindow* RootWindow = this;

	if (RootWindow != NULL)
	{
		while (RootWindow->parent != NULL)
			RootWindow = RootWindow->parent;
	}

	if ((RootWindow == NULL) || (RootWindow->windowType < WIN_Root))
		return NULL;

	return (XRootWindow*)RootWindow;
}

APlayerPawnExt* XWindow::GetPlayerPawn(void)
{
	guard(XWindow::GetPlayerPawn);

	XRootWindow* RootWindow = GetRootWindow();
	if (RootWindow != NULL)
		return RootWindow->parentPawn;

	return NULL;
	unguard;
}

XWindow* XWindow::GetLowerSibling(UBOOL bVisibleOnly)
{
	XWindow* Result = prevSibling;

	while ((bVisibleOnly == TRUE) && (Result != NULL) && (Result->bIsVisible == FALSE))
		Result = Result->prevSibling;

	return Result;
}

XWindow* XWindow::GetHigherSibling(UBOOL bVisibleOnly)
{
	XWindow* Result = nextSibling;

	while ((bVisibleOnly == TRUE) && (Result != NULL) && (Result->bIsVisible == FALSE))
		Result = Result->nextSibling;

	return Result;
}

XWindow* XWindow::GetTopChild(UBOOL bVisibleOnly)
{
	XWindow* Result = lastChild;

	if ((bVisibleOnly == TRUE) && (Result != NULL) && (Result->bIsVisible == FALSE))
		Result = Result->GetLowerSibling(bVisibleOnly);

	return Result;
}

XWindow* XWindow::GetBottomChild(UBOOL bVisibleOnly)
{
	XWindow* Result = firstChild;

	if ((bVisibleOnly == TRUE) && (Result != NULL) && (Result->bIsVisible == FALSE))
		Result = Result->GetHigherSibling(bVisibleOnly);

	return Result;
}

void XWindow::AddChild(XWindow* Child, UBOOL bRaise)
{
	guard(XWindow::AddChild);

	if (Child != NULL)
	{
		if (Child->parent != NULL)
			Child->parent->RemoveChild(Child);

		if (bRaise == TRUE)
		{
			Child->nextSibling = NULL;
			Child->prevSibling = lastChild;

			if (lastChild != NULL)
				lastChild->nextSibling = Child;
			else
				firstChild = Child;

			lastChild = Child;
		}
		else
		{
			Child->prevSibling = NULL;
			Child->nextSibling = firstChild;

			if (firstChild != NULL)
				firstChild->prevSibling = Child;
			else
				lastChild = Child;

			firstChild = Child;
		}

		Child->parent = this;
	}

	unguard;
}

void XWindow::RemoveChild(XWindow* Child)
{
	if (Child != NULL)
	{
		if (Child->prevSibling != NULL)
			Child->prevSibling->nextSibling = Child->nextSibling;
		else
			firstChild = Child->nextSibling;

		if (Child->nextSibling != NULL)
			Child->nextSibling->prevSibling = Child->prevSibling;
		else
			lastChild = Child->prevSibling;

		Child->nextSibling = NULL;
		Child->prevSibling = NULL;
		Child->parent = NULL;
	}
}

void XWindow::Raise(void)
{
	guard(XWindow::Raise);

	if ((parent != NULL) && (nextSibling != NULL))
	{
		parent->AddChild(this, TRUE);
		AskParentForReconfigure();
		CheckFocusWindow();
	}

	unguard;
}

void XWindow::Lower(void)
{
	guard(XWindow::Lower);

	if ((parent != NULL) && (prevSibling != NULL))
	{
		parent->AddChild(this, FALSE);
		AskParentForReconfigure();
		CheckFocusWindow();
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Visibility, sensitivity and traversal.
-----------------------------------------------------------------------------*/

void XWindow::SetVisibility(UBOOL bNewVisibility)
{
	guard(XWindow::SetVisibility);

	if (bIsVisible != bNewVisibility)
	{
		if (parent != NULL)
		{
			XWindow* OldAuthority = GReconConfigAuthority;
			GReconConfigAuthority = parent;
			parent->ChildRequestedVisibilityChange(this, bNewVisibility);
			GReconConfigAuthority = OldAuthority;
		}
		else
		{
			SetChildVisibility(bNewVisibility);
		}
	}

	unguard;
}

void XWindow::SetSensitivity(UBOOL bNewSensitivity)
{
	guard(XWindow::SetSensitivity);

	if (bIsSensitive != bNewSensitivity)
	{
		UBOOL bWasEffectivelySensitive = XReconIsEffectivelySensitive(this);
		bIsSensitive = bNewSensitivity;
		UBOOL bNowEffectivelySensitive = XReconIsEffectivelySensitive(this);

		if (bNowEffectivelySensitive != bWasEffectivelySensitive)
		{
			CheckFocusWindow();
			CheckGrabbedWindow();
			MakeAcceleratorTableDirty();
			InvokeSensitivityChange(bNowEffectivelySensitive);
		}
	}

	unguard;
}

void XWindow::SetSelectability(UBOOL bNewSelectable)
{
	guard(XWindow::SetSelectability);

	if (bIsSelectable != bNewSelectable)
	{
		XTabGroupWindow* TabGroupWindow = GetTabGroupWindow();

		bIsSelectable = bNewSelectable;
		CheckFocusWindow();

		if (bIsSelectable == TRUE)
			TabGroupWindow->AddWindowToTables(this);
		else
			TabGroupWindow->RemoveWindowFromTables(this);

		if (acceleratorKey != 0)
			MakeAcceleratorTableDirty();
	}

	unguard;
}

UBOOL XWindow::IsTraversable(UBOOL bCheckModal)
{
	guard(XWindow::IsTraversable);

	if (bIsSelectable == FALSE)
		return FALSE;

	XWindow* LocalModalWindow = NULL;

	for (XWindow* TestWindow = this; TestWindow != NULL; TestWindow = TestWindow->parent)
	{
		if ((TestWindow->bIsVisible == FALSE) || (TestWindow->bIsSensitive == FALSE))
			return FALSE;

		if (bCheckModal == TRUE)
		{
			if ((LocalModalWindow == NULL) && (TestWindow->windowType >= WIN_Modal))
				LocalModalWindow = TestWindow;

			if (TestWindow->windowType == WIN_Root)
			{
				XWindow* CurrentModalWindow = NULL;

				for (XWindow* Child = TestWindow->GetTopChild(TRUE); Child != NULL; Child = Child->GetLowerSibling(TRUE))
				{
					if (Child->windowType >= WIN_Modal)
					{
						CurrentModalWindow = Child;
						break;
					}
				}

				if (CurrentModalWindow == NULL)
					CurrentModalWindow = TestWindow;

				if (CurrentModalWindow != LocalModalWindow)
					return FALSE;
			}
		}
	}

	return TRUE;
	unguard;
}


XWindow* XWindow::MoveFocus(EMove moveDir, UBOOL bForceWrap)
{
	guard(XWindow::MoveFocus);

	XRootWindow* RootWindow = GetRootWindow();
	if (RootWindow == NULL)
		return NULL;

	XWindow* FocusWindow = RootWindow->GetFocusWindow();
	if (FocusWindow == NULL)
		return RootWindow->MoveTabGroup(moveDir);

	XTabGroupWindow* TabGroupWindow = FocusWindow->GetTabGroupWindow();
	if (TabGroupWindow == NULL)
		return RootWindow->MoveTabGroup(moveDir);

	TArray<XWindow*>* WindowList;	
	INT CurrentIndex;
	if (moveDir == MOVE_Left || moveDir == MOVE_Right)
	{
		WindowList = &TabGroupWindow->rowMajorWindowList;
		CurrentIndex = FocusWindow->rowMajorIndex;
	}
	else
	{
		WindowList = &TabGroupWindow->colMajorWindowList;
		CurrentIndex = FocusWindow->colMajorIndex;
	}

	INT Count = WindowList->Num();
	if (Count <= 0)
		return RootWindow->MoveTabGroup(moveDir);

	if (CurrentIndex < 0 || CurrentIndex >= Count || (*WindowList)(CurrentIndex) != FocusWindow)
	{
		CurrentIndex = INDEX_NONE;
		for (INT FindIndex = 0; FindIndex < Count; FindIndex++)
		{
			if ((*WindowList)(FindIndex) == FocusWindow)
			{
				CurrentIndex = FindIndex;
				break;
			}
		}
	}

	if (CurrentIndex < 0 || CurrentIndex >= Count)
		return RootWindow->MoveTabGroup(moveDir);

	UBOOL bReverse = (moveDir == MOVE_Left || moveDir == MOVE_Up);
	UBOOL bWrap = bForceWrap;
	if (TabGroupWindow->bWrapFocus == TRUE)
		bWrap = TRUE;

	INT TestIndex = CurrentIndex;
	INT Remaining = Count;
	while (Remaining > 0)
	{
		if (bReverse == TRUE)
		{
			TestIndex--;
			if (TestIndex < 0)
			{
				if (bWrap == TRUE)
				{
					TestIndex = Count - 1;
				}
				else
				{
					TestIndex = CurrentIndex;
					bReverse = FALSE;
				}
			}
		}
		else
		{
			TestIndex++;
			if (TestIndex >= Count)
			{
				if (bWrap == TRUE)
				{
					TestIndex = 0;
				}
				else
				{
					TestIndex = CurrentIndex;
					bReverse = TRUE;
				}
			}
		}

		XWindow* Candidate = (*WindowList)(TestIndex);
		if (Candidate != NULL && Candidate->IsTraversable(TRUE) == TRUE)
		{
			RootWindow->SetFocusWindow(Candidate);
			return RootWindow->GetFocusWindow();
		}

		Remaining--;
	}

	return RootWindow->MoveTabGroup(moveDir);

	unguard;
}

XWindow* XWindow::MoveTabGroup(EMove moveDir)
{
	guard(XWindow::MoveTabGroup);

	XRootWindow* RootWindow = GetRootWindow();
	if (RootWindow == NULL)
		return NULL;

	XModalWindow* ModalWindow = NULL;
	for (XWindow* Child = RootWindow->GetTopChild(TRUE); Child != NULL; Child = Child->GetLowerSibling(TRUE))
	{
		if (Child->windowType >= WIN_Modal)
		{
			ModalWindow = (XModalWindow*)Child;
			break;
		}
	}

	if (ModalWindow == NULL)
		ModalWindow = (XModalWindow*)RootWindow;

	if (ModalWindow == NULL)
		return NULL;

	UBOOL bResort = FALSE;
	for (INT i = 0; i < ModalWindow->tabGroupWindowList.Num(); i++)
	{
		XTabGroupWindow* TabGroupWindow = ModalWindow->tabGroupWindowList(i);
		if (TabGroupWindow != NULL && TabGroupWindow->ComputeTabGroupLocation() == TRUE)
			bResort = TRUE;
	}

	if (bResort == TRUE)
		ModalWindow->ResortTabGroupTable();

	INT Count = ModalWindow->tabGroupWindowList.Num();
	if (Count <= 0)
		return RootWindow->GetFocusWindow();

	INT StartIndex = 0;
	XWindow* FocusWindow = RootWindow->GetFocusWindow();
	if (FocusWindow != NULL && FocusWindow->GetModalWindow() == ModalWindow)
	{
		XTabGroupWindow* CurrentTabGroup = FocusWindow->GetTabGroupWindow();
		if (CurrentTabGroup != NULL && CurrentTabGroup->tabGroupIndex >= 0 && CurrentTabGroup->tabGroupIndex < Count)
		{
			if (moveDir == MOVE_Left || moveDir == MOVE_Up)
				StartIndex = CurrentTabGroup->tabGroupIndex - 1;
			else
				StartIndex = CurrentTabGroup->tabGroupIndex + 1;
		}
	}

	if (StartIndex < 0)
		StartIndex = Count - 1;
	else if (StartIndex >= Count)
		StartIndex = 0;

	INT TestIndex = StartIndex;
	for (INT Step = 0; Step < Count; Step++)
	{
		XTabGroupWindow* TabGroupWindow = ModalWindow->tabGroupWindowList(TestIndex);
		if (TabGroupWindow != NULL)
		{
			for (INT RowIndex = 0; RowIndex < TabGroupWindow->rowMajorWindowList.Num(); RowIndex++)
			{
				XWindow* Candidate = TabGroupWindow->rowMajorWindowList(RowIndex);
				if (Candidate != NULL && Candidate->IsTraversable(FALSE) == TRUE)
				{
					RootWindow->SetFocusWindow(Candidate);
					return RootWindow->GetFocusWindow();
				}
			}
		}

		if (moveDir == MOVE_Left || moveDir == MOVE_Up)
		{
			TestIndex--;
			if (TestIndex < 0)
				TestIndex = Count - 1;
		}
		else
		{
			TestIndex++;
			if (TestIndex >= Count)
				TestIndex = 0;
		}
	}

	return RootWindow->GetFocusWindow();

	unguard;
}

UBOOL XWindow::IsFocusWindow(void)
{
	guard(XWindow::IsFocusWindow);

	XRootWindow* RootWindow = GetRootWindow();
	if (RootWindow != NULL)
		return RootWindow->focusWindow == this;

	return FALSE;
	unguard;
}

UBOOL XWindow::IsKeyDown(int ikValue)
{
	guard(XWindow::IsKeyDown);

	XRootWindow* RootWindow = GetRootWindow();
	if ((RootWindow != NULL) && (ikValue >= 0) && (ikValue < IK_MAX))
		return RootWindow->keyDownMap[ikValue] != 0;

	return FALSE;
	unguard;
}

void XWindow::SetChildVisibility(UBOOL bNewVisibility)
{
	guard(XWindow::SetChildVisibility);

	if (bIsVisible != bNewVisibility)
	{
		UBOOL bWasEffectivelyVisible = XReconIsEffectivelyVisible(this);
		bIsVisible = bNewVisibility;
		UBOOL bNowEffectivelyVisible = XReconIsEffectivelyVisible(this);

		if (bNowEffectivelyVisible != bWasEffectivelyVisible)
		{
			CheckFocusWindow();
			CheckGrabbedWindow();
			bNeedsReconfigure = TRUE;
			RecomputeInitTree(bNowEffectivelyVisible);
			InvokeVisibilityChange(bNowEffectivelyVisible);
			ReconfigureTree(FALSE);
			MakeAcceleratorTableDirty();
		}
	}

	unguard;
}

void XWindow::RecomputeInitTree(UBOOL bNewVisibility)
{
	guard(XWindow::RecomputeInitTree);

	XRootWindow* RootWindow = GetRootWindow();

	if ((bIsInitialized == FALSE) && (RootWindow != NULL))
	{
		if (bNewVisibility == TRUE)
			RootWindow->initCount++;
		else
			RootWindow->initCount--;
	}

	for (XWindow* Child = firstChild; Child != NULL; Child = Child->nextSibling)
	{
		if (Child->bIsVisible == TRUE)
			Child->RecomputeInitTree(bNewVisibility);
	}

	unguard;
}

void XWindow::InvokeVisibilityChange(UBOOL bNewVisibility)
{
	guard(XWindow::InvokeVisibilityChange);

	VisibilityChanged(bNewVisibility);

	if (bNewVisibility == TRUE)
		PlaySound(visibleSound);
	else
		PlaySound(invisibleSound);

	for (XWindow* Child = firstChild; Child != NULL; Child = Child->nextSibling)
	{
		if (Child->bIsVisible == TRUE)
			Child->InvokeVisibilityChange(bNewVisibility);
	}

	unguard;
}

void XWindow::InvokeSensitivityChange(UBOOL bNewSensitivity)
{
	guard(XWindow::InvokeSensitivityChange);

	SensitivityChanged(bNewSensitivity);

	for (XWindow* Child = firstChild; Child != NULL; Child = Child->nextSibling)
	{
		if (Child->bIsSensitive == TRUE)
			Child->InvokeSensitivityChange(bNewSensitivity);
	}

	unguard;
}

void XWindow::CheckFocusWindow(void)
{
	guard(XWindow::CheckFocusWindow);

	XRootWindow* RootWindow = GetRootWindow();
	if ((RootWindow != NULL) && (RootWindow->focusWindow != NULL))
	{
		XWindow* OldFocusWindow = RootWindow->focusWindow;

		if (OldFocusWindow->IsTraversable(TRUE) == FALSE)
		{
			XModalWindow* CurrentModalWindow = RootWindow->GetCurrentModal();
			XWindow* PreferredFocusWindow = NULL;

			if (CurrentModalWindow != NULL)
				PreferredFocusWindow = CurrentModalWindow->preferredFocus;

			if ((PreferredFocusWindow != NULL) && (PreferredFocusWindow->IsTraversable(TRUE) == TRUE))
				SetFocusWindow(PreferredFocusWindow);

			if (RootWindow->focusWindow == OldFocusWindow)
			{
				XWindow* MovedFocusWindow = RootWindow->MoveFocus(MOVE_Right, TRUE);
				if (MovedFocusWindow == OldFocusWindow)
					RootWindow->SetFocusWindow(NULL);
			}
		}
	}

	unguard;
}

void XWindow::CheckGrabbedWindow(void)
{
	guard(XWindow::CheckGrabbedWindow);

	XRootWindow* RootWindow = GetRootWindow();
	if ((RootWindow != NULL) && (RootWindow->grabbedWindow != NULL))
	{
		XWindow* GrabbedWindow = RootWindow->grabbedWindow;

		if ((XReconIsEffectivelyVisible(GrabbedWindow) == FALSE) || (XReconIsEffectivelySensitive(GrabbedWindow) == FALSE))
		{
			GrabbedWindow->MouseGrabLost(NULL);
			RootWindow->grabbedWindow = NULL;
		}
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Focus, mouse grab and cursor position.
-----------------------------------------------------------------------------*/

void XWindow::GrabMouse(void)
{
	guard(XWindow::GrabMouse);

	XRootWindow* RootWindow = GetRootWindow();
	if ((RootWindow != NULL) && (RootWindow->grabbedWindow != this))
	{
		if (RootWindow->grabbedWindow != NULL)
			RootWindow->grabbedWindow->MouseGrabLost(this);

		RootWindow->grabbedWindow = this;
	}

	unguard;
}

void XWindow::UngrabMouse(void)
{
	guard(XWindow::UngrabMouse);

	XRootWindow* RootWindow = GetRootWindow();
	if ((RootWindow != NULL) && (RootWindow->grabbedWindow == this))
		RootWindow->grabbedWindow = NULL;

	unguard;
}

void XWindow::GetCursorPos(FLOAT* pPointX, FLOAT* pPointY)
{
	guard(XWindow::GetCursorPos);

	XRootWindow* RootWindow = GetRootWindow();
	if (RootWindow != NULL)
	{
		RootWindow->GetRootCursorPos(pPointX, pPointY);

		if (pPointX != NULL)
			*pPointX -= clipRect.originX;

		if (pPointY != NULL)
			*pPointY -= clipRect.originY;
	}

	unguard;
}

void XWindow::SetCursorPos(FLOAT newPointX, FLOAT newPointY)
{
	guard(XWindow::SetCursorPos);

	XRootWindow* RootWindow = GetRootWindow();
	if (RootWindow != NULL)
		RootWindow->SetRootCursorPos(newPointX + clipRect.originX, newPointY + clipRect.originY);

	unguard;
}

UBOOL XWindow::SetFocusWindow(XWindow* pNewFocusWindow)
{
	guard(XWindow::SetFocusWindow);

	XRootWindow* RootWindow = GetRootWindow();
	if (RootWindow == NULL)
		return FALSE;

	XWindow* NewFocusWindow = pNewFocusWindow;

	if (NewFocusWindow != NULL)
	{
		for (XWindow* TestWindow = NewFocusWindow; TestWindow != NULL; TestWindow = TestWindow->parent)
		{
			if (TestWindow->bIsSelectable == TRUE)
			{
				NewFocusWindow = TestWindow;
				break;
			}
		}
	}

	if (RootWindow->focusWindow == NewFocusWindow)
		return TRUE;

	if ((NewFocusWindow != NULL) && (NewFocusWindow->IsTraversable(TRUE) == FALSE))
		return FALSE;

	if (NewFocusWindow != NULL)
	{
		XModalWindow* ModalWindow = NewFocusWindow->GetModalWindow();
		if (ModalWindow != NULL)
			ModalWindow->preferredFocus = NewFocusWindow;
	}

	XWindow* OldFocusWindow = RootWindow->focusWindow;
	RootWindow->focusWindow = NewFocusWindow;

	if (OldFocusWindow != NULL)
	{
		OldFocusWindow->FocusLeftWindow();
		OldFocusWindow->PlaySound(OldFocusWindow->unfocusSound);
	}

	for (XWindow* LeaveNotifyWindow = OldFocusWindow; LeaveNotifyWindow != NULL; LeaveNotifyWindow = LeaveNotifyWindow->parent)
		LeaveNotifyWindow->FocusLeftDescendant(OldFocusWindow);

	if (RootWindow->focusWindow != NULL)
	{
		XWindow* FocusWindow = RootWindow->focusWindow;
		FocusWindow->AskParentToShowArea(0.0f, 0.0f, FocusWindow->width, FocusWindow->height);
		FocusWindow->PlaySound(FocusWindow->focusSound);
		FocusWindow->FocusEnteredWindow();
	}

	for (XWindow* EnterNotifyWindow = RootWindow->focusWindow; EnterNotifyWindow != NULL; EnterNotifyWindow = EnterNotifyWindow->parent)
		EnterNotifyWindow->FocusEnteredDescendant(RootWindow->focusWindow);

	return TRUE;
	unguard;
}

XWindow* XWindow::GetFocusWindow(void)
{
	guard(XWindow::GetFocusWindow);

	XRootWindow* RootWindow = GetRootWindow();
	if (RootWindow != NULL)
		return RootWindow->focusWindow;

	return NULL;
	unguard;
}

/*-----------------------------------------------------------------------------
	Background, colors and fonts.
-----------------------------------------------------------------------------*/

void XWindow::SetBackground(UTexture* newBackground)
{
	guard(XWindow::SetBackground);

	if (background != newBackground)
	{
		background = newBackground;
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::SetBackgroundStyle(BYTE newStyle)
{
	guard(XWindow::SetBackgroundStyle);

	backgroundStyle = newStyle;
	if (winGC != NULL)
		winGC->SetStyle(newStyle);

	unguard;
}

void XWindow::SetBackgroundSmoothing(UBOOL bIsSmoothed)
{
	guard(XWindow::SetBackgroundSmoothing);

	bSmoothBackground = bIsSmoothed;
	if (winGC != NULL)
		winGC->EnableSmoothing(bIsSmoothed);

	unguard;
}

void XWindow::SetTextColor(FColor newColor)
{
	guard(XWindow::SetTextColor);

	textColor = newColor;
	textPlane = XReconColorToPlane(newColor);

	if (winGC != NULL)
		winGC->SetTextColor(newColor);

	unguard;
}

FColor XWindow::GetTextColor(void)
{
	return textColor;
}

void XWindow::SetTileColor(FColor newColor)
{
	guard(XWindow::SetTileColor);

	tileColor = newColor;
	tilePlane = XReconColorToPlane(newColor);

	if (winGC != NULL)
		winGC->SetTileColor(newColor);

	unguard;
}

FColor XWindow::GetTileColor(void)
{
	return tileColor;
}

void XWindow::EnableSpecialText(UBOOL bEnabled)
{
	guard(XWindow::EnableSpecialText);

	if (bSpecialText != bEnabled)
	{
		bSpecialText = bEnabled;
		if (winGC != NULL)
			winGC->EnableSpecialText(bSpecialText);
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::EnableTranslucentText(UBOOL bNewTranslucency)
{
	guard(XWindow::EnableTranslucentText);

	if (bTextTranslucent != bNewTranslucency)
	{
		bTextTranslucent = bNewTranslucency;
		if (winGC != NULL)
			winGC->EnableTranslucentText(bTextTranslucent);
	}

	unguard;
}

void XWindow::SetFonts(UFont* newNormalFont, UFont* newBoldFont)
{
	guard(XWindow::SetFonts);

	normalFont = newNormalFont;
	boldFont = newBoldFont;

	if (winGC != NULL)
		winGC->SetFonts(newNormalFont, newBoldFont);

	AskParentForReconfigure();
	unguard;
}

void XWindow::SetFont(UFont* newFont)
{
	guard(XWindow::SetFont);
	SetFonts(newFont, newFont);
	unguard;
}

void XWindow::SetNormalFont(UFont* newNormalFont)
{
	guard(XWindow::SetNormalFont);

	normalFont = newNormalFont;
	if (winGC != NULL)
		winGC->SetNormalFont(newNormalFont);

	AskParentForReconfigure();
	unguard;
}

void XWindow::SetBoldFont(UFont* newBoldFont)
{
	guard(XWindow::SetBoldFont);

	boldFont = newBoldFont;
	if (winGC != NULL)
		winGC->SetBoldFont(newBoldFont);

	AskParentForReconfigure();
	unguard;
}

void XWindow::SetBaselineData(FLOAT newBaselineOffset, FLOAT newUnderlineHeight)
{
	guard(XWindow::SetBaselineData);

	if (winGC != NULL)
		winGC->SetBaselineData(newBaselineOffset, newUnderlineHeight);

	unguard;
}

/*-----------------------------------------------------------------------------
	Geometry negotiation.
-----------------------------------------------------------------------------*/

void XWindow::Configure(FLOAT newX, FLOAT newY, FLOAT newWidth, FLOAT newHeight)
{
	guard(XWindow::Configure);

	if (newWidth < 0.0f)
		newWidth = 0.0f;

	if (newHeight < 0.0f)
		newHeight = 0.0f;

	if ((winHAlign != HALIGN_Left) || (winVAlign != VALIGN_Top) ||
		(newX != hMargin0) || (newY != vMargin0) ||
		(newWidth != hardcodedWidth) || (newHeight != hardcodedHeight))
	{
		winHAlign = HALIGN_Left;
		winVAlign = VALIGN_Top;
		hMargin0 = newX;
		vMargin0 = newY;
		hardcodedWidth = newWidth;
		hardcodedHeight = newHeight;
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::Move(FLOAT newX, FLOAT newY)
{
	guard(XWindow::Move);

	if ((winHAlign != HALIGN_Left) || (winVAlign != VALIGN_Top) ||
		(newX != hMargin0) || (newY != vMargin0))
	{
		winHAlign = HALIGN_Left;
		winVAlign = VALIGN_Top;
		hMargin0 = newX;
		vMargin0 = newY;
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::Resize(FLOAT newWidth, FLOAT newHeight)
{
	guard(XWindow::Resize);

	if (newWidth < 0.0f)
		newWidth = 0.0f;

	if (newHeight < 0.0f)
		newHeight = 0.0f;

	if ((newWidth != hardcodedWidth) || (newHeight != hardcodedHeight))
	{
		hardcodedWidth = newWidth;
		hardcodedHeight = newHeight;
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::SetWidth(FLOAT newWidth)
{
	guard(XWindow::SetWidth);

	if (newWidth < 0.0f)
		newWidth = 0.0f;

	if (newWidth != hardcodedWidth)
	{
		hardcodedWidth = newWidth;
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::SetHeight(FLOAT newHeight)
{
	guard(XWindow::SetHeight);

	if (newHeight < 0.0f)
		newHeight = 0.0f;

	if (newHeight != hardcodedHeight)
	{
		hardcodedHeight = newHeight;
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::SetHorizontalWindowAlignment(EHAlign hAlign, FLOAT newHMargin0, FLOAT newHMargin1)
{
	guard(XWindow::SetHorizontalWindowAlignment);

	if ((winHAlign != hAlign) || (hMargin0 != newHMargin0) || (hMargin1 != newHMargin1))
	{
		winHAlign = hAlign;
		hMargin0 = newHMargin0;
		hMargin1 = newHMargin1;
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::SetVerticalWindowAlignment(EVAlign vAlign, FLOAT newVMargin0, FLOAT newVMargin1)
{
	guard(XWindow::SetVerticalWindowAlignment);

	if ((winVAlign != vAlign) || (vMargin0 != newVMargin0) || (vMargin1 != newVMargin1))
	{
		winVAlign = vAlign;
		vMargin0 = newVMargin0;
		vMargin1 = newVMargin1;
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::SetWindowAlignments(EHAlign hAlign, EVAlign vAlign, FLOAT newHMargin0, FLOAT newVMargin0, FLOAT newHMargin1, FLOAT newVMargin1)
{
	guard(XWindow::SetWindowAlignments);

	if ((winHAlign != hAlign) || (winVAlign != vAlign) ||
		(hMargin0 != newHMargin0) || (hMargin1 != newHMargin1) ||
		(vMargin0 != newVMargin0) || (vMargin1 != newVMargin1))
	{
		winHAlign = hAlign;
		winVAlign = vAlign;
		hMargin0 = newHMargin0;
		hMargin1 = newHMargin1;
		vMargin0 = newVMargin0;
		vMargin1 = newVMargin1;
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::ResetSize(void)
{
	guard(XWindow::ResetSize);

	if ((hardcodedWidth >= 0.0f) || (hardcodedHeight >= 0.0f))
	{
		hardcodedWidth = -1.0f;
		hardcodedHeight = -1.0f;
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::ResetWidth(void)
{
	guard(XWindow::ResetWidth);

	if (hardcodedWidth >= 0.0f)
	{
		hardcodedWidth = -1.0f;
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::ResetHeight(void)
{
	guard(XWindow::ResetHeight);

	if (hardcodedHeight >= 0.0f)
	{
		hardcodedHeight = -1.0f;
		AskParentForReconfigure();
	}

	unguard;
}

void XWindow::AskParentForReconfigure(void)
{
	guard(XWindow::AskParentForReconfigure);

	if (GReconInsideConfigurationChanged > 0 && XReconShouldLogDiagnostics() == TRUE)
		GLog->Logf(NAME_Warning, TEXT("***WINDOW WARNING: AskParentForReconfigure() called during configure (%s)"), GetFullName());

	if (GReconInsidePreferredSizeQuery > 0 && XReconShouldLogDiagnostics() == TRUE)
		GLog->Logf(NAME_Warning, TEXT("***WINDOW WARNING: AskParentForReconfigure() called during size query (%s)"), GetFullName());

	if (GReconInsideDraw > 0 && XReconShouldLogDiagnostics() == TRUE)
		GLog->Logf(NAME_Warning, TEXT("***WINDOW WARNING: AskParentForReconfigure() called during draw (%s)"), GetFullName());

	bNeedsReconfigure = TRUE;
	bNeedsQuery = TRUE;

	if (XReconIsEffectivelyVisible(this) == FALSE)
		return;

	if (parent != NULL)
	{
		XWindow* OldAuthority = GReconConfigAuthority;
		GReconConfigAuthority = parent;
		UBOOL bParentHandled = parent->ChildRequestedReconfiguration(this);
		GReconConfigAuthority = OldAuthority;

		if (bParentHandled == FALSE)
			parent->AskParentForReconfigure();

		OldAuthority = GReconConfigAuthority;
		GReconConfigAuthority = parent;

		if (bNeedsReconfigure == TRUE)
			ResizeChild();

		GReconConfigAuthority = OldAuthority;
	}
	else
	{
		bNeedsReconfigure = FALSE;

		XWindow* OldAuthority = GReconConfigAuthority;
		GReconConfigAuthority = this;
		GReconInsideConfigurationChanged++;
		ConfigurationChanged();
		GReconInsideConfigurationChanged--;
		GReconConfigAuthority = OldAuthority;
	}

	unguard;
}

void XWindow::InvalidateTreeConfiguration(void)
{
	guard(XWindow::InvalidateTreeConfiguration);

	bNeedsReconfigure = TRUE;
	for (XWindow* Child = firstChild; Child != NULL; Child = Child->nextSibling)
		Child->InvalidateTreeConfiguration();

	unguard;
}

void XWindow::ReconfigureTree(UBOOL bInvalidateTree)
{
	guard(XWindow::ReconfigureTree);

	if (bInvalidateTree == TRUE)
		InvalidateTreeConfiguration();

	if (XReconIsEffectivelyVisible(this) == TRUE)
	{
		if (bNeedsReconfigure == TRUE)
			AskParentForReconfigure();

		for (XWindow* Child = firstChild; Child != NULL; Child = Child->nextSibling)
			Child->ReconfigureTree(FALSE);
	}

	unguard;
}

FLOAT XWindow::QueryPreferredWidth(FLOAT queryHeight)
{
	guard(XWindow::QueryPreferredWidth);

	FLOAT PreferredWidth = 0.0f;
	QueryPreferredSize(FALSE, 0.0f, &PreferredWidth, TRUE, queryHeight, NULL);
	return PreferredWidth;
	unguard;
}

FLOAT XWindow::QueryPreferredHeight(FLOAT queryWidth)
{
	guard(XWindow::QueryPreferredHeight);

	FLOAT PreferredHeight = 0.0f;
	QueryPreferredSize(TRUE, queryWidth, NULL, FALSE, 0.0f, &PreferredHeight);
	return PreferredHeight;
	unguard;
}

void XWindow::QueryPreferredSize(FLOAT* pPreferredWidth, FLOAT* pPreferredHeight)
{
	guard(XWindow::QueryPreferredSize);
	QueryPreferredSize(FALSE, 0.0f, pPreferredWidth, FALSE, 0.0f, pPreferredHeight);
	unguard;
}

void XWindow::QueryPreferredSize(UBOOL bWidthSpecified, FLOAT queryWidth, FLOAT* pPreferredWidth, UBOOL bHeightSpecified, FLOAT queryHeight, FLOAT* pPreferredHeight)
{
	guard(XWindow::QueryPreferredSize);

	FLOAT PreferredWidth = queryWidth;
	FLOAT PreferredHeight = queryHeight;

	if (bWidthSpecified == FALSE)
	{
		if (hardcodedWidth >= 0.0f)
		{
			bWidthSpecified = TRUE;
			PreferredWidth = hardcodedWidth;
		}
		else
		{
			PreferredWidth = -1.0f;
		}
	}

	if (bHeightSpecified == FALSE)
	{
		if (hardcodedHeight >= 0.0f)
		{
			bHeightSpecified = TRUE;
			PreferredHeight = hardcodedHeight;
		}
		else
		{
			PreferredHeight = -1.0f;
		}
	}

	if ((bWidthSpecified == FALSE) || (bHeightSpecified == FALSE))
	{
		UBOOL bUseCached = FALSE;

		if ((bNeedsQuery == FALSE) &&
			(bLastWidthSpecified == bWidthSpecified) &&
			(bLastHeightSpecified == bHeightSpecified) &&
			(lastSpecifiedWidth == PreferredWidth) &&
			(lastSpecifiedHeight == PreferredHeight))
		{
			bUseCached = TRUE;
		}

		if (bUseCached == TRUE)
		{
			PreferredWidth = lastQueryWidth;
			PreferredHeight = lastQueryHeight;
		}
		else
		{
			lastSpecifiedWidth = PreferredWidth;
			lastSpecifiedHeight = PreferredHeight;
			bLastWidthSpecified = bWidthSpecified;
			bLastHeightSpecified = bHeightSpecified;
			bNeedsQuery = FALSE;

			FLOAT QueryWidth = PreferredWidth;
			FLOAT QueryHeight = PreferredHeight;

			GReconInsidePreferredSizeQuery++;
			ParentRequestedPreferredSize(bWidthSpecified, QueryWidth, bHeightSpecified, QueryHeight);
			GReconInsidePreferredSizeQuery--;

			if (bWidthSpecified == FALSE)
				PreferredWidth = QueryWidth;

			if (bHeightSpecified == FALSE)
				PreferredHeight = QueryHeight;

			lastQueryWidth = PreferredWidth;
			lastQueryHeight = PreferredHeight;
		}

		if (PreferredWidth < 0.0f)
		{
			if (background != NULL)
				PreferredWidth = FLOAT(background->USize);
			else
				PreferredWidth = width;
		}

		if (PreferredHeight < 0.0f)
		{
			if (background != NULL)
				PreferredHeight = FLOAT(background->VSize);
			else
				PreferredHeight = height;
		}
	}

	if (pPreferredWidth != NULL)
		*pPreferredWidth = PreferredWidth;

	if (pPreferredHeight != NULL)
		*pPreferredHeight = PreferredHeight;

	unguard;
}

void XWindow::QueryGranularity(FLOAT* pHGranularity, FLOAT* pVGranularity)
{
	guard(XWindow::QueryGranularity);

	FLOAT HGranularity = 1.0f;
	FLOAT VGranularity = 1.0f;

	ParentRequestedGranularity(HGranularity, VGranularity);

	if (HGranularity < 1.0f)
		HGranularity = 1.0f;

	if (VGranularity < 1.0f)
		VGranularity = 1.0f;

	if (pHGranularity != NULL)
		*pHGranularity = HGranularity;

	if (pVGranularity != NULL)
		*pVGranularity = VGranularity;

	unguard;
}

void XWindow::ResizeChild(void)
{
	guard(XWindow::ResizeChild);

	FLOAT ParentWidth = 0.0f;
	FLOAT ParentHeight = 0.0f;

	if (parent != NULL)
	{
		ParentWidth = parent->width;
		ParentHeight = parent->height;
	}

	UBOOL bWidthSpecified = FALSE;
	UBOOL bHeightSpecified = FALSE;
	FLOAT NewWidth = 0.0f;
	FLOAT NewHeight = 0.0f;

	if (winHAlign == HALIGN_Full)
	{
		bWidthSpecified = TRUE;
		NewWidth = ParentWidth - (hMargin0 + hMargin1);
	}

	if (winVAlign == VALIGN_Full)
	{
		bHeightSpecified = TRUE;
		NewHeight = ParentHeight - (vMargin0 + vMargin1);
	}

	QueryPreferredSize(bWidthSpecified, NewWidth, &NewWidth, bHeightSpecified, NewHeight, &NewHeight);

	FLOAT NewX;
	if (winHAlign == HALIGN_Center)
	{
		INT CenterOffset = INT((ParentWidth - NewWidth) * 0.5f);
		NewX = FLOAT(CenterOffset) + hMargin0;
	}
	else if (winHAlign == HALIGN_Right)
	{
		NewX = ParentWidth - NewWidth - hMargin0;
	}
	else
	{
		NewX = hMargin0;
	}

	FLOAT NewY;
	if (winVAlign == VALIGN_Center)
	{
		INT CenterOffset = INT((ParentHeight - NewHeight) * 0.5f);
		NewY = FLOAT(CenterOffset) + vMargin0;
	}
	else if (winVAlign == VALIGN_Bottom)
	{
		NewY = ParentHeight - NewHeight - vMargin0;
	}
	else
	{
		NewY = vMargin0;
	}

	ConfigureChild(NewX, NewY, NewWidth, NewHeight);
	unguard;
}

void XWindow::ConfigureChild(FLOAT newX, FLOAT newY, FLOAT newWidth, FLOAT newHeight)
{
	guard(XWindow::ConfigureChild);

	if ((GReconConfigAuthority != parent) && (GReconConfigAuthority != NULL) && XReconShouldLogDiagnostics() == TRUE)
		GLog->Logf(NAME_Warning, TEXT("***WINDOW WARNING: ConfigureChild() not called by parent during geometry negotiation (%s)"), GetFullName());

	if (GReconInsidePreferredSizeQuery > 0 && XReconShouldLogDiagnostics() == TRUE)
		GLog->Logf(NAME_Warning, TEXT("***WINDOW WARNING: ConfigureChild() called during size query (%s)"), GetFullName());

	if (GReconInsideDraw > 0 && XReconShouldLogDiagnostics() == TRUE)
		GLog->Logf(NAME_Warning, TEXT("***WINDOW WARNING: ConfigureChild() called during draw (%s)"), GetFullName());

	LockWindow();
	GReconInsideConfigureChild++;

	bConfigured = TRUE;

	if (bIsVisible == TRUE)
	{
		UBOOL bSizeChanged = FALSE;
		if ((width != newWidth) || (height != newHeight) || (bNeedsReconfigure == TRUE))
			bSizeChanged = TRUE;

		bNeedsReconfigure = FALSE;

		x = newX;
		y = newY;
		width = newWidth;
		height = newHeight;

		if (bSizeChanged == TRUE)
			ChangeConfiguration();

		if ((GReconInsideConfigureChild == 1) || (windowType != WIN_Normal))
		{
			ClipTree();

			XTabGroupWindow* TabGroupWindow = GetTabGroupWindow();
			if (TabGroupWindow != NULL)
				TabGroupWindow->ResortWindowTables();
		}
	}

	GReconInsideConfigureChild--;
	UnlockWindow();

	unguard;
}

void XWindow::AskParentToShowArea(FLOAT showX, FLOAT showY, FLOAT showWidth, FLOAT showHeight)
{
	guard(XWindow::AskParentToShowArea);

	if ((parent != NULL) && (XReconIsEffectivelyVisible(this) == TRUE))
	{
		XClipRect WholeWindow(clipRect.originX, clipRect.originY, 0.0f, 0.0f, width, height);
		XClipRect RequestedArea(clipRect.originX + showX, clipRect.originY + showY, 0.0f, 0.0f, showWidth, showHeight);

		WholeWindow.Intersect(RequestedArea);

		if (WholeWindow.HasArea() == TRUE)
		{
			XClipRect VisibleArea(clipRect);
			VisibleArea.Intersect(WholeWindow);

			if ((VisibleArea.clipWidth < WholeWindow.clipWidth) || (VisibleArea.clipHeight < WholeWindow.clipHeight))
			{
				parent->ChildRequestedShowArea(this, WholeWindow.clipX, WholeWindow.clipY, WholeWindow.clipWidth, WholeWindow.clipHeight);

				FLOAT ParentShowX = 0.0f;
				FLOAT ParentShowY = 0.0f;
				ConvertCoordinates(this, WholeWindow.clipX, WholeWindow.clipY, parent, &ParentShowX, &ParentShowY);
				parent->AskParentToShowArea(ParentShowX, ParentShowY, WholeWindow.clipWidth, WholeWindow.clipHeight);
			}
		}
	}

	unguard;
}

void XWindow::ChangeConfiguration(void)
{
	guard(XWindow::ChangeConfiguration);

	for (XWindow* ConfigChild = firstChild; ConfigChild != NULL; ConfigChild = ConfigChild->nextSibling)
	{
		XWindow* ConfigTarget = ConfigChild;
		ConfigTarget->bConfigured = TRUE;

		if ((ConfigTarget->bIsVisible == TRUE) && ((ConfigTarget->winHAlign != HALIGN_Left) || (ConfigTarget->winVAlign != VALIGN_Top)))
			ConfigTarget->bConfigured = FALSE;
	}

	XWindow* OldAuthority = GReconConfigAuthority;
	GReconConfigAuthority = this;
	GReconInsideConfigurationChanged++;
	ConfigurationChanged();
	GReconInsideConfigurationChanged--;
	GReconConfigAuthority = OldAuthority;

	for (XWindow* RecheckChild = firstChild; RecheckChild != NULL; RecheckChild = RecheckChild->nextSibling)
	{
		XWindow* RecheckTarget = RecheckChild;
		if ((RecheckTarget->bIsVisible == TRUE) &&
			((RecheckTarget->winHAlign != HALIGN_Left) || (RecheckTarget->winVAlign != VALIGN_Top)) &&
			(RecheckTarget->bConfigured == FALSE))
		{
			GReconConfigAuthority = this;
			RecheckTarget->ResizeChild();
			GReconConfigAuthority = OldAuthority;
		}
	}

	unguard;
}

void XWindow::ChangeStyleTree(void)
{
	guard(XWindow::ChangeStyleTree);

	StyleChanged();

	for (XWindow* Child = GetTopChild(FALSE); Child != NULL; Child = Child->GetLowerSibling(FALSE))
		Child->ChangeStyleTree();

	unguard;
}

void XWindow::ChangeStyle(void)
{
	guard(XWindow::ChangeStyle);

	XRootWindow* RootWindow = GetRootWindow();
	if (RootWindow != NULL)
		RootWindow->ChangeStyleTree();

	unguard;
}

void XWindow::MakeAcceleratorTableDirty(void)
{
	guard(XWindow::MakeAcceleratorTableDirty);

	XModalWindow* ModalWindow = GetModalWindow();
	if (ModalWindow != NULL)
		ModalWindow->bDirtyAccelerators = TRUE;

	unguard;
}

/*-----------------------------------------------------------------------------
	GC access and script dispatch.
-----------------------------------------------------------------------------*/

XGC* XWindow::GetGC(void)
{
	guard(XWindow::GetGC);

	if (getGC != NULL)
	{
		getGC->PushGC();
		if (winGC != NULL)
			getGC->CopyGC(*winGC);
	}

	return getGC;
	unguard;
}

void XWindow::ReleaseGC(XGC* gc)
{
	guard(XWindow::ReleaseGC);

	if (gc != NULL)
		gc->PopGC();

	unguard;
}

void XWindow::ProcessScript(FName functionName, void* params, UBOOL bCheck)
{
	guard(XWindow::ProcessScript);

	UFunction* Function = NULL;

	if (bCheck == TRUE)
		Function = FindFunctionChecked(functionName);
	else
		Function = FindFunction(functionName);

	if (Function != NULL)
		ProcessEvent(Function, params, NULL);

	unguard;
}

XWindow* XWindow::CreateNewWindow(UClass* newClass, XWindow* parentWindow, UBOOL bShow)
{
	guard(XWindow::CreateNewWindow);

	XWindow* NewWindow = NULL;

	if (newClass != NULL)
	{
		NewWindow = Cast<XWindow>(StaticConstructObject(newClass, parentWindow, NAME_None, 0, NULL, GError));

		if (NewWindow != NULL)
		{
			NewWindow->Init(parentWindow);
			NewWindow->LockWindow();
			NewWindow->ProcessScript(EXTENSION_InitWindow, NULL, TRUE);

			if (bShow == TRUE)
				NewWindow->SetVisibility(TRUE);

			NewWindow->AskParentForReconfigure();

			UBOOL bDestroyedDuringInit = NewWindow->bBeingDestroyed;
			NewWindow->UnlockWindow();

			if (bDestroyedDuringInit == TRUE)
				NewWindow = NULL;
		}
	}

	return NewWindow;
	unguard;
}

/*-----------------------------------------------------------------------------
	Cursor / hit testing.
-----------------------------------------------------------------------------*/

void XWindow::SetDefaultCursor(XCursor* newCursor, UTexture* newCursorShadow, FLOAT hotX, FLOAT hotY, FColor color)
{
	defaultCursor = newCursor;
	defaultCursorShadow = newCursorShadow;
	defaultHotX = hotX;
	defaultHotY = hotY;
	defaultCursorColor = color;
}

XWindow* XWindow::FindWindow(FLOAT pointX, FLOAT pointY, FLOAT* pRelativeX, FLOAT* pRelativeY)
{
	guard(XWindow::FindWindow);

	XRootWindow* RootWindow = GetRootWindow();
	if (RootWindow == NULL)
		return NULL;

	XWindow* SearchRoot = NULL;
	for (XWindow* Child = RootWindow->GetTopChild(TRUE); Child != NULL; Child = Child->GetLowerSibling(TRUE))
	{
		if (Child->windowType >= WIN_Modal)
		{
			SearchRoot = Child;
			break;
		}
	}

	if (SearchRoot == NULL)
		SearchRoot = RootWindow;

	FLOAT SearchX = pointX;
	FLOAT SearchY = pointY;
	ConvertCoordinates(this, pointX, pointY, SearchRoot, &SearchX, &SearchY);

	return SearchRoot->FindWindowByPoint(SearchX, SearchY, pRelativeX, pRelativeY);
	unguard;
}

XWindow* XWindow::FindWindowByPoint(FLOAT pointX, FLOAT pointY, FLOAT* pRelativeX, FLOAT* pRelativeY)
{
	guard(XWindow::FindWindowByPoint);

	if ((bIsVisible == TRUE) && (bIsSensitive == TRUE) && (IsPointInWindow(pointX, pointY) == TRUE))
	{
		for (XWindow* Child = lastChild; Child != NULL; Child = Child->prevSibling)
		{
			if (Child->windowType < WIN_Modal)
			{
				XWindow* HitWindow = Child->FindWindowByPoint(pointX - Child->x, pointY - Child->y, pRelativeX, pRelativeY);
				if (HitWindow != NULL)
					return HitWindow;
			}
		}

		if (pRelativeX != NULL)
			*pRelativeX = pointX;

		if (pRelativeY != NULL)
			*pRelativeY = pointY;

		return this;
	}

	return NULL;
	unguard;
}

UBOOL XWindow::IsPointInWindow(FLOAT pointX, FLOAT pointY)
{
	guard(XWindow::IsPointInWindow);

	return (pointX >= 0.0f) &&
		(pointX < width) &&
		(pointY >= 0.0f) &&
		(pointY < height) &&
		(QueryPointInWindow(pointX, pointY) == TRUE);

	unguard;
}

XCursor* XWindow::QueryCursor(FLOAT pointX, FLOAT pointY, FLOAT* pHotX, FLOAT* pHotY, FColor* pColor, UTexture** pCursorShadow)
{
	guard(XWindow::QueryCursor);

	for (XWindow* TestWindow = this; TestWindow != NULL; TestWindow = TestWindow->parent)
	{
		FLOAT HotX = -1.0f;
		FLOAT HotY = -1.0f;
		FColor CursorColor = FColor(255, 255, 255);
		UTexture* Shadow = NULL;

		XCursor* Cursor = TestWindow->CursorRequested(this, pointX, pointY, HotX, HotY, CursorColor, &Shadow);

		if (Cursor == NULL)
		{
			Cursor = TestWindow->defaultCursor;
			Shadow = TestWindow->defaultCursorShadow;
			HotX = TestWindow->defaultHotX;
			HotY = TestWindow->defaultHotY;
			CursorColor = TestWindow->defaultCursorColor;
		}

		if (Cursor != NULL)
		{
			if ((HotX < 0.0f) || (HotX >= Cursor->USize))
				HotX = FLOAT((Cursor->USize >> 1) - 1);

			if ((HotY < 0.0f) || (HotY >= Cursor->VSize))
				HotY = FLOAT((Cursor->VSize >> 1) - 1);

			if (pHotX != NULL)
				*pHotX = HotX;

			if (pHotY != NULL)
				*pHotY = HotY;

			if (pColor != NULL)
				*pColor = CursorColor;

			if (pCursorShadow != NULL)
				*pCursorShadow = Shadow;

			return Cursor;
		}
	}

	return NULL;
	unguard;
}

void XWindow::ConvertCoordinates(XWindow* fromWin, FLOAT fromX, FLOAT fromY, XWindow* toWin, FLOAT* pToX, FLOAT* pToY)
{
	FLOAT ToX = fromX;
	FLOAT ToY = fromY;

	if (fromWin != NULL)
	{
		ToX += fromWin->clipRect.originX;
		ToY += fromWin->clipRect.originY;
	}

	if (toWin != NULL)
	{
		ToX -= toWin->clipRect.originX;
		ToY -= toWin->clipRect.originY;
	}

	if (pToX != NULL)
		*pToX = ToX;

	if (pToY != NULL)
		*pToY = ToY;
}

UBOOL XWindow::ConvertVectorToCoordinates(FVector location, FLOAT* pRelativeX, FLOAT* pRelativeY)
{
	FLOAT RelativeX = width * 0.5f;
	FLOAT RelativeY = height * 0.5f;
	UBOOL bResult = FALSE;

	if (parent != NULL)
	{
		bResult = parent->ConvertVectorToCoordinates(location, &RelativeX, &RelativeY);
		RelativeX -= x;
		RelativeY -= y;
	}

	if (pRelativeX != NULL)
		*pRelativeX = RelativeX;

	if (pRelativeY != NULL)
		*pRelativeY = RelativeY;

	return bResult;
}

/*-----------------------------------------------------------------------------
	Timers, actor refs and sound.
-----------------------------------------------------------------------------*/

XTimerId XWindow::AddTimer(FLOAT timeout, UBOOL bLoop, XTimerData clientData, FName function)
{
	guard(XWindow::AddTimer);

	if (timeout < 0.000001f)
		timeout = 0.000001f;

	if (bBeingDestroyed == TRUE)
		return 0;

	XTimerStruct* NewTimer = freeTimer;
	if (NewTimer != NULL)
	{
		freeTimer = NewTimer->nextTimer;
	}
	else
	{
		NewTimer = (XTimerStruct*)GMalloc->Malloc(sizeof(XTimerStruct), TEXT("New"));
	}

	if (NewTimer != NULL)
	{
		NewTimer->timeout = timeout;
		NewTimer->timeRemaining = GetTickOffset() + timeout;
		NewTimer->function = function;
		NewTimer->bLoop = bLoop;
		NewTimer->clientData = clientData;
		NewTimer->refCount = 0;
		NewTimer->bBeingDestroyed = FALSE;
		NewTimer->nextTimer = firstTimer;
		firstTimer = NewTimer;
	}

	return (XTimerId)NewTimer;
	unguard;
}

void XWindow::RemoveTimer(XTimerId timerId)
{
	guard(XWindow::RemoveTimer);

	if ((timerId != 0) && (bBeingDestroyed == FALSE))
	{
		XTimerStruct* PrevTimer = NULL;
		XTimerStruct* FoundTimer = firstTimer;

		while (FoundTimer != NULL)
		{
			if (FoundTimer == (XTimerStruct*)timerId)
				break;

			PrevTimer = FoundTimer;
			FoundTimer = FoundTimer->nextTimer;
		}

		if (FoundTimer != NULL)
		{
			FoundTimer->bBeingDestroyed = TRUE;

			if (FoundTimer->refCount <= 0)
			{
				if (PrevTimer != NULL)
					PrevTimer->nextTimer = FoundTimer->nextTimer;
				else
					firstTimer = FoundTimer->nextTimer;

				FoundTimer->nextTimer = freeTimer;
				freeTimer = FoundTimer;
			}
		}
	}

	unguard;
}

void XWindow::InvokeTimers(FLOAT deltaSeconds)
{
	guard(XWindow::InvokeTimers);

	if (bBeingDestroyed == TRUE)
		return;

	LockWindow();

	XTimerStruct* CurTimer = firstTimer;
	while (CurTimer != NULL)
	{
		if (CurTimer->bBeingDestroyed == FALSE)
		{
			CurTimer->refCount++;
			CurTimer->timeRemaining -= deltaSeconds;

			if (CurTimer->timeRemaining < 0.0f)
			{
				INT Invocations = 1;

				if (CurTimer->bLoop == TRUE)
				{
					Invocations = INT(-CurTimer->timeRemaining / CurTimer->timeout) + 1;
					CurTimer->timeRemaining += FLOAT(Invocations) * CurTimer->timeout;
				}
				else
				{
					CurTimer->bBeingDestroyed = TRUE;
				}

				if (CurTimer->function != NAME_None)
				{
					struct
					{
						XTimerId timerId;
						INT invocations;
						XTimerData clientData;
					} Params;

					Params.timerId = (XTimerId)CurTimer;
					Params.invocations = Invocations;
					Params.clientData = CurTimer->clientData;

					ProcessScript(CurTimer->function, &Params, TRUE);
				}
				else
				{
					Timer((XTimerId)CurTimer, Invocations, CurTimer->clientData);
				}
			}

			if (bBeingDestroyed == TRUE)
				break;

			XTimerStruct* NextTimer = CurTimer->nextTimer;
			CurTimer->refCount--;

			if ((CurTimer->refCount <= 0) && (CurTimer->bBeingDestroyed == TRUE))
				RemoveTimer((XTimerId)CurTimer);

			CurTimer = NextTimer;
		}
		else
		{
			CurTimer = CurTimer->nextTimer;
		}
	}

	UnlockWindow();
	unguard;
}

FLOAT XWindow::GetTickOffset(void)
{
	guard(XWindow::GetTickOffset);
	return XRootWindow::GetWindowsTickOffset(FALSE);
	unguard;
}

void XWindow::AddActorRef(AActor* actor)
{
	guard(XWindow::AddActorRef);

	APlayerPawnExt* PlayerPawn = GetPlayerPawn();
	if (PlayerPawn != NULL)
		PlayerPawn->AddActorRef(actor);

	unguard;
}

void XWindow::RemoveActorRef(AActor* actor)
{
	guard(XWindow::RemoveActorRef);

	APlayerPawnExt* PlayerPawn = GetPlayerPawn();
	if (PlayerPawn != NULL)
		PlayerPawn->RemoveActorRef(actor);

	unguard;
}

UBOOL XWindow::IsActorValid(AActor* actor)
{
	guard(XWindow::IsActorValid);

	APlayerPawnExt* PlayerPawn = GetPlayerPawn();
	if (PlayerPawn != NULL)
		return PlayerPawn->IsActorValid(actor);

	return FALSE;
	unguard;
}

void XWindow::PlaySound(USound* sound, FLOAT volume, FLOAT pitch)
{
	guard(XWindow::PlaySound);
	PlaySound(sound, volume, pitch, width * 0.5f, height * 0.5f);
	unguard;
}

void XWindow::PlaySound(USound* sound, FLOAT volume, FLOAT pitch, FLOAT posX, FLOAT posY)
{
	guard(XWindow::PlaySound);

	if (volume == -1.0f)
		volume = soundVolume;

	XRootWindow* RootWindow = GetRootWindow();
	APlayerPawnExt* PlayerPawn = GetPlayerPawn();

	if ((RootWindow != NULL) && (PlayerPawn != NULL) && (sound != NULL))
	{
		FLOAT Pan = 0.0f;

		if ((XReconIsEffectivelyVisible(this) == TRUE) && (RootWindow->bPositionalSound == TRUE))
		{
			FLOAT RootX = 0.0f;
			ConvertCoordinates(this, posX, posY, RootWindow, &RootX, NULL);

			FLOAT RootWidth = RootWindow->width;
			if (RootWidth < 1.0f)
				RootWidth = 1.0f;

			Pan = (RootX + RootX) / RootWidth - 1.0f;

			if (Pan < -1.0f)
				Pan = -1.0f;

			if (Pan > 1.0f)
				Pan = 1.0f;
		}

		FLOAT Radius = 100.0f;
		FLOAT Distance = 1.0f;
		FLOAT Gain = volume;

		if ((Radius - Distance) > 0.0f)
			Gain = (Radius / (Radius - Distance)) * volume;

		FRotator Rot = PlayerPawn->ViewRotation;
		Rot.Pitch = 0;
		Rot.Yaw += INT(Pan * 16384.0f);

		FCoords Coords = GMath.UnitCoords / Rot;
		FVector Location = PlayerPawn->Location + (Coords.XAxis * Distance);

		if ((PlayerPawn->XLevel != NULL) && (PlayerPawn->XLevel->Engine != NULL) && (PlayerPawn->XLevel->Engine->Audio != NULL))
			PlayerPawn->XLevel->Engine->Audio->PlaySound(NULL, 0, sound, Location, Gain, Radius, pitch);
	}

	unguard;
}

void XWindow::SetSoundVolume(FLOAT newVolume)
{
	soundVolume = newVolume;
}

void XWindow::SetFocusSounds(USound* focusInSound, USound* focusOutSound)
{
	focusSound = focusInSound;
	unfocusSound = focusOutSound;
}

void XWindow::SetVisibilitySounds(USound* showSound, USound* hideSound)
{
	visibleSound = showSound;
	invisibleSound = hideSound;
}

/*-----------------------------------------------------------------------------
	Clipping and drawing.
-----------------------------------------------------------------------------*/

void XWindow::ClipTree(void)
{
	guard(XWindow::ClipTree);

	if (parent != NULL)
	{
		clipRect.originX = parent->clipRect.originX + x;
		clipRect.originY = parent->clipRect.originY + y;
		clipRect.clipX = 0.0f;
		clipRect.clipY = 0.0f;
		clipRect.clipWidth = width;
		clipRect.clipHeight = height;
		clipRect.Intersect(parent->clipRect);
	}
	else
	{
		clipRect.originX = 0.0f;
		clipRect.originY = 0.0f;
		clipRect.clipX = 0.0f;
		clipRect.clipY = 0.0f;
		clipRect.clipWidth = width;
		clipRect.clipHeight = height;
	}

	if (winGC != NULL)
		winGC->SetClipRect(clipRect);

	for (XWindow* Child = firstChild; Child != NULL; Child = Child->nextSibling)
		Child->ClipTree();

	unguard;
}

void XWindow::DrawTree(UCanvas* canvas, UTexture* debugTexture, FLOAT frameTimer, INT hMult, INT vMult)
{
	guard(XWindow::DrawTree);

	LockWindow();

	if ((canvas != NULL) && (winGC != NULL) && (bIsVisible == TRUE) && (clipRect.HasArea() == TRUE))
	{
		if (canvas->Frame != NULL)
			canvas->Frame->ComputeRenderSize();

		INT SavedGC = winGC->PushGC();
		winGC->SetCanvas(canvas);
		winGC->SetMultipliers(hMult, vMult);

		if ((backgroundStyle != STY_None) && (background != NULL))
		{
			FLOAT SourceWidth = 0.0f;
			FLOAT SourceHeight = 0.0f;

			if (bStretchBackground == TRUE)
			{
				SourceWidth = FLOAT(background->USize);
				SourceHeight = FLOAT(background->VSize);
			}

			winGC->DrawIconPattern(0.0f, 0.0f, width, height, 0.0f, 0.0f, SourceWidth, SourceHeight, background);
		}

		GReconInsideDraw++;
		Draw(winGC);

		for (XWindow* Child = firstChild; Child != NULL; Child = Child->nextSibling)
			Child->DrawTree(canvas, debugTexture, frameTimer, hMult, vMult);

		PostDraw(winGC);
		GReconInsideDraw--;

		if (debugTexture != NULL)
		{
			winGC->PopGC(SavedGC);
			SavedGC = winGC->PushGC();
			winGC->SetCanvas(canvas);
			winGC->SetMultipliers(hMult, vMult);

			if (frameTimer >= 0.1f)
			{
				winGC->SetStyle(STY_Translucent);
				winGC->SetTileColor(FColor(64, 64, 64));
				winGC->DrawBox(0.0f, 0.0f, width, height, 0.0f, 0.0f, 1.0f, debugTexture);

				winGC->SetTileColor(FColor(32, 32, 32));
				winGC->DrawBox(1.0f, 1.0f, width - 2.0f, height - 2.0f, 1.0f, 1.0f, 1.0f, debugTexture);

				winGC->SetTileColor(FColor(16, 16, 16));
				winGC->DrawBox(2.0f, 2.0f, width - 4.0f, height - 4.0f, 2.0f, 2.0f, 1.0f, debugTexture);
			}
			else
			{
				winGC->SetStyle(STY_Normal);
				winGC->SetTileColor(FColor(0, 0, 0));
				winGC->DrawBox(0.0f, 0.0f, width, height, 0.0f, 0.0f, 1.0f, debugTexture);
			}
		}

		winGC->PopGC(SavedGC);
	}

	UnlockWindow();
	unguard;
}

/*-----------------------------------------------------------------------------
	Script callback wrappers reconstructed from native C++ bodies.
-----------------------------------------------------------------------------*/

void XWindow::ParentRequestedPreferredSize(UBOOL bWidthSpecified, FLOAT& preferredWidth, UBOOL bHeightSpecified, FLOAT& preferredHeight)
{
	guard(XWindow::ParentRequestedPreferredSize);

	struct
	{
		BITFIELD bWidthSpecified;
		FLOAT preferredWidth;
		BITFIELD bHeightSpecified;
		FLOAT preferredHeight;
	} Params;

	Params.bWidthSpecified = bWidthSpecified;
	Params.preferredWidth = preferredWidth;
	Params.bHeightSpecified = bHeightSpecified;
	Params.preferredHeight = preferredHeight;

	ProcessScript(EXTENSION_ParentRequestedPreferredSize, &Params);

	preferredWidth = Params.preferredWidth;
	preferredHeight = Params.preferredHeight;

	unguard;
}

void XWindow::ParentRequestedGranularity(FLOAT& hGranulatity, FLOAT& vGranularity)
{
	guard(XWindow::ParentRequestedGranularity);

	struct
	{
		FLOAT hGranulatity;
		FLOAT vGranularity;
	} Params;

	Params.hGranulatity = hGranulatity;
	Params.vGranularity = vGranularity;

	ProcessScript(EXTENSION_ParentRequestedGranularity, &Params);

	hGranulatity = Params.hGranulatity;
	vGranularity = Params.vGranularity;

	unguard;
}

void XWindow::Timer(XTimerId timerId, INT invocations, XTimerData clientData)
{
	guard(XWindow::Timer);
	unguard;
}

/*-----------------------------------------------------------------------------
	Accelerators and misc.
-----------------------------------------------------------------------------*/

void XWindow::SetAcceleratorText(const TCHAR* acceleratorText)
{
	guard(XWindow::SetAcceleratorText);

	TCHAR Key = 0;

	if (acceleratorText != NULL)
	{
		const TCHAR* Text = acceleratorText;
		while (*Text != 0)
		{
			if (*Text == TEXT('|'))
			{
				Text++;
				if (*Text == TEXT('&'))
				{
					Key = Text[1];
					break;
				}
			}

			if (*Text != 0)
				Text++;
		}
	}

	SetAcceleratorKey(Key);
	unguard;
}

void XWindow::SetAcceleratorKey(TCHAR key)
{
	guard(XWindow::SetAcceleratorKey);

	TCHAR NewKey = key;
	if (NewKey >= 0xFF)
		NewKey = 0;

	INT OldKey = acceleratorKey;
	acceleratorKey = NewKey;

	if (OldKey != acceleratorKey)
		MakeAcceleratorTableDirty();

	unguard;
}

const TCHAR* XWindow::ConvertScriptString(const TCHAR* oldStr)
{
	guard(XWindow::ConvertScriptString);

	static FString ConvertedString;
	ConvertedString.Empty();

	if (oldStr != NULL)
	{
		for (const TCHAR* Text = oldStr; *Text != 0; Text++)
		{
			// Original Extension.dll converts only the script marker |n.
			// A literal backslash-n is left untouched.
			if ((*Text == TEXT('|')) && (Text[1] == TEXT('n')))
			{
				ConvertedString += TEXT("\n");
				Text++;
			}
			else
			{
				TCHAR ConvertedTemp[2];
				ConvertedTemp[0] = *Text;
				ConvertedTemp[1] = 0;
				ConvertedString += ConvertedTemp;
			}
		}
	}

	return *ConvertedString;
	unguard;
}
