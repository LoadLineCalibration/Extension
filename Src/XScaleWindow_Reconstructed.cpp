/*=============================================================================
	XScaleWindow_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 05.

	Focus: scale/scrollbar mechanics: tick/value conversion, span thumbs,
	texture geometry, dragging, page-repeat behavior and parent notifications.

	This is reconstruction-first source. It preserves observed behavior and names
	the decompiled fields through the original headers. A later compile pass should
	fold helpers and adjust any private declarations required by VC98.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XScaleWindow);

static FLOAT ReconClampNonNegative(FLOAT Value)
{
	if (Value < 0.0f)
		return 0.0f;

	return Value;
}

static INT ReconRoundToInt(FLOAT Value)
{
	if (Value >= 0.0f)
		return appFloor(Value + 0.5f);

	return appCeil(Value - 0.5f);
}

XScaleWindow::XScaleWindow(XWindow* Parent)
	: XWindow(Parent)
{
}

void XScaleWindow::Init(XWindow* Parent)
{
	guard(XScaleWindow::Init);

	XWindow::Init(Parent);

	orientation = ORIENT_Horizontal;

	scaleTexture = NULL;
	thumbTexture = NULL;
	tickTexture = NULL;
	preCapTexture = NULL;
	postCapTexture = NULL;

	bRepeatScaleTexture = FALSE;
	bRepeatThumbTexture = FALSE;
	bDrawEndTicks = TRUE;
	bStretchScale = FALSE;
	bSpanThumb = FALSE;

	borderPattern = NULL;

	scaleBorderSize = 0.0f;
	thumbBorderSize = 0.0f;
	scaleBorderColor = FColor(255, 255, 255);
	thumbBorderColor = FColor(255, 255, 255);

	/*
		The original Extension.dll initializes all ScaleWindow draw styles to
		DSTY_Masked (2).  Tool/menu scrollbars and sliders often do not set
		ScaleStyle explicitly, so STY_Normal/DSTY_Normal makes the black mask
		pixels of the scale textures visible.
	*/
	scaleStyle = DSTY_Masked;
	thumbStyle = DSTY_Masked;
	tickStyle = DSTY_Masked;

	scaleColor = FColor(255, 255, 255);
	thumbColor = FColor(255, 255, 255);
	tickColor = FColor(255, 255, 255);

	scaleWidth = 0.0f;
	scaleHeight = 0.0f;
	thumbWidth = 0.0f;
	thumbHeight = 0.0f;
	tickWidth = 0.0f;
	tickHeight = 0.0f;
	preCapWidth = 0.0f;
	preCapHeight = 0.0f;
	postCapWidth = 0.0f;
	postCapHeight = 0.0f;

	startOffset = 0.0f;
	endOffset = 0.0f;

	marginWidth = 0.0f;
	marginHeight = 0.0f;

	numPositions = 10;
	currentPos = 0;
	spanRange = 1;
	thumbStep = 1;

	fromValue = 0.0f;
	toValue = 1.0f;
	valueFmt = TEXT("%1.2f");

	initialDelay = 0.5f;
	repeatRate = 0.1f;
	initialPos = 0;

	setSound = NULL;
	clickSound = NULL;
	dragSound = NULL;

	scaleX = 0.0f;
	scaleY = 0.0f;
	scaleW = 0.0f;
	scaleH = 0.0f;

	thumbX = 0.0f;
	thumbY = 0.0f;
	thumbW = 0.0f;
	thumbH = 0.0f;

	tickX = 0.0f;
	tickY = 0.0f;
	tickW = 0.0f;
	tickH = 0.0f;

	preCapXOff = 0.0f;
	preCapYOff = 0.0f;
	preCapW = 0.0f;
	preCapH = 0.0f;
	postCapXOff = 0.0f;
	postCapYOff = 0.0f;
	postCapW = 0.0f;
	postCapH = 0.0f;

	absStartScale = 0.0f;
	absEndScale = 0.0f;

	bDraggingThumb = FALSE;
	mousePos = 0.0f;
	repeatDir = REPEATDIR_None;
	remainingTime = 0.0f;

	enumStrings.Empty();

	unguard;
}

void XScaleWindow::CleanUp(void)
{
	guard(XScaleWindow::CleanUp);

	enumStrings.Empty();
	XWindow::CleanUp();

	unguard;
}

void XScaleWindow::Serialize(FArchive& Ar)
{
	guard(XScaleWindow::Serialize);

	XWindow::Serialize(Ar);
	Ar << enumStrings;

	unguard;
}

void XScaleWindow::SetOrientation(EOrientation NewOrientation)
{
	guard(XScaleWindow::SetOrientation);

	orientation = NewOrientation;
	AskParentForReconfigure();

	unguard;
}

void XScaleWindow::SetScaleTexture(UTexture* NewTexture, FLOAT NewWidth, FLOAT NewHeight, FLOAT NewStartOffset, FLOAT NewEndOffset)
{
	guard(XScaleWindow::SetScaleTexture);

	scaleTexture = NewTexture;
	scaleWidth = ReconClampNonNegative(NewWidth);
	scaleHeight = ReconClampNonNegative(NewHeight);
	startOffset = NewStartOffset;
	endOffset = NewEndOffset;
	AskParentForReconfigure();

	unguard;
}

void XScaleWindow::SetThumbTexture(UTexture* NewTexture, FLOAT NewWidth, FLOAT NewHeight)
{
	guard(XScaleWindow::SetThumbTexture);

	thumbTexture = NewTexture;
	thumbWidth = ReconClampNonNegative(NewWidth);
	thumbHeight = ReconClampNonNegative(NewHeight);
	AskParentForReconfigure();

	unguard;
}

void XScaleWindow::SetTickTexture(UTexture* NewTexture, UBOOL bNewDrawEndTicks, FLOAT NewWidth, FLOAT NewHeight)
{
	guard(XScaleWindow::SetTickTexture);

	tickTexture = NewTexture;
	tickWidth = ReconClampNonNegative(NewWidth);
	tickHeight = ReconClampNonNegative(NewHeight);
	bDrawEndTicks = bNewDrawEndTicks;
	AskParentForReconfigure();

	unguard;
}

void XScaleWindow::SetThumbCaps(UTexture* NewPreCapTexture, UTexture* NewPostCapTexture, FLOAT NewPreCapWidth, FLOAT NewPreCapHeight, FLOAT NewPostCapWidth, FLOAT NewPostCapHeight)
{
	guard(XScaleWindow::SetThumbCaps);

	preCapTexture = NewPreCapTexture;
	postCapTexture = NewPostCapTexture;
	preCapWidth = ReconClampNonNegative(NewPreCapWidth);
	preCapHeight = ReconClampNonNegative(NewPreCapHeight);
	postCapWidth = ReconClampNonNegative(NewPostCapWidth);
	postCapHeight = ReconClampNonNegative(NewPostCapHeight);
	AskParentForReconfigure();

	unguard;
}

void XScaleWindow::EnableStretchedScale(UBOOL bNewStretch)
{
	guard(XScaleWindow::EnableStretchedScale);

	if (bStretchScale != bNewStretch)
	{
		bStretchScale = bNewStretch;

		if (bStretchScale == TRUE)
			bRepeatScaleTexture = TRUE;
		else
			bRepeatScaleTexture = FALSE;

		AskParentForReconfigure();
	}

	unguard;
}

void XScaleWindow::SetBorderPattern(UTexture* NewBorder)
{
	guard(XScaleWindow::SetBorderPattern);
	borderPattern = NewBorder;
	unguard;
}

void XScaleWindow::SetScaleBorder(FLOAT NewBorderSize, FColor NewBorderColor)
{
	guard(XScaleWindow::SetScaleBorder);

	scaleBorderColor = NewBorderColor;
	if (scaleBorderSize != NewBorderSize)
	{
		scaleBorderSize = NewBorderSize;
		AskParentForReconfigure();
	}

	unguard;
}

void XScaleWindow::SetThumbBorder(FLOAT NewBorderSize, FColor NewBorderColor)
{
	guard(XScaleWindow::SetThumbBorder);

	thumbBorderColor = NewBorderColor;
	if (thumbBorderSize != NewBorderSize)
	{
		thumbBorderSize = NewBorderSize;
		AskParentForReconfigure();
	}

	unguard;
}

void XScaleWindow::SetScaleStyle(BYTE NewStyle)
{
	scaleStyle = NewStyle;
}

void XScaleWindow::SetThumbStyle(BYTE NewStyle)
{
	thumbStyle = NewStyle;
}

void XScaleWindow::SetTickStyle(BYTE NewStyle)
{
	tickStyle = NewStyle;
}

void XScaleWindow::SetScaleColor(FColor NewColor)
{
	scaleColor = NewColor;
}

void XScaleWindow::SetThumbColor(FColor NewColor)
{
	thumbColor = NewColor;
}

void XScaleWindow::SetTickColor(FColor NewColor)
{
	tickColor = NewColor;
}

void XScaleWindow::SetScaleMargins(FLOAT NewMarginWidth, FLOAT NewMarginHeight)
{
	guard(XScaleWindow::SetScaleMargins);

	NewMarginWidth = ReconClampNonNegative(NewMarginWidth);
	NewMarginHeight = ReconClampNonNegative(NewMarginHeight);

	if (marginWidth != NewMarginWidth || marginHeight != NewMarginHeight)
	{
		marginWidth = NewMarginWidth;
		marginHeight = NewMarginHeight;
		AskParentForReconfigure();
	}

	unguard;
}

void XScaleWindow::SetNumTicks(INT NewTickRange)
{
	guard(XScaleWindow::SetNumTicks);

	if (NewTickRange < 1)
		NewTickRange = 1;

	if (numPositions != NewTickRange)
	{
		numPositions = NewTickRange;
		ChangeThumbPosition(currentPos, TRUE, TRUE);
	}

	unguard;
}

void XScaleWindow::SetThumbSpan(INT NewThumbSpan)
{
	guard(XScaleWindow::SetThumbSpan);

	UBOOL bNewSpanThumb = FALSE;
	if (NewThumbSpan >= 1)
		bNewSpanThumb = TRUE;

	if (spanRange != NewThumbSpan || bSpanThumb != bNewSpanThumb)
	{
		bSpanThumb = bNewSpanThumb;
		if (bSpanThumb == TRUE)
		{
			bRepeatThumbTexture = TRUE;
			spanRange = NewThumbSpan;
		}
		else
		{
			bRepeatThumbTexture = FALSE;
		}

		ChangeThumbPosition(currentPos, TRUE, TRUE);
	}

	unguard;
}

INT XScaleWindow::GetThumbSpan(void)
{
	if (bSpanThumb == TRUE)
		return spanRange;

	return 0;
}

void XScaleWindow::SetRanges(INT NewThumbSpan, INT NewNumTicks)
{
	guard(XScaleWindow::SetRanges);

	if (NewThumbSpan < 1)
		NewThumbSpan = 1;

	if (spanRange != NewThumbSpan || numPositions != NewNumTicks)
	{
		spanRange = NewThumbSpan;
		numPositions = NewNumTicks;
		ChangeThumbPosition(currentPos, TRUE, TRUE);
	}

	unguard;
}

void XScaleWindow::SetTickPosition(INT NewTickPosition)
{
	guard(XScaleWindow::SetTickPosition);
	ChangeThumbPosition(NewTickPosition, FALSE, TRUE);
	unguard;
}

void XScaleWindow::SetValueRange(FLOAT NewFromValue, FLOAT NewToValue)
{
	guard(XScaleWindow::SetValueRange);

	if (fromValue != NewFromValue || toValue != NewToValue)
	{
		fromValue = NewFromValue;
		toValue = NewToValue;
		ChangeScalePosition(TRUE);
	}

	unguard;
}

void XScaleWindow::SetValue(FLOAT NewValue)
{
	guard(XScaleWindow::SetValue);
	ChangeThumbPosition(ValueToTick(NewValue), FALSE, TRUE);
	unguard;
}

FLOAT XScaleWindow::GetValue(void)
{
	guard(XScaleWindow::GetValue);
	return TickToValue(currentPos);
	unguard;
}

void XScaleWindow::GetValues(FLOAT* pFromValue, FLOAT* pToValue)
{
	guard(XScaleWindow::GetValues);

	FLOAT StartValue = TickToValue(currentPos);
	FLOAT EndValue = fromValue;

	if (bSpanThumb == TRUE)
		EndValue = TickToValue(currentPos + spanRange);

	if (pFromValue != NULL)
		*pFromValue = StartValue;

	if (pToValue != NULL)
		*pToValue = EndValue;

	unguard;
}

void XScaleWindow::SetValueFormat(const TCHAR* NewFmt)
{
	guard(XScaleWindow::SetValueFormat);

	if (NewFmt != NULL)
		valueFmt = NewFmt;
	else
		valueFmt = TEXT("");

	ChangeScalePosition(TRUE);

	unguard;
}

TCHAR* XScaleWindow::GetValueString(void)
{
	guard(XScaleWindow::GetValueString);

	static TCHAR StaticBuffer[255];

	if (currentPos >= 0 && currentPos < enumStrings.Num())
	{
		if (enumStrings(currentPos).Len() > 0)
		{
			appStrcpy(StaticBuffer, *enumStrings(currentPos));
			return StaticBuffer;
		}
	}

	const TCHAR* Fmt = TEXT("");
	if (valueFmt.Len() > 0)
		Fmt = *valueFmt;

	appSprintf(StaticBuffer, Fmt, GetValue());
	return StaticBuffer;

	unguard;
}

void XScaleWindow::SetEnumeration(INT TickPos, const TCHAR* NewStr)
{
	guard(XScaleWindow::SetEnumeration);

	if (TickPos >= 0 && TickPos <= 511)
	{
		while (enumStrings.Num() <= TickPos)
			new(enumStrings) FString(TEXT(""));

		if (NewStr != NULL)
			enumStrings(TickPos) = NewStr;
		else
			enumStrings(TickPos) = TEXT("");

		if (TickPos == currentPos)
			ChangeScalePosition(TRUE);
	}

	unguard;
}

void XScaleWindow::ClearAllEnumerations(void)
{
	guard(XScaleWindow::ClearAllEnumerations);
	enumStrings.Empty();
	ChangeScalePosition(TRUE);
	unguard;
}

void XScaleWindow::MoveThumb(EMoveThumb MoveThumb)
{
	guard(XScaleWindow::MoveThumb);

	INT PageAmount = 4;
	if (bSpanThumb == TRUE)
		PageAmount = spanRange;

	INT NewPos = 0;
	switch (MoveThumb)
	{
		case MOVETHUMB_End:
			NewPos = numPositions;
			break;

		case MOVETHUMB_Prev:
			NewPos = currentPos - 1;
			break;

		case MOVETHUMB_Next:
			NewPos = currentPos + 1;
			break;

		case MOVETHUMB_StepUp:
			NewPos = currentPos - thumbStep;
			break;

		case MOVETHUMB_StepDown:
			NewPos = currentPos + thumbStep;
			break;

		case MOVETHUMB_PageUp:
			NewPos = currentPos - PageAmount;
			break;

		case MOVETHUMB_PageDown:
			NewPos = currentPos + PageAmount;
			break;

		case MOVETHUMB_Home:
		default:
			NewPos = 0;
			break;
	}

	ChangeThumbPosition(NewPos, FALSE, TRUE);

	unguard;
}

void XScaleWindow::SetThumbStep(INT NewStep)
{
	if (NewStep < 1)
		NewStep = 1;

	thumbStep = NewStep;
}

void XScaleWindow::SetScaleSounds(USound* NewSetSound, USound* NewClickSound, USound* NewDragSound)
{
	setSound = NewSetSound;
	clickSound = NewClickSound;
	dragSound = NewDragSound;
}

void XScaleWindow::PlayScaleSound(USound* Sound, FLOAT Volume, FLOAT Pitch)
{
	guard(XScaleWindow::PlayScaleSound);

	FLOAT SoundX = thumbX + thumbW * 0.5f;
	FLOAT SoundY = thumbY + thumbH * 0.5f;
	PlaySound(Sound, Volume, Pitch, SoundX, SoundY);

	unguard;
}

FLOAT XScaleWindow::TickToPixel(INT Tick)
{
	guard(XScaleWindow::TickToPixel);

	INT Denominator;
	FLOAT UsablePixels;

	if (bSpanThumb == TRUE)
	{
		Denominator = numPositions - spanRange;
		UsablePixels = absEndScale - absStartScale + 1.0f;
		if (orientation == ORIENT_Vertical)
			UsablePixels -= thumbH;
		else
			UsablePixels -= thumbW;
	}
	else
	{
		Denominator = numPositions - 1;
		UsablePixels = absEndScale - absStartScale;
	}

	if (Denominator < 1)
		Denominator = 1;

	return FLOAT(ReconRoundToInt((FLOAT(Tick) * UsablePixels / FLOAT(Denominator)) + absStartScale));

	unguard;
}

INT XScaleWindow::PixelToTick(FLOAT Pixel)
{
	guard(XScaleWindow::PixelToTick);

	INT Denominator;
	FLOAT UsablePixels;

	if (bSpanThumb == TRUE)
	{
		Denominator = numPositions - spanRange;
		UsablePixels = absEndScale - absStartScale + 1.0f;
		if (orientation == ORIENT_Vertical)
			UsablePixels -= thumbH;
		else
			UsablePixels -= thumbW;
	}
	else
	{
		Denominator = numPositions - 1;
		UsablePixels = absEndScale - absStartScale;
	}

	if (UsablePixels < 1.0f)
		UsablePixels = 1.0f;

	return ReconRoundToInt((Pixel - absStartScale) * FLOAT(Denominator) / UsablePixels);

	unguard;
}

FLOAT XScaleWindow::TickToValue(INT Tick)
{
	guard(XScaleWindow::TickToValue);

	FLOAT Denominator;
	if (bSpanThumb == TRUE)
		Denominator = FLOAT(numPositions);
	else
		Denominator = FLOAT(numPositions - 1);

	if (Denominator <= 0.0f)
		return (toValue - fromValue) * 0.5f + fromValue;

	return (toValue - fromValue) * FLOAT(Tick) / Denominator + fromValue;

	unguard;
}

INT XScaleWindow::ValueToTick(FLOAT Value)
{
	guard(XScaleWindow::ValueToTick);

	FLOAT Denominator;
	if (bSpanThumb == TRUE)
		Denominator = FLOAT(numPositions);
	else
		Denominator = FLOAT(numPositions - 1);

	FLOAT TickValue = 0.0f;
	if (toValue != fromValue)
		TickValue = Denominator * (Value - fromValue) / (toValue - fromValue);

	return ReconRoundToInt(TickValue);

	unguard;
}

void XScaleWindow::ComputeTextureSize(UTexture* Texture, FLOAT CodedWidth, FLOAT CodedHeight, FLOAT BorderSize, FLOAT& NewWidth, FLOAT& NewHeight)
{
	guard(XScaleWindow::ComputeTextureSize);

	if (CodedWidth <= 0.0f)
	{
		if (Texture != NULL)
			NewWidth = FLOAT(Texture->USize);
		else
			NewWidth = 0.0f;
	}
	else
	{
		NewWidth = CodedWidth;
	}

	if (CodedHeight <= 0.0f)
	{
		if (Texture != NULL)
			NewHeight = FLOAT(Texture->VSize);
		else
			NewHeight = 0.0f;
	}
	else
	{
		NewHeight = CodedHeight;
	}

	FLOAT Extra = BorderSize + BorderSize;
	NewWidth += Extra;
	NewHeight += Extra;

	unguard;
}

void XScaleWindow::ComputeThumbConfig(void)
{
	guard(XScaleWindow::ComputeThumbConfig);

	ComputeTextureSize(thumbTexture, thumbWidth, thumbHeight, thumbBorderSize, thumbW, thumbH);

	preCapXOff = 0.0f;
	preCapYOff = 0.0f;
	postCapXOff = 0.0f;
	postCapYOff = 0.0f;

	ComputeTextureSize(preCapTexture, preCapWidth, preCapHeight, 0.0f, preCapW, preCapH);
	ComputeTextureSize(postCapTexture, postCapWidth, postCapHeight, 0.0f, postCapW, postCapH);

	if (orientation == ORIENT_Vertical)
	{
		thumbH += preCapH + postCapH;
		preCapW = thumbW - thumbBorderSize - thumbBorderSize;
		postCapW = preCapW;
	}
	else
	{
		thumbW += preCapW + postCapW;
		preCapH = thumbH - thumbBorderSize - thumbBorderSize;
		postCapH = preCapH;
	}

	thumbX = FLOAT(appFloor((width - thumbW) * 0.5f));
	thumbY = FLOAT(appFloor((height - thumbH) * 0.5f));

	if (bSpanThumb == TRUE)
	{
		INT SafeNumPositions = numPositions;
		if (SafeNumPositions < 1)
			SafeNumPositions = 1;

		FLOAT SpanPixels = (absEndScale + 1.0f - absStartScale) * FLOAT(spanRange) / FLOAT(SafeNumPositions) + 0.5f;

		if (orientation == ORIENT_Vertical)
		{
			thumbH = FLOAT(appFloor(SpanPixels));
			if (thumbH < 6.0f)
				thumbH = 6.0f;

			FLOAT ScalePixels = absEndScale - absStartScale + 1.0f;
			if (ScalePixels < thumbH)
				thumbH = ScalePixels;

			FLOAT CapExcess = thumbBorderSize + thumbBorderSize + preCapH + postCapH - thumbH;
			if (CapExcess > 0.0f)
			{
				FLOAT PostTrim = FLOAT(appFloor(CapExcess * 0.5f));
				preCapH -= CapExcess - PostTrim;
				postCapH -= PostTrim;
				postCapYOff += PostTrim;
			}

			thumbY = TickToPixel(currentPos);
		}
		else
		{
			thumbW = FLOAT(appFloor(SpanPixels));
			if (thumbW < 6.0f)
				thumbW = 6.0f;

			FLOAT ScalePixels = absEndScale - absStartScale + 1.0f;
			if (ScalePixels < thumbW)
				thumbW = ScalePixels;

			FLOAT CapExcess = thumbBorderSize + thumbBorderSize + preCapW + postCapW - thumbW;
			if (CapExcess > 0.0f)
			{
				FLOAT PostTrim = FLOAT(appFloor(CapExcess * 0.5f));
				preCapW -= CapExcess - PostTrim;
				postCapW -= PostTrim;
				postCapXOff += PostTrim;
			}

			thumbX = TickToPixel(currentPos);
		}
	}
	else
	{
		if (orientation == ORIENT_Vertical)
		{
			FLOAT Pixel = TickToPixel(currentPos);
			thumbY = FLOAT(appFloor(Pixel - thumbH * 0.5f + 0.5f));
		}
		else
		{
			FLOAT Pixel = TickToPixel(currentPos);
			thumbX = FLOAT(appFloor(Pixel - thumbW * 0.5f + 0.5f));
		}
	}

	unguard;
}

void XScaleWindow::ChangeThumbPosition(INT NewPos, UBOOL bForceAttEvent, UBOOL bFinal)
{
	guard(XScaleWindow::ChangeThumbPosition);

	INT ClampedPos = NewPos;
	if (bSpanThumb == TRUE)
	{
		INT MaxPos = numPositions - spanRange;
		if (ClampedPos > MaxPos)
			ClampedPos = MaxPos;
	}
	else
	{
		if (ClampedPos >= numPositions)
			ClampedPos = numPositions - 1;
	}

	if (ClampedPos < 0)
		ClampedPos = 0;

	UBOOL bAttributesChanged = bForceAttEvent;
	if (currentPos != ClampedPos)
	{
		bAttributesChanged = TRUE;
		currentPos = ClampedPos;
		ComputeThumbConfig();
		ChangeScalePosition(bFinal);
	}

	if (bAttributesChanged == TRUE)
	{
		ChangeScaleAttributes();
		ComputeThumbConfig();
	}

	unguard;
}

void XScaleWindow::DrawScaleTexture(XGC* GC, UTexture* Texture, FLOAT tX, FLOAT tY, FLOAT tW, FLOAT tH, UBOOL bRepeat, BYTE Style, FColor Color, FLOAT BorderSize, FColor BorderColor, UTexture* PreCap, UTexture* PostCap, FLOAT PreXOff, FLOAT PreYOff, FLOAT PreW, FLOAT PreH, FLOAT PostXOff, FLOAT PostYOff, FLOAT PostW, FLOAT PostH)
{
	guard(XScaleWindow::DrawScaleTexture);

	BYTE OldStyle = GC->GetStyle();
	FColor OldColor = GC->GetTileColor();

	GC->SetStyle(Style);

	if (BorderSize > 0.0f)
	{
		if (borderPattern != NULL)
		{
			GC->SetTileColor(BorderColor);
			GC->DrawBox(tX, tY, tW, tH, 0.0f, 0.0f, BorderSize, borderPattern);
		}

		tX += BorderSize;
		tY += BorderSize;
		tW -= BorderSize + BorderSize;
		tH -= BorderSize + BorderSize;
	}

	GC->SetTileColor(Color);

	if (PreCap != NULL && PreW > 0.0f && PreH > 0.0f && tW > 0.0f && tH > 0.0f)
	{
		if (orientation == ORIENT_Vertical)
		{
			GC->DrawIconPattern(tX, tY, PreW, PreH, PreXOff, PreYOff, 0.0f, PreH, PreCap);
			tY += PreH;
			tH -= PreH;
		}
		else
		{
			GC->DrawIconPattern(tX, tY, PreW, PreH, PreXOff, PreYOff, PreW, 0.0f, PreCap);
			tX += PreW;
			tW -= PreW;
		}
	}

	if (PostCap != NULL && PostW > 0.0f && PostH > 0.0f && tW > 0.0f && tH > 0.0f)
	{
		if (orientation == ORIENT_Vertical)
		{
			FLOAT PostY = tY + tH - PostH;
			GC->DrawIconPattern(tX, PostY, PostW, PostH, PostXOff, PostYOff, 0.0f, PostH, PostCap);
			tH -= PostH;
		}
		else
		{
			FLOAT PostX = tX + tW - PostW;
			GC->DrawIconPattern(PostX, tY, PostW, PostH, PostXOff, PostYOff, PostW, 0.0f, PostCap);
			tW -= PostW;
		}
	}

	if (Texture != NULL && tW > 0.0f && tH > 0.0f)
	{
		FLOAT RepeatW = 0.0f;
		FLOAT RepeatH = 0.0f;

		if (orientation == ORIENT_Vertical)
		{
			if (bRepeat == FALSE)
				RepeatH = tH;
		}
		else
		{
			if (bRepeat == FALSE)
				RepeatW = tW;
		}

		GC->DrawIconPattern(tX, tY, tW, tH, 0.0f, 0.0f, RepeatW, RepeatH, Texture);
	}

	GC->SetStyle(OldStyle);
	GC->SetTileColor(OldColor);

	unguard;
}

void XScaleWindow::ChangeScalePosition(UBOOL bFinal)
{
	guard(XScaleWindow::ChangeScalePosition);

	if (bSpanThumb == TRUE)
	{
		INT FromTick = currentPos;
		INT ToTick = currentPos + spanRange;
		FLOAT From = TickToValue(FromTick);
		FLOAT To = TickToValue(ToTick);

		for (XWindow* Win = this; Win != NULL; Win = Win->GetParent())
		{
			if (Win->ScaleRangeChanged(this, FromTick, ToTick, From, To, bFinal) == TRUE)
				break;
		}
	}
	else
	{
		FLOAT Value = TickToValue(currentPos);

		for (XWindow* Win = this; Win != NULL; Win = Win->GetParent())
		{
			if (Win->ScalePositionChanged(this, currentPos, Value, bFinal) == TRUE)
				break;
		}
	}

	unguard;
}

void XScaleWindow::ChangeScaleAttributes(void)
{
	guard(XScaleWindow::ChangeScaleAttributes);

	UBOOL bAllVisible = TRUE;
	for (XWindow* Win = this; Win != NULL; Win = Win->GetParent())
	{
		if (Win->bIsVisible == FALSE)
		{
			bAllVisible = FALSE;
			break;
		}
	}

	if (bAllVisible == TRUE)
	{
		for (XWindow* Win = this; Win != NULL; Win = Win->GetParent())
		{
			if (Win->ScaleAttributesChanged(this, currentPos, spanRange, numPositions) == TRUE)
				break;
		}
	}

	unguard;
}

void XScaleWindow::ParentRequestedPreferredSize(UBOOL bWidthSpecified, FLOAT& PreferredWidth, UBOOL bHeightSpecified, FLOAT& PreferredHeight)
{
	guard(XScaleWindow::ParentRequestedPreferredSize);

	PreferredWidth = 0.0f;
	PreferredHeight = 0.0f;

	FLOAT TestW;
	FLOAT TestH;

	ComputeTextureSize(scaleTexture, scaleWidth, scaleHeight, scaleBorderSize, PreferredWidth, PreferredHeight);
	ComputeTextureSize(thumbTexture, thumbWidth, thumbHeight, thumbBorderSize, TestW, TestH);
	if (PreferredWidth < TestW)
		PreferredWidth = TestW;
	if (PreferredHeight < TestH)
		PreferredHeight = TestH;

	ComputeTextureSize(tickTexture, tickWidth, tickHeight, 0.0f, TestW, TestH);
	if (PreferredWidth < TestW)
		PreferredWidth = TestW;
	if (PreferredHeight < TestH)
		PreferredHeight = TestH;

	PreferredWidth += marginWidth + marginWidth;
	PreferredHeight += marginHeight + marginHeight;

	unguard;
}

void XScaleWindow::ConfigurationChanged(void)
{
	guard(XScaleWindow::ConfigurationChanged);

	ComputeTextureSize(scaleTexture, scaleWidth, scaleHeight, scaleBorderSize, scaleW, scaleH);

	if (bStretchScale == TRUE)
	{
		if (orientation == ORIENT_Vertical)
			scaleH = height - (marginHeight + marginHeight);
		else
			scaleW = width - (marginWidth + marginWidth);
	}

	scaleX = FLOAT(appFloor((width - scaleW) * 0.5f));
	scaleY = FLOAT(appFloor((height - scaleH) * 0.5f));

	if (orientation == ORIENT_Vertical)
	{
		absStartScale = scaleY + startOffset;
		absEndScale = scaleY + scaleH;
	}
	else
	{
		absStartScale = scaleX + startOffset;
		absEndScale = scaleX + scaleW;
	}

	absEndScale -= endOffset + 1.0f;
	absStartScale += scaleBorderSize;
	absEndScale -= scaleBorderSize;

	if (absStartScale > absEndScale)
	{
		FLOAT Mid = (absEndScale - absStartScale) * 0.5f + absStartScale;
		absStartScale = Mid;
		absEndScale = Mid;
	}

	ComputeThumbConfig();

	ComputeTextureSize(tickTexture, tickWidth, tickHeight, 0.0f, tickW, tickH);
	tickX = FLOAT(appFloor((width - tickW) * 0.5f));
	tickY = FLOAT(appFloor((height - tickH) * 0.5f));

	unguard;
}

void XScaleWindow::Draw(XGC* GC)
{
	guard(XScaleWindow::Draw);

	DrawScaleTexture(GC, scaleTexture, scaleX, scaleY, scaleW, scaleH, bRepeatScaleTexture, scaleStyle, scaleColor, scaleBorderSize, scaleBorderColor, NULL, NULL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	if (tickTexture != NULL)
	{
		INT FirstTick = 0;
		INT LastTick = numPositions;
		if (bSpanThumb == TRUE)
			LastTick = numPositions + 1;

		if (bDrawEndTicks == FALSE)
		{
			FirstTick = 1;
			LastTick--;
		}

		for (INT Tick = FirstTick; Tick < LastTick; Tick++)
		{
			if (orientation == ORIENT_Vertical)
			{
				FLOAT TickPixel = TickToPixel(Tick);
				FLOAT DrawY = TickPixel - FLOAT(appFloor(tickH * 0.5f));
				DrawScaleTexture(GC, tickTexture, tickX, DrawY, tickW, tickH, FALSE, tickStyle, tickColor, 0.0f, FColor(0,0,0), NULL, NULL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			}
			else
			{
				FLOAT TickPixel = TickToPixel(Tick);
				FLOAT DrawX = TickPixel - FLOAT(appFloor(tickW * 0.5f));
				DrawScaleTexture(GC, tickTexture, DrawX, tickY, tickW, tickH, FALSE, tickStyle, tickColor, 0.0f, FColor(0,0,0), NULL, NULL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			}
		}
	}

	DrawScaleTexture(GC, thumbTexture, thumbX, thumbY, thumbW, thumbH, bRepeatThumbTexture, thumbStyle, thumbColor, thumbBorderSize, thumbBorderColor, preCapTexture, postCapTexture, preCapXOff, preCapYOff, preCapW, preCapH, postCapXOff, postCapYOff, postCapW, postCapH);

	unguard;
}

UBOOL XScaleWindow::MouseButtonPressed(FLOAT PointX, FLOAT PointY, EInputKey Button, INT NumClicks)
{
	guard(XScaleWindow::MouseButtonPressed);

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

	if (Button == IK_LeftMouse)
	{
		if (PointX >= thumbX && PointX < thumbX + thumbW && PointY >= thumbY && PointY < thumbY + thumbH)
		{
			bDraggingThumb = TRUE;
			initialPos = currentPos;
			if (orientation == ORIENT_Vertical)
				mousePos = PointY - thumbY;
			else
				mousePos = PointX - thumbX;

			PlayScaleSound(clickSound, -1.0f, 1.0f);
			return TRUE;
		}

		if (bSpanThumb == FALSE)
		{
			initialPos = currentPos;
			bDraggingThumb = TRUE;

			INT NewPos;
			if (orientation == ORIENT_Vertical)
				NewPos = PixelToTick(PointY);
			else
				NewPos = PixelToTick(PointX);

			ChangeThumbPosition(NewPos, FALSE, FALSE);
			PlayScaleSound(setSound, -1.0f, 1.0f);
			return TRUE;
		}

		if (orientation == ORIENT_Vertical)
		{
			if (PointY < thumbY)
				repeatDir = REPEATDIR_Dec;
			else
				repeatDir = REPEATDIR_Inc;
		}
		else
		{
			if (PointX < thumbX)
				repeatDir = REPEATDIR_Dec;
			else
				repeatDir = REPEATDIR_Inc;
		}

		if (repeatDir == REPEATDIR_Dec)
			MoveThumb(MOVETHUMB_PageUp);
		else
			MoveThumb(MOVETHUMB_PageDown);

		PlayScaleSound(setSound, -1.0f, 1.0f);
		remainingTime = GetTickOffset() + initialDelay;
		if (remainingTime <= 0.0f)
			remainingTime = repeatRate;
		if (remainingTime <= 0.0f)
			repeatDir = REPEATDIR_None;

		return TRUE;
	}

	return Parms.bResult;

	unguard;
}

void XScaleWindow::MouseMoved(FLOAT PointX, FLOAT PointY)
{
	guard(XScaleWindow::MouseMoved);

	struct
	{
		FLOAT pointX;
		FLOAT pointY;
	} Parms;

	Parms.pointX = PointX;
	Parms.pointY = PointY;
	ProcessScript(EXTENSION_MouseMoved, &Parms, FALSE);

	if (bDraggingThumb == TRUE)
	{
		FLOAT Pixel;
		if (bSpanThumb == TRUE)
		{
			if (orientation == ORIENT_Vertical)
				Pixel = PointY - mousePos;
			else
				Pixel = PointX - mousePos;
		}
		else
		{
			if (orientation == ORIENT_Vertical)
				Pixel = PointY;
			else
				Pixel = PointX;
		}

		INT OldPos = currentPos;
		ChangeThumbPosition(PixelToTick(Pixel), FALSE, FALSE);

		if (OldPos != currentPos)
			PlayScaleSound(dragSound, -1.0f, 1.0f);
	}

	unguard;
}

UBOOL XScaleWindow::MouseButtonReleased(FLOAT PointX, FLOAT PointY, EInputKey Button, INT NumClicks)
{
	guard(XScaleWindow::MouseButtonReleased);

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
	ProcessScript(EXTENSION_MouseButtonReleased, &Parms, FALSE);

	if (Button == IK_LeftMouse)
	{
		if (bDraggingThumb == TRUE && currentPos != initialPos)
		{
			ChangeScalePosition(TRUE);
			PlayScaleSound(setSound, -1.0f, 1.0f);
		}

		repeatDir = REPEATDIR_None;
		bDraggingThumb = FALSE;
		return TRUE;
	}

	return Parms.bResult;

	unguard;
}

void XScaleWindow::Tick(FLOAT DeltaSeconds)
{
	guard(XScaleWindow::Tick);

	if (repeatDir != REPEATDIR_None)
	{
		remainingTime -= DeltaSeconds;
		if (remainingTime < 0.0f)
		{
			FLOAT CursorX;
			FLOAT CursorY;
			GetCursorPos(&CursorX, &CursorY);

			if (CursorX < 0.0f || CursorX >= width || CursorY < 0.0f || CursorY >= height)
			{
				remainingTime += repeatRate;
				if (remainingTime < 0.0f)
					remainingTime = 0.05f;
				return;
			}

			INT OldPos = currentPos;

			if (orientation == ORIENT_Vertical)
			{
				if (CursorY < thumbY && repeatDir == REPEATDIR_Dec)
					MoveThumb(MOVETHUMB_PageUp);
				if (CursorY >= thumbY + thumbH && repeatDir == REPEATDIR_Inc)
					MoveThumb(MOVETHUMB_PageDown);
			}
			else
			{
				if (CursorX < thumbX && repeatDir == REPEATDIR_Dec)
					MoveThumb(MOVETHUMB_PageUp);
				if (CursorX >= thumbX + thumbW && repeatDir == REPEATDIR_Inc)
					MoveThumb(MOVETHUMB_PageDown);
			}

			if (OldPos != currentPos)
				PlayScaleSound(setSound, -1.0f, 1.0f);

			remainingTime += repeatRate;
			if (remainingTime < 0.0f)
				remainingTime = 0.05f;
		}
	}

	unguard;
}
