/*=============================================================================
	XButtonToggleCheckbox_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 03.

	Focus: button state machine, toggle semantics and checkbox layout/drawing.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XButtonWindow);
IMPLEMENT_CLASS(XToggleWindow);
IMPLEMENT_CLASS(XCheckboxWindow);

static const TCHAR* XReconButtonGetTextData(const FStringNoInit& Text)
{
	if (Text.Len() > 0)
		return *Text;

	return TEXT("");
}


/*-----------------------------------------------------------------------------
	XButtonWindow.
-----------------------------------------------------------------------------*/

XButtonWindow::XButtonWindow(XWindow* Parent)
	: XTextWindow(Parent)
{
}

void XButtonWindow::Init(XWindow* Parent)
{
	guard(XButtonWindow::Init);

	XTextWindow::Init(Parent);

	bButtonPressed = FALSE;
	bMousePressed = FALSE;
	bAutoRepeat = FALSE;
	bEnableRightMouseClick = FALSE;

	activateDelay = 0.3f;
	initialDelay = 0.5f;
	repeatRate = 0.1f;

	pressSound = NULL;
	clickSound = NULL;
	activateTimer = 0;
	repeatTime = 0.0f;
	lastInputKey = IK_None;

	curTexture = NULL;
	curTileColor = FColor(255, 255, 255);
	curTextColor = FColor(0, 255, 0);

	for (INT StateIndex = 0; StateIndex < MAX_BUTTON_STATES; StateIndex++)
	{
		info[StateIndex].texture = NULL;
		info[StateIndex].tileColor = FColor(255, 255, 255);
		info[StateIndex].textColor = FColor(0, 255, 0);
	}

	SetSelectability(TRUE);

	unguard;
}

void XButtonWindow::CleanUp(void)
{
	guard(XButtonWindow::CleanUp);

	XTextWindow::CleanUp();

	unguard;
}

void XButtonWindow::ActivateButton(EInputKey Key)
{
	guard(XButtonWindow::ActivateButton);

	for (XWindow* Window = this; Window != NULL; Window = Window->parent)
	{
		UBOOL bHandled = FALSE;

		if (Key == IK_RightMouse)
			bHandled = Window->ButtonActivatedRight(this);
		else
			bHandled = Window->ButtonActivated(this);

		if (bHandled == TRUE)
			break;
	}

	unguard;
}

void XButtonWindow::SetButtonTextures(
	UTexture* Normal,
	UTexture* Pressed,
	UTexture* NormalFocus,
	UTexture* PressedFocus,
	UTexture* NormalInsensitive,
	UTexture* PressedInsensitive)
{
	guard(XButtonWindow::SetButtonTextures);

	info[0].texture = Normal;
	info[1].texture = Pressed;
	info[2].texture = NormalFocus;
	info[3].texture = PressedFocus;
	info[4].texture = NormalInsensitive;
	info[5].texture = PressedInsensitive;

	ChangeButtonAppearance();

	unguard;
}

void XButtonWindow::SetButtonColors(
	FColor Normal,
	FColor Pressed,
	FColor NormalFocus,
	FColor PressedFocus,
	FColor NormalInsensitive,
	FColor PressedInsensitive)
{
	guard(XButtonWindow::SetButtonColors);

	info[0].tileColor = Normal;
	info[1].tileColor = Pressed;
	info[2].tileColor = NormalFocus;
	info[3].tileColor = PressedFocus;
	info[4].tileColor = NormalInsensitive;
	info[5].tileColor = PressedInsensitive;

	ChangeButtonAppearance();

	unguard;
}

void XButtonWindow::SetTextColors(
	FColor Normal,
	FColor Pressed,
	FColor NormalFocus,
	FColor PressedFocus,
	FColor NormalInsensitive,
	FColor PressedInsensitive)
{
	guard(XButtonWindow::SetTextColors);

	info[0].textColor = Normal;
	info[1].textColor = Pressed;
	info[2].textColor = NormalFocus;
	info[3].textColor = PressedFocus;
	info[4].textColor = NormalInsensitive;
	info[5].textColor = PressedInsensitive;

	ChangeButtonAppearance();

	unguard;
}

void XButtonWindow::EnableAutoRepeat(UBOOL bEnable, FLOAT NewInitialDelay, FLOAT NewRepeatRate)
{
	bAutoRepeat = bEnable;
	initialDelay = NewInitialDelay;
	repeatRate = NewRepeatRate;
}

void XButtonWindow::EnableRightMouseClick(UBOOL bEnable)
{
	bEnableRightMouseClick = bEnable;
}

void XButtonWindow::SetButtonSounds(USound* NewPressSound, USound* NewReleaseSound)
{
	pressSound = NewPressSound;
	clickSound = NewReleaseSound;
}

void XButtonWindow::PressButton(EInputKey Key)
{
	guard(XButtonWindow::PressButton);

	bButtonPressed = TRUE;
	ChangeButtonAppearance();
	AddActivateTimer();
	PlaySound(clickSound, -1.0f, 1.0f);

	lastInputKey = Key;
	ActivateButton(Key);

	unguard;
}

UBOOL XButtonWindow::MouseButtonPressed(FLOAT PointX, FLOAT PointY, EInputKey Button, INT NumClicks)
{
	guard(XButtonWindow::MouseButtonPressed);

	struct
	{
		FLOAT PointX;
		FLOAT PointY;
		EInputKey Button;
		INT NumClicks;
		UBOOL bResult;
	} Parms;

	Parms.PointX = PointX;
	Parms.PointY = PointY;
	Parms.Button = Button;
	Parms.NumClicks = NumClicks;
	Parms.bResult = FALSE;

	ProcessScript(EXTENSION_MouseButtonPressed, &Parms, FALSE);

	if (Button == IK_LeftMouse || (Button == IK_RightMouse && bEnableRightMouseClick == TRUE))
	{
		bButtonPressed = TRUE;
		bMousePressed = TRUE;
		ChangeButtonAppearance();

		if (bAutoRepeat == TRUE)
		{
			repeatTime = GetTickOffset() + initialDelay;
			PlaySound(clickSound, -1.0f, 1.0f);
			lastInputKey = Button;
			ActivateButton(Button);
		}
		else
		{
			PlaySound(pressSound, -1.0f, 1.0f);
		}

		return TRUE;
	}

	return Parms.bResult;

	unguard;
}

UBOOL XButtonWindow::MouseButtonReleased(FLOAT PointX, FLOAT PointY, EInputKey Button, INT NumClicks)
{
	guard(XButtonWindow::MouseButtonReleased);

	struct
	{
		FLOAT PointX;
		FLOAT PointY;
		EInputKey Button;
		INT NumClicks;
		UBOOL bResult;
	} Parms;

	Parms.PointX = PointX;
	Parms.PointY = PointY;
	Parms.Button = Button;
	Parms.NumClicks = NumClicks;
	Parms.bResult = FALSE;

	ProcessScript(EXTENSION_MouseButtonReleased, &Parms, FALSE);

	if (Button == IK_LeftMouse || (Button == IK_RightMouse && bEnableRightMouseClick == TRUE))
	{
		bButtonPressed = FALSE;
		bMousePressed = FALSE;
		repeatTime = 0.0f;
		lastInputKey = IK_None;
		ChangeButtonAppearance();

		if (IsPointInWindow(PointX, PointY) == TRUE)
		{
			if (bAutoRepeat == FALSE)
			{
				PlaySound(clickSound, -1.0f, 1.0f);
				ActivateButton(Button);
			}
		}

		return TRUE;
	}

	return Parms.bResult;

	unguard;
}

void XButtonWindow::MouseMoved(FLOAT PointX, FLOAT PointY)
{
	guard(XButtonWindow::MouseMoved);

	struct
	{
		FLOAT PointX;
		FLOAT PointY;
	} Parms;

	Parms.PointX = PointX;
	Parms.PointY = PointY;
	ProcessScript(EXTENSION_MouseMoved, &Parms, FALSE);

	if (bMousePressed == TRUE)
	{
		if (bIsSensitive == TRUE && IsPointInWindow(PointX, PointY) == TRUE)
			bButtonPressed = TRUE;
		else
			bButtonPressed = FALSE;

		ChangeButtonAppearance();
	}

	unguard;
}

void XButtonWindow::SensitivityChanged(UBOOL bNewSensitivity)
{
	guard(XButtonWindow::SensitivityChanged);

	const UBOOL bOriginalSensitivity = bNewSensitivity;
	ProcessScript(EXTENSION_SensitivityChanged, &bNewSensitivity, FALSE);

	if (bOriginalSensitivity == FALSE)
	{
		if (bMousePressed == TRUE)
		{
			bButtonPressed = FALSE;
			bMousePressed = FALSE;
		}
	}

	ChangeButtonAppearance();

	unguard;
}

void XButtonWindow::FocusEnteredWindow(void)
{
	guard(XButtonWindow::FocusEnteredWindow);

	ProcessScript(EXTENSION_FocusEnteredWindow, NULL, FALSE);
	ChangeButtonAppearance();

	unguard;
}

void XButtonWindow::FocusLeftWindow(void)
{
	guard(XButtonWindow::FocusLeftWindow);

	ProcessScript(EXTENSION_FocusLeftWindow, NULL, FALSE);
	ChangeButtonAppearance();

	unguard;
}

void XButtonWindow::Timer(XTimerId TimerId, INT Invocations, XTimerData ClientData)
{
	guard(XButtonWindow::Timer);

	XWindow::Timer(TimerId, Invocations, ClientData);

	if (TimerId == activateTimer)
	{
		bButtonPressed = FALSE;
		ChangeButtonAppearance();
		RemoveActivateTimer();
	}

	unguard;
}

void XButtonWindow::Tick(FLOAT DeltaSeconds)
{
	guard(XButtonWindow::Tick);

	if (bMousePressed == TRUE && bAutoRepeat == TRUE)
	{
		repeatTime -= DeltaSeconds;
		if (repeatTime < 0.0f)
		{
			repeatTime += repeatRate;
			if (repeatTime < 0.0f)
				repeatTime = repeatRate;

			FLOAT CursorX = 0.0f;
			FLOAT CursorY = 0.0f;
			GetCursorPos(&CursorX, &CursorY);

			if (IsPointInWindow(CursorX, CursorY) == TRUE)
			{
				PlaySound(clickSound, -1.0f, 1.0f);
				ActivateButton((EInputKey)lastInputKey);
			}
		}
	}

	unguard;
}

void XButtonWindow::Draw(XGC* GC)
{
	guard(XButtonWindow::Draw);

	GC->SetTileColor(curTileColor);
	GC->SetTextColor(curTextColor);
	GC->DrawPattern(0.0f, 0.0f, width, height, 0.0f, 0.0f, curTexture);

	XTextWindow::Draw(GC);

	unguard;
}

void XButtonWindow::ChangeButtonAppearance(void)
{
	guard(XButtonWindow::ChangeButtonAppearance);

	INT SensitiveFocusState = 2;

	XWindow* TestWindow = this;
	while (TestWindow != NULL && TestWindow->bIsSensitive == TRUE)
		TestWindow = TestWindow->parent;

	if (TestWindow == NULL)
		SensitiveFocusState = IsFocusWindow();

	INT StateIndex = SensitiveFocusState * 2;
	if (bButtonPressed == TRUE)
		StateIndex++;

	curTexture = info[StateIndex].texture;
	curTileColor = info[StateIndex].tileColor;
	curTextColor = info[StateIndex].textColor;

	if (curTexture == NULL)
	{
		if (bButtonPressed == TRUE)
			curTexture = info[1].texture;

		if (curTexture == NULL)
			curTexture = info[0].texture;
	}

	unguard;
}

void XButtonWindow::RemoveActivateTimer(void)
{
	guard(XButtonWindow::RemoveActivateTimer);

	if (activateTimer != 0)
	{
		RemoveTimer(activateTimer);
		activateTimer = 0;
	}

	unguard;
}

void XButtonWindow::AddActivateTimer(void)
{
	guard(XButtonWindow::AddActivateTimer);

	RemoveActivateTimer();
	activateTimer = AddTimer(activateDelay, FALSE, 0, NAME_None);

	unguard;
}

/*-----------------------------------------------------------------------------
	XToggleWindow.
-----------------------------------------------------------------------------*/

XToggleWindow::XToggleWindow(XWindow* Parent)
	: XButtonWindow(Parent)
{
}

void XToggleWindow::Init(XWindow* Parent)
{
	guard(XToggleWindow::Init);

	XButtonWindow::Init(Parent);
	enableSound = NULL;
	disableSound = NULL;

	unguard;
}

void XToggleWindow::CleanUp(void)
{
	guard(XToggleWindow::CleanUp);

	XButtonWindow::CleanUp();

	unguard;
}

void XToggleWindow::ChangeToggle(void)
{
	guard(XToggleWindow::ChangeToggle);

	for (XWindow* Window = this; Window != NULL; Window = Window->parent)
	{
		if (Window->ToggleChanged(this, bButtonPressed) == TRUE)
			break;
	}

	unguard;
}

void XToggleWindow::SetToggle(UBOOL bNewToggle)
{
	guard(XToggleWindow::SetToggle);

	if ((UBOOL)bButtonPressed != bNewToggle)
	{
		bButtonPressed = bNewToggle;
		ChangeButtonAppearance();
		ChangeToggle();
	}

	unguard;
}

UBOOL XToggleWindow::GetToggle(void)
{
	return bButtonPressed;
}

void XToggleWindow::SetToggleSounds(USound* NewEnableSound, USound* NewDisableSound)
{
	enableSound = NewEnableSound;
	disableSound = NewDisableSound;
}

UBOOL XToggleWindow::MouseButtonPressed(FLOAT PointX, FLOAT PointY, EInputKey Button, INT NumClicks)
{
	guard(XToggleWindow::MouseButtonPressed);

	struct
	{
		FLOAT PointX;
		FLOAT PointY;
		EInputKey Button;
		INT NumClicks;
		UBOOL bResult;
	} Parms;

	Parms.PointX = PointX;
	Parms.PointY = PointY;
	Parms.Button = Button;
	Parms.NumClicks = NumClicks;
	Parms.bResult = FALSE;

	ProcessScript(EXTENSION_MouseButtonPressed, &Parms, FALSE);

	if (Button == IK_LeftMouse)
		return TRUE;

	return Parms.bResult;

	unguard;
}

UBOOL XToggleWindow::MouseButtonReleased(FLOAT PointX, FLOAT PointY, EInputKey Button, INT NumClicks)
{
	guard(XToggleWindow::MouseButtonReleased);

	struct
	{
		FLOAT PointX;
		FLOAT PointY;
		EInputKey Button;
		INT NumClicks;
		UBOOL bResult;
	} Parms;

	Parms.PointX = PointX;
	Parms.PointY = PointY;
	Parms.Button = Button;
	Parms.NumClicks = NumClicks;
	Parms.bResult = FALSE;

	ProcessScript(EXTENSION_MouseButtonReleased, &Parms, FALSE);

	if (Button == IK_LeftMouse)
	{
		if (IsPointInWindow(PointX, PointY) == TRUE)
		{
			if (bButtonPressed == TRUE)
				PlaySound(enableSound, -1.0f, 1.0f);
			else
				PlaySound(disableSound, -1.0f, 1.0f);

			SetToggle((UBOOL)(bButtonPressed == FALSE));
		}

		return TRUE;
	}

	return Parms.bResult;

	unguard;
}

void XToggleWindow::PressButton(EInputKey Key)
{
	guard(XToggleWindow::PressButton);

	SetToggle((UBOOL)(bButtonPressed == FALSE));

	if (bButtonPressed == TRUE)
		PlaySound(enableSound, -1.0f, 1.0f);
	else
		PlaySound(disableSound, -1.0f, 1.0f);

	unguard;
}

/*-----------------------------------------------------------------------------
	XCheckboxWindow.
-----------------------------------------------------------------------------*/

XCheckboxWindow::XCheckboxWindow(XWindow* Parent)
	: XToggleWindow(Parent)
{
}

void XCheckboxWindow::Init(XWindow* Parent)
{
	guard(XCheckboxWindow::Init);

	XToggleWindow::Init(Parent);

	toggleOff = NULL;
	toggleOn = NULL;
	textureWidth = 0.0f;
	textureHeight = 0.0f;
	checkboxSpacing = 3.0f;
	bRightSide = FALSE;
	checkboxStyle = STY_Masked;
	checkboxColor = FColor(255, 255, 255);

	unguard;
}

void XCheckboxWindow::CleanUp(void)
{
	guard(XCheckboxWindow::CleanUp);

	XToggleWindow::CleanUp();

	unguard;
}

void XCheckboxWindow::SetCheckboxTextures(
	UTexture* NewToggleOff,
	UTexture* NewToggleOn,
	FLOAT NewTextureWidth,
	FLOAT NewTextureHeight)
{
	guard(XCheckboxWindow::SetCheckboxTextures);

	if (toggleOff != NewToggleOff ||
		toggleOn != NewToggleOn ||
		textureWidth != NewTextureWidth ||
		textureHeight != NewTextureHeight)
	{
		toggleOff = NewToggleOff;
		toggleOn = NewToggleOn;
		textureWidth = NewTextureWidth;
		textureHeight = NewTextureHeight;
		AskParentForReconfigure();
	}

	unguard;
}

void XCheckboxWindow::SetCheckboxSpacing(FLOAT NewSpacing)
{
	guard(XCheckboxWindow::SetCheckboxSpacing);

	if (checkboxSpacing != NewSpacing)
	{
		checkboxSpacing = NewSpacing;
		AskParentForReconfigure();
	}

	unguard;
}

void XCheckboxWindow::SetCheckboxStyle(BYTE NewStyle)
{
	checkboxStyle = NewStyle;
}

void XCheckboxWindow::SetCheckboxColor(FColor NewColor)
{
	checkboxColor = NewColor;
}

void XCheckboxWindow::ComputeTextureSize(FLOAT& TextureWidth, FLOAT& TextureHeight, FLOAT& TextureOffset)
{
	guard(XCheckboxWindow::ComputeTextureSize);

	if (this->textureWidth > 0.0f)
	{
		TextureWidth = this->textureWidth;
	}
	else
	{
		TextureWidth = 0.0f;

		if (toggleOn != NULL && FLOAT(toggleOn->USize) > TextureWidth)
			TextureWidth = FLOAT(toggleOn->USize);

		if (toggleOff != NULL && FLOAT(toggleOff->USize) > TextureWidth)
			TextureWidth = FLOAT(toggleOff->USize);
	}

	if (this->textureHeight > 0.0f)
	{
		TextureHeight = this->textureHeight;
	}
	else
	{
		TextureHeight = 0.0f;

		if (toggleOn != NULL && FLOAT(toggleOn->VSize) > TextureHeight)
			TextureHeight = FLOAT(toggleOn->VSize);

		if (toggleOff != NULL && FLOAT(toggleOff->VSize) > TextureHeight)
			TextureHeight = FLOAT(toggleOff->VSize);
	}

	if (TextureWidth <= 0.0f)
		TextureOffset = 0.0f;
	else
		TextureOffset = TextureWidth + checkboxSpacing;

	unguard;
}

void XCheckboxWindow::Draw(XGC* GC)
{
	guard(XCheckboxWindow::Draw);

	GC->SetAlignments((EHAlign)hAlign, (EVAlign)vAlign);
	GC->EnableWordWrap(bWordWrap);

	XWindow::Draw(GC);

	UTexture* ToggleTexture = toggleOff;
	if (bButtonPressed == TRUE)
		ToggleTexture = toggleOn;

	FLOAT CheckWidth = 0.0f;
	FLOAT CheckHeight = 0.0f;
	FLOAT CheckOffset = 0.0f;
	ComputeTextureSize(CheckWidth, CheckHeight, CheckOffset);

	FLOAT CheckY = FLOAT(INT((height - CheckHeight) * 0.5f));
	FLOAT TextY = vMargin + 1.0f;
	FLOAT TextWidth = width - hMargin - hMargin - CheckOffset;
	FLOAT TextHeight = height - vMargin - vMargin;
	FLOAT CheckX = hMargin;
	FLOAT TextX = hMargin + CheckOffset;

	if (bRightSide == TRUE)
	{
		TextX = hMargin;
		CheckX = width - hMargin - CheckOffset;
	}

	GC->SetTextColor(curTextColor);
	GC->DrawText(TextX, TextY, TextWidth, TextHeight, XReconButtonGetTextData(text));

	if (ToggleTexture != NULL)
	{
		GC->SetStyle(checkboxStyle);
		GC->SetTileColor(checkboxColor);
		GC->DrawIconPattern(CheckX, CheckY, CheckWidth, CheckHeight, 0.0f, 0.0f, CheckWidth, CheckHeight, ToggleTexture);
	}

	unguard;
}

void XCheckboxWindow::ParentRequestedPreferredSize(
	UBOOL bWidthSpecified,
	FLOAT& PreferredWidth,
	UBOOL bHeightSpecified,
	FLOAT& PreferredHeight)
{
	guard(XCheckboxWindow::ParentRequestedPreferredSize);

	XGC* GC = GetGC();
	if (GC == NULL)
		return;

	FLOAT CheckWidth = 0.0f;
	FLOAT CheckHeight = 0.0f;
	FLOAT CheckOffset = 0.0f;
	ComputeTextureSize(CheckWidth, CheckHeight, CheckOffset);

	const TCHAR* CurrentText = XReconButtonGetTextData(text);
	const UBOOL bHasText = appStricmp(CurrentText, TEXT("")) != 0;

	if (bHasText == TRUE)
	{
		if (bHeightSpecified == TRUE || (minLines < 0 && maxLines < 0))
		{
			if (bWidthSpecified == TRUE && bWordWrap == TRUE)
			{
				FLOAT TextQueryWidth = PreferredWidth - hMargin - hMargin - CheckOffset;
				GC->GetTextExtent(TextQueryWidth, PreferredWidth, PreferredHeight, CurrentText);
			}
			else
			{
				GC->GetTextExtent(0.0f, PreferredWidth, PreferredHeight, CurrentText);
			}
		}
		else
		{
			if (bWidthSpecified == TRUE)
				GC->GetTextExtent(PreferredWidth, PreferredWidth, PreferredHeight, CurrentText);
			else
				GC->GetTextExtent(0.0f, PreferredWidth, PreferredHeight, CurrentText);

			FLOAT FontHeight = GC->GetFontHeight(TRUE);
			FLOAT NewHeight = PreferredHeight;

			if (minLines >= 0)
			{
				FLOAT MinHeight = FLOAT(minLines) * FontHeight;
				if (MinHeight > NewHeight)
					NewHeight = MinHeight;
			}

			if (maxLines >= 0)
			{
				FLOAT MaxHeight = FLOAT(maxLines) * FontHeight;
				if (NewHeight > MaxHeight)
					NewHeight = MaxHeight;
			}

			PreferredHeight = NewHeight;
		}

		PreferredWidth += hMargin + hMargin + CheckOffset;
		PreferredHeight += vMargin + vMargin;
	}
	else
	{
		if (background != NULL)
		{
			PreferredWidth = FLOAT(background->USize);
			PreferredHeight = FLOAT(background->VSize);

			if (bHeightSpecified == FALSE)
			{
				if (minLines >= 0)
					PreferredHeight = FLOAT(minLines) * GC->GetFontHeight(TRUE);
				else if (maxLines >= 0)
					PreferredHeight = FLOAT(maxLines) * GC->GetFontHeight(TRUE);
			}
		}
		else
		{
			XTextWindow::ParentRequestedPreferredSize(
				bWidthSpecified,
				PreferredWidth,
				bHeightSpecified,
				PreferredHeight);
		}
	}

	FLOAT MinimumHeight = vMargin + vMargin + CheckHeight;
	if (MinimumHeight > PreferredHeight)
		PreferredHeight = MinimumHeight;

	FLOAT MinimumWidth = hMargin + hMargin + CheckWidth;
	if (MinimumWidth > PreferredWidth)
		PreferredWidth = MinimumWidth;

	ReleaseGC(GC);

	unguard;
}

/*-----------------------------------------------------------------------------
	Native ids documented by this pass:
	XButtonWindow: 1590..1598.
	XToggleWindow: 1610..1613.
	XCheckboxWindow: 1620..1624.
-----------------------------------------------------------------------------*/

