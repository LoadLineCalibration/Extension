/*=============================================================================
	XScrollAreaWindow_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 05.

	Focus: compound scroll area: embedded horizontal/vertical ScaleManagerWindow,
	ClipWindow coupling, auto-hide negotiation and mouse wheel routing.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XScrollAreaWindow);

XScrollAreaWindow::XScrollAreaWindow(XWindow* Parent)
	: XWindow(Parent)
{
}

void XScrollAreaWindow::Init(XWindow* Parent)
{
	guard(XScrollAreaWindow::Init);

	XWindow::Init(Parent);

	marginWidth = 3.0f;
	marginHeight = 3.0f;
	scrollbarDistance = 3.0f;
	bHideScrollbars = TRUE;
	bHLastShow = FALSE;
	bVLastShow = FALSE;

	hScaleMgr = Cast<XScaleManagerWindow>(CreateNewWindow(XScaleManagerWindow::StaticClass(), this, FALSE));
	vScaleMgr = Cast<XScaleManagerWindow>(CreateNewWindow(XScaleManagerWindow::StaticClass(), this, FALSE));
	clipWindow = Cast<XClipWindow>(CreateNewWindow(XClipWindow::StaticClass(), this, TRUE));

	leftButton = Cast<XButtonWindow>(CreateNewWindow(XButtonWindow::StaticClass(), hScaleMgr, TRUE));
	hScale = Cast<XScaleWindow>(CreateNewWindow(XScaleWindow::StaticClass(), hScaleMgr, TRUE));
	rightButton = Cast<XButtonWindow>(CreateNewWindow(XButtonWindow::StaticClass(), hScaleMgr, TRUE));

	upButton = Cast<XButtonWindow>(CreateNewWindow(XButtonWindow::StaticClass(), vScaleMgr, TRUE));
	vScale = Cast<XScaleWindow>(CreateNewWindow(XScaleWindow::StaticClass(), vScaleMgr, TRUE));
	downButton = Cast<XButtonWindow>(CreateNewWindow(XButtonWindow::StaticClass(), vScaleMgr, TRUE));

	if (hScaleMgr != NULL)
	{
		hScaleMgr->SetOrientation(ORIENT_Horizontal);
		hScaleMgr->SetScaleButtons(leftButton, rightButton);
		hScaleMgr->SetScale(hScale);
		hScaleMgr->StretchScaleField(TRUE);
	}

	if (vScaleMgr != NULL)
	{
		vScaleMgr->SetOrientation(ORIENT_Vertical);
		vScaleMgr->SetScaleButtons(upButton, downButton);
		vScaleMgr->SetScale(vScale);
		vScaleMgr->StretchScaleField(TRUE);
	}

	if (hScale != NULL)
	{
		hScale->SetOrientation(ORIENT_Horizontal);
		hScale->SetSelectability(FALSE);
		hScale->SetNumTicks(1);
		hScale->SetThumbSpan(1);
		hScale->EnableStretchedScale(TRUE);
	}

	if (vScale != NULL)
	{
		vScale->SetOrientation(ORIENT_Vertical);
		vScale->SetSelectability(FALSE);
		vScale->SetNumTicks(1);
		vScale->SetThumbSpan(1);
		vScale->EnableStretchedScale(TRUE);
	}

	if (leftButton != NULL)
	{
		leftButton->SetSelectability(FALSE);
		leftButton->EnableAutoRepeat(TRUE, 0.5f, 0.1f);
	}
	if (rightButton != NULL)
	{
		rightButton->SetSelectability(FALSE);
		rightButton->EnableAutoRepeat(TRUE, 0.5f, 0.1f);
	}
	if (upButton != NULL)
	{
		upButton->SetSelectability(FALSE);
		upButton->EnableAutoRepeat(TRUE, 0.5f, 0.1f);
	}
	if (downButton != NULL)
	{
		downButton->SetSelectability(FALSE);
		downButton->EnableAutoRepeat(TRUE, 0.5f, 0.1f);
	}

	EnableScrolling(FALSE, TRUE);
	SetSelectability(FALSE);

	unguard;
}

void XScrollAreaWindow::CleanUp(void)
{
	guard(XScrollAreaWindow::CleanUp);
	XWindow::CleanUp();
	unguard;
}

void XScrollAreaWindow::EnableScrolling(UBOOL bHScrolling, UBOOL bVScrolling)
{
	guard(XScrollAreaWindow::EnableScrolling);

	if (hScaleMgr != NULL)
		hScaleMgr->SetVisibility(bHScrolling);
	if (vScaleMgr != NULL)
		vScaleMgr->SetVisibility(bVScrolling);
	if (clipWindow != NULL)
		clipWindow->ForceChildSize(bHScrolling == FALSE, bVScrolling == FALSE);

	unguard;
}

void XScrollAreaWindow::GetButtons(XButtonWindow** pUpButton, XButtonWindow** pDownButton, XButtonWindow** pLeftButton, XButtonWindow** pRightButton)
{
	if (pUpButton != NULL)
		*pUpButton = upButton;
	if (pDownButton != NULL)
		*pDownButton = downButton;
	if (pLeftButton != NULL)
		*pLeftButton = leftButton;
	if (pRightButton != NULL)
		*pRightButton = rightButton;
}

void XScrollAreaWindow::GetScrollbars(XScaleWindow** pHScale, XScaleWindow** pVScale)
{
	if (pHScale != NULL)
		*pHScale = hScale;
	if (pVScale != NULL)
		*pVScale = vScale;
}

void XScrollAreaWindow::GetScrollManagers(XScaleManagerWindow** pHMgr, XScaleManagerWindow** pVMgr)
{
	if (pHMgr != NULL)
		*pHMgr = hScaleMgr;
	if (pVMgr != NULL)
		*pVMgr = vScaleMgr;
}

void XScrollAreaWindow::ComputeChildSizes(UBOOL bWidthSpecified, FLOAT QueryWidth, FLOAT* pTotalWidth, UBOOL bHeightSpecified, FLOAT QueryHeight, FLOAT* pTotalHeight)
{
	guard(XScrollAreaWindow::ComputeChildSizes);

	UBOOL bHMgrVisible = FALSE;
	UBOOL bVMgrVisible = FALSE;
	if (hScaleMgr != NULL && hScaleMgr->IsVisible() == TRUE)
		bHMgrVisible = TRUE;
	if (vScaleMgr != NULL && vScaleMgr->IsVisible() == TRUE)
		bVMgrVisible = TRUE;

	if (clipWindow != NULL && clipWindow->IsVisible() == TRUE)
	{
		FLOAT HMgrHeight = 0.0f;
		FLOAT VMgrWidth = 0.0f;
		FLOAT HBarTotalHeight = 0.0f;
		FLOAT VBarTotalWidth = 0.0f;

		UBOOL bCanShowH = FALSE;
		UBOOL bCanShowV = FALSE;
		UBOOL bShowH = FALSE;
		UBOOL bShowV = FALSE;

		if (hScaleMgr != NULL && hScaleMgr->IsVisible() == TRUE)
		{
			hScaleMgr->QueryPreferredSize(NULL, &HMgrHeight);
			HBarTotalHeight = HMgrHeight + scrollbarDistance;
			bCanShowH = TRUE;
		}

		if (vScaleMgr != NULL && vScaleMgr->IsVisible() == TRUE)
		{
			vScaleMgr->QueryPreferredSize(&VMgrWidth, NULL);
			VBarTotalWidth = VMgrWidth + scrollbarDistance;
			bCanShowV = TRUE;
		}

		if (bHideScrollbars == TRUE)
		{
			if (bCanShowH == TRUE)
				bShowH = bHLastShow;
			if (bCanShowV == TRUE)
				bShowV = bVLastShow;
		}
		else
		{
			bShowH = bCanShowH;
			bShowV = bCanShowV;
		}

		FLOAT AvailableW = 0.0f;
		FLOAT AvailableH = 0.0f;

		if (bWidthSpecified == TRUE)
			AvailableW = QueryWidth - (marginWidth + marginWidth);
		if (bHeightSpecified == TRUE)
			AvailableH = QueryHeight - (marginHeight + marginHeight);

		FLOAT ClipPreferredW = 0.0f;
		FLOAT ClipPreferredH = 0.0f;
		INT bNeedH = 0;
		INT bNeedV = 0;

		INT MaxPasses = 1;
		if (bCanShowH == TRUE)
			MaxPasses *= 2;
		if (bCanShowV == TRUE)
			MaxPasses *= 2;

		INT Pass = 0;
		while (TRUE)
		{
			Pass++;

			if (Pass > MaxPasses)
			{
				if (bCanShowH == TRUE)
					bShowH = TRUE;
				if (bCanShowV == TRUE)
					bShowV = TRUE;
			}

			FLOAT UsedH = 0.0f;
			FLOAT UsedV = 0.0f;
			if (bShowH == TRUE && bHeightSpecified == TRUE)
				UsedH = HBarTotalHeight;
			if (bShowV == TRUE && bWidthSpecified == TRUE)
				UsedV = VBarTotalWidth;

			FLOAT ClipQueryW = AvailableW - UsedV;
			FLOAT ClipQueryH = AvailableH - UsedH;

			clipWindow->QueryClipPreferredSize(bWidthSpecified, ClipQueryW, &ClipPreferredW, &bNeedH, bHeightSpecified, ClipQueryH, &ClipPreferredH, &bNeedV);

			if (Pass > MaxPasses)
				break;

			if (bCanShowH == TRUE && bNeedH != 0 && bShowH == FALSE)
			{
				bShowH = TRUE;
			}
			else if (bCanShowV == TRUE && bNeedV != 0 && bShowV == FALSE)
			{
				bShowV = TRUE;
			}
			else if (bCanShowH == TRUE && bNeedH == 0 && bShowH == TRUE)
			{
				bShowH = FALSE;
			}
			else
			{
				if (bCanShowV == TRUE && bNeedV == 0 && bShowV == TRUE)
					bShowV = FALSE;
				else
					break;
			}
		}

		bHLastShow = bShowH;
		bVLastShow = bShowV;

		clipWindow->holdX = marginWidth;
		clipWindow->holdY = marginHeight;
		clipWindow->holdWidth = ClipPreferredW;
		clipWindow->holdHeight = ClipPreferredH;

		if (hScaleMgr != NULL)
		{
			hScaleMgr->holdX = clipWindow->holdX;
			hScaleMgr->holdY = clipWindow->holdY + clipWindow->holdHeight + scrollbarDistance;
			hScaleMgr->holdWidth = clipWindow->holdWidth;
			if (bShowH == TRUE)
				hScaleMgr->holdHeight = HMgrHeight;
			else
				hScaleMgr->holdHeight = 0.0f;
		}

		if (vScaleMgr != NULL)
		{
			vScaleMgr->holdX = clipWindow->holdX + clipWindow->holdWidth + scrollbarDistance;
			vScaleMgr->holdY = clipWindow->holdY;
			if (bShowV == TRUE)
				vScaleMgr->holdWidth = VMgrWidth;
			else
				vScaleMgr->holdWidth = 0.0f;
			vScaleMgr->holdHeight = clipWindow->holdHeight;
		}

		FLOAT TotalW = marginWidth + marginWidth + clipWindow->holdWidth;
		if (bShowV == TRUE)
			TotalW += VBarTotalWidth;

		FLOAT TotalH = marginHeight + marginHeight + clipWindow->holdHeight;
		if (bShowH == TRUE)
			TotalH += HBarTotalHeight;

		if (pTotalWidth != NULL)
			*pTotalWidth = TotalW;
		if (pTotalHeight != NULL)
			*pTotalHeight = TotalH;
	}
	else
	{
		if (hScaleMgr != NULL)
		{
			hScaleMgr->holdX = 0.0f;
			hScaleMgr->holdY = 0.0f;
			hScaleMgr->holdWidth = 0.0f;
			hScaleMgr->holdHeight = 0.0f;
		}
		if (vScaleMgr != NULL)
		{
			vScaleMgr->holdX = 0.0f;
			vScaleMgr->holdY = 0.0f;
			vScaleMgr->holdWidth = 0.0f;
			vScaleMgr->holdHeight = 0.0f;
		}

		if (pTotalWidth != NULL)
			*pTotalWidth = 10.0f;
		if (pTotalHeight != NULL)
			*pTotalHeight = 10.0f;
	}

	unguard;
}


UBOOL XScrollAreaWindow::ChildRequestedReconfiguration(XWindow* Child)
{
	guard(XScrollAreaWindow::ChildRequestedReconfiguration);
	return FALSE;
	unguard;
}

void XScrollAreaWindow::ChildRequestedVisibilityChange(XWindow* Child, UBOOL bNewVisibility)
{
	guard(XScrollAreaWindow::ChildRequestedVisibilityChange);
	Child->SetChildVisibility(bNewVisibility);
	AskParentForReconfigure();
	unguard;
}

void XScrollAreaWindow::ConfigurationChanged(void)
{
	guard(XScrollAreaWindow::ConfigurationChanged);

	ComputeChildSizes(TRUE, width, NULL, TRUE, height, NULL);

	if (hScaleMgr != NULL && hScaleMgr->IsVisible() == TRUE)
		hScaleMgr->ConfigureChild(hScaleMgr->holdX, hScaleMgr->holdY, hScaleMgr->holdWidth, hScaleMgr->holdHeight);
	if (vScaleMgr != NULL && vScaleMgr->IsVisible() == TRUE)
		vScaleMgr->ConfigureChild(vScaleMgr->holdX, vScaleMgr->holdY, vScaleMgr->holdWidth, vScaleMgr->holdHeight);
	if (clipWindow != NULL && clipWindow->IsVisible() == TRUE)
		clipWindow->ConfigureChild(clipWindow->holdX, clipWindow->holdY, clipWindow->holdWidth, clipWindow->holdHeight);

	unguard;
}

void XScrollAreaWindow::ParentRequestedPreferredSize(UBOOL bWidthSpecified, FLOAT& PreferredWidth, UBOOL bHeightSpecified, FLOAT& PreferredHeight)
{
	guard(XScrollAreaWindow::ParentRequestedPreferredSize);
	ComputeChildSizes(bWidthSpecified, PreferredWidth, &PreferredWidth, bHeightSpecified, PreferredHeight, &PreferredHeight);
	unguard;
}

void XScrollAreaWindow::DescendantRemoved(XWindow* Child)
{
	if (Child == hScaleMgr)
		hScaleMgr = NULL;
	else if (Child == vScaleMgr)
		vScaleMgr = NULL;
	else if (Child == hScale)
		hScale = NULL;
	else if (Child == vScale)
		vScale = NULL;
	else if (Child == leftButton)
		leftButton = NULL;
	else if (Child == rightButton)
		rightButton = NULL;
	else if (Child == upButton)
		upButton = NULL;
	else if (Child == downButton)
		downButton = NULL;
	else if (Child == clipWindow)
		clipWindow = NULL;
}

UBOOL XScrollAreaWindow::ScaleRangeChanged(XWindow* pScale, INT NewFromTick, INT NewToTick, FLOAT NewFromValue, FLOAT NewToValue, UBOOL bFinal)
{
	guard(XScrollAreaWindow::ScaleRangeChanged);

	struct
	{
		XWindow* pScale;
		INT newFromTick;
		INT newToTick;
		FLOAT newFromValue;
		FLOAT newToValue;
		UBOOL bFinal;
		UBOOL bResult;
	} Parms;

	Parms.pScale = pScale;
	Parms.newFromTick = NewFromTick;
	Parms.newToTick = NewToTick;
	Parms.newFromValue = NewFromValue;
	Parms.newToValue = NewToValue;
	Parms.bFinal = bFinal;
	Parms.bResult = FALSE;
	ProcessScript(EXTENSION_ScaleRangeChanged, &Parms, FALSE);

	if (clipWindow != NULL)
	{
		INT ChildX;
		INT ChildY;
		clipWindow->GetChildPosition(&ChildX, &ChildY);

		if (pScale == hScale)
		{
			ChildX = NewFromTick;
			clipWindow->SetChildPosition(ChildX, ChildY);
			return TRUE;
		}
		else if (pScale == vScale)
		{
			ChildY = NewFromTick;
			clipWindow->SetChildPosition(ChildX, ChildY);
			return TRUE;
		}
	}

	return Parms.bResult;

	unguard;
}

UBOOL XScrollAreaWindow::ClipAttributesChanged(XWindow* pClip, INT NewClipWidth, INT NewClipHeight, INT NewChildWidth, INT NewChildHeight)
{
	guard(XScrollAreaWindow::ClipAttributesChanged);

	struct
	{
		XWindow* pClip;
		INT newClipWidth;
		INT newClipHeight;
		INT newChildWidth;
		INT newChildHeight;
		UBOOL bResult;
	} Parms;

	Parms.pClip = pClip;
	Parms.newClipWidth = NewClipWidth;
	Parms.newClipHeight = NewClipHeight;
	Parms.newChildWidth = NewChildWidth;
	Parms.newChildHeight = NewChildHeight;
	Parms.bResult = FALSE;
	ProcessScript(EXTENSION_ClipAttributesChanged, &Parms, FALSE);

	if (pClip == clipWindow)
	{
		if (NewChildWidth < 1)
			NewChildWidth = 1;
		if (NewChildHeight < 1)
			NewChildHeight = 1;

		if (hScale != NULL)
			hScale->SetRanges(NewClipWidth, NewChildWidth);
		if (vScale != NULL)
			vScale->SetRanges(NewClipHeight, NewChildHeight);

		return TRUE;
	}

	return Parms.bResult;

	unguard;
}

UBOOL XScrollAreaWindow::ClipPositionChanged(XWindow* pClip, INT NewCol, INT NewRow)
{
	guard(XScrollAreaWindow::ClipPositionChanged);

	struct
	{
		XWindow* pClip;
		INT newCol;
		INT newRow;
		UBOOL bResult;
	} Parms;

	Parms.pClip = pClip;
	Parms.newCol = NewCol;
	Parms.newRow = NewRow;
	Parms.bResult = FALSE;
	ProcessScript(EXTENSION_ClipPositionChanged, &Parms, FALSE);

	if (pClip == clipWindow)
	{
		if (hScale != NULL)
			hScale->SetTickPosition(NewCol);
		if (vScale != NULL)
			vScale->SetTickPosition(NewRow);

		return TRUE;
	}

	return Parms.bResult;

	unguard;
}

UBOOL XScrollAreaWindow::MouseButtonPressed(FLOAT PointX, FLOAT PointY, EInputKey Button, INT NumClicks)
{
	guard(XScrollAreaWindow::MouseButtonPressed);

	struct
	{
		FLOAT pointX;
		FLOAT pointY;
		EInputKey button;
		INT numClicks;
		UBOOL bResult;
	} Parms;

	Parms.pointX = PointX;
	Parms.pointY = PointY;
	Parms.button = Button;
	Parms.numClicks = NumClicks;
	Parms.bResult = FALSE;
	ProcessScript(EXTENSION_MouseButtonPressed, &Parms, FALSE);

	if (Button == IK_MouseWheelUp)
	{
		if (vScale != NULL)
		{
			XWindow* Win = vScale;
			while (Win != NULL && Win->IsVisible() == TRUE)
				Win = Win->GetParent();

			if (Win == NULL)
				vScale->MoveThumb(MOVETHUMB_StepUp);
		}
		return TRUE;
	}
	else if (Button == IK_MouseWheelDown)
	{
		if (vScale != NULL)
		{
			XWindow* Win = vScale;
			while (Win != NULL && Win->IsVisible() == TRUE)
				Win = Win->GetParent();

			if (Win == NULL)
				vScale->MoveThumb(MOVETHUMB_StepDown);
		}
		return TRUE;
	}

	return Parms.bResult;

	unguard;
}
