/*=============================================================================
	XModalTabRadio_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 10.

	Focus: keyboard traversal containers and mutually-exclusive toggle groups:
	XTabGroupWindow, XModalWindow, XRadioBoxWindow.

	This is reconstruction-first source. It is intentionally explicit and
	behavioural; it is not yet a final VC98 drop-in translation unit.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

struct XReconModalKeyPressedParms
{
	FString Key;
	UBOOL bResult;
};

IMPLEMENT_CLASS(XTabGroupWindow);
IMPLEMENT_CLASS(XModalWindow);
IMPLEMENT_CLASS(XRadioBoxWindow);

/*-----------------------------------------------------------------------------
	Ordering helpers.
-----------------------------------------------------------------------------*/

static INT ReconCompareWindowRowMajor(const void* LeftPtr, const void* RightPtr)
{
	XWindow* Left = *(XWindow**)LeftPtr;
	XWindow* Right = *(XWindow**)RightPtr;

	if (Left == Right)
		return 0;

	if (Left == NULL)
		return 1;

	if (Right == NULL)
		return -1;

	if (Left->y < Right->y)
		return -1;

	if (Left->y > Right->y)
		return 1;

	if (Left->x < Right->x)
		return -1;

	if (Left->x > Right->x)
		return 1;

	if ((DWORD)Left < (DWORD)Right)
		return -1;

	return 1;
}

static INT ReconCompareWindowColMajor(const void* LeftPtr, const void* RightPtr)
{
	XWindow* Left = *(XWindow**)LeftPtr;
	XWindow* Right = *(XWindow**)RightPtr;

	if (Left == Right)
		return 0;

	if (Left == NULL)
		return 1;

	if (Right == NULL)
		return -1;

	if (Left->x < Right->x)
		return -1;

	if (Left->x > Right->x)
		return 1;

	if (Left->y < Right->y)
		return -1;

	if (Left->y > Right->y)
		return 1;

	if ((DWORD)Left < (DWORD)Right)
		return -1;

	return 1;
}

static INT ReconCompareTabGroupLocation(const void* LeftPtr, const void* RightPtr)
{
	XTabGroupWindow* Left = *(XTabGroupWindow**)LeftPtr;
	XTabGroupWindow* Right = *(XTabGroupWindow**)RightPtr;

	if (Left == Right)
		return 0;

	if (Left == NULL)
		return 1;

	if (Right == NULL)
		return -1;

	if (Left->firstAbsY < Right->firstAbsY)
		return -1;

	if (Left->firstAbsY > Right->firstAbsY)
		return 1;

	if (Left->firstAbsX < Right->firstAbsX)
		return -1;

	if (Left->firstAbsX > Right->firstAbsX)
		return 1;

	if ((DWORD)Left < (DWORD)Right)
		return -1;

	return 1;
}

static UBOOL ReconIsChildOfClass(UObject* Object, UClass* Class)
{
	if (Object == NULL)
		return FALSE;

	for (UClass* TestClass = Object->GetClass(); TestClass != NULL; TestClass = TestClass->GetSuperClass())
	{
		if (TestClass == Class)
			return TRUE;
	}

	return FALSE;
}

static TCHAR ReconUpperAccelerator(TCHAR Key)
{
	if (Key >= TEXT('a') && Key <= TEXT('z'))
		return Key - TEXT('a') + TEXT('A');

	return Key;
}

static TCHAR ReconLowerAccelerator(TCHAR Key)
{
	if (Key >= TEXT('A') && Key <= TEXT('Z'))
		return Key - TEXT('A') + TEXT('a');

	return Key;
}

/*-----------------------------------------------------------------------------
	XTabGroupWindow.
-----------------------------------------------------------------------------*/

XTabGroupWindow::XTabGroupWindow(XWindow* Parent)
: XWindow(Parent)
{
	guard(XTabGroupWindow::XTabGroupWindow);
	unguard;
}

void XTabGroupWindow::Init(XWindow* Parent)
{
	guard(XTabGroupWindow::Init);

	XWindow::Init(Parent);

	windowType = WIN_TabGroup;
	rowMajorWindowList.Empty();
	colMajorWindowList.Empty();
	tabGroupIndex = INDEX_NONE;
	firstAbsX = 0.0f;
	firstAbsY = 0.0f;

	bSizeParentToChildren = TRUE;
	bSizeChildrenToParent = FALSE;
	bWrapFocus = TRUE;

	/*
		A modal is itself a tab group, but it must not be inserted into an outer
		modal's list here. XModalWindow::Init() performs its own AddTabGroupToTable().
	*/
	if (ReconIsChildOfClass(this, XModalWindow::StaticClass()) == FALSE)
	{
		XModalWindow* ModalWindow = GetModalWindow();
		if (ModalWindow != NULL)
			ModalWindow->AddTabGroupToTable(this);
	}

	unguard;
}

void XTabGroupWindow::CleanUp(void)
{
	guard(XTabGroupWindow::CleanUp);

	XModalWindow* ModalWindow = GetModalWindow();
	if (ModalWindow != NULL && ModalWindow != this)
		ModalWindow->RemoveTabGroupFromTable(this);

	rowMajorWindowList.Empty();
	colMajorWindowList.Empty();

	XWindow::CleanUp();

	unguard;
}

void XTabGroupWindow::Serialize(FArchive& Ar)
{
	guard(XTabGroupWindow::Serialize);

	XWindow::Serialize(Ar);
	Ar << rowMajorWindowList;
	Ar << colMajorWindowList;

	unguardobj;
}

UBOOL XTabGroupWindow::ComputeTabGroupLocation(void)
{
	guard(XTabGroupWindow::ComputeTabGroupLocation);

	FLOAT NewFirstAbsX = 0.0f;
	FLOAT NewFirstAbsY = 0.0f;

	for (INT i = 0; i < rowMajorWindowList.Num(); i++)
	{
		XWindow* Window = rowMajorWindowList(i);
		if (Window == NULL)
			continue;

		if (Window->clipRect.clipWidth <= 0.0f)
			continue;

		if (Window->clipRect.clipHeight <= 0.0f)
			continue;

		if (Window->IsTraversable(FALSE) == FALSE)
			continue;

		NewFirstAbsX = Window->clipRect.originX + Window->clipRect.clipX;
		NewFirstAbsY = Window->clipRect.originY + Window->clipRect.clipY;
		break;
	}

	if (firstAbsX == NewFirstAbsX && firstAbsY == NewFirstAbsY)
		return FALSE;

	firstAbsX = NewFirstAbsX;
	firstAbsY = NewFirstAbsY;
	return TRUE;

	unguard;
}

void XTabGroupWindow::ResortWindowTables(void)
{
	guard(XTabGroupWindow::ResortWindowTables);

	if (rowMajorWindowList.Num() > 1)
		appQsort(&rowMajorWindowList(0), rowMajorWindowList.Num(), sizeof(XWindow*), ReconCompareWindowRowMajor);

	for (INT i = 0; i < rowMajorWindowList.Num(); i++)
	{
		if (rowMajorWindowList(i) != NULL)
			rowMajorWindowList(i)->rowMajorIndex = i;
	}

	if (colMajorWindowList.Num() > 1)
		appQsort(&colMajorWindowList(0), colMajorWindowList.Num(), sizeof(XWindow*), ReconCompareWindowColMajor);

	for (INT j = 0; j < colMajorWindowList.Num(); j++)
	{
		if (colMajorWindowList(j) != NULL)
			colMajorWindowList(j)->colMajorIndex = j;
	}

	if (ComputeTabGroupLocation() == TRUE)
	{
		XModalWindow* ModalWindow = GetModalWindow();
		if (ModalWindow != NULL)
			ModalWindow->ResortTabGroupTable();
	}

	unguard;
}

void XTabGroupWindow::AddWindowToTables(XWindow* Window)
{
	guard(XTabGroupWindow::AddWindowToTables);

	if (Window != NULL)
	{
		rowMajorWindowList.AddItem(Window);
		colMajorWindowList.AddItem(Window);
		ResortWindowTables();
	}

	unguard;
}

void XTabGroupWindow::RemoveWindowFromTables(XWindow* Window)
{
	guard(XTabGroupWindow::RemoveWindowFromTables);

	if (Window != NULL)
	{
		INT RowIndex = Window->rowMajorIndex;
		if (RowIndex >= 0 && RowIndex < rowMajorWindowList.Num())
			rowMajorWindowList.Remove(RowIndex, 1);

		INT ColIndex = Window->colMajorIndex;
		if (ColIndex >= 0 && ColIndex < colMajorWindowList.Num())
			colMajorWindowList.Remove(ColIndex, 1);

		for (INT i = RowIndex; i >= 0 && i < rowMajorWindowList.Num(); i++)
		{
			if (rowMajorWindowList(i) != NULL)
				rowMajorWindowList(i)->rowMajorIndex = i;
		}

		for (INT j = ColIndex; j >= 0 && j < colMajorWindowList.Num(); j++)
		{
			if (colMajorWindowList(j) != NULL)
				colMajorWindowList(j)->colMajorIndex = j;
		}

		Window->rowMajorIndex = INDEX_NONE;
		Window->colMajorIndex = INDEX_NONE;
	}

	unguard;
}

void XTabGroupWindow::ConfigurationChanged(void)
{
	guard(XTabGroupWindow::ConfigurationChanged);

	if (bSizeChildrenToParent == TRUE)
	{
		for (XWindow* Child = GetTopChild(TRUE); Child != NULL; Child = Child->GetLowerSibling(TRUE))
			Child->ConfigureChild(0.0f, 0.0f, width, height);
	}
	else
	{
		ProcessScript(EXTENSION_ConfigurationChanged, NULL, FALSE);
	}

	unguard;
}

void XTabGroupWindow::ParentRequestedPreferredSize(UBOOL bWidthSpecified, FLOAT& PreferredWidth, UBOOL bHeightSpecified, FLOAT& PreferredHeight)
{
	guard(XTabGroupWindow::ParentRequestedPreferredSize);

	if (bSizeParentToChildren == TRUE || bSizeChildrenToParent == TRUE)
	{
		FLOAT MaxWidth = 0.0f;
		FLOAT MaxHeight = 0.0f;

		for (XWindow* Child = GetTopChild(TRUE); Child != NULL; Child = Child->GetLowerSibling(TRUE))
		{
			FLOAT ChildWidth = 0.0f;
			FLOAT ChildHeight = 0.0f;

			Child->QueryPreferredSize(bWidthSpecified, PreferredWidth, &ChildWidth, bHeightSpecified, PreferredHeight, &ChildHeight);

			if (bSizeChildrenToParent == FALSE)
			{
				ChildWidth += Child->hMargin0;
				ChildHeight += Child->vMargin0;

				if (Child->winHAlign == HALIGN_Full)
					ChildWidth += Child->hMargin1;

				if (Child->winVAlign == VALIGN_Full)
					ChildHeight += Child->vMargin1;
			}

			if (MaxWidth < ChildWidth)
				MaxWidth = ChildWidth;

			if (MaxHeight < ChildHeight)
				MaxHeight = ChildHeight;
		}

		PreferredWidth = MaxWidth;
		PreferredHeight = MaxHeight;
	}
	else
	{
		XWindow::ParentRequestedPreferredSize(bWidthSpecified, PreferredWidth, bHeightSpecified, PreferredHeight);
	}

	unguard;
}


UBOOL XTabGroupWindow::ChildRequestedReconfiguration(XWindow* Child)
{
	guard(XTabGroupWindow::ChildRequestedReconfiguration);
	return FALSE;
	unguard;
}

void XTabGroupWindow::ChildRequestedVisibilityChange(XWindow* Child, UBOOL bNewVisibility)
{
	guard(XTabGroupWindow::ChildRequestedVisibilityChange);

	Child->SetChildVisibility(bNewVisibility);

	if (bNewVisibility == FALSE)
		AskParentForReconfigure();

	unguard;
}

/*-----------------------------------------------------------------------------
	XModalWindow.
-----------------------------------------------------------------------------*/

XModalWindow::XModalWindow(XRootWindow* Parent)
: XTabGroupWindow((XWindow*)Parent)
{
	guard(XModalWindow::XModalWindow);
	unguard;
}

void XModalWindow::Init(XWindow* Parent)
{
	guard(XModalWindow::Init);

	XTabGroupWindow::Init(Parent);

	if (Parent != NULL)
	{
		if (Parent->windowType == WIN_Root)
			windowType = WIN_Modal;
		else if (XReconShouldLogDiagnostics() == TRUE)
			GLog->Logf(TEXT("Modal window must be a child of Root!"));
	}
	else
	{
		windowType = WIN_Modal;
	}

	focusMode = MFOCUS_Enter;
	preferredFocus = NULL;
	tabGroupWindowList.Empty();
	ClearAcceleratorTable();
	bDirtyAccelerators = TRUE;

	AddTabGroupToTable(this);

	unguard;
}

void XModalWindow::CleanUp(void)
{
	guard(XModalWindow::CleanUp);

	RemoveTabGroupFromTable(this);
	tabGroupWindowList.Empty();
	ClearAcceleratorTable();

	XTabGroupWindow::CleanUp();

	unguard;
}

void XModalWindow::Serialize(FArchive& Ar)
{
	guard(XModalWindow::Serialize);

	XTabGroupWindow::Serialize(Ar);
	Ar << tabGroupWindowList;

	unguardobj;
}

UBOOL XModalWindow::IsCurrentModal(void)
{
	guard(XModalWindow::IsCurrentModal);

	XRootWindow* RootWindow = GetRootWindow();
	if (RootWindow == NULL)
		return FALSE;

	XWindow* CurrentModal = NULL;
	for (XWindow* Child = RootWindow->GetTopChild(TRUE); Child != NULL; Child = Child->GetLowerSibling(TRUE))
	{
		if (Child->windowType >= WIN_Modal)
		{
			CurrentModal = Child;
			break;
		}
	}

	if (CurrentModal == NULL)
		CurrentModal = RootWindow;

	if (CurrentModal == this)
		return TRUE;

	return FALSE;

	unguard;
}

void XModalWindow::ResortTabGroupTable(void)
{
	guard(XModalWindow::ResortTabGroupTable);

	if (tabGroupWindowList.Num() > 1)
		appQsort(&tabGroupWindowList(0), tabGroupWindowList.Num(), sizeof(XTabGroupWindow*), ReconCompareTabGroupLocation);

	for (INT i = 0; i < tabGroupWindowList.Num(); i++)
	{
		if (tabGroupWindowList(i) != NULL)
			tabGroupWindowList(i)->tabGroupIndex = i;
	}

	unguard;
}

void XModalWindow::AddTabGroupToTable(XTabGroupWindow* TabGroup)
{
	guard(XModalWindow::AddTabGroupToTable);

	if (TabGroup != NULL)
	{
		tabGroupWindowList.AddItem(TabGroup);
		ResortTabGroupTable();
	}

	unguard;
}

void XModalWindow::RemoveTabGroupFromTable(XTabGroupWindow* TabGroup)
{
	guard(XModalWindow::RemoveTabGroupFromTable);

	if (TabGroup != NULL)
	{
		INT Index = TabGroup->tabGroupIndex;
		if (Index >= 0 && Index < tabGroupWindowList.Num())
			tabGroupWindowList.Remove(Index, 1);

		for (INT i = Index; i >= 0 && i < tabGroupWindowList.Num(); i++)
		{
			if (tabGroupWindowList(i) != NULL)
				tabGroupWindowList(i)->tabGroupIndex = i;
		}

		TabGroup->tabGroupIndex = INDEX_NONE;
	}

	unguard;
}

void XModalWindow::ClearAcceleratorTable(void)
{
	guard(XModalWindow::ClearAcceleratorTable);

	for (INT i = 0; i < IK_MAX; i++)
		acceleratorTable[i] = NULL;

	unguard;
}

void XModalWindow::SetAcceleratorWindows(XWindow* Window)
{
	guard(XModalWindow::SetAcceleratorWindows);

	if (Window == NULL)
		return;

	TCHAR AcceleratorKey = (TCHAR)Window->acceleratorKey;
	if (AcceleratorKey != 0 && AcceleratorKey < IK_MAX)
	{
		UBOOL bVisibleChain = TRUE;
		for (XWindow* TestWindow = Window; TestWindow != NULL; TestWindow = TestWindow->parent)
		{
			if (TestWindow->bIsVisible == FALSE)
			{
				bVisibleChain = FALSE;
				break;
			}
		}

		UBOOL bSensitiveChain = TRUE;
		if (bVisibleChain == TRUE)
		{
			for (XWindow* TestWindow = Window; TestWindow != NULL; TestWindow = TestWindow->parent)
			{
				if (TestWindow->bIsSensitive == FALSE)
				{
					bSensitiveChain = FALSE;
					break;
				}
			}
		}

		if (bVisibleChain == TRUE && bSensitiveChain == TRUE && Window->bIsSelectable == TRUE)
		{
			TCHAR UpperKey = ReconUpperAccelerator(AcceleratorKey);
			if (acceleratorTable[UpperKey] == NULL)
				acceleratorTable[UpperKey] = Window;

			TCHAR LowerKey = ReconLowerAccelerator(UpperKey);
			if (acceleratorTable[LowerKey] == NULL)
				acceleratorTable[LowerKey] = Window;
		}
	}

	for (XWindow* Child = Window->GetBottomChild(TRUE); Child != NULL; Child = Child->GetHigherSibling(TRUE))
	{
		if (Child->windowType >= WIN_Modal)
			continue;

		UBOOL bSensitiveChain = TRUE;
		for (XWindow* TestWindow = Child; TestWindow != NULL; TestWindow = TestWindow->parent)
		{
			if (TestWindow->bIsSensitive == FALSE)
			{
				bSensitiveChain = FALSE;
				break;
			}
		}

		if (bSensitiveChain == TRUE)
			SetAcceleratorWindows(Child);
	}

	unguard;
}

void XModalWindow::BuildAcceleratorTable(void)
{
	guard(XModalWindow::BuildAcceleratorTable);

	if (bDirtyAccelerators == TRUE)
	{
		ClearAcceleratorTable();
		SetAcceleratorWindows(this);
		bDirtyAccelerators = FALSE;
	}

	unguard;
}

XWindow* XModalWindow::GetAcceleratorWindow(TCHAR Key)
{
	guard(XModalWindow::GetAcceleratorWindow);

	if (Key == 0)
		return NULL;

	if (Key >= IK_MAX)
		return NULL;

	BuildAcceleratorTable();
	return acceleratorTable[Key];

	unguard;
}

void XModalWindow::VisibilityChanged(UBOOL bNewVisibility)
{
	guard(XModalWindow::VisibilityChanged);

	struct
	{
		UBOOL bNewVisibility;
	} Parms;

	Parms.bNewVisibility = bNewVisibility;
	ProcessScript(EXTENSION_VisibilityChanged, &Parms, FALSE);

	if (parent != NULL)
	{
		XRootWindow* RootWindow = GetRootWindow();
		if (RootWindow != NULL)
		{
			if (bNewVisibility == TRUE)
			{
				RootWindow->GrabKeyboardEvents();
				RootWindow->GrabMouseEvents();
			}
			else
			{
				RootWindow->UngrabKeyboardEvents();
				RootWindow->UngrabMouseEvents();
			}
		}
	}

	unguard;
}

void XModalWindow::DescendantRemoved(XWindow* Descendant)
{
	guard(XModalWindow::DescendantRemoved);

	struct
	{
		XWindow* Descendant;
	} Parms;

	Parms.Descendant = Descendant;
	ProcessScript(EXTENSION_DescendantRemoved, &Parms, FALSE);

	if (Descendant == preferredFocus)
		preferredFocus = NULL;

	unguard;
}

void XModalWindow::Tick(FLOAT DeltaSeconds)
{
	guard(XModalWindow::Tick);

	BuildAcceleratorTable();

	unguard;
}

UBOOL XModalWindow::KeyPressed(TCHAR Key)
{
	guard(XModalWindow::KeyPressed);

	TCHAR KeyText[2];
	KeyText[0] = Key;
	KeyText[1] = 0;

	XReconModalKeyPressedParms Parms;

	Parms.Key = KeyText;
	Parms.bResult = FALSE;

	ProcessScript(EXTENSION_KeyPressed, &Parms, FALSE);

	if (Parms.bResult == FALSE)
	{
		if (IsCurrentModal() == TRUE)
		{
			if (IsKeyDown(IK_Alt) == TRUE)
			{
				XWindow* AcceleratorWindow = GetAcceleratorWindow(Key);
				if (AcceleratorWindow != NULL)
					Parms.bResult = AcceleratorWindow->AcceleratorKeyPressed(Key);
			}
		}
	}

	return Parms.bResult;

	unguard;
}

void XModalWindow::execSetMouseFocusMode(FFrame& Stack, RESULT_DECL)
{
	guard(XModalWindow::execSetMouseFocusMode);

	P_GET_BYTE(NewFocusMode);
	P_FINISH;

	SetMouseFocusMode((EMouseFocusMode)NewFocusMode);

	unguardexec;
}

void XModalWindow::execIsCurrentModal(FFrame& Stack, RESULT_DECL)
{
	guard(XModalWindow::execIsCurrentModal);

	P_FINISH;
	*(UBOOL*)Result = IsCurrentModal();

	unguardexec;
}

/*-----------------------------------------------------------------------------
	XRadioBoxWindow.
-----------------------------------------------------------------------------*/

XRadioBoxWindow::XRadioBoxWindow(XWindow* Parent)
: XTabGroupWindow(Parent)
{
	guard(XRadioBoxWindow::XRadioBoxWindow);
	unguard;
}

void XRadioBoxWindow::Init(XWindow* Parent)
{
	guard(XRadioBoxWindow::Init);

	XTabGroupWindow::Init(Parent);
	toggleButtons.Empty();
	bOneChecked = TRUE;
	currentSelection = NULL;

	unguard;
}

void XRadioBoxWindow::CleanUp(void)
{
	guard(XRadioBoxWindow::CleanUp);

	toggleButtons.Empty();
	currentSelection = NULL;
	XTabGroupWindow::CleanUp();

	unguard;
}

void XRadioBoxWindow::Serialize(FArchive& Ar)
{
	guard(XRadioBoxWindow::Serialize);

	XTabGroupWindow::Serialize(Ar);
	Ar << toggleButtons;

	unguardobj;
}

void XRadioBoxWindow::ConfigurationChanged(void)
{
	guard(XRadioBoxWindow::ConfigurationChanged);

	for (XWindow* Child = GetTopChild(TRUE); Child != NULL; Child = Child->GetLowerSibling(TRUE))
		Child->ConfigureChild(0.0f, 0.0f, width, height);

	unguard;
}

void XRadioBoxWindow::ParentRequestedPreferredSize(UBOOL bWidthSpecified, FLOAT& PreferredWidth, UBOOL bHeightSpecified, FLOAT& PreferredHeight)
{
	guard(XRadioBoxWindow::ParentRequestedPreferredSize);

	FLOAT MaxWidth = 0.0f;
	FLOAT MaxHeight = 0.0f;

	for (XWindow* Child = GetTopChild(TRUE); Child != NULL; Child = Child->GetLowerSibling(TRUE))
	{
		FLOAT ChildWidth = 0.0f;
		FLOAT ChildHeight = 0.0f;

		Child->QueryPreferredSize(bWidthSpecified, PreferredWidth, &ChildWidth, bHeightSpecified, PreferredHeight, &ChildHeight);

		if (MaxWidth < ChildWidth)
			MaxWidth = ChildWidth;

		if (MaxHeight < ChildHeight)
			MaxHeight = ChildHeight;
	}

	PreferredWidth = MaxWidth;
	PreferredHeight = MaxHeight;

	unguard;
}


UBOOL XRadioBoxWindow::ChildRequestedReconfiguration(XWindow* Child)
{
	guard(XRadioBoxWindow::ChildRequestedReconfiguration);
	return FALSE;
	unguard;
}

void XRadioBoxWindow::ChildRequestedVisibilityChange(XWindow* Child, UBOOL bNewVisibility)
{
	guard(XRadioBoxWindow::ChildRequestedVisibilityChange);

	Child->SetChildVisibility(bNewVisibility);

	if (bNewVisibility == FALSE)
		AskParentForReconfigure();

	unguard;
}

UBOOL XRadioBoxWindow::ToggleChanged(XWindow* Child, UBOOL bNewState)
{
	guard(XRadioBoxWindow::ToggleChanged);

	if (ReconIsChildOfClass(Child, XToggleWindow::StaticClass()) == TRUE)
	{
		XToggleWindow* Toggle = (XToggleWindow*)Child;

		if (bNewState == TRUE)
		{
			for (INT i = 0; i < toggleButtons.Num(); i++)
			{
				if (toggleButtons(i) == Toggle)
				{
					if (currentSelection != Toggle)
					{
						XToggleWindow* OldSelection = currentSelection;
						currentSelection = Toggle;

						if (OldSelection != NULL)
							OldSelection->SetToggle(FALSE);
					}
					else
					{
						return TRUE;
					}

					break;
				}
			}
		}
		else
		{
			if (bOneChecked == TRUE)
			{
				if (currentSelection == Toggle)
					Toggle->SetToggle(TRUE);

				return TRUE;
			}

			if (currentSelection == Toggle)
				currentSelection = NULL;
		}
	}

	struct
	{
		XWindow* Child;
		UBOOL bNewState;
		UBOOL bResult;
	} Parms;

	Parms.Child = Child;
	Parms.bNewState = bNewState;
	Parms.bResult = FALSE;

	ProcessScript(EXTENSION_ToggleChanged, &Parms, FALSE);
	return Parms.bResult;

	unguard;
}

void XRadioBoxWindow::DescendantAdded(XWindow* Child)
{
	guard(XRadioBoxWindow::DescendantAdded);

	struct
	{
		XWindow* Child;
	} Parms;

	Parms.Child = Child;
	ProcessScript(EXTENSION_DescendantAdded, &Parms, FALSE);

	if (ReconIsChildOfClass(Child, XToggleWindow::StaticClass()) == FALSE)
		return;

	XWindow* Ancestor = Child;
	while (Ancestor != NULL)
	{
		if (ReconIsChildOfClass(Ancestor, XRadioBoxWindow::StaticClass()) == TRUE)
			break;

		Ancestor = Ancestor->parent;
	}

	if (Ancestor == this)
		toggleButtons.AddItem((XToggleWindow*)Child);

	unguard;
}

void XRadioBoxWindow::DescendantRemoved(XWindow* Child)
{
	guard(XRadioBoxWindow::DescendantRemoved);

	for (INT i = 0; i < toggleButtons.Num(); i++)
	{
		if (toggleButtons(i) == Child)
		{
			if (currentSelection == toggleButtons(i))
				currentSelection = NULL;

			toggleButtons.Remove(i, 1);
			break;
		}
	}

	struct
	{
		XWindow* Child;
	} Parms;

	Parms.Child = Child;
	ProcessScript(EXTENSION_DescendantRemoved, &Parms, FALSE);

	unguard;
}

void XRadioBoxWindow::execGetEnabledToggle(FFrame& Stack, RESULT_DECL)
{
	guard(XRadioBoxWindow::execGetEnabledToggle);

	P_FINISH;
	*(XToggleWindow**)Result = currentSelection;

	unguardexec;
}
