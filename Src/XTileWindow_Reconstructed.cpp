/*=============================================================================
	XTileWindow_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 04.

	Focus: exact TileWindow layout model: preferred-size negotiation, row building,
	wrapping, equal child sizes, direction and alignment.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XTileWindow);

static INT XReconTileTrunc(FLOAT Value)
{
	return (INT)Value;
}

static FLOAT XReconNonNegative(FLOAT Value)
{
	if (Value < 0.0f)
		return 0.0f;
	return Value;
}

XTileWindow::XTileWindow(XWindow* Parent)
	: XWindow(Parent)
{
}

void XTileWindow::Init(XWindow* Parent)
{
	guard(XTileWindow::Init);

	XWindow::Init(Parent);

	hMargin = 4.0f;
	vMargin = 4.0f;
	minorSpacing = 1.0f;
	majorSpacing = 1.0f;

	orientation = ORIENT_Horizontal;
	hDirection = HDIR_LeftToRight;
	vDirection = VDIR_TopToBottom;
	hChildAlign = HALIGN_Full;
	vChildAlign = VALIGN_Full;

	bWrap = TRUE;
	bFillParent = TRUE;
	bEqualWidth = FALSE;
	bEqualHeight = TRUE;

	unguard;
}

void XTileWindow::CleanUp(void)
{
	guard(XTileWindow::CleanUp);
	rowArray.Empty();
	XWindow::CleanUp();
	unguard;
}

void XTileWindow::Serialize(FArchive& Ar)
{
	guard(XTileWindow::Serialize);
	XWindow::Serialize(Ar);

	// rowArray is a transient layout cache in the original; it is rebuilt
	// from children/configuration rather than serialized.
	unguard;
}

void XTileWindow::SetOrder(EOrder NewOrder)
{
	guard(XTileWindow::SetOrder);

	switch (NewOrder)
	{
		case ORDER_Right:
			orientation = ORIENT_Horizontal;
			hDirection = HDIR_LeftToRight;
			vDirection = VDIR_TopToBottom;
			bWrap = FALSE;
			break;

		case ORDER_Left:
			orientation = ORIENT_Horizontal;
			hDirection = HDIR_RightToLeft;
			vDirection = VDIR_TopToBottom;
			bWrap = FALSE;
			break;

		case ORDER_Down:
			orientation = ORIENT_Vertical;
			hDirection = HDIR_LeftToRight;
			vDirection = VDIR_TopToBottom;
			bWrap = FALSE;
			break;

		case ORDER_Up:
			orientation = ORIENT_Vertical;
			hDirection = HDIR_LeftToRight;
			vDirection = VDIR_BottomToTop;
			bWrap = FALSE;
			break;

		case ORDER_RightThenUp:
			orientation = ORIENT_Horizontal;
			hDirection = HDIR_LeftToRight;
			vDirection = VDIR_BottomToTop;
			bWrap = TRUE;
			break;

		case ORDER_LeftThenDown:
			orientation = ORIENT_Horizontal;
			hDirection = HDIR_RightToLeft;
			vDirection = VDIR_TopToBottom;
			bWrap = TRUE;
			break;

		case ORDER_LeftThenUp:
			orientation = ORIENT_Horizontal;
			hDirection = HDIR_RightToLeft;
			vDirection = VDIR_BottomToTop;
			bWrap = TRUE;
			break;

		case ORDER_DownThenRight:
			orientation = ORIENT_Vertical;
			hDirection = HDIR_LeftToRight;
			vDirection = VDIR_TopToBottom;
			bWrap = TRUE;
			break;

		case ORDER_DownThenLeft:
			orientation = ORIENT_Vertical;
			hDirection = HDIR_RightToLeft;
			vDirection = VDIR_TopToBottom;
			bWrap = TRUE;
			break;

		case ORDER_UpThenRight:
			orientation = ORIENT_Vertical;
			hDirection = HDIR_LeftToRight;
			vDirection = VDIR_BottomToTop;
			bWrap = TRUE;
			break;

		case ORDER_UpThenLeft:
			orientation = ORIENT_Vertical;
			hDirection = HDIR_RightToLeft;
			vDirection = VDIR_BottomToTop;
			bWrap = TRUE;
			break;

		case ORDER_RightThenDown:
		default:
			orientation = ORIENT_Horizontal;
			hDirection = HDIR_LeftToRight;
			vDirection = VDIR_TopToBottom;
			bWrap = TRUE;
			break;
	}

	AskParentForReconfigure();

	unguard;
}

void XTileWindow::ParentRequestedPreferredSize(UBOOL bWidthSpecified, FLOAT& PreferredWidth,
	UBOOL bHeightSpecified, FLOAT& PreferredHeight)
{
	guard(XTileWindow::ParentRequestedPreferredSize);
	ComputeChildSizes(bWidthSpecified, PreferredWidth, bHeightSpecified, PreferredHeight);
	unguard;
}

void XTileWindow::ParentRequestedGranularity(FLOAT& hGranularity, FLOAT& vGranularity)
{
	guard(XTileWindow::ParentRequestedGranularity);

	XWindow::ParentRequestedGranularity(hGranularity, vGranularity);

	XWindow* TopChild = GetTopChild(TRUE);
	if (TopChild != NULL)
	{
		if (bEqualWidth == TRUE)
		{
			if (orientation == ORIENT_Vertical)
				hGranularity = minorSpacing + TopChild->width;
			else
				hGranularity = majorSpacing + TopChild->width;
		}
		if (bEqualHeight == TRUE)
		{
			if (orientation == ORIENT_Vertical)
				vGranularity = majorSpacing + TopChild->height;
			else
				vGranularity = minorSpacing + TopChild->height;
		}
	}

	unguard;
}

void XTileWindow::ChildRequestedVisibilityChange(XWindow* Child, UBOOL bNewVisibility)
{
	guard(XTileWindow::ChildRequestedVisibilityChange);
	Child->SetChildVisibility(bNewVisibility);
	AskParentForReconfigure();
	unguard;
}

UBOOL XTileWindow::ChildRequestedReconfiguration(XWindow* Child)
{
	guard(XTileWindow::ChildRequestedReconfiguration);
	return FALSE;
	unguard;
}

void XTileWindow::ConfigurationChanged(void)
{
	guard(XTileWindow::ConfigurationChanged);

	ComputeChildSizes(TRUE, width, TRUE, height);
	for (XWindow* ApplyChild = GetBottomChild(TRUE); ApplyChild != NULL; ApplyChild = ApplyChild->GetHigherSibling(TRUE))
		ApplyChild->ConfigureChild(ApplyChild->holdX, ApplyChild->holdY, ApplyChild->holdWidth, ApplyChild->holdHeight);

	unguard;
}

void XTileWindow::ComputeChildSizes(UBOOL bWidthSpecified, FLOAT& PreferredWidth,
	UBOOL bHeightSpecified, FLOAT& PreferredHeight)
{
	guard(XTileWindow::ComputeChildSizes);

	UBOOL bForceWidth = FALSE;
	UBOOL bForceHeight = FALSE;
	FLOAT ForcedCrossSize = 0.0f;

	if (bWrap == FALSE && bFillParent == TRUE)
	{
		if (orientation == ORIENT_Vertical && bWidthSpecified == TRUE)
		{
			bForceWidth = TRUE;
			ForcedCrossSize = XReconNonNegative(PreferredWidth - hMargin - hMargin);
		}
		else if (orientation == ORIENT_Horizontal && bHeightSpecified == TRUE)
		{
			bForceHeight = TRUE;
			ForcedCrossSize = XReconNonNegative(PreferredHeight - vMargin - vMargin);
		}
	}

	FLOAT EqualWidth = 0.0f;
	FLOAT EqualHeight = 0.0f;

	for (XWindow* Child = GetBottomChild(FALSE); Child != NULL; Child = Child->GetHigherSibling(FALSE))
	{
		if (Child->IsVisible(FALSE) == FALSE)
		{
			Child->holdX = 0.0f;
			Child->holdY = 0.0f;
			Child->holdWidth = 0.0f;
			Child->holdHeight = 0.0f;
			continue;
		}

		if (bForceWidth == TRUE)
		{
			Child->holdWidth = ForcedCrossSize;
			Child->holdHeight = Child->QueryPreferredHeight(ForcedCrossSize);
		}
		else if (bForceHeight == TRUE)
		{
			Child->holdHeight = ForcedCrossSize;
			Child->holdWidth = Child->QueryPreferredWidth(ForcedCrossSize);
		}
		else
		{
			Child->QueryPreferredSize(&Child->holdWidth, &Child->holdHeight);
		}

		if (bEqualWidth == TRUE && EqualWidth < Child->holdWidth)
			EqualWidth = Child->holdWidth;
		if (bEqualHeight == TRUE && EqualHeight < Child->holdHeight)
			EqualHeight = Child->holdHeight;
	}

	if (bEqualWidth == TRUE || bEqualHeight == TRUE)
	{
		for (XWindow* Child = GetBottomChild(TRUE); Child != NULL; Child = Child->GetHigherSibling(TRUE))
		{
			if (bEqualWidth == TRUE)
				Child->holdWidth = EqualWidth;
			if (bEqualHeight == TRUE)
				Child->holdHeight = EqualHeight;
		}
	}

	FLOAT MaxRowAxisLength = 1000000.0f;
	if (bWrap == TRUE)
	{
		if (orientation == ORIENT_Horizontal && bWidthSpecified == TRUE)
			MaxRowAxisLength = PreferredWidth - hMargin - hMargin;
		if (orientation == ORIENT_Vertical && bHeightSpecified == TRUE)
			MaxRowAxisLength = PreferredHeight - vMargin - vMargin;
	}

	rowArray.Empty();

	// Original Extension.dll keeps the per-row accumulators as INTs and
	// converts back through MSVC _ftol after each accumulation/update.
	// Do not keep these as FLOATs until row finalization: fractional child
	// sizes/spacing otherwise change wrapping and preferred-size results.
	INT RowCrossSize = XReconTileTrunc(ForcedCrossSize);
	INT RowAxisLength = 0;
	INT RowItems = 0;

	for (XWindow* RowChild = GetBottomChild(TRUE); RowChild != NULL; RowChild = RowChild->GetHigherSibling(TRUE))
	{
		XWindow* Child = RowChild;
		FLOAT ChildAxisSize = 0.0f;
		FLOAT ChildCrossSize = 0.0f;
		if (orientation == ORIENT_Vertical)
		{
			ChildAxisSize = Child->holdHeight;
			ChildCrossSize = Child->holdWidth;
		}
		else
		{
			ChildAxisSize = Child->holdWidth;
			ChildCrossSize = Child->holdHeight;
		}

		if (RowItems > 0 && (FLOAT)RowAxisLength + ChildAxisSize > MaxRowAxisLength)
		{
			INT RowIndex = rowArray.Add(1);
			RowAxisLength = XReconTileTrunc((FLOAT)RowAxisLength - minorSpacing);
			rowArray(RowIndex).rowHeight = RowCrossSize;
			rowArray(RowIndex).rowLength = RowAxisLength;
			rowArray(RowIndex).rowItems = RowItems;
			RowCrossSize = XReconTileTrunc(ForcedCrossSize);
			RowAxisLength = 0;
			RowItems = 0;
		}

		RowAxisLength = XReconTileTrunc((FLOAT)RowAxisLength + minorSpacing + ChildAxisSize);
		if ((FLOAT)RowCrossSize < ChildCrossSize)
			RowCrossSize = XReconTileTrunc(ChildCrossSize);
		RowItems++;
	}

	if (RowItems > 0)
	{
		INT RowIndex = rowArray.Add(1);
		RowAxisLength = XReconTileTrunc((FLOAT)RowAxisLength - minorSpacing);
		rowArray(RowIndex).rowHeight = RowCrossSize;
		rowArray(RowIndex).rowLength = RowAxisLength;
		rowArray(RowIndex).rowItems = RowItems;
	}

	if (bWidthSpecified == TRUE && bHeightSpecified == TRUE)
	{
		INT RowIndex = 0;
		FLOAT CurrentX = hMargin;
		FLOAT CurrentY = vMargin;

		for (XWindow* Child = GetBottomChild(TRUE); Child != NULL; Child = Child->GetHigherSibling(TRUE))
		{
			if (RowIndex >= rowArray.Num())
				break;

			XRowStruct& Row = rowArray(RowIndex);

			if (orientation == ORIENT_Horizontal)
			{
				if (hDirection == HDIR_RightToLeft)
					Child->holdX = PreferredWidth - CurrentX - Child->holdWidth;
				else
					Child->holdX = CurrentX;

				if (vDirection == VDIR_BottomToTop)
					Child->holdY = PreferredHeight - FLOAT(Row.rowHeight) - CurrentY;
				else
					Child->holdY = CurrentY;

				if (vChildAlign == VALIGN_Center)
					Child->holdY += FLOAT(XReconTileTrunc((FLOAT(Row.rowHeight) - Child->holdHeight) / 2.0f));
				else if (vChildAlign == VALIGN_Bottom)
					Child->holdY += FLOAT(Row.rowHeight) - Child->holdHeight;
				else if (vChildAlign == VALIGN_Full)
					Child->holdHeight = FLOAT(Row.rowHeight);

				CurrentX += minorSpacing + Child->holdWidth;
				Row.rowItems--;
				if (Row.rowItems < 1)
				{
					CurrentX = hMargin;
					CurrentY += FLOAT(Row.rowHeight) + majorSpacing;
					RowIndex++;
				}
			}
			else
			{
				if (hDirection == HDIR_RightToLeft)
					Child->holdX = PreferredWidth - FLOAT(Row.rowHeight) - CurrentX;
				else
					Child->holdX = CurrentX;

				if (vDirection == VDIR_BottomToTop)
					Child->holdY = PreferredHeight - CurrentY - Child->holdHeight;
				else
					Child->holdY = CurrentY;

				if (hChildAlign == HALIGN_Center)
					Child->holdX += FLOAT(XReconTileTrunc((FLOAT(Row.rowHeight) - Child->holdWidth) / 2.0f));
				else if (hChildAlign == HALIGN_Right)
					Child->holdX += FLOAT(Row.rowHeight) - Child->holdWidth;
				else if (hChildAlign == HALIGN_Full)
					Child->holdWidth = FLOAT(Row.rowHeight);

				CurrentY += minorSpacing + Child->holdHeight;
				Row.rowItems--;
				if (Row.rowItems < 1)
				{
					CurrentY = vMargin;
					CurrentX += FLOAT(Row.rowHeight) + majorSpacing;
					RowIndex++;
				}
			}
		}
	}

	FLOAT TotalAxisSize = 0.0f;
	FLOAT TotalCrossSize = 0.0f;
	for (INT i = 0; i < rowArray.Num(); i++)
	{
		if (TotalAxisSize < FLOAT(rowArray(i).rowLength))
			TotalAxisSize = FLOAT(rowArray(i).rowLength);
		TotalCrossSize += FLOAT(rowArray(i).rowHeight);
		if (i > 0)
			TotalCrossSize += majorSpacing;
	}

	if (orientation == ORIENT_Vertical)
	{
		if (bWidthSpecified == FALSE)
			PreferredWidth = hMargin + hMargin + TotalCrossSize;
		if (bHeightSpecified == FALSE)
			PreferredHeight = vMargin + vMargin + TotalAxisSize;
	}
	else
	{
		if (bWidthSpecified == FALSE)
			PreferredWidth = hMargin + hMargin + TotalAxisSize;
		if (bHeightSpecified == FALSE)
			PreferredHeight = vMargin + vMargin + TotalCrossSize;
	}

	unguard;
}
