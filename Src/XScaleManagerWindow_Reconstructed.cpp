/*=============================================================================
	XScaleManagerWindow_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 05.

	Focus: compound scale widget layout, value field propagation, step buttons and
	scale notification interception.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XScaleManagerWindow);

XScaleManagerWindow::XScaleManagerWindow(XWindow* Parent)
	: XWindow(Parent)
{
}

void XScaleManagerWindow::Init(XWindow* Parent)
{
	guard(XScaleManagerWindow::Init);

	XWindow::Init(Parent);

	decButton = NULL;
	incButton = NULL;
	valueField = NULL;
	scale = NULL;

	orientation = ORIENT_Horizontal;
	bStretchScaleField = FALSE;
	bStretchValueField = TRUE;

	marginWidth = 0.0f;
	marginHeight = 0.0f;
	spacing = 0.0f;

	childHAlign = HALIGN_Full;
	childVAlign = VALIGN_Full;

	unguard;
}

void XScaleManagerWindow::CleanUp(void)
{
	guard(XScaleManagerWindow::CleanUp);
	XWindow::CleanUp();
	unguard;
}

void XScaleManagerWindow::SetScaleButtons(XButtonWindow* NewDecButton, XButtonWindow* NewIncButton)
{
	guard(XScaleManagerWindow::SetScaleButtons);

	if (IsDescendant(NewDecButton) == FALSE)
		NewDecButton = NULL;
	if (IsDescendant(NewIncButton) == FALSE)
		NewIncButton = NULL;

	decButton = NewDecButton;
	incButton = NewIncButton;

	unguard;
}

void XScaleManagerWindow::SetScale(XScaleWindow* NewScale)
{
	guard(XScaleManagerWindow::SetScale);

	if (IsDescendant(NewScale) == FALSE)
		NewScale = NULL;

	if (scale != NewScale)
	{
		scale = NewScale;
		AskParentForReconfigure();
		ChangeValueField();
	}

	unguard;
}

void XScaleManagerWindow::SetValueField(XTextWindow* NewValueField)
{
	guard(XScaleManagerWindow::SetValueField);

	if (IsDescendant(NewValueField) == FALSE)
		NewValueField = NULL;

	if (valueField != NewValueField)
	{
		valueField = NewValueField;
		AskParentForReconfigure();
		ChangeValueField();
	}

	unguard;
}

UBOOL XScaleManagerWindow::IsDescendant(XWindow* Child)
{
	guard(XScaleManagerWindow::IsDescendant);

	while (Child != NULL && Child != this)
		Child = Child->GetParent();

	if (Child != NULL)
		return TRUE;

	return FALSE;

	unguard;
}

void XScaleManagerWindow::ChangeValueField(void)
{
	guard(XScaleManagerWindow::ChangeValueField);

	if (valueField != NULL && scale != NULL)
		valueField->SetText(scale->GetValueString());

	unguard;
}

void XScaleManagerWindow::AddToBoundingBox(FLOAT& BoxWidth, FLOAT& BoxHeight, FLOAT SubWidth, FLOAT SubHeight, FLOAT Distance)
{
	guard(XScaleManagerWindow::AddToBoundingBox);

	if (orientation == ORIENT_Vertical)
	{
		BoxHeight += SubHeight + Distance;
		if (BoxWidth < SubWidth)
			BoxWidth = SubWidth;
	}
	else
	{
		BoxWidth += SubWidth + Distance;
		if (BoxHeight < SubHeight)
			BoxHeight = SubHeight;
	}

	unguard;
}

void XScaleManagerWindow::ComputeChildConfig(XWindow* Child, FLOAT& CurPos, FLOAT Distance)
{
	guard(XScaleManagerWindow::ComputeChildConfig);

	Child->QueryPreferredSize(&Child->holdWidth, &Child->holdHeight);
	Child->holdX = 0.0f;
	Child->holdY = 0.0f;

	FLOAT NewPos = CurPos + Distance;

	if (orientation == ORIENT_Vertical)
	{
		Child->holdY = NewPos;

		if (childHAlign == HALIGN_Left)
		{
			Child->holdX = marginWidth;
		}
		else if (childHAlign == HALIGN_Center)
		{
			Child->holdX = (width - Child->holdWidth) * 0.5f;
		}
		else if (childHAlign == HALIGN_Right)
		{
			Child->holdX = width - Child->holdWidth - marginWidth;
		}
		else
		{
			Child->holdX = marginWidth;
			// ASM-confirmed original Extension.dll behaviour: vertical/full align
			// writes the width-derived value into holdHeight.
			Child->holdHeight = width - (marginWidth + marginWidth);
		}

		CurPos = Child->holdY + Child->holdHeight;
	}
	else
	{
		Child->holdX = NewPos;

		if (childVAlign == VALIGN_Top)
		{
			Child->holdY = marginHeight;
		}
		else if (childVAlign == VALIGN_Center)
		{
			Child->holdY = (height - Child->holdHeight) * 0.5f;
		}
		else if (childVAlign == VALIGN_Bottom)
		{
			Child->holdY = height - Child->holdHeight - marginHeight;
		}
		else
		{
			Child->holdY = marginHeight;
			Child->holdHeight = height - (marginHeight + marginHeight);
		}

		CurPos = Child->holdX + Child->holdWidth;
	}

	unguard;
}

void XScaleManagerWindow::ComputeChildOffset(XWindow* Child, FLOAT& CurAdd, FLOAT AddSize)
{
	guard(XScaleManagerWindow::ComputeChildOffset);

	if (orientation == ORIENT_Vertical)
	{
		Child->holdY += CurAdd;
		Child->holdHeight += AddSize;
	}
	else
	{
		Child->holdX += CurAdd;
		Child->holdWidth += AddSize;
	}

	CurAdd += AddSize;

	unguard;
}

void XScaleManagerWindow::ParentRequestedPreferredSize(UBOOL bWidthSpecified, FLOAT& PreferredWidth, UBOOL bHeightSpecified, FLOAT& PreferredHeight)
{
	guard(XScaleManagerWindow::ParentRequestedPreferredSize);

	PreferredWidth = 0.0f;
	PreferredHeight = 0.0f;

	FLOAT Distance = 0.0f;
	for (XWindow* Child = GetBottomChild(TRUE); Child != NULL; Child = Child->GetHigherSibling(TRUE))
	{
		FLOAT ChildW = 0.0f;
		FLOAT ChildH = 0.0f;
		Child->QueryPreferredSize(&ChildW, &ChildH);
		AddToBoundingBox(PreferredWidth, PreferredHeight, ChildW, ChildH, Distance);
		Distance = spacing;
	}

	PreferredWidth += marginWidth + marginWidth;
	PreferredHeight += marginHeight + marginHeight;

	unguard;
}

UBOOL XScaleManagerWindow::ChildRequestedReconfiguration(XWindow* Child)
{
	guard(XScaleManagerWindow::ChildRequestedReconfiguration);
	return FALSE;
	unguard;
}

void XScaleManagerWindow::ChildRequestedVisibilityChange(XWindow* Child, UBOOL bNewVisibility)
{
	guard(XScaleManagerWindow::ChildRequestedVisibilityChange);

	Child->SetChildVisibility(bNewVisibility);
	AskParentForReconfigure();

	unguard;
}

void XScaleManagerWindow::ConfigurationChanged(void)
{
	guard(XScaleManagerWindow::ConfigurationChanged);

	FLOAT Available;
	FLOAT FirstDistance;

	if (orientation == ORIENT_Vertical)
	{
		Available = height - marginHeight;
		FirstDistance = marginHeight;
	}
	else
	{
		Available = width - marginWidth;
		FirstDistance = marginWidth;
	}

	FLOAT CurPos = 0.0f;
	FLOAT Distance = FirstDistance;

	for (XWindow* ConfigChild = GetBottomChild(TRUE); ConfigChild != NULL; ConfigChild = ConfigChild->GetHigherSibling(TRUE))
	{
		ComputeChildConfig(ConfigChild, CurPos, Distance);
		Distance = spacing;
	}

	UBOOL bCanStretchScale = FALSE;
	if (scale != NULL && scale->GetParent() == this)
	{
		if (scale->IsVisible() == TRUE && bStretchScaleField == TRUE)
			bCanStretchScale = TRUE;
	}

	UBOOL bCanStretchValue = FALSE;
	if (valueField != NULL && valueField->GetParent() == this)
	{
		if (valueField->IsVisible() == TRUE && bStretchValueField == TRUE)
			bCanStretchValue = TRUE;
	}

	if (CurPos != Available && (bCanStretchScale == TRUE || bCanStretchValue == TRUE))
	{
		FLOAT Extra = Available - CurPos;
		FLOAT ScaleExtra = 0.0f;
		FLOAT ValueExtra = 0.0f;

		if (bCanStretchScale == TRUE)
		{
			if (bCanStretchValue == TRUE)
			{
				ScaleExtra = Extra * 0.5f;
				ValueExtra = Extra - ScaleExtra;
			}
			else
			{
				ScaleExtra = Extra;
			}
		}
		else
		{
			ValueExtra = Extra;
		}

		FLOAT CurAdd = 0.0f;
		for (XWindow* OffsetChild = GetBottomChild(TRUE); OffsetChild != NULL; OffsetChild = OffsetChild->GetHigherSibling(TRUE))
		{
			if (OffsetChild == scale)
				ComputeChildOffset(OffsetChild, CurAdd, ScaleExtra);
			else if (OffsetChild == valueField)
				ComputeChildOffset(OffsetChild, CurAdd, ValueExtra);
			else
				ComputeChildOffset(OffsetChild, CurAdd, 0.0f);
		}
	}

	for (XWindow* ApplyChild = GetBottomChild(TRUE); ApplyChild != NULL; ApplyChild = ApplyChild->GetHigherSibling(TRUE))
		ApplyChild->ConfigureChild(ApplyChild->holdX, ApplyChild->holdY, ApplyChild->holdWidth, ApplyChild->holdHeight);

	unguard;
}

void XScaleManagerWindow::ChildRemoved(XWindow* Child)
{
	guard(XScaleManagerWindow::ChildRemoved);

	ProcessScript(EXTENSION_ChildRemoved, &Child, FALSE);

	if (Child == decButton)
		decButton = NULL;
	else if (Child == incButton)
		incButton = NULL;
	else if (Child == scale)
		scale = NULL;
	else if (Child == valueField)
		valueField = NULL;

	unguard;
}

UBOOL XScaleManagerWindow::ScalePositionChanged(XWindow* pScale, INT NewTickPosition, FLOAT NewValue, UBOOL bFinal)
{
	guard(XScaleManagerWindow::ScalePositionChanged);

	struct
	{
		XWindow* pScale;
		INT newTickPosition;
		FLOAT newValue;
		UBOOL bFinal;
		UBOOL bResult;
	} Parms;

	Parms.pScale = pScale;
	Parms.newTickPosition = NewTickPosition;
	Parms.newValue = NewValue;
	Parms.bFinal = bFinal;
	Parms.bResult = FALSE;
	ProcessScript(EXTENSION_ScalePositionChanged, &Parms, FALSE);

	if (pScale == scale)
		ChangeValueField();

	return Parms.bResult;

	unguard;
}

UBOOL XScaleManagerWindow::ScaleAttributesChanged(XWindow* pScale, INT TickPosition, INT TickSpan, INT NumTicks)
{
	guard(XScaleManagerWindow::ScaleAttributesChanged);

	struct
	{
		XWindow* pScale;
		INT tickPosition;
		INT tickSpan;
		INT numTicks;
		UBOOL bResult;
	} Parms;

	Parms.pScale = pScale;
	Parms.tickPosition = TickPosition;
	Parms.tickSpan = TickSpan;
	Parms.numTicks = NumTicks;
	Parms.bResult = FALSE;
	ProcessScript(EXTENSION_ScaleAttributesChanged, &Parms, FALSE);

	if (pScale == scale)
	{
		if (decButton != NULL)
			decButton->SetSensitivity(TickPosition > 0);
		if (incButton != NULL)
			incButton->SetSensitivity(TickPosition + TickSpan < NumTicks);
	}

	return Parms.bResult;

	unguard;
}

UBOOL XScaleManagerWindow::ButtonActivated(XWindow* Button)
{
	guard(XScaleManagerWindow::ButtonActivated);

	struct
	{
		XWindow* pButton;
		UBOOL bResult;
	} Parms;

	Parms.pButton = Button;
	Parms.bResult = FALSE;
	ProcessScript(EXTENSION_ButtonActivated, &Parms, FALSE);

	if (scale != NULL)
	{
		if (Button == incButton)
		{
			scale->MoveThumb(MOVETHUMB_StepDown);
			return TRUE;
		}
		else if (Button == decButton)
		{
			scale->MoveThumb(MOVETHUMB_StepUp);
			return TRUE;
		}
	}

	return Parms.bResult;

	unguard;
}
