/*=============================================================================
	XComputerWindow_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 11.

	Focus: ComputerWindow behaviour: queued terminal events, timed display buffer,
	input/edit bridge, one-key waits, cursor blinking, fadeout, game-pause gating,
	and the relationship between script API calls and the two internal buffers.

	This is reconstruction-first source. It is intentionally explicit and
	behavioural; it is not yet a final VC98 drop-in translation unit.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XComputerWindow);

/*-----------------------------------------------------------------------------
	VC98-compatible script parameter structs.
-----------------------------------------------------------------------------*/

struct XReconComputerEditActivatedParms
{
	XWindow* Edit;
	UBOOL bModified;
	UBOOL bHandled;
};

struct XReconComputerVirtualKeyPressedParms
{
	EInputKey Key;
	UBOOL bRepeat;
	UBOOL bHandled;
};

struct XReconComputerStartParms
{
	UBOOL bHandled;
};

struct XReconComputerInputFinishedParms
{
	FString InputKey;
	FString InputValue;
	UBOOL bHandled;
};

static UBOOL XReconComputerIsLevelPaused(APlayerPawnExt* Player)
{
	guard(XReconComputerIsLevelPaused);

	if (Player == NULL)
		return FALSE;

	if (Player->Level == NULL)
		return FALSE;

	return appStricmp(*Player->Level->Pauser, TEXT("")) != 0;

	unguard;
}

/*-----------------------------------------------------------------------------
	Small reconstruction helpers.
-----------------------------------------------------------------------------*/

static const TCHAR* ReconSafeText(const FString& Text)
{
	guard(ReconSafeText);

	if (Text.Len() <= 0)
		return TEXT("");

	return *Text;

	unguard;
}

static void ReconFreeEventString(TCHAR*& Text)
{
	guard(ReconFreeEventString);

	if (Text != NULL)
	{
		appFree(Text);
		Text = NULL;
	}

	unguard;
}

static TCHAR* ReconDuplicateEventString(const FString& Text)
{
	guard(ReconDuplicateEventString);

	const TCHAR* Source = ReconSafeText(Text);
	INT Count = appStrlen(Source) + 1;
	TCHAR* Copy = (TCHAR*)appMalloc(Count * sizeof(TCHAR), TEXT("New"));
	appStrcpy(Copy, Source);
	return Copy;

	unguard;
}

static FString ReconSingleKeyString(EInputKey Key)
{
	guard(ReconSingleKeyString);

	TCHAR Buffer[2];
	Buffer[0] = (TCHAR)Key;
	Buffer[1] = 0;
	return FString(Buffer);

	unguard;
}

/*-----------------------------------------------------------------------------
	Structors / object lifetime.
-----------------------------------------------------------------------------*/

XComputerWindow::XComputerWindow(XWindow* Parent)
: XWindow(Parent)
{
	guard(XComputerWindow::XComputerWindow);
	unguard;
}

void XComputerWindow::Init(XWindow* Parent)
{
	guard(XComputerWindow::Init);

	XWindow::Init(Parent);

	XWindow* Root = GetRootWindow();
	if (Root != NULL)
		player = Root->GetPlayerPawn();

	// If the level is already paused when the computer window is created, the
	// original sets bIgnoreGamePaused. That prevents the terminal from entering
	// pause-processing immediately because of an already existing pause owner.
	if (XReconComputerIsLevelPaused(player) == TRUE)
		bIgnoreGamePaused = TRUE;

	cursorNextEvent = cursorBlinkSpeed;

	CreateTextWindow();

	inputWindow = Cast<XEditWindow>(CreateNewWindow(XEditWindow::StaticClass(), this, FALSE));
	if (inputWindow != NULL)
	{
		inputWindow->SetTextMargins(0.0f, 0.0f);
		inputWindow->SetInsertionPointBlinkRate(0.0f, cursorBlinkSpeed + cursorBlinkSpeed);
	}

	SetSelectability(TRUE);

	unguard;
}

void XComputerWindow::CleanUp(void)
{
	guard(XComputerWindow::CleanUp);

	bIgnoreTick = TRUE;
	FreeQueuedBuffer();

	// Display buffer owns no heap strings in original layout. It can be dropped
	// by emptying the array directly.
	displayBuffer.Empty();

	XWindow::CleanUp();

	unguard;
}

void XComputerWindow::Serialize(FArchive& Ar)
{
	guard(XComputerWindow::Serialize);
	XWindow::Serialize(Ar);
	unguard;
}

void XComputerWindow::FreeQueuedBuffer(void)
{
	guard(XComputerWindow::FreeQueuedBuffer);

	bWaitingForKey = FALSE;

	for (INT Index = 0; Index < queuedBuffer.Num(); Index++)
	{
		CompWinEventStruct& Event = queuedBuffer(Index);

		if (Event.eventType == ET_GetInput || Event.eventType == ET_GetChar)
		{
			ReconFreeEventString(Event.inputKey);
			ReconFreeEventString(Event.defaultInputString);
		}
	}

	queuedBuffer.Empty();
	queuedBufferStart = 0;

	unguard;
}

/*-----------------------------------------------------------------------------
	Simple public setters.
-----------------------------------------------------------------------------*/

void XComputerWindow::SetBackgroundTextures(
	UTexture* BackTexture1,
	UTexture* BackTexture2,
	UTexture* BackTexture3,
	UTexture* BackTexture4,
	UTexture* BackTexture5,
	UTexture* BackTexture6)
{
	guard(XComputerWindow::SetBackgroundTextures);

	backgroundTextures[0] = BackTexture1;
	backgroundTextures[1] = BackTexture2;
	backgroundTextures[2] = BackTexture3;
	backgroundTextures[3] = BackTexture4;
	backgroundTextures[4] = BackTexture5;
	backgroundTextures[5] = BackTexture6;

	unguard;
}

void XComputerWindow::SetTextSize(INT NewCols, INT NewRows)
{
	guard(XComputerWindow::SetTextSize);

	textCols = NewCols;
	textRows = NewRows;
	CalculateTextWindowSize();

	unguard;
}

void XComputerWindow::SetTextWindowPosition(INT NewX, INT NewY)
{
	guard(XComputerWindow::SetTextWindowPosition);

	if (textWindow != NULL)
		textWindow->Move((FLOAT)NewX, (FLOAT)NewY);

	unguard;
}

void XComputerWindow::SetTextFont(UFont* NewFont, INT NewFontWidth, INT NewFontHeight, FColor NewFontColor)
{
	guard(XComputerWindow::SetTextFont);

	textFont = NewFont;
	fontWidth = NewFontWidth;
	fontHeight = NewFontHeight;
	fontColor = NewFontColor;

	// The original immediately appends a text color event. That means pending
	// text after this point adopts the new color through the same timed event
	// stream as script-level SetFontColor().
	AddTextColorEvent(NewFontColor);

	CalculateTextWindowSize();

	unguard;
}

void XComputerWindow::SetTextColor(FColor NewColor)
{
	guard(XComputerWindow::SetTextColor);
	AddTextColorEvent(NewColor);
	unguard;
}

void XComputerWindow::SetTextTiming(FLOAT NewTiming)
{
	guard(XComputerWindow::SetTextTiming);
	eventTimeInterval = NewTiming;
	unguard;
}

void XComputerWindow::SetFadeSpeed(FLOAT NewFadeSpeed)
{
	guard(XComputerWindow::SetFadeSpeed);
	fadeSpeed = NewFadeSpeed;
	unguard;
}

void XComputerWindow::SetCursorTexture(UTexture* NewCursorTexture, INT NewCursorWidth, INT NewCursorHeight)
{
	guard(XComputerWindow::SetCursorTexture);

	cursorTexture = NewCursorTexture;
	cursorWidth = NewCursorWidth;
	cursorHeight = NewCursorHeight;

	unguard;
}

void XComputerWindow::SetCursorColor(FColor NewColor)
{
	guard(XComputerWindow::SetCursorColor);
	cursorColor = NewColor;
	unguard;
}

void XComputerWindow::SetCursorBlinkSpeed(FLOAT NewBlinkSpeed)
{
	guard(XComputerWindow::SetCursorBlinkSpeed);

	cursorBlinkSpeed = NewBlinkSpeed;

	if (inputWindow != NULL)
		inputWindow->SetInsertionPointBlinkRate(0.0f, cursorBlinkSpeed + cursorBlinkSpeed);

	unguard;
}

void XComputerWindow::ShowTextCursor(UBOOL bShow)
{
	guard(XComputerWindow::ShowTextCursor);
	bShowCursor = bShow;
	unguard;
}

void XComputerWindow::SetTextSound(USound* NewTextSound)
{
	guard(XComputerWindow::SetTextSound);
	AddTextSoundEvent(NewTextSound);
	unguard;
}

void XComputerWindow::SetTypingSound(USound* NewTypingSound)
{
	guard(XComputerWindow::SetTypingSound);

	typingSound = NewTypingSound;

	if (inputWindow != NULL)
		inputWindow->SetEditSounds(NewTypingSound, NewTypingSound, NewTypingSound, NewTypingSound);

	unguard;
}

void XComputerWindow::SetComputerSoundVolume(FLOAT NewSoundVolume)
{
	guard(XComputerWindow::SetComputerSoundVolume);
	computerSoundVolume = NewSoundVolume;
	unguard;
}

void XComputerWindow::SetTypingSoundVolume(FLOAT NewSoundVolume)
{
	guard(XComputerWindow::SetTypingSoundVolume);

	if (inputWindow != NULL)
		inputWindow->SetSoundVolume(NewSoundVolume);

	unguard;
}

void XComputerWindow::SetThrottle(FLOAT ThrottleModifier)
{
	guard(XComputerWindow::SetThrottle);
	throttle = ThrottleModifier;
	unguard;
}

FLOAT XComputerWindow::GetThrottle(void)
{
	guard(XComputerWindow::GetThrottle);
	return throttle;
	unguard;
}

void XComputerWindow::ResetThrottle(void)
{
	guard(XComputerWindow::ResetThrottle);
	throttle = 1.0f;
	unguard;
}

void XComputerWindow::EnableWordWrap(UBOOL bNewWordWrap)
{
	guard(XComputerWindow::EnableWordWrap);
	bWordWrap = bNewWordWrap;
	unguard;
}

/*-----------------------------------------------------------------------------
	Immediate operations and queue API.
-----------------------------------------------------------------------------*/

void XComputerWindow::ClearScreen(void)
{
	guard(XComputerWindow::ClearScreen);

	bWaitingForKey = FALSE;
	displayBuffer.Empty();
	textX = 0;
	textY = 0;

	unguard;
}

void XComputerWindow::ClearLine(INT RowToClear)
{
	guard(XComputerWindow::ClearLine);

	bWaitingForKey = FALSE;

	for (INT Index = 0; Index < displayBuffer.Num(); Index++)
	{
		CompWinEventStruct& Event = displayBuffer(Index);
		if (Event.eventType == ET_Text && Event.charY == RowToClear)
		{
			displayBuffer.Remove(Index);
			Index--;
		}
	}

	unguard;
}

void XComputerWindow::Print(FString& PrintText, UBOOL bNewLine)
{
	guard(XComputerWindow::Print);

	bWaitingForKey = FALSE;

	const TCHAR* Text = ReconSafeText(PrintText);
	for (const TCHAR* Pos = Text; *Pos != 0; Pos++)
	{
		if (*Pos == TEXT('^'))
			AddNewLineEvent();
		else
			AddCharacterEvent(*Pos, -1, -1, FALSE);
	}

	// The exported native wrapper appends the optional newline after calling the
	// same character-add path. The C++ method body in the decompile ignores the
	// bNewLine parameter, but the public interface declares it, so we preserve the
	// native-facing behaviour here.
	if (bNewLine == TRUE)
		AddNewLineEvent();

	unguard;
}

void XComputerWindow::PrintImmediate(FString& PrintText)
{
	guard(XComputerWindow::PrintImmediate);

	// ASM verified: original saves eventTimeInterval and textSound, zeros both,
	// appends no-fade character/newline events, then restores both fields.
	// It also copies the input FString to a temporary TCHAR buffer before walking it.
	FLOAT SavedEventTimeInterval = eventTimeInterval;
	USound* SavedTextSound = textSound;

	eventTimeInterval = 0.0f;
	textSound = NULL;

	const TCHAR* Text = ReconSafeText(PrintText);
	for (const TCHAR* Pos = Text; *Pos != 0; Pos++)
	{
		if (*Pos == TEXT('^'))
			AddNewLineEvent();
		else
			AddCharacterEvent(*Pos, -1, -1, TRUE);
	}

	eventTimeInterval = SavedEventTimeInterval;
	textSound = SavedTextSound;

	unguard;
}

void XComputerWindow::PrintLn(void)
{
	guard(XComputerWindow::PrintLn);

	bWaitingForKey = FALSE;
	AddNewLineEvent();

	unguard;
}

void XComputerWindow::GetInput(INT MaxLength, FString& NewInputKey, FString& DefaultInputString, FString& InputMask)
{
	guard(XComputerWindow::GetInput);

	bWaitingForKey = FALSE;
	AddGetInputEvent(MaxLength, NewInputKey, DefaultInputString, InputMask);

	unguard;
}

void XComputerWindow::GetChar(FString& NewInputKey, UBOOL bEcho)
{
	guard(XComputerWindow::GetChar);
	AddGetCharEvent(NewInputKey, bEcho);
	unguard;
}

void XComputerWindow::PrintGraphic(UTexture* Graphic, INT Width, INT Height, INT PosX, INT PosY, UBOOL bStatic, UBOOL bPixelPos)
{
	guard(XComputerWindow::PrintGraphic);
	AddGraphicEvent(Graphic, Width, Height, PosX, PosY, bStatic, bPixelPos);
	unguard;
}

void XComputerWindow::PlaySoundLater(USound* Sound)
{
	guard(XComputerWindow::PlaySoundLater);
	AddSoundEvent(Sound);
	unguard;
}

void XComputerWindow::SetTextPosition(INT NewPosX, INT NewPosY)
{
	guard(XComputerWindow::SetTextPosition);
	AddTextPositionEvent(NewPosX, NewPosY);
	unguard;
}

UBOOL XComputerWindow::IsBufferFlushed(void)
{
	guard(XComputerWindow::IsBufferFlushed);
	return queuedBuffer.Num() == 0;
	unguard;
}

void XComputerWindow::Pause(FLOAT PauseLength)
{
	guard(XComputerWindow::Pause);

	if (PauseLength == -1.0f)
	{
		bPauseProcessing = TRUE;
	}
	else
	{
		timeNextEvent += PauseLength;
	}

	unguard;
}

void XComputerWindow::Resume(void)
{
	guard(XComputerWindow::Resume);

	timeNextEvent = timeCurrent;
	bPauseProcessing = FALSE;

	unguard;
}

UBOOL XComputerWindow::IsPaused(void)
{
	guard(XComputerWindow::IsPaused);
	return bPauseProcessing;
	unguard;
}

void XComputerWindow::FadeOutText(FLOAT FadeDuration)
{
	guard(XComputerWindow::FadeOutText);

	fadeOutStart = FadeDuration;
	fadeOutTimer = FadeDuration;

	unguard;
}

/*-----------------------------------------------------------------------------
	Queue event creation.
-----------------------------------------------------------------------------*/

INT XComputerWindow::AddEvent(ECompWinEventTypes EventType)
{
	guard(XComputerWindow::AddEvent);

	INT Index = queuedBuffer.AddZeroed();
	CompWinEventStruct& Event = queuedBuffer(Index);

	Event.eventType = EventType;
	Event.eventTime = timeNextEvent;
	timeLastEvent = timeCurrent;

	return Index;

	unguard;
}

void XComputerWindow::AddCharacterEvent(TCHAR CurrentChar, INT PosX, INT PosY, UBOOL bNoFade)
{
	guard(XComputerWindow::AddCharacterEvent);

	ECompWinEventTypes EventType = ET_Text;
	if (CurrentChar == TEXT('^'))
		EventType = ET_NewLine;

	INT Index = AddEvent(EventType);
	CompWinEventStruct& Event = queuedBuffer(Index);

	if (Event.eventType == ET_Text)
	{
		Event.character = CurrentChar;
		Event.charX = PosX;
		Event.charY = PosY;
		Event.bNoFade = bNoFade;
	}

	timeNextEvent += eventTimeInterval;

	unguard;
}

void XComputerWindow::AddTextColorEvent(FColor TextColor)
{
	guard(XComputerWindow::AddTextColorEvent);

	INT Index = AddEvent(ET_TextColor);
	queuedBuffer(Index).textColor = TextColor;

	unguard;
}

void XComputerWindow::AddTextSoundEvent(USound* NewTextSound)
{
	guard(XComputerWindow::AddTextSoundEvent);

	INT Index = AddEvent(ET_TextSound);
	queuedBuffer(Index).textSound = NewTextSound;

	unguard;
}

void XComputerWindow::AddTextPositionEvent(INT NewPosX, INT NewPosY)
{
	guard(XComputerWindow::AddTextPositionEvent);

	INT Index = AddEvent(ET_TextPosition);
	queuedBuffer(Index).newTextX = NewPosX;
	queuedBuffer(Index).newTextY = NewPosY;

	unguard;
}

void XComputerWindow::AddNewLineEvent(void)
{
	guard(XComputerWindow::AddNewLineEvent);

	AddEvent(ET_NewLine);
	timeNextEvent += eventTimeInterval;

	unguard;
}

void XComputerWindow::AddGraphicEvent(UTexture* Graphic, INT Width, INT Height, INT PosX, INT PosY, UBOOL bStatic, UBOOL bPixelPos)
{
	guard(XComputerWindow::AddGraphicEvent);

	INT Index = AddEvent(ET_Graphic);
	CompWinEventStruct& Event = queuedBuffer(Index);

	Event.graphic = Graphic;
	Event.graphicX = PosX;
	Event.graphicY = PosY;
	Event.width = Width;
	Event.height = Height;
	Event.bStatic = bStatic;
	Event.bPixelPos = bPixelPos;

	timeNextEvent += eventTimeInterval;

	unguard;
}

void XComputerWindow::AddClearScreenEvent(void)
{
	guard(XComputerWindow::AddClearScreenEvent);

	bWaitingForKey = FALSE;
	AddEvent(ET_Clear);

	unguard;
}

void XComputerWindow::AddSoundEvent(USound* Sound)
{
	guard(XComputerWindow::AddSoundEvent);

	INT Index = AddEvent(ET_Sound);
	queuedBuffer(Index).sound = Sound;

	unguard;
}

void XComputerWindow::AddGetInputEvent(INT InputLength, FString& NewInputKey, FString& DefaultInputString, FString& InputMask)
{
	guard(XComputerWindow::AddGetInputEvent);

	INT Index = AddEvent(ET_GetInput);
	CompWinEventStruct& Event = queuedBuffer(Index);

	Event.inputLength = InputLength;
	Event.inputKey = ReconDuplicateEventString(NewInputKey);

	const TCHAR* Mask = ReconSafeText(InputMask);
	Event.inputMask[0] = Mask[0];
	Event.inputMask[1] = 0;

	if (DefaultInputString.Len() > 0)
		Event.defaultInputString = ReconDuplicateEventString(DefaultInputString);
	else
		Event.defaultInputString = NULL;

	unguard;
}

void XComputerWindow::AddGetCharEvent(FString& NewInputKey, UBOOL bEcho)
{
	guard(XComputerWindow::AddGetCharEvent);

	INT Index = AddEvent(ET_GetChar);
	CompWinEventStruct& Event = queuedBuffer(Index);

	Event.inputKey = ReconDuplicateEventString(NewInputKey);
	Event.bEcho = bEcho;

	unguard;
}

void XComputerWindow::AddGetRawKeypress(FString& NewInputKey)
{
	guard(XComputerWindow::AddGetRawKeypress);

	// The public script API exposes GetChar(). The header also names this helper;
	// original behaviour is the same event type with echo disabled.
	AddGetCharEvent(NewInputKey, FALSE);

	unguard;
}

void XComputerWindow::AddFadeOutEvent(FLOAT NewFadeOutTimer)
{
	guard(XComputerWindow::AddFadeOutEvent);

	INT Index = AddEvent(ET_FadeOut);
	queuedBuffer(Index).fadeOutTimer = NewFadeOutTimer;

	unguard;
}

/*-----------------------------------------------------------------------------
	Text window and geometry.
-----------------------------------------------------------------------------*/

void XComputerWindow::CreateTextWindow(void)
{
	guard(XComputerWindow::CreateTextWindow);

	textWindow = CreateNewWindow(XWindow::StaticClass(), this, TRUE);
	CalculateTextWindowSize();

	if (textWindow != NULL)
		textWindow->SetWindowAlignments(HALIGN_Full, VALIGN_Full, 0.0f, 0.0f, 0.0f, 0.0f);

	unguard;
}

void XComputerWindow::CalculateTextWindowSize(void)
{
	guard(XComputerWindow::CalculateTextWindowSize);

	if (textFont == NULL)
		return;

	if (textWindow == NULL)
		return;

	Resize((FLOAT)(textCols * fontWidth), (FLOAT)(textRows * fontHeight));

	unguard;
}

void XComputerWindow::Scroll(void)
{
	guard(XComputerWindow::Scroll);

	for (INT Index = displayBuffer.Num() - 1; Index >= 0; Index--)
	{
		CompWinEventStruct& Event = displayBuffer(Index);

		if (Event.eventType == ET_Text)
		{
			Event.charY--;
			if (Event.charY < 0)
				displayBuffer.Remove(Index);
		}
		else if (Event.eventType == ET_Graphic)
		{
			Event.graphicY--;
			if (Event.graphicY < 0)
				displayBuffer.Remove(Index);
		}
	}

	unguard;
}

void XComputerWindow::ProcessNewLineEvent(void)
{
	guard(XComputerWindow::ProcessNewLineEvent);

	textX = 0;
	textY++;

	if (textY == textRows)
	{
		textY--;
		Scroll();
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Queued event execution.
-----------------------------------------------------------------------------*/

void XComputerWindow::ProcessQueuedBuffer(void)
{
	guard(XComputerWindow::ProcessQueuedBuffer);

	while (queuedBufferStart < queuedBuffer.Num())
	{
		CompWinEventStruct& Event = queuedBuffer(queuedBufferStart);

		if (timeCurrent <= Event.eventTime)
			break;

		switch (Event.eventType)
		{
			case ET_Text:
			{
				bCursorVisible = TRUE;
				cursorNextEvent = cursorBlinkSpeed;

				INT PosX = 0;
				INT PosY = 0;

				if (CalculateCharDisplayPosition(queuedBufferStart, PosX, PosY) == TRUE)
				{
					INT DisplayIndex = displayBuffer.AddItem(Event);
					CompWinEventStruct& DisplayEvent = displayBuffer(DisplayIndex);

					DisplayEvent.charColor = fontColor;
					if (DisplayEvent.bNoFade == TRUE)
						DisplayEvent.eventTime = 0.0f;
					else
						DisplayEvent.eventTime = fadeSpeed;

					DisplayEvent.charX = PosX;
					DisplayEvent.charY = PosY;
				}

				if (textSound != NULL)
				{
					FLOAT SoundX = (FLOAT)(fontWidth * textX) + ((FLOAT)fontWidth * 0.5f);
					FLOAT SoundY = (FLOAT)(fontHeight * textY) + ((FLOAT)fontHeight * 0.5f);
					PlaySound(textSound, computerSoundVolume, 1.0f, SoundX, SoundY);
				}
				break;
			}

			case ET_TextColor:
				fontColor = Event.textColor;
				break;

			case ET_TextSound:
				textSound = Event.textSound;
				break;

			case ET_TextPosition:
				textX = Event.newTextX;
				textY = Event.newTextY;
				break;

			case ET_NewLine:
				ProcessNewLineEvent();
				break;

			case ET_Graphic:
			{
				bCursorVisible = TRUE;
				cursorNextEvent = cursorBlinkSpeed;

				INT DisplayIndex = displayBuffer.AddItem(Event);
				// ASM-confirmed original Extension.dll behaviour: fadeSpeed is written
				// back to the local queued event copy, not to the display event.
				Event.eventTime = fadeSpeed;
				CalculateGraphicDisplayPosition(DisplayIndex);
				break;
			}

			case ET_Clear:
				displayBuffer.Empty();
				textX = 0;
				textY = 0;
				break;

			case ET_Sound:
			{
				FLOAT SoundX = (FLOAT)(fontWidth * textX) + ((FLOAT)fontWidth * 0.5f);
				FLOAT SoundY = (FLOAT)(fontHeight * textY) + ((FLOAT)fontHeight * 0.5f);
				PlaySound(Event.sound, computerSoundVolume, 1.0f, SoundX, SoundY);
				break;
			}

			case ET_Wait:
				break;

			case ET_GetInput:
				CreateInputControl(queuedBufferStart);
				break;

			case ET_GetChar:
				WaitForKey(queuedBufferStart);
				break;

			case ET_FadeOut:
				fadeOutStart = Event.fadeOutTimer;
				fadeOutTimer = Event.fadeOutTimer;
				break;
		}

		queuedBufferStart++;

		if (queuedBufferStart == QUEUED_BUFFER_START_MAX)
			ResetQueuedBuffer();
	}

	unguard;
}

void XComputerWindow::ResetQueuedBuffer(void)
{
	guard(XComputerWindow::ResetQueuedBuffer);

	if (queuedBufferStart > 0)
	{
		queuedBuffer.Remove(0, queuedBufferStart);
		queuedBufferStart = 0;
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Character and graphic positioning.
-----------------------------------------------------------------------------*/

UBOOL XComputerWindow::CalculateCharDisplayPosition(INT& QueuedIndex, INT& NewPosX, INT& NewPosY)
{
	guard(XComputerWindow::CalculateCharDisplayPosition);

	CompWinEventStruct& Event = queuedBuffer(QueuedIndex);

	if (Event.charX != -1)
	{
		// ASM-confirmed original Extension.dll behaviour: explicit charX/charY
		// makes the call succeed without copying coordinates to the output refs.
		return TRUE;
	}

	INT OriginalQueuedIndex = QueuedIndex;
	UBOOL bWrapNow = FALSE;

	// Original Extension.dll probes the whole upcoming word on every text
	// character when bWordWrap is enabled, not only after the previous line was
	// wrapped. If the word cannot fit in the remaining columns, wrapping happens
	// before drawing the first character of that word. This is what prevents
	// HUDInfoLink/ComputerWindow text from splitting "Reed" into "R" + "eed".
	if (bWordWrap == TRUE)
	{
		if (textX == 0 && Event.character == TEXT(' '))
			return FALSE;

		bLastLineWrapped = FALSE;

		if (textX < textCols)
		{
			UBOOL bWordStillRunning = TRUE;
			INT ProbeX = textX;
			INT ProbeIndex = QueuedIndex;

			while (ProbeX < textCols)
			{
				if (ProbeIndex == queuedBuffer.Num())
				{
					bWordStillRunning = FALSE;
					break;
				}

				CompWinEventStruct& ProbeEvent = queuedBuffer(ProbeIndex);

				if (ProbeEvent.eventType != ET_Text)
				{
					bWordStillRunning = FALSE;
					break;
				}

				if (ProbeEvent.charX != -1)
				{
					bWordStillRunning = FALSE;
					break;
				}

				if (ProbeEvent.character == TEXT(' '))
				{
					bWordStillRunning = FALSE;
					break;
				}

				ProbeX++;
				ProbeIndex++;
			}

			if (bWordStillRunning == TRUE && ProbeX >= textCols)
				bWrapNow = TRUE;
		}
	}

	QueuedIndex = OriginalQueuedIndex;

	if (bWrapNow == FALSE)
	{
		if (textX + 1 != textCols)
		{
			NewPosX = textX;
			NewPosY = textY;
			bLastLineWrapped = FALSE;
			textX++;
			return TRUE;
		}
	}

	textX = 0;
	NewPosX = 0;
	NewPosY = textY + 1;
	textY++;

	if (textY == textRows)
	{
		textY--;
		NewPosY = textY;
		Scroll();
	}

	bLastLineWrapped = TRUE;

	if (Event.character == TEXT(' '))
		return FALSE;

	textX++;
	return TRUE;

	unguard;
}

void XComputerWindow::CalculateGraphicDisplayPosition(INT DisplayIndex)
{
	guard(XComputerWindow::CalculateGraphicDisplayPosition);

	CompWinEventStruct& Event = displayBuffer(DisplayIndex);

	if (Event.graphicY == -1)
	{
		Event.graphicY = textY;

		INT FullPixelHeight = fontHeight * textRows;
		INT BottomPixel = Event.height + textY * fontHeight;

		if (BottomPixel > FullPixelHeight)
		{
			INT NewGraphicY = FullPixelHeight - Event.height;
			INT ScrollSteps = Event.height + textY * fontHeight - FullPixelHeight / fontHeight;

			if (ScrollSteps > 0)
			{
				while (ScrollSteps > 0)
				{
					Scroll();
					ScrollSteps--;
				}
			}

			Event.graphicY = NewGraphicY;
			textY = textRows - 1;
		}
		else
		{
			textY += Event.height / fontHeight;
		}
	}
	else
	{
		textY = (Event.graphicY + Event.height) / fontHeight + 1;
	}

	if (Event.graphicX == -1)
		Event.graphicX = textX;

	unguard;
}

/*-----------------------------------------------------------------------------
	Input creation and completion.
-----------------------------------------------------------------------------*/

void XComputerWindow::CreateInputControl(INT QueuedIndex)
{
	guard(XComputerWindow::CreateInputControl);

	CompWinEventStruct& Event = queuedBuffer(QueuedIndex);

	inputKey = Event.inputKey;
	ReconFreeEventString(Event.inputKey);

	if (inputWindow == NULL)
		return;

	inputWindow->SetVisibility(TRUE);
	inputWindow->SetFont(textFont);
	inputWindow->SetTextColor(fontColor);
	inputWindow->Move((FLOAT)(fontWidth * textX), (FLOAT)(fontHeight * textY));
	inputWindow->SetInsertionPointType(INSTYPE_Underscore, 0.0f, 0.0f);
	inputWindow->EnableSingleLineEditing(TRUE);
	inputWindow->SetMaxSize(Event.inputLength);

	inputMask = Event.inputMask;

	if (Event.defaultInputString != NULL)
	{
		inputWindow->SetText(Event.defaultInputString);
		inputWindow->MoveInsertionPoint(MOVEINSERT_End, FALSE);
		ReconFreeEventString(Event.defaultInputString);
	}
	else
	{
		inputWindow->SetText(TEXT(""));
	}

	SetFocusWindow(inputWindow);

	unguard;
}

void XComputerWindow::WaitForKey(INT QueuedIndex)
{
	guard(XComputerWindow::WaitForKey);

	CompWinEventStruct& Event = queuedBuffer(QueuedIndex);

	inputKey = Event.inputKey;
	ReconFreeEventString(Event.inputKey);

	bWaitingForKey = TRUE;
	bEchoKey = Event.bEcho;

	unguard;
}

UBOOL XComputerWindow::EditActivated(XWindow* Edit, UBOOL bModified)
{
	guard(XComputerWindow::EditActivated);

	XReconComputerEditActivatedParms Parms;

	Parms.Edit = Edit;
	Parms.bModified = bModified;
	Parms.bHandled = FALSE;
	ProcessScript(EXTENSION_EditActivated, &Parms, FALSE);

	FString InputValue;
	if (inputWindow != NULL)
		InputValue = inputWindow->GetText();

	FLOAT SavedEventTimeInterval = eventTimeInterval;
	USound* SavedTextSound = textSound;
	eventTimeInterval = 0.0f;
	textSound = NULL;

	const TCHAR* Text = ReconSafeText(InputValue);
	for (const TCHAR* Pos = Text; *Pos != 0; Pos++)
		AddCharacterEvent(*Pos, -1, -1, TRUE);

	eventTimeInterval = SavedEventTimeInterval;
	textSound = SavedTextSound;

	if (inputWindow != NULL)
		inputWindow->SetVisibility(FALSE);

	InvokeComputerInputFinished(inputKey, InputValue);
	return TRUE;

	unguard;
}

UBOOL XComputerWindow::VirtualKeyPressed(EInputKey Key, UBOOL bRepeat)
{
	guard(XComputerWindow::VirtualKeyPressed);

	if (bWaitingForKey == FALSE || Key == IK_Escape)
	{
		bWaitingForKey = FALSE;

		XReconComputerVirtualKeyPressedParms Parms;

		Parms.Key = Key;
		Parms.bRepeat = bRepeat;
		Parms.bHandled = FALSE;
		ProcessScript(EXTENSION_VirtualKeyPressed, &Parms, FALSE);
		return Parms.bHandled;
	}

	FString KeyString = ReconSingleKeyString(Key);

	if (bEchoKey == TRUE)
	{
		FLOAT SavedEventTimeInterval = eventTimeInterval;
		USound* SavedTextSound = textSound;
		eventTimeInterval = 0.0f;
		textSound = NULL;

		const TCHAR* Text = ReconSafeText(KeyString);
		for (const TCHAR* Pos = Text; *Pos != 0; Pos++)
			AddCharacterEvent(*Pos, -1, -1, TRUE);

		eventTimeInterval = SavedEventTimeInterval;
		textSound = SavedTextSound;
		bEchoKey = FALSE;
	}

	if (typingSound != NULL)
	{
		FLOAT SoundX = (FLOAT)(fontWidth * textX) + ((FLOAT)fontWidth * 0.5f);
		FLOAT SoundY = (FLOAT)(fontHeight * textY) + ((FLOAT)fontHeight * 0.5f);
		PlaySound(typingSound, computerSoundVolume, 1.0f, SoundX, SoundY);
	}

	InvokeComputerInputFinished(inputKey, KeyString);
	return TRUE;

	unguard;
}

/*-----------------------------------------------------------------------------
	Script event dispatch.
-----------------------------------------------------------------------------*/

void XComputerWindow::InvokeComputerStart(void)
{
	guard(XComputerWindow::InvokeComputerStart);

	bComputerStartInvoked = TRUE;

	XReconComputerStartParms Parms;

	for (XWindow* Window = this; Window != NULL; Window = Window->parentOwner)
	{
		UFunction* Function = Window->FindFunction(FName(TEXT("ComputerStart"), FNAME_Add));
		if (Function != NULL)
		{
			Parms.bHandled = FALSE;
			Window->ProcessEvent(Function, &Parms, NULL);
			if (Parms.bHandled == TRUE)
				break;
		}
	}

	unguard;
}

void XComputerWindow::InvokeComputerInputFinished(FString& NewInputKey, FString& InputValue)
{
	guard(XComputerWindow::InvokeComputerInputFinished);

	bPauseProcessing = TRUE;

	XReconComputerInputFinishedParms Parms;

	Parms.InputKey = NewInputKey;
	Parms.InputValue = InputValue;

	for (XWindow* Window = this; Window != NULL; Window = Window->parentOwner)
	{
		UFunction* Function = Window->FindFunction(FName(TEXT("ComputerInputFinished"), FNAME_Add));
		if (Function != NULL)
		{
			Parms.bHandled = FALSE;
			Window->ProcessEvent(Function, &Parms, NULL);
			if (Parms.bHandled == TRUE)
				break;
		}
	}

	timeNextEvent = timeCurrent;
	bPauseProcessing = FALSE;

	unguard;
}

void XComputerWindow::InvokeComputerFadeOutCompleted(void)
{
	guard(XComputerWindow::InvokeComputerFadeOutCompleted);

	for (XWindow* Window = this; Window != NULL; Window = Window->parentOwner)
	{
		UFunction* Function = Window->FindFunction(FName(TEXT("ComputerFadeOutCompleted"), FNAME_Add));
		if (Function != NULL)
			Window->ProcessEvent(Function, NULL, NULL);
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Tick / draw.
-----------------------------------------------------------------------------*/

void XComputerWindow::VisibilityChanged(UBOOL bNewVisibility)
{
	guard(XComputerWindow::VisibilityChanged);

	XWindow::ProcessScript(EXTENSION_VisibilityChanged, &bNewVisibility, FALSE);

	if (bNewVisibility == TRUE)
	{
		if (bComputerStartInvoked == FALSE)
			bInvokeComputerStart = TRUE;
	}

	unguard;
}

void XComputerWindow::Tick(FLOAT DeltaSeconds)
{
	guard(XComputerWindow::Tick);

	if (bIgnoreTick == TRUE)
		return;

	if (bFirstTick == TRUE)
	{
		bFirstTick = FALSE;
		return;
	}

	if (bIgnoreGamePaused == FALSE)
	{
		UBOOL bLevelPaused = FALSE;

		if (XReconComputerIsLevelPaused(player) == TRUE)
			bLevelPaused = TRUE;

		if (bLevelPaused == TRUE)
		{
			if (bGamePaused == FALSE)
			{
				bPauseProcessing = TRUE;
				bGamePaused = TRUE;
				return;
			}
		}
		else
		{
			if (bGamePaused == TRUE)
			{
				bPauseProcessing = FALSE;
				bGamePaused = FALSE;
			}
		}
	}

	if (bInvokeComputerStart == TRUE)
	{
		bInvokeComputerStart = FALSE;
		bComputerStartInvoked = TRUE;
		InvokeComputerStart();
	}

	if (bGamePaused == TRUE)
		return;

	ProcessCursorBlink(DeltaSeconds);

	if (bPauseProcessing == FALSE)
	{
		timeCurrent += DeltaSeconds * throttle;

		if (timeCurrent - timeLastEvent > eventTimeInterval)
			timeNextEvent = timeCurrent + eventTimeInterval;

		ProcessQueuedBuffer();

		if (fadeOutTimer > 0.0f)
		{
			fadeOutTimer -= DeltaSeconds;
			if (fadeOutTimer <= 0.0f)
			{
				fadeOutTimer = 0.0f;
				InvokeComputerFadeOutCompleted();
			}
		}
	}

	ProcessDisplayBuffer(DeltaSeconds);

	unguard;
}

void XComputerWindow::ProcessCursorBlink(FLOAT DeltaSeconds)
{
	guard(XComputerWindow::ProcessCursorBlink);

	if (cursorBlinkSpeed > 0.0f)
	{
		cursorNextEvent -= DeltaSeconds;
		if (cursorNextEvent <= 0.0f)
		{
			cursorNextEvent += cursorBlinkSpeed;
			if (bCursorVisible == TRUE)
				bCursorVisible = FALSE;
			else
				bCursorVisible = TRUE;
		}
	}

	unguard;
}

void XComputerWindow::ProcessDisplayBuffer(FLOAT DeltaSeconds)
{
	guard(XComputerWindow::ProcessDisplayBuffer);

	if (fadeSpeed > 0.0f)
	{
		for (INT Index = 0; Index < displayBuffer.Num(); Index++)
		{
			displayBuffer(Index).eventTime -= DeltaSeconds;
			if (displayBuffer(Index).eventTime < 0.0f)
				displayBuffer(Index).eventTime = 0.0f;
		}
	}

	unguard;
}

void XComputerWindow::Draw(XGC* GC)
{
	guard(XComputerWindow::Draw);

	XWindow::Draw(GC);

	GC->SetStyle(STY_Translucent);

	if (backgroundTextures[0] != NULL)
	{
		GC->DrawIcon(0.0f, 0.0f, backgroundTextures[0]);
		GC->DrawIcon(256.0f, 0.0f, backgroundTextures[1]);
		GC->DrawIcon(512.0f, 0.0f, backgroundTextures[2]);
		GC->DrawIcon(0.0f, 256.0f, backgroundTextures[3]);
		GC->DrawIcon(256.0f, 256.0f, backgroundTextures[4]);
		GC->DrawIcon(512.0f, 256.0f, backgroundTextures[5]);
	}

	DrawDisplayBuffer(GC);

	unguard;
}

void XComputerWindow::DrawDisplayBuffer(XGC* GC)
{
	guard(XComputerWindow::DrawDisplayBuffer);

	TCHAR CharBuffer[2];
	CharBuffer[1] = 0;

	GC->SetFonts(textFont, textFont);

	for (INT Index = 0; Index < displayBuffer.Num(); Index++)
	{
		CompWinEventStruct& Event = displayBuffer(Index);

		if (Event.eventType == ET_Graphic)
		{
			GC->SetTileColor(colGraphicTile);

			if (Event.bPixelPos == TRUE)
			{
				GC->DrawIcon((FLOAT)Event.graphicX, (FLOAT)Event.graphicY, Event.graphic);
			}
			else
			{
				GC->DrawIcon((FLOAT)(fontWidth * Event.graphicX), (FLOAT)(fontHeight * Event.graphicY), Event.graphic);
			}
			continue;
		}

		if (Event.eventType != ET_Text)
			continue;

		CharBuffer[0] = Event.character;

		if (fadeOutTimer > 0.0f)
		{
			FLOAT Scale = fadeOutTimer / fadeOutStart;
			FColor DrawColor = Event.charColor;
			DrawColor.R = (BYTE)((FLOAT)DrawColor.R * Scale);
			DrawColor.G = (BYTE)((FLOAT)DrawColor.G * Scale);
			DrawColor.B = (BYTE)((FLOAT)DrawColor.B * Scale);
			GC->SetTextColor(DrawColor);
		}
		else if (fadeSpeed > 0.0f && Event.eventTime > 0.0f)
		{
			FLOAT Scale = (fadeSpeed - Event.eventTime) / fadeSpeed;
			FColor DrawColor = Event.charColor;
			DrawColor.R = (BYTE)((FLOAT)DrawColor.R * Scale);
			DrawColor.G = (BYTE)((FLOAT)DrawColor.G * Scale);
			DrawColor.B = (BYTE)((FLOAT)DrawColor.B * Scale);
			GC->SetTextColor(DrawColor);
		}
		else
		{
			GC->SetTextColor(Event.charColor);
		}

		GC->DrawText(
			(FLOAT)(fontWidth * Event.charX),
			(FLOAT)(fontHeight * Event.charY),
			(FLOAT)fontWidth,
			(FLOAT)fontHeight,
			CharBuffer);
	}

	if (cursorTexture != NULL && inputWindow != NULL)
	{
		if (inputWindow->IsVisible() == FALSE && bCursorVisible == TRUE && bShowCursor == TRUE)
		{
			GC->SetStyle(STY_Masked);
			GC->SetTileColor(cursorColor);
			GC->DrawPattern(
				(FLOAT)(fontWidth * textX),
				(FLOAT)(fontHeight * (textY + 1) - cursorHeight),
				(FLOAT)cursorWidth,
				(FLOAT)cursorHeight,
				0.0f,
				0.0f,
				cursorTexture);
		}
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Sounds.
-----------------------------------------------------------------------------*/

void XComputerWindow::PlaySoundNow(USound* Sound)
{
	guard(XComputerWindow::PlaySoundNow);

	FLOAT SoundX = (FLOAT)(fontWidth * textX) + ((FLOAT)fontWidth * 0.5f);
	FLOAT SoundY = (FLOAT)(fontHeight * textY) + ((FLOAT)fontHeight * 0.5f);
	PlaySound(Sound, computerSoundVolume, 1.0f, SoundX, SoundY);

	unguard;
}

/*-----------------------------------------------------------------------------
	Native wrappers.

	The original DLL has one thin exec wrapper for every native id 1970..2002.
	Those wrappers only read stack parameters and call the methods above, except
	execPrint(), which implements bNewLine after character enqueueing. The full
	wrapper table is kept for the later compile/pass, because this pass is about
	behavioural recovery rather than final UCC ABI code.
-----------------------------------------------------------------------------*/
