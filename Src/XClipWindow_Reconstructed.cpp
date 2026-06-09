/*=============================================================================
	XClipWindow_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 04.

	Focus: single-child clipping viewport, logical scroll units, child sizing and
	scroll notifications.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XClipWindow);

XClipWindow::XClipWindow(XWindow* Parent)
	: XTabGroupWindow(Parent)
{
}

void XClipWindow::Init(XWindow* Parent)
{
	guard(XClipWindow::Init);

	XTabGroupWindow::Init(Parent);

	childH = -1;
	childV = -1;
	prefHUnits = -1;
	prefVUnits = -1;
	bForceChildWidth = FALSE;
	bForceChildHeight = FALSE;
	bSnapToUnits = TRUE;
	bFillWindow = TRUE;
	areaHSize = 0;
	areaVSize = 0;
	childHSize = 0;
	childVSize = 0;
	hMult = 1.0f;
	vMult = 1.0f;

	unguard;
}

void XClipWindow::CleanUp(void)
{
	guard(XClipWindow::CleanUp);
	XTabGroupWindow::CleanUp();
	unguard;
}

void XClipWindow::SetChildPosition(INT NewX, INT NewY)
{
	guard(XClipWindow::SetChildPosition);

	if (childH == NewX && childV == NewY)
		return;

	childH = NewX;
	childV = NewY;

	XWindow* Child = GetChild();
	if (Child != NULL)
	{
		FLOAT ChildX = 0.0f;
		if (bForceChildWidth == FALSE)
			ChildX = -hMult * FLOAT(childH);

		FLOAT ChildY = 0.0f;
		if (bForceChildHeight == FALSE)
			ChildY = -vMult * FLOAT(childV);

		ClampChildPosition(ChildX, ChildY, Child->width, Child->height);

		XWindow* OldAuthority = XReconPushConfigAuthority(this);
		Child->ConfigureChild(ChildX, ChildY, Child->width, Child->height);
		XReconPopConfigAuthority(OldAuthority);

		for (XWindow* Win = this; Win != NULL; Win = Win->GetParent())
		{
			if (Win->ClipPositionChanged(this, childH, childV) == TRUE)
				break;
		}
	}

	unguard;
}

void XClipWindow::GetChildPosition(INT* pNewX, INT* pNewY)
{
	if (pNewX != NULL)
		*pNewX = childH;
	if (pNewY != NULL)
		*pNewY = childV;
}

void XClipWindow::SetUnitSize(INT hUnits, INT vUnits)
{
	guard(XClipWindow::SetUnitSize);

	if (hUnits < 1)
		hUnits = 1;
	if (vUnits < 1)
		vUnits = 1;

	if (prefHUnits != hUnits || prefVUnits != vUnits)
	{
		prefHUnits = hUnits;
		prefVUnits = vUnits;
		AskParentForReconfigure();
	}

	unguard;
}

void XClipWindow::SetUnitWidth(INT hUnits)
{
	guard(XClipWindow::SetUnitWidth);

	if (hUnits < 1)
		hUnits = 1;
	if (prefHUnits != hUnits)
	{
		prefHUnits = hUnits;
		AskParentForReconfigure();
	}

	unguard;
}

void XClipWindow::SetUnitHeight(INT vUnits)
{
	guard(XClipWindow::SetUnitHeight);

	if (vUnits < 1)
		vUnits = 1;
	if (prefVUnits != vUnits)
	{
		prefVUnits = vUnits;
		AskParentForReconfigure();
	}

	unguard;
}

void XClipWindow::ResetUnitSize(void)
{
	guard(XClipWindow::ResetUnitSize);

	if (prefHUnits >= 0 || prefVUnits >= 0)
	{
		prefHUnits = -1;
		prefVUnits = -1;
		AskParentForReconfigure();
	}

	unguard;
}

void XClipWindow::ResetUnitWidth(void)
{
	guard(XClipWindow::ResetUnitWidth);

	if (prefHUnits >= 0)
	{
		prefHUnits = -1;
		AskParentForReconfigure();
	}

	unguard;
}

void XClipWindow::ResetUnitHeight(void)
{
	guard(XClipWindow::ResetUnitHeight);

	// ASM-confirmed original Extension.dll behaviour: this tests prefVUnits,
	// but clears prefHUnits.
	if (prefVUnits >= 0)
	{
		prefHUnits = -1;
		AskParentForReconfigure();
	}

	unguard;
}

void XClipWindow::GetUnitSize(INT* pAreaHSize, INT* pAreaVSize, INT* pChildHSize, INT* pChildVSize)
{
	if (pAreaHSize != NULL)
		*pAreaHSize = areaHSize;
	if (pAreaVSize != NULL)
		*pAreaVSize = areaVSize;
	if (pChildHSize != NULL)
		*pChildHSize = childHSize;
	if (pChildVSize != NULL)
		*pChildVSize = childVSize;
}

void XClipWindow::ForceChildSize(UBOOL bNewForceChildWidth, UBOOL bNewForceChildHeight)
{
	guard(XClipWindow::ForceChildSize);

	if ((UBOOL)bForceChildWidth != bNewForceChildWidth || (UBOOL)bForceChildHeight != bNewForceChildHeight)
	{
		bForceChildWidth = bNewForceChildWidth;
		bForceChildHeight = bNewForceChildHeight;
		AskParentForReconfigure();
	}

	unguard;
}

void XClipWindow::EnableSnapToUnits(UBOOL bNewSnapToUnits)
{
	guard(XClipWindow::EnableSnapToUnits);

	if ((UBOOL)bSnapToUnits != bNewSnapToUnits)
	{
		bSnapToUnits = bNewSnapToUnits;
		AskParentForReconfigure();
	}

	unguard;
}

XWindow* XClipWindow::GetChild(void)
{
	guard(XClipWindow::GetChild);

	XWindow* Child = GetTopChild(FALSE);

	// ASM-confirmed original Extension.dll behaviour: after an invisible top child
	// this advances from the clip window itself.
	while (Child != NULL && Child->IsVisible(FALSE) == FALSE)
		Child = GetLowerSibling(FALSE);

	return Child;

	unguard;
}

void XClipWindow::GetChildUnits(XWindow* Child, FLOAT& hUnits, FLOAT& vUnits)
{
	guard(XClipWindow::GetChildUnits);

	hUnits = 1.0f;
	vUnits = 1.0f;
	if (bSnapToUnits == TRUE && Child != NULL)
		Child->QueryGranularity(&hUnits, &vUnits);

	unguard;
}

void XClipWindow::ClampChildPosition(FLOAT& NewChildX, FLOAT& NewChildY, FLOAT NewChildWidth, FLOAT NewChildHeight)
{
	FLOAT MinX = width - NewChildWidth;
	if (MinX > NewChildX)
		NewChildX = MinX;
	if (NewChildX > 0.0f)
		NewChildX = 0.0f;

	FLOAT MinY = height - NewChildHeight;
	if (MinY > NewChildY)
		NewChildY = MinY;
	if (NewChildY > 0.0f)
		NewChildY = 0.0f;
}

void XClipWindow::GetChildPreferredSize(XWindow* Child, UBOOL bWidthSpecified, FLOAT QueryWidth,
	UBOOL bHeightSpecified, FLOAT QueryHeight)
{
	guard(XClipWindow::GetChildPreferredSize);

	if (Child == NULL)
		return;

	UBOOL bForceW = FALSE;
	UBOOL bForceH = FALSE;
	if (bWidthSpecified == TRUE && bForceChildWidth == TRUE)
		bForceW = TRUE;
	if (bHeightSpecified == TRUE && bForceChildHeight == TRUE)
		bForceH = TRUE;

	if (bForceW == TRUE)
	{
		Child->holdX = 0.0f;
		Child->holdWidth = QueryWidth;
	}
	else
	{
		Child->holdX = FLOAT(childH);
	}

	if (bForceH == TRUE)
	{
		Child->holdY = 0.0f;
		Child->holdHeight = QueryHeight;
	}
	else
	{
		Child->holdY = FLOAT(childV);
	}

	if (bForceW == TRUE && bForceH == TRUE)
		return;
	if (bForceW == TRUE)
	{
		Child->holdHeight = Child->QueryPreferredHeight(QueryWidth);
		return;
	}
	if (bForceH == TRUE)
	{
		Child->holdWidth = Child->QueryPreferredWidth(QueryHeight);
		return;
	}

	Child->QueryPreferredSize(&Child->holdWidth, &Child->holdHeight);

	unguard;
}

void XClipWindow::GetClipPreferredSize(XWindow* Child, UBOOL bWidthSpecified, FLOAT& PreferredWidth,
	UBOOL bHeightSpecified, FLOAT& PreferredHeight)
{
	guard(XClipWindow::GetClipPreferredSize);

	if (Child == NULL)
	{
		XTabGroupWindow::ParentRequestedPreferredSize(bWidthSpecified, PreferredWidth, bHeightSpecified, PreferredHeight);
		return;
	}

	FLOAT UnitW = 1.0f;
	FLOAT UnitH = 1.0f;
	GetChildUnits(Child, UnitW, UnitH);

	UBOOL bUseWidth = bWidthSpecified;
	if (prefHUnits >= 0 && bWidthSpecified == FALSE)
	{
		bUseWidth = TRUE;
		PreferredWidth = FLOAT(prefHUnits) * UnitW;
	}

	UBOOL bUseHeight = bHeightSpecified;
	if (prefVUnits >= 0 && bHeightSpecified == FALSE)
	{
		bUseHeight = TRUE;
		PreferredHeight = FLOAT(prefVUnits) * UnitH;
	}

	GetChildPreferredSize(Child, bUseWidth, PreferredWidth, bUseHeight, PreferredHeight);

	if (bUseWidth == FALSE)
		PreferredWidth = Child->holdWidth;
	if (bUseHeight == FALSE)
		PreferredHeight = Child->holdHeight;

	unguard;
}

void XClipWindow::QueryClipPreferredSize(UBOOL bWidthSpecified, FLOAT QueryWidth,
	FLOAT* pPreferredWidth, UBOOL* pBHClip,
	UBOOL bHeightSpecified, FLOAT QueryHeight,
	FLOAT* pPreferredHeight, UBOOL* pBVClip)
{
	guard(XClipWindow::QueryClipPreferredSize);

	
	FLOAT WorkWidth = 10.0f;
	if (bWidthSpecified == TRUE)
		WorkWidth = QueryWidth;
	FLOAT WorkHeight = 10.0f;
	if (bHeightSpecified == TRUE)
		WorkHeight = QueryHeight;

	XWindow* Child = GetChild();
	GetClipPreferredSize(Child, bWidthSpecified, WorkWidth, bHeightSpecified, WorkHeight);

	UBOOL bHClip = FALSE;
	UBOOL bVClip = FALSE;
	if (Child != NULL)
	{
		if (WorkWidth < Child->holdWidth)
			bHClip = TRUE;
		if (WorkHeight < Child->holdHeight)
			bVClip = TRUE;
	}

	if (bWidthSpecified == TRUE)
		WorkWidth = QueryWidth;
	if (bHeightSpecified == TRUE)
		WorkHeight = QueryHeight;

	if (pPreferredWidth != NULL)
		*pPreferredWidth = WorkWidth;
	if (pPreferredHeight != NULL)
		*pPreferredHeight = WorkHeight;
	if (pBHClip != NULL)
		*pBHClip = bHClip;
	if (pBVClip != NULL)
		*pBVClip = bVClip;

	unguard;
}

void XClipWindow::ReconfigureChild(XWindow* Child, INT NewChildH, INT NewChildV, FLOAT NewChildWidth, FLOAT NewChildHeight)
{
	guard(XClipWindow::ReconfigureChild);

	GetChildUnits(Child, hMult, vMult);

	FLOAT ChildX = -hMult * FLOAT(NewChildH);
	FLOAT ChildY = -vMult * FLOAT(NewChildV);

	INT NewChildHSize = appCeil(NewChildWidth / hMult);
	INT NewChildVSize = appCeil(NewChildHeight / vMult);

	if (bFillWindow == TRUE)
	{
		if (NewChildWidth < width)
			NewChildWidth = width;
		if (NewChildHeight < height)
			NewChildHeight = height;
	}

	INT NewAreaHSize = NewChildHSize - appCeil((NewChildWidth - width) / hMult);
	INT NewAreaVSize = NewChildVSize - appCeil((NewChildHeight - height) / vMult);

	ClampChildPosition(ChildX, ChildY, NewChildWidth, NewChildHeight);

	UBOOL bAttributesChanged = FALSE;
	if (areaHSize != NewAreaHSize || areaVSize != NewAreaVSize || childHSize != NewChildHSize || childVSize != NewChildVSize)
		bAttributesChanged = TRUE;

	XWindow* OldAuthority = XReconPushConfigAuthority(this);
	Child->ConfigureChild(ChildX, ChildY, NewChildWidth, NewChildHeight);
	XReconPopConfigAuthority(OldAuthority);

	areaHSize = NewAreaHSize;
	areaVSize = NewAreaVSize;
	childHSize = NewChildHSize;
	childVSize = NewChildVSize;

	if (bAttributesChanged == TRUE)
	{
		for (XWindow* Win = this; Win != NULL; Win = Win->GetParent())
		{
			if (Win->ClipAttributesChanged(this, areaHSize, areaVSize, childHSize, childVSize) == TRUE)
				break;
		}
	}

	unguard;
}

void XClipWindow::ChildRequestedShowArea(XWindow* Child, FLOAT ShowX, FLOAT ShowY, FLOAT ShowWidth, FLOAT ShowHeight)
{
	guard(XClipWindow::ChildRequestedShowArea);

	GetChildUnits(Child, hMult, vMult);
	if (hMult <= 0.0f || vMult <= 0.0f)
		return;

	FLOAT AdjustedX = ShowX + 0.005f;
	FLOAT AdjustedY = ShowY + 0.005f;
	FLOAT AdjustedW = ShowWidth - 0.01f;
	FLOAT AdjustedH = ShowHeight - 0.01f;

	INT TargetH = appFloor(AdjustedX / hMult);
	INT TargetHEnd = appCeil((AdjustedX + AdjustedW) / hMult);
	INT TargetHSize = TargetHEnd - TargetH;

	INT TargetV = appFloor(AdjustedY / vMult);
	INT TargetVEnd = appCeil((AdjustedY + AdjustedH) / vMult);
	INT TargetVSize = TargetVEnd - TargetV;

	INT NewH = childH;
	INT NewV = childV;

	INT DeltaH = TargetH - childH;
	if (areaHSize < TargetHSize)
	{
		if (DeltaH < areaHSize - TargetHSize)
			NewH = TargetH + TargetHSize - areaHSize;
		else if (DeltaH > 0)
			NewH = childH + DeltaH;
	}
	else
	{
		if (DeltaH > areaHSize - TargetHSize)
			NewH = TargetH + TargetHSize - areaHSize;
		else if (DeltaH < 0)
			NewH = TargetH;
	}

	INT DeltaV = TargetV - childV;
	if (areaVSize < TargetVSize)
	{
		if (DeltaV < areaVSize - TargetVSize)
			NewV = TargetV + TargetVSize - areaVSize;
		else if (DeltaV > 0)
			NewV = childV + DeltaV;
	}
	else
	{
		if (DeltaV > areaVSize - TargetVSize)
			NewV = TargetV + TargetVSize - areaVSize;
		else if (DeltaV < 0)
			NewV = TargetV;
	}

	SetChildPosition(NewH, NewV);

	unguard;
}

UBOOL XClipWindow::ChildRequestedReconfiguration(XWindow* Child)
{
	guard(XClipWindow::ChildRequestedReconfiguration);
	return FALSE;
	unguard;
}

void XClipWindow::ChildRequestedVisibilityChange(XWindow* Child, UBOOL bNewVisibility)
{
	guard(XClipWindow::ChildRequestedVisibilityChange);
	Child->SetChildVisibility(bNewVisibility);
	AskParentForReconfigure();
	unguard;
}

void XClipWindow::ParentRequestedPreferredSize(UBOOL bWidthSpecified, FLOAT& PreferredWidth,
	UBOOL bHeightSpecified, FLOAT& PreferredHeight)
{
	guard(XClipWindow::ParentRequestedPreferredSize);
	XWindow* Child = GetChild();
	GetClipPreferredSize(Child, bWidthSpecified, PreferredWidth, bHeightSpecified, PreferredHeight);
	unguard;
}

void XClipWindow::ConfigurationChanged(void)
{
	guard(XClipWindow::ConfigurationChanged);

	XWindow* MainChild = GetChild();
	for (XWindow* Child = GetTopChild(TRUE); Child != NULL; Child = Child->GetLowerSibling(TRUE))
	{
		if (Child == MainChild)
		{
			GetChildPreferredSize(Child, TRUE, width, TRUE, height);
			ReconfigureChild(Child, Child->holdX, Child->holdY, Child->holdWidth, Child->holdHeight);
		}
		else
		{
			XWindow* OldAuthority = XReconPushConfigAuthority(this);
			Child->ConfigureChild(0.0f, 0.0f, 0.0f, 0.0f);
			XReconPopConfigAuthority(OldAuthority);
		}
	}

	unguard;
}
