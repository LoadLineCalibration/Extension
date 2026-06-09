/*=============================================================================
	XBorderWindow_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 04.

	Focus: border measurement, child margins, native border drawing, resize/move
	dragging and resize cursor selection.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XBorderWindow);

struct XReconBorderCursorRequestedParms
{
	XWindow* win;
	FLOAT pointX;
	FLOAT pointY;
	FLOAT hotX;
	FLOAT hotY;
	FColor color;
	UTexture** cursorShadow;
	XCursor* result;

	XReconBorderCursorRequestedParms()
		: win(NULL)
		, pointX(0.0f)
		, pointY(0.0f)
		, hotX(0.0f)
		, hotY(0.0f)
		, color(FColor(0,0,0,0))
		, cursorShadow(NULL)
		, result(NULL)
	{
	}
};

XBorderWindow::XBorderWindow(XWindow* Parent)
	: XWindow(Parent)
{
}

void XBorderWindow::Init(XWindow* Parent)
{
	guard(XBorderWindow::Init);

	XWindow::Init(Parent);

	borderLeft = NULL;
	borderTopLeft = NULL;
	borderTop = NULL;
	borderTopRight = NULL;
	borderRight = NULL;
	borderBottomRight = NULL;
	borderBottom = NULL;
	borderBottomLeft = NULL;
	center = NULL;

	moveCursor = NULL;
	hMoveCursor = NULL;
	vMoveCursor = NULL;
	tlMoveCursor = NULL;
	trMoveCursor = NULL;

	borderStyle = DSTY_None;
	bSmoothBorder = TRUE;
	bStretchBorder = FALSE;
	bResizeable = FALSE;
	bMarginsFromBorder = TRUE;

	childLeftMargin = 0.0f;
	childRightMargin = 0.0f;
	childTopMargin = 0.0f;
	childBottomMargin = 0.0f;

	leftMargin = 0.0f;
	rightMargin = 0.0f;
	topMargin = 0.0f;
	bottomMargin = 0.0f;

	bLeftDrag = FALSE;
	bRightDrag = FALSE;
	bUpDrag = FALSE;
	bDownDrag = FALSE;
	lastMouseX = 0.0f;
	lastMouseY = 0.0f;
	dragX = 0.0f;
	dragY = 0.0f;
	dragWidth = 0.0f;
	dragHeight = 0.0f;

	ComputeWindowBorders();

	unguard;
}

void XBorderWindow::CleanUp(void)
{
	guard(XBorderWindow::CleanUp);
	XWindow::CleanUp();
	unguard;
}

void XBorderWindow::SetBorders(UTexture* bordTL, UTexture* bordTR,
	UTexture* bordBL, UTexture* bordBR,
	UTexture* bordL, UTexture* bordR,
	UTexture* bordT, UTexture* bordB,
	UTexture* newCenter)
{
	guard(XBorderWindow::SetBorders);

	borderTopLeft = bordTL;
	borderTopRight = bordTR;
	borderBottomLeft = bordBL;
	borderBottomRight = bordBR;
	borderLeft = bordL;
	borderRight = bordR;
	borderTop = bordT;
	borderBottom = bordB;
	center = newCenter;

	ComputeWindowBorders();
	AskParentForReconfigure();

	unguard;
}

void XBorderWindow::SetBorderMargins(FLOAT NewLeft, FLOAT NewRight, FLOAT NewTop, FLOAT NewBottom)
{
	guard(XBorderWindow::SetBorderMargins);

	childLeftMargin = NewLeft;
	childRightMargin = NewRight;
	childTopMargin = NewTop;
	childBottomMargin = NewBottom;
	AskParentForReconfigure();

	unguard;
}

void XBorderWindow::BaseMarginsFromBorder(UBOOL bBorder)
{
	guard(XBorderWindow::BaseMarginsFromBorder);
	bMarginsFromBorder = bBorder;

	// Original Extension.dll only flips bMarginsFromBorder and asks parent for
	// reconfigure.  The ToolWindow scripts, however, set textures before
	// SetBorderStyle(), so the original side effect can leave cached border
	// margins at zero.  Keep the fixed behavior selectable and enabled by
	// default; set bFixBorderBaseMarginsAfterStyleChange=False for byte-for-byte
	// legacy side effects.
	if (XReconGetCompatibilityBool(TEXT("bFixBorderBaseMarginsAfterStyleChange"), TRUE) == TRUE)
		ComputeWindowBorders();

	AskParentForReconfigure();
	unguard;
}

void XBorderWindow::EnableResizing(UBOOL bNewResize)
{
	bResizeable = bNewResize;
}

void XBorderWindow::SetMoveCursors(XCursor* NewMove, XCursor* NewHMove,
	XCursor* NewVMove, XCursor* NewTLMove, XCursor* NewTRMove)
{
	moveCursor = NewMove;
	hMoveCursor = NewHMove;
	vMoveCursor = NewVMove;
	tlMoveCursor = NewTLMove;
	trMoveCursor = NewTRMove;
}

void XBorderWindow::ComputeWindowBorders(void)
{
	guard(XBorderWindow::ComputeWindowBorders);

	leftMargin = 0.0f;
	rightMargin = 0.0f;
	topMargin = 0.0f;
	bottomMargin = 0.0f;

	if (borderStyle != DSTY_None)
	{
		if (borderTopLeft != NULL)
		{
			if (FLOAT(borderTopLeft->USize) > leftMargin)
				leftMargin = FLOAT(borderTopLeft->USize);
			if (FLOAT(borderTopLeft->VSize) > topMargin)
				topMargin = FLOAT(borderTopLeft->VSize);
		}
		if (borderTopRight != NULL)
		{
			if (FLOAT(borderTopRight->USize) > rightMargin)
				rightMargin = FLOAT(borderTopRight->USize);
			if (FLOAT(borderTopRight->VSize) > topMargin)
				topMargin = FLOAT(borderTopRight->VSize);
		}
		if (borderBottomRight != NULL)
		{
			if (FLOAT(borderBottomRight->USize) > rightMargin)
				rightMargin = FLOAT(borderBottomRight->USize);
			if (FLOAT(borderBottomRight->VSize) > bottomMargin)
				bottomMargin = FLOAT(borderBottomRight->VSize);
		}
		if (borderBottomLeft != NULL)
		{
			if (FLOAT(borderBottomLeft->USize) > leftMargin)
				leftMargin = FLOAT(borderBottomLeft->USize);
			if (FLOAT(borderBottomLeft->VSize) > bottomMargin)
				bottomMargin = FLOAT(borderBottomLeft->VSize);
		}
		if (borderTop != NULL && FLOAT(borderTop->VSize) > topMargin)
			topMargin = FLOAT(borderTop->VSize);
		if (borderRight != NULL && FLOAT(borderRight->USize) > rightMargin)
			rightMargin = FLOAT(borderRight->USize);
		if (borderBottom != NULL && FLOAT(borderBottom->VSize) > bottomMargin)
			bottomMargin = FLOAT(borderBottom->VSize);
		if (borderLeft != NULL && FLOAT(borderLeft->USize) > leftMargin)
			leftMargin = FLOAT(borderLeft->USize);
	}

	unguard;
}

void XBorderWindow::ParentRequestedPreferredSize(UBOOL bWidthSpecified, FLOAT& PreferredWidth,
	UBOOL bHeightSpecified, FLOAT& PreferredHeight)
{
	guard(XBorderWindow::ParentRequestedPreferredSize);

	XWindow* Child = GetTopChild(TRUE);
	if (Child != NULL)
	{
		PreferredWidth -= childLeftMargin + childRightMargin;
		PreferredHeight -= childTopMargin + childBottomMargin;
		if (bMarginsFromBorder == TRUE)
		{
			PreferredWidth -= leftMargin + rightMargin;
			PreferredHeight -= topMargin + bottomMargin;
		}

		Child->QueryPreferredSize(bWidthSpecified, PreferredWidth, &PreferredWidth,
			bHeightSpecified, PreferredHeight, &PreferredHeight);

		PreferredWidth += childLeftMargin + childRightMargin;
		PreferredHeight += childTopMargin + childBottomMargin;
		if (bMarginsFromBorder == TRUE)
		{
			PreferredWidth += leftMargin + rightMargin;
			PreferredHeight += topMargin + bottomMargin;
		}

		if (leftMargin + rightMargin > PreferredWidth)
			PreferredWidth = leftMargin + rightMargin;
		if (topMargin + bottomMargin > PreferredHeight)
			PreferredHeight = topMargin + bottomMargin;
	}

	unguard;
}

UBOOL XBorderWindow::ChildRequestedReconfiguration(XWindow* Child)
{
	guard(XBorderWindow::ChildRequestedReconfiguration);
	return FALSE;
	unguard;
}

void XBorderWindow::ConfigurationChanged(void)
{
	guard(XBorderWindow::ConfigurationChanged);

	XWindow* Child = GetTopChild(TRUE);
	if (Child != NULL)
	{
		FLOAT ChildX = childLeftMargin;
		FLOAT ChildY = childTopMargin;
		FLOAT ChildW = width - childLeftMargin - childRightMargin;
		FLOAT ChildH = height - childTopMargin - childBottomMargin;

		if (bMarginsFromBorder == TRUE)
		{
			ChildX += leftMargin;
			ChildY += topMargin;
			ChildW -= leftMargin + rightMargin;
			ChildH -= topMargin + bottomMargin;
		}

		Child->ConfigureChild(ChildX, ChildY, ChildW, ChildH);
	}

	unguard;
}

void XBorderWindow::Draw(XGC* gc)
{
	guard(XBorderWindow::Draw);

	if (borderStyle != DSTY_None)
	{
		gc->SetStyle((EDrawStyle)borderStyle);
		gc->EnableSmoothing(bSmoothBorder);
		gc->DrawBorders(0.0f, 0.0f, width, height,
			leftMargin, rightMargin, topMargin, bottomMargin,
			borderTopLeft, borderTopRight, borderBottomLeft, borderBottomRight,
			borderLeft, borderRight, borderTop, borderBottom,
			center, bStretchBorder, bStretchBorder);
	}

	unguard;
}

static XCursor* XReconChooseCursor(XCursor* LocalCursor, XRootWindow* Root, INT RootFallbackIndex)
{
	if (LocalCursor != NULL)
		return LocalCursor;

	if (Root == NULL)
		return NULL;

	switch (RootFallbackIndex)
	{
		case 0:
			return Root->defaultMoveCursor;

		case 1:
			return Root->defaultTopLeftMoveCursor;

		case 2:
			return Root->defaultTopRightMoveCursor;

		case 3:
			return Root->defaultHorizontalMoveCursor;

		case 4:
			return Root->defaultVerticalMoveCursor;
	}

	return NULL;
}

XCursor* XBorderWindow::CursorRequested(XWindow* Win, FLOAT PointX, FLOAT PointY,
	FLOAT& HotX, FLOAT& HotY, FColor& NewColor, UTexture** CursorShadow)
{
	guard(XBorderWindow::CursorRequested);

	if (bResizeable == TRUE)
	{
		XRootWindow* Root = Cast<XRootWindow>(GetRootWindow());

		UBOOL bLeft = bLeftDrag;
		UBOOL bRight = bRightDrag;
		UBOOL bUp = bUpDrag;
		UBOOL bDown = bDownDrag;

		if (PointX >= 0.0f && PointX < leftMargin)
			bLeft = TRUE;
		if (PointX < width && width - rightMargin <= PointX)
			bRight = TRUE;
		if (PointY >= 0.0f && PointY < topMargin)
			bUp = TRUE;
		if (PointY < height && height - bottomMargin <= PointY)
			bDown = TRUE;

		if ((bLeft == TRUE && bRight == TRUE) || (bUp == TRUE && bDown == TRUE))
			return XReconChooseCursor(moveCursor, Root, 0);
		if ((bLeft == TRUE && bUp == TRUE) || (bRight == TRUE && bDown == TRUE))
			return XReconChooseCursor(tlMoveCursor, Root, 1);
		if ((bLeft == TRUE && bDown == TRUE) || (bRight == TRUE && bUp == TRUE))
			return XReconChooseCursor(trMoveCursor, Root, 2);
		if (bLeft == TRUE || bRight == TRUE)
			return XReconChooseCursor(hMoveCursor, Root, 3);
		if (bUp == TRUE || bDown == TRUE)
			return XReconChooseCursor(vMoveCursor, Root, 4);
	}

	XReconBorderCursorRequestedParms Parms;

	Parms.win = Win;
	Parms.pointX = PointX;
	Parms.pointY = PointY;
	Parms.hotX = HotX;
	Parms.hotY = HotY;
	Parms.color = NewColor;
	Parms.cursorShadow = CursorShadow;
	Parms.result = NULL;
	ProcessScript(EXTENSION_CursorRequested, &Parms, FALSE);
	HotX = Parms.hotX;
	HotY = Parms.hotY;
	NewColor = Parms.color;
	return Parms.result;

	unguard;
}

UBOOL XBorderWindow::MouseButtonPressed(FLOAT PointX, FLOAT PointY, EInputKey Button, INT NumClicks)
{
	guard(XBorderWindow::MouseButtonPressed);

	struct { FLOAT pointX; FLOAT pointY; EInputKey button; INT numClicks; UBOOL bResult; } Parms;
	Parms.pointX = PointX;
	Parms.pointY = PointY;
	Parms.button = Button;
	Parms.numClicks = NumClicks;
	Parms.bResult = FALSE;
	ProcessScript(EXTENSION_MouseButtonPressed, &Parms, FALSE);

	if (Button == IK_LeftMouse)
	{
		if (bResizeable == FALSE)
			return Parms.bResult;

		bLeftDrag = FALSE;
		bRightDrag = FALSE;
		bUpDrag = FALSE;
		bDownDrag = FALSE;
		if (PointX < leftMargin)
			bLeftDrag = TRUE;
		if (width - rightMargin <= PointX)
			bRightDrag = TRUE;
		if (PointY < topMargin)
			bUpDrag = TRUE;
		if (height - bottomMargin <= PointY)
			bDownDrag = TRUE;
	}
	else if (Button == IK_RightMouse && bResizeable == TRUE)
	{
		bLeftDrag = TRUE;
		bRightDrag = TRUE;
		bUpDrag = TRUE;
		bDownDrag = TRUE;
	}
	else
	{
		return Parms.bResult;
	}

	XWindow* Root = GetRootWindow();
	ConvertCoordinates(this, PointX, PointY, Root, &lastMouseX, &lastMouseY);
	dragX = x;
	dragY = y;
	dragWidth = width;
	dragHeight = height;
	Raise();
	return TRUE;

	unguard;
}

UBOOL XBorderWindow::MouseButtonReleased(FLOAT PointX, FLOAT PointY, EInputKey Button, INT NumClicks)
{
	guard(XBorderWindow::MouseButtonReleased);

	struct { FLOAT pointX; FLOAT pointY; EInputKey button; INT numClicks; UBOOL bResult; } Parms;
	Parms.pointX = PointX;
	Parms.pointY = PointY;
	Parms.button = Button;
	Parms.numClicks = NumClicks;
	Parms.bResult = FALSE;
	ProcessScript(EXTENSION_MouseButtonReleased, &Parms, FALSE);

	if (Button == IK_LeftMouse || Button == IK_RightMouse)
	{
		bLeftDrag = FALSE;
		bRightDrag = FALSE;
		bUpDrag = FALSE;
		bDownDrag = FALSE;
		if (bResizeable == TRUE)
			return TRUE;
	}

	return Parms.bResult;

	unguard;
}

void XBorderWindow::MouseMoved(FLOAT PointX, FLOAT PointY)
{
	guard(XBorderWindow::MouseMoved);

	struct { FLOAT pointX; FLOAT pointY; } Parms;
	Parms.pointX = PointX;
	Parms.pointY = PointY;
	ProcessScript(EXTENSION_MouseMoved, &Parms, FALSE);

	if (bLeftDrag == TRUE || bRightDrag == TRUE || bUpDrag == TRUE || bDownDrag == TRUE)
	{
		XWindow* Root = GetRootWindow();
		FLOAT RootX = PointX;
		FLOAT RootY = PointY;
		ConvertCoordinates(this, PointX, PointY, Root, &RootX, &RootY);

		FLOAT DeltaX = RootX - lastMouseX;
		FLOAT DeltaY = RootY - lastMouseY;
		lastMouseX = RootX;
		lastMouseY = RootY;

		if (bLeftDrag == TRUE)
		{
			dragWidth -= DeltaX;
			dragX += DeltaX;
		}
		if (bRightDrag == TRUE)
			dragWidth += DeltaX;
		if (bUpDrag == TRUE)
		{
			dragHeight -= DeltaY;
			dragY += DeltaY;
		}
		if (bDownDrag == TRUE)
			dragHeight += DeltaY;

		Configure(dragX, dragY, dragWidth, dragHeight);
	}

	unguard;
}
