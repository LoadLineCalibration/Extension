/* Auto-generated real native wrappers. No fallback bodies. */
#include "ExtensionPrivate_Reconstructed.h"

static INT ExtNativeTextLength(const TCHAR* S) { return S ? appStrlen(S) : 0; }
static INT ExtNativeGetTextPart(const TCHAR* S, INT StartPos, INT Count, FString* OutText)
{
	INT Len = ExtNativeTextLength(S);
	if (StartPos < 0 || StartPos >= Len || Count <= 0) { *OutText = TEXT(""); return 0; }
	if (Count > Len - StartPos) Count = Len - StartPos;
	if (Count > 240) Count = 240;
	*OutText = FString(S).Mid(StartPos, Count);
	return Count;
}
static INT ExtNativeTextWindowGetTextPart(const TCHAR* S, INT StartPos, INT Count, FString* OutText)
{
	*OutText = FString(S ? S : TEXT("")).Mid(StartPos, Count);
	return OutText->Len();
}
static UBOOL ExtNativeGetNextFlagName(XFlagBase* Base, INT Iterator, FName* OutName)
{
	EFlagType FlagType = MAX_FLAG_TYPE;
	return Base->GetNextFlag(Iterator, OutName, &FlagType);
}

void XFlagBase::execSetBool(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execSetBool);
	P_GET_NAME(flagName);
	P_GET_UBOOL(newValue);
	P_GET_UBOOL_OPTX(bAdd,TRUE);
	P_GET_INT_OPTX(expiration,-1);
	P_FINISH;
	*(UBOOL*)Result = SetBool(flagName, newValue, bAdd, expiration);
	unguardexec;
}

void XFlagBase::execSetByte(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execSetByte);
	P_GET_NAME(flagName);
	P_GET_BYTE(newValue);
	P_GET_UBOOL_OPTX(bAdd,TRUE);
	P_GET_INT_OPTX(expiration,-1);
	P_FINISH;
	*(UBOOL*)Result = SetByte(flagName, newValue, bAdd, expiration);
	unguardexec;
}

void XFlagBase::execSetInt(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execSetInt);
	P_GET_NAME(flagName);
	P_GET_INT(newValue);
	P_GET_UBOOL_OPTX(bAdd,TRUE);
	P_GET_INT_OPTX(expiration,-1);
	P_FINISH;
	*(UBOOL*)Result = SetInt(flagName, newValue, bAdd, expiration);
	unguardexec;
}

void XFlagBase::execSetFloat(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execSetFloat);
	P_GET_NAME(flagName);
	P_GET_FLOAT(newValue);
	P_GET_UBOOL_OPTX(bAdd,TRUE);
	P_GET_INT_OPTX(expiration,-1);
	P_FINISH;
	*(UBOOL*)Result = SetFloat(flagName, newValue, bAdd, expiration);
	unguardexec;
}

void XFlagBase::execSetName(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execSetName);
	P_GET_NAME(flagName);
	P_GET_NAME(newValue);
	P_GET_UBOOL_OPTX(bAdd,TRUE);
	P_GET_INT_OPTX(expiration,-1);
	P_FINISH;
	*(UBOOL*)Result = SetName(flagName, newValue, bAdd, expiration);
	unguardexec;
}

void XFlagBase::execSetVector(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execSetVector);
	P_GET_NAME(flagName);
	P_GET_VECTOR(newValue);
	P_GET_UBOOL_OPTX(bAdd,TRUE);
	P_GET_INT_OPTX(expiration,-1);
	P_FINISH;
	*(UBOOL*)Result = SetVector(flagName, newValue, bAdd, expiration);
	unguardexec;
}

void XFlagBase::execSetRotator(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execSetRotator);
	P_GET_NAME(flagName);
	P_GET_ROTATOR(newValue);
	P_GET_UBOOL_OPTX(bAdd,TRUE);
	P_GET_INT_OPTX(expiration,-1);
	P_FINISH;
	*(UBOOL*)Result = SetRotator(flagName, newValue, bAdd, expiration);
	unguardexec;
}

void XFlagBase::execGetBool(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execGetBool);
	P_GET_NAME(flagName);
	P_FINISH;
	UBOOL value = FALSE;
	GetBool(flagName, value);
	*(UBOOL*)Result = value;
	unguardexec;
}

void XFlagBase::execGetByte(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execGetByte);
	P_GET_NAME(flagName);
	P_FINISH;
	BYTE value = 0;
	GetByte(flagName, value);
	*(BYTE*)Result = value;
	unguardexec;
}

void XFlagBase::execGetInt(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execGetInt);
	P_GET_NAME(flagName);
	P_FINISH;
	INT value = 0;
	GetInt(flagName, value);
	*(INT*)Result = value;
	unguardexec;
}

void XFlagBase::execGetFloat(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execGetFloat);
	P_GET_NAME(flagName);
	P_FINISH;
	FLOAT value = 0.0f;
	GetFloat(flagName, value);
	*(FLOAT*)Result = value;
	unguardexec;
}

void XFlagBase::execGetName(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execGetName);
	P_GET_NAME(flagName);
	P_FINISH;
	FName value = NAME_None;
	GetName(flagName, value);
	*(FName*)Result = value;
	unguardexec;
}

void XFlagBase::execGetVector(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execGetVector);
	P_GET_NAME(flagName);
	P_FINISH;
	FVector value(0,0,0);
	GetVector(flagName, value);
	*(FVector*)Result = value;
	unguardexec;
}

void XFlagBase::execGetRotator(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execGetRotator);
	P_GET_NAME(flagName);
	P_FINISH;
	FRotator value(0,0,0);
	GetRotator(flagName, value);
	*(FRotator*)Result = value;
	unguardexec;
}

void XFlagBase::execCheckFlag(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execCheckFlag);
	P_GET_NAME(flagName);
	P_GET_BYTE(flagType);
	P_FINISH;
	*(UBOOL*)Result = CheckFlag(flagName, flagType);
	unguardexec;
}

void XFlagBase::execDeleteFlag(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execDeleteFlag);
	P_GET_NAME(flagName);
	P_GET_BYTE(flagType);
	P_FINISH;
	*(UBOOL*)Result = DeleteFlag(flagName, flagType);
	unguardexec;
}

void XFlagBase::execSetExpiration(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execSetExpiration);
	P_GET_NAME(flagName);
	P_GET_BYTE(flagType);
	P_GET_INT(expiration);
	P_FINISH;
	SetExpiration(flagName, flagType, expiration);
	unguardexec;
}

void XFlagBase::execGetExpiration(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execGetExpiration);
	P_GET_NAME(flagName);
	P_GET_BYTE(flagType);
	P_FINISH;
	*(INT*)Result = GetExpiration(flagName, flagType);
	unguardexec;
}

void XFlagBase::execDeleteExpiredFlags(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execDeleteExpiredFlags);
	P_GET_INT(criteria);
	P_FINISH;
	DeleteExpiredFlags(criteria);
	unguardexec;
}

void XFlagBase::execSetDefaultExpiration(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execSetDefaultExpiration);
	P_GET_INT(expiration);
	P_FINISH;
	SetDefaultExpiration(expiration);
	unguardexec;
}

void XFlagBase::execCreateIterator(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execCreateIterator);
	P_GET_BYTE_OPTX(flagType,MAX_FLAG_TYPE);
	P_FINISH;
	*(INT*)Result = CreateIterator((EFlagType)flagType);
	unguardexec;
}

void XFlagBase::execGetNextFlagName(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execGetNextFlagName);
	P_GET_INT(iterator);
	P_GET_NAME_REF(flagName);
	P_FINISH;
	*(UBOOL*)Result = ExtNativeGetNextFlagName(this, iterator, flagName);
	unguardexec;
}

void XFlagBase::execGetNextFlag(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execGetNextFlag);
	P_GET_INT(iterator);
	P_GET_NAME_REF(flagName);
	P_GET_BYTE_REF(flagType);
	P_FINISH;
	EFlagType localFlagType = (EFlagType)*flagType;
	UBOOL bOk = GetNextFlag(iterator, flagName, &localFlagType);
	*flagType = (BYTE)localFlagType;
	*(UBOOL*)Result = bOk;
	unguardexec;
}

void XFlagBase::execDestroyIterator(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execDestroyIterator);
	P_GET_INT(iterator);
	P_FINISH;
	DestroyIterator(iterator);
	unguardexec;
}

void XFlagBase::execDeleteAllFlags(FFrame& Stack, RESULT_DECL)
{
	guard(XFlagBase::execDeleteAllFlags);
	P_FINISH;
	DeleteAllFlags();
	unguardexec;
}

void XExtString::execSetText(FFrame& Stack, RESULT_DECL)
{
	guard(XExtString::execSetText);
	P_GET_STR(newText);
	P_FINISH;
	SetText(*newText);
	unguardexec;
}

void XExtString::execAppendText(FFrame& Stack, RESULT_DECL)
{
	guard(XExtString::execAppendText);
	P_GET_STR(newText);
	P_FINISH;
	AppendText(*newText);
	unguardexec;
}

void XExtString::execGetText(FFrame& Stack, RESULT_DECL)
{
	guard(XExtString::execGetText);
	P_FINISH;
	*(FString*)Result = GetText();
	unguardexec;
}

void XExtString::execGetTextLength(FFrame& Stack, RESULT_DECL)
{
	guard(XExtString::execGetTextLength);
	P_FINISH;
	*(INT*)Result = ExtNativeTextLength(GetText());
	unguardexec;
}

void XExtString::execGetTextPart(FFrame& Stack, RESULT_DECL)
{
	guard(XExtString::execGetTextPart);
	P_GET_INT(startPos);
	P_GET_INT(count);
	P_GET_STR_REF(outText);
	P_FINISH;
	*(INT*)Result = ExtNativeGetTextPart(GetText(), startPos, count, outText);
	unguardexec;
}

void XExtString::execGetFirstTextPart(FFrame& Stack, RESULT_DECL)
{
	guard(XExtString::execGetFirstTextPart);
	P_GET_STR_REF(outText);
	P_FINISH;

	const TCHAR* SourceText = GetText();
	INT Len = ExtNativeTextLength(SourceText);
	INT Count = Len;
	if (Count > TEXT_PART_SIZE)
		Count = TEXT_PART_SIZE;

	*outText = FString(SourceText ? SourceText : TEXT("")).Left(Count);
	speechPage = 0;
	*(INT*)Result = Count;
	unguardexec;
}

void XExtString::execGetNextTextPart(FFrame& Stack, RESULT_DECL)
{
	guard(XExtString::execGetNextTextPart);
	P_GET_STR_REF(outText);
	P_FINISH;

	const TCHAR* SourceText = GetText();
	INT Len = ExtNativeTextLength(SourceText);
	INT Page = speechPage + 1;
	speechPage = Page;

	INT Count = 0;
	if (Page <= (Len / TEXT_PART_SIZE))
	{
		INT StartPos = TEXT_PART_SIZE * Page;
		Count = Len - StartPos;
		if (Count > TEXT_PART_SIZE)
			Count = TEXT_PART_SIZE;

		*outText = FString(SourceText ? SourceText : TEXT("")).Mid(StartPos, Count);
	}

	*(INT*)Result = Count;
	unguardexec;
}

void XGC::execIntersect(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execIntersect);
	P_GET_FLOAT(clipX);
	P_GET_FLOAT(clipY);
	P_GET_FLOAT(clipWidth);
	P_GET_FLOAT(clipHeight);
	P_FINISH;
	Intersect(clipX, clipY, clipWidth, clipHeight);
	unguardexec;
}

void XGC::execEnableSmoothing(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execEnableSmoothing);
	P_GET_UBOOL(bNewSmoothing);
	P_FINISH;
	EnableSmoothing(bNewSmoothing);
	unguardexec;
}

void XGC::execIsSmoothingEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execIsSmoothingEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsSmoothingEnabled();
	unguardexec;
}

void XGC::execSetStyle(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execSetStyle);
	P_GET_BYTE(newStyle);
	P_FINISH;
	SetStyle(newStyle);
	unguardexec;
}

void XGC::execGetStyle(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execGetStyle);
	P_FINISH;
	*(BYTE*)Result = GetStyle();
	unguardexec;
}

void XGC::execEnableDrawing(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execEnableDrawing);
	P_GET_UBOOL(bDrawEnabled);
	P_FINISH;
	EnableDrawing(bDrawEnabled);
	unguardexec;
}

void XGC::execIsDrawingEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execIsDrawingEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsDrawingEnabled();
	unguardexec;
}

void XGC::execEnableMasking(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execEnableMasking);
	P_GET_UBOOL(bNewMasking);
	P_FINISH;
	EnableMasking(bNewMasking);
	unguardexec;
}

void XGC::execIsMaskingEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execIsMaskingEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsMaskingEnabled();
	unguardexec;
}

void XGC::execEnableTranslucency(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execEnableTranslucency);
	P_GET_UBOOL(bNewTranslucency);
	P_FINISH;
	EnableTranslucency(bNewTranslucency);
	unguardexec;
}

void XGC::execIsTranslucencyEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execIsTranslucencyEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsTranslucencyEnabled();
	unguardexec;
}

void XGC::execEnableModulation(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execEnableModulation);
	P_GET_UBOOL(bNewModulation);
	P_FINISH;
	EnableModulation(bNewModulation);
	unguardexec;
}

void XGC::execIsModulationEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execIsModulationEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsModulationEnabled();
	unguardexec;
}

void XGC::execSetTileColor(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execSetTileColor);
	P_GET_STRUCT(FColor,newTileColor);
	P_FINISH;
	SetTileColor(newTileColor);
	unguardexec;
}

void XGC::execGetTileColor(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execGetTileColor);
	P_GET_STRUCT_REF(FColor,tileColor);
	P_FINISH;
	*tileColor = GetTileColor();
	unguardexec;
}

void XGC::execSetTextColor(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execSetTextColor);
	P_GET_STRUCT(FColor,newTextColor);
	P_FINISH;
	SetTextColor(newTextColor);
	unguardexec;
}

void XGC::execGetTextColor(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execGetTextColor);
	P_GET_STRUCT_REF(FColor,textColor);
	P_FINISH;
	*textColor = GetTextColor();
	unguardexec;
}

void XGC::execSetFont(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execSetFont);
	P_GET_OBJECT(UFont,newFont);
	P_FINISH;
	SetFont(newFont);
	unguardexec;
}

void XGC::execSetNormalFont(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execSetNormalFont);
	P_GET_OBJECT(UFont,newNormalFont);
	P_FINISH;
	SetNormalFont(newNormalFont);
	unguardexec;
}

void XGC::execSetBoldFont(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execSetBoldFont);
	P_GET_OBJECT(UFont,newBoldFont);
	P_FINISH;
	SetBoldFont(newBoldFont);
	unguardexec;
}

void XGC::execSetFonts(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execSetFonts);
	P_GET_OBJECT(UFont,newNormalFont);
	P_GET_OBJECT(UFont,newBoldFont);
	P_FINISH;
	SetFonts(newNormalFont, newBoldFont);
	unguardexec;
}

void XGC::execGetFonts(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execGetFonts);
	P_GET_OBJECT_REF(UFont,normalFont);
	P_GET_OBJECT_REF(UFont,boldFont);
	P_FINISH;
	GetFonts(normalFont, boldFont);
	unguardexec;
}

void XGC::execEnableTranslucentText(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execEnableTranslucentText);
	P_GET_UBOOL(bNewTranslucency);
	P_FINISH;
	EnableTranslucentText(bNewTranslucency);
	unguardexec;
}

void XGC::execIsTranslucentTextEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execIsTranslucentTextEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsTranslucentTextEnabled();
	unguardexec;
}

void XGC::execSetTextVSpacing(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execSetTextVSpacing);
	P_GET_FLOAT(newVSpacing);
	P_FINISH;
	SetTextVSpacing(newVSpacing);
	unguardexec;
}

void XGC::execGetTextVSpacing(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execGetTextVSpacing);
	P_FINISH;
	*(FLOAT*)Result = GetTextVSpacing();
	unguardexec;
}

void XGC::execSetHorizontalAlignment(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execSetHorizontalAlignment);
	P_GET_BYTE(newHAlign);
	P_FINISH;
	SetHorizontalAlignment((EHAlign)newHAlign);
	unguardexec;
}

void XGC::execGetHorizontalAlignment(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execGetHorizontalAlignment);
	P_FINISH;
	*(BYTE*)Result = GetHorizontalAlignment();
	unguardexec;
}

void XGC::execSetVerticalAlignment(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execSetVerticalAlignment);
	P_GET_BYTE(newVAlign);
	P_FINISH;
	SetVerticalAlignment((EVAlign)newVAlign);
	unguardexec;
}

void XGC::execGetVerticalAlignment(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execGetVerticalAlignment);
	P_FINISH;
	*(BYTE*)Result = GetVerticalAlignment();
	unguardexec;
}

void XGC::execSetAlignments(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execSetAlignments);
	P_GET_BYTE(newHAlign);
	P_GET_BYTE(newVAlign);
	P_FINISH;
	SetAlignments((EHAlign)newHAlign, (EVAlign)newVAlign);
	unguardexec;
}

void XGC::execGetAlignments(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execGetAlignments);
	P_GET_BYTE_REF(hAlign);
	P_GET_BYTE_REF(vAlign);
	P_FINISH;
	EHAlign localHAlign = HALIGN_Left;
	EVAlign localVAlign = VALIGN_Top;
	GetAlignments(&localHAlign, &localVAlign);
	*hAlign = (BYTE)localHAlign;
	*vAlign = (BYTE)localVAlign;
	unguardexec;
}

void XGC::execEnableWordWrap(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execEnableWordWrap);
	P_GET_UBOOL(bNewWordWrap);
	P_FINISH;
	EnableWordWrap(bNewWordWrap);
	unguardexec;
}

void XGC::execIsWordWrapEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execIsWordWrapEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsWordWrapEnabled();
	unguardexec;
}

void XGC::execEnableSpecialText(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execEnableSpecialText);
	P_GET_UBOOL(bNewSpecialText);
	P_FINISH;
	EnableSpecialText(bNewSpecialText);
	unguardexec;
}

void XGC::execIsSpecialTextEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execIsSpecialTextEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsSpecialTextEnabled();
	unguardexec;
}

void XGC::execSetBaselineData(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execSetBaselineData);
	P_GET_FLOAT_OPTX(newBaselineOffset,2.0f);
	P_GET_FLOAT_OPTX(newUnderlineHeight,1.0f);
	P_FINISH;
	SetBaselineData(newBaselineOffset, newUnderlineHeight);
	unguardexec;
}

void XGC::execCopyGC(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execCopyGC);
	P_GET_OBJECT(XGC,copy);
	P_FINISH;
	if (copy != NULL)
		CopyGC(*copy);
	unguardexec;
}

void XGC::execPushGC(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execPushGC);
	P_FINISH;
	*(INT*)Result = PushGC();
	unguardexec;
}

void XGC::execPopGC(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execPopGC);
	P_GET_INT_OPTX(gcNum,-1);
	P_FINISH;
	if (gcNum == -1)
	{
		PopGC();
	}
	else
	{
		PopGC(gcNum);
	}
	unguardexec;
}

void XGC::execGetTextExtent(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execGetTextExtent);
	P_GET_FLOAT(destWidth);
	P_GET_FLOAT_REF(xExtent);
	P_GET_FLOAT_REF(yExtent);
	P_GET_STR(textStr);
	P_FINISH;
	GetTextExtent(destWidth, *xExtent, *yExtent, *textStr);
	unguardexec;
}

void XGC::execGetFontHeight(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execGetFontHeight);
	P_GET_UBOOL_OPTX(bIncludeSpace,0);
	P_FINISH;
	*(FLOAT*)Result = GetFontHeight(bIncludeSpace);
	unguardexec;
}

void XGC::execDrawText(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execDrawText);
	P_GET_FLOAT(destX);
	P_GET_FLOAT(destY);
	P_GET_FLOAT(destWidth);
	P_GET_FLOAT(destHeight);
	P_GET_STR(textStr);
	P_FINISH;
	DrawText(destX, destY, destWidth, destHeight, *textStr);
	unguardexec;
}

void XGC::execDrawIcon(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execDrawIcon);
	P_GET_FLOAT(destX);
	P_GET_FLOAT(destY);
	P_GET_OBJECT(UTexture,tx);
	P_FINISH;
	DrawIcon(destX, destY, tx);
	unguardexec;
}

void XGC::execDrawTexture(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execDrawTexture);
	P_GET_FLOAT(destX);
	P_GET_FLOAT(destY);
	P_GET_FLOAT(destWidth);
	P_GET_FLOAT(destHeight);
	P_GET_FLOAT(srcX);
	P_GET_FLOAT(srcY);
	P_GET_OBJECT(UTexture,tx);
	P_FINISH;
	DrawTexture(destX, destY, destWidth, destHeight, srcX, srcY, tx);
	unguardexec;
}

void XGC::execDrawPattern(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execDrawPattern);
	P_GET_FLOAT(destX);
	P_GET_FLOAT(destY);
	P_GET_FLOAT(destWidth);
	P_GET_FLOAT(destHeight);
	P_GET_FLOAT(orgX);
	P_GET_FLOAT(orgY);
	P_GET_OBJECT(UTexture,tx);
	P_FINISH;
	DrawPattern(destX, destY, destWidth, destHeight, orgX, orgY, tx);
	unguardexec;
}

void XGC::execDrawBox(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execDrawBox);
	P_GET_FLOAT(destX);
	P_GET_FLOAT(destY);
	P_GET_FLOAT(destWidth);
	P_GET_FLOAT(destHeight);
	P_GET_FLOAT(orgX);
	P_GET_FLOAT(orgY);
	P_GET_FLOAT(boxThickness);
	P_GET_OBJECT(UTexture,tx);
	P_FINISH;
	DrawBox(destX, destY, destWidth, destHeight, orgX, orgY, boxThickness, tx);
	unguardexec;
}

void XGC::execDrawStretchedTexture(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execDrawStretchedTexture);
	P_GET_FLOAT(destX);
	P_GET_FLOAT(destY);
	P_GET_FLOAT(destWidth);
	P_GET_FLOAT(destHeight);
	P_GET_FLOAT(srcX);
	P_GET_FLOAT(srcY);
	P_GET_FLOAT(srcWidth);
	P_GET_FLOAT(srcHeight);
	P_GET_OBJECT(UTexture,tx);
	P_FINISH;
	DrawStretchedTexture(destX, destY, destWidth, destHeight, srcX, srcY, srcWidth, srcHeight, tx);
	unguardexec;
}

void XGC::execDrawActor(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execDrawActor);
	P_GET_OBJECT(AActor,actor);
	P_GET_UBOOL_OPTX(bClearZ,0);
	P_GET_UBOOL_OPTX(bConstrain,0);
	P_GET_UBOOL_OPTX(bUnlit,0);
	P_GET_FLOAT_OPTX(drawScale,1.0f);
	P_GET_FLOAT_OPTX(scaleGlow,1.0f);
	P_GET_OBJECT_OPTX(UTexture,skin,NULL);
	P_FINISH;
	DrawActor(actor, bClearZ, bConstrain, bUnlit, drawScale, scaleGlow, skin);
	unguardexec;
}

void XGC::execDrawBorders(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execDrawBorders);
	P_GET_FLOAT(destX);
	P_GET_FLOAT(destY);
	P_GET_FLOAT(destWidth);
	P_GET_FLOAT(destHeight);
	P_GET_FLOAT(leftMargin);
	P_GET_FLOAT(rightMargin);
	P_GET_FLOAT(topMargin);
	P_GET_FLOAT(bottomMargin);

	BYTE BorderStorage[1024];
	appMemzero(BorderStorage, sizeof(BorderStorage));

	GPropAddr = NULL;
	Stack.Step(Stack.Object, BorderStorage);

	UTexture** Borders = (UTexture**)GPropAddr;
	if (Borders == NULL)
		Borders = (UTexture**)BorderStorage;

	P_GET_UBOOL_OPTX(bStretchHorizontally,0);
	P_GET_UBOOL_OPTX(bStretchVertically,0);
	P_FINISH;

	DrawBorders(destX, destY, destWidth, destHeight, leftMargin, rightMargin, topMargin, bottomMargin,
		Borders[0], Borders[1], Borders[2], Borders[3], Borders[4], Borders[5], Borders[6], Borders[7], Borders[8],
		bStretchHorizontally, bStretchVertically);
	unguardexec;
}

void XGC::execClearZ(FFrame& Stack, RESULT_DECL)
{
	guard(XGC::execClearZ);
	P_FINISH;
	ClearZ();
	unguardexec;
}

void XWindow::execDestroy(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execDestroy);
	P_FINISH;
	SafeDestroy();
	unguardexec;
}

void XWindow::execNewChild(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execNewChild);
	P_GET_OBJECT(UClass,newClass);
	P_GET_UBOOL_OPTX(bShow,TRUE);
	P_FINISH;
	*(XWindow**)Result = XWindow::CreateNewWindow(newClass, this, bShow);
	unguardexec;
}

void XWindow::execRaise(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execRaise);
	P_FINISH;
	Raise();
	unguardexec;
}

void XWindow::execLower(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execLower);
	P_FINISH;
	Lower();
	unguardexec;
}

void XWindow::execShow(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execShow);
	P_GET_UBOOL_OPTX(bShow,TRUE);
	P_FINISH;
	SetVisibility(bShow);
	unguardexec;
}

void XWindow::execHide(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execHide);
	P_FINISH;
	SetVisibility(FALSE);
	unguardexec;
}

void XWindow::execIsVisible(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execIsVisible);
	P_GET_UBOOL_OPTX(bRecurse,TRUE);
	P_FINISH;
	*(UBOOL*)Result = IsVisible(bRecurse);
	unguardexec;
}

void XWindow::execSetSensitivity(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetSensitivity);
	P_GET_UBOOL(newSensitivity);
	P_FINISH;
	SetSensitivity(newSensitivity);
	unguardexec;
}

void XWindow::execEnable(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execEnable);
	P_GET_UBOOL_OPTX(bEnable,TRUE);
	P_FINISH;
	SetSensitivity(bEnable);
	unguardexec;
}

void XWindow::execDisable(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execDisable);
	P_FINISH;
	SetSensitivity(FALSE);
	unguardexec;
}

void XWindow::execIsSensitive(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execIsSensitive);
	P_GET_UBOOL_OPTX(bRecurse,TRUE);
	P_FINISH;
	*(UBOOL*)Result = IsSensitive(bRecurse);
	unguardexec;
}

void XWindow::execSetSelectability(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetSelectability);
	P_GET_UBOOL(newSelectability);
	P_FINISH;
	SetSelectability(newSelectability);
	unguardexec;
}

void XWindow::execSetBackground(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetBackground);
	P_GET_OBJECT(UTexture,newBackground);
	P_FINISH;
	SetBackground(newBackground);
	unguardexec;
}

void XWindow::execSetBackgroundStyle(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetBackgroundStyle);
	P_GET_BYTE(newStyle);
	P_FINISH;
	SetBackgroundStyle(newStyle);
	unguardexec;
}

void XWindow::execSetBackgroundSmoothing(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetBackgroundSmoothing);
	P_GET_UBOOL(newSmoothing);
	P_FINISH;
	SetBackgroundSmoothing(newSmoothing);
	unguardexec;
}

void XWindow::execSetBackgroundStretching(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetBackgroundStretching);
	P_GET_UBOOL(newStretching);
	P_FINISH;
	SetBackgroundStretching(newStretching);
	unguardexec;
}

void XWindow::execGetRootWindow(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetRootWindow);
	P_FINISH;
	*(XRootWindow**)Result = GetRootWindow();
	unguardexec;
}

void XWindow::execGetModalWindow(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetModalWindow);
	P_FINISH;
	*(XModalWindow**)Result = GetModalWindow();
	unguardexec;
}

void XWindow::execGetTabGroupWindow(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetTabGroupWindow);
	P_FINISH;
	*(XTabGroupWindow**)Result = GetTabGroupWindow();
	unguardexec;
}

void XWindow::execGetParent(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetParent);
	P_FINISH;
	*(XWindow**)Result = GetParent();
	unguardexec;
}

void XWindow::execGetPlayerPawn(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetPlayerPawn);
	P_FINISH;
	*(APlayerPawnExt**)Result = GetPlayerPawn();
	unguardexec;
}

void XWindow::execSetConfiguration(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetConfiguration);
	P_GET_FLOAT(newX);
	P_GET_FLOAT(newY);
	P_GET_FLOAT(newWidth);
	P_GET_FLOAT(newHeight);
	P_FINISH;
	SetConfiguration(newX, newY, newWidth, newHeight);
	unguardexec;
}

void XWindow::execSetSize(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetSize);
	P_GET_FLOAT(newWidth);
	P_GET_FLOAT(newHeight);
	P_FINISH;
	SetSize(newWidth, newHeight);
	unguardexec;
}

void XWindow::execSetPos(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetPos);
	P_GET_FLOAT(newX);
	P_GET_FLOAT(newY);
	P_FINISH;
	SetPos(newX, newY);
	unguardexec;
}

void XWindow::execSetWidth(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetWidth);
	P_GET_FLOAT(newWidth);
	P_FINISH;
	SetWidth(newWidth);
	unguardexec;
}

void XWindow::execSetHeight(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetHeight);
	P_GET_FLOAT(newHeight);
	P_FINISH;
	SetHeight(newHeight);
	unguardexec;
}

void XWindow::execResetSize(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execResetSize);
	P_FINISH;
	ResetSize();
	unguardexec;
}

void XWindow::execResetWidth(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execResetWidth);
	P_FINISH;
	ResetWidth();
	unguardexec;
}

void XWindow::execResetHeight(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execResetHeight);
	P_FINISH;
	ResetHeight();
	unguardexec;
}

void XWindow::execSetWindowAlignments(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetWindowAlignments);
	P_GET_BYTE(hAlign);
	P_GET_BYTE(vAlign);
	P_GET_FLOAT_OPTX(hMargin0,0);
	P_GET_FLOAT_OPTX(vMargin0,0);
	P_GET_FLOAT_OPTX(hMargin1,hMargin0);
	P_GET_FLOAT_OPTX(vMargin1,vMargin0);
	P_FINISH;
	SetWindowAlignments((EHAlign)hAlign, (EVAlign)vAlign, hMargin0, vMargin0, hMargin1, vMargin1);
	unguardexec;
}

void XWindow::execSetAcceleratorText(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetAcceleratorText);
	P_GET_STR(newStr);
	P_FINISH;
	SetAcceleratorText(*newStr);
	unguardexec;
}

void XWindow::execSetFocusWindow(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetFocusWindow);
	P_GET_OBJECT(XWindow,newFocusWindow);
	P_FINISH;
	*(UBOOL*)Result = SetFocusWindow(newFocusWindow);
	unguardexec;
}

void XWindow::execGetFocusWindow(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetFocusWindow);
	P_FINISH;
	*(XWindow**)Result = GetFocusWindow();
	unguardexec;
}

void XWindow::execMoveFocusLeft(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execMoveFocusLeft);
	P_FINISH;
	*(XWindow**)Result = MoveFocusLeft();
	unguardexec;
}

void XWindow::execMoveFocusRight(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execMoveFocusRight);
	P_FINISH;
	*(XWindow**)Result = MoveFocusRight();
	unguardexec;
}

void XWindow::execMoveFocusUp(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execMoveFocusUp);
	P_FINISH;
	*(XWindow**)Result = MoveFocusUp();
	unguardexec;
}

void XWindow::execMoveFocusDown(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execMoveFocusDown);
	P_FINISH;
	*(XWindow**)Result = MoveFocusDown();
	unguardexec;
}

void XWindow::execMoveTabGroupNext(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execMoveTabGroupNext);
	P_FINISH;
	*(XWindow**)Result = MoveTabGroupNext();
	unguardexec;
}

void XWindow::execMoveTabGroupPrev(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execMoveTabGroupPrev);
	P_FINISH;
	*(XWindow**)Result = MoveTabGroupPrev();
	unguardexec;
}

void XWindow::execIsFocusWindow(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execIsFocusWindow);
	P_FINISH;
	*(UBOOL*)Result = IsFocusWindow();
	unguardexec;
}

void XWindow::execConvertCoordinates(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execConvertCoordinates);
	P_GET_OBJECT(XWindow,fromWin);
	P_GET_FLOAT(fromX);
	P_GET_FLOAT(fromY);
	P_GET_OBJECT(XWindow,toWin);
	P_GET_FLOAT_REF(toX);
	P_GET_FLOAT_REF(toY);
	P_FINISH;
	ConvertCoordinates(fromWin, fromX, fromY, toWin, toX, toY);
	unguardexec;
}

void XWindow::execGrabMouse(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGrabMouse);
	P_FINISH;
	GrabMouse();
	unguardexec;
}

void XWindow::execUngrabMouse(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execUngrabMouse);
	P_FINISH;
	UngrabMouse();
	unguardexec;
}

void XWindow::execGetCursorPos(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetCursorPos);
	P_GET_FLOAT_REF(mouseX);
	P_GET_FLOAT_REF(mouseY);
	P_FINISH;
	GetCursorPos(mouseX, mouseY);
	unguardexec;
}

void XWindow::execSetCursorPos(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetCursorPos);
	P_GET_FLOAT(newMouseX);
	P_GET_FLOAT(newMouseY);
	P_FINISH;
	SetCursorPos(newMouseX, newMouseY);
	unguardexec;
}

void XWindow::execSetDefaultCursor(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetDefaultCursor);
	P_GET_OBJECT(UTexture,tx);
	P_GET_OBJECT_OPTX(UTexture,shadowTexture,NULL);
	P_GET_FLOAT_OPTX(hotX,-1.0f);
	P_GET_FLOAT_OPTX(hotY,-1.0f);
	P_GET_STRUCT_OPTX(FColor,cursorColor,FColor(255,255,255));
	P_FINISH;
	SetDefaultCursor(tx, shadowTexture, hotX, hotY, cursorColor);
	unguardexec;
}

void XWindow::execGetTopChild(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetTopChild);
	P_GET_UBOOL_OPTX(bVisibleOnly,TRUE);
	P_FINISH;
	*(XWindow**)Result = GetTopChild(bVisibleOnly);
	unguardexec;
}

void XWindow::execGetBottomChild(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetBottomChild);
	P_GET_UBOOL_OPTX(bVisibleOnly,TRUE);
	P_FINISH;
	*(XWindow**)Result = GetBottomChild(bVisibleOnly);
	unguardexec;
}

void XWindow::execGetHigherSibling(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetHigherSibling);
	P_GET_UBOOL_OPTX(bVisibleOnly,TRUE);
	P_FINISH;
	*(XWindow**)Result = GetHigherSibling(bVisibleOnly);
	unguardexec;
}

void XWindow::execGetLowerSibling(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetLowerSibling);
	P_GET_UBOOL_OPTX(bVisibleOnly,TRUE);
	P_FINISH;
	*(XWindow**)Result = GetLowerSibling(bVisibleOnly);
	unguardexec;
}

void XWindow::execDestroyAllChildren(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execDestroyAllChildren);
	P_FINISH;
	DestroyAllChildren();
	unguardexec;
}

void XWindow::execAskParentForReconfigure(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execAskParentForReconfigure);
	P_FINISH;
	AskParentForReconfigure();
	unguardexec;
}

void XWindow::execConfigureChild(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execConfigureChild);
	P_GET_FLOAT(newX);
	P_GET_FLOAT(newY);
	P_GET_FLOAT(newWidth);
	P_GET_FLOAT(newHeight);
	P_FINISH;
	ConfigureChild(newX, newY, newWidth, newHeight);
	unguardexec;
}

void XWindow::execResizeChild(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execResizeChild);
	P_FINISH;
	ResizeChild();
	unguardexec;
}

void XWindow::execQueryPreferredWidth(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execQueryPreferredWidth);
	P_GET_FLOAT(queryHeight);
	P_FINISH;
	*(FLOAT*)Result = QueryPreferredWidth(queryHeight);
	unguardexec;
}

void XWindow::execQueryPreferredHeight(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execQueryPreferredHeight);
	P_GET_FLOAT(queryWidth);
	P_FINISH;
	*(FLOAT*)Result = QueryPreferredHeight(queryWidth);
	unguardexec;
}

void XWindow::execQueryPreferredSize(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execQueryPreferredSize);
	P_GET_FLOAT_REF(preferredWidth);
	P_GET_FLOAT_REF(preferredHeight);
	P_FINISH;
	QueryPreferredSize(preferredWidth, preferredHeight);
	unguardexec;
}

void XWindow::execQueryGranularity(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execQueryGranularity);
	P_GET_FLOAT_REF(hGranularity);
	P_GET_FLOAT_REF(vGranularity);
	P_FINISH;
	QueryGranularity(hGranularity, vGranularity);
	unguardexec;
}

void XWindow::execSetChildVisibility(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetChildVisibility);
	P_GET_UBOOL(bNewVisibility);
	P_FINISH;
	SetChildVisibility(bNewVisibility);
	unguardexec;
}

void XWindow::execAskParentToShowArea(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execAskParentToShowArea);
	P_GET_FLOAT_OPTX(areaX,0);
	P_GET_FLOAT_OPTX(areaY,0);
	P_GET_FLOAT_OPTX(areaWidth,0);
	P_GET_FLOAT_OPTX(areaHeight,0);
	P_FINISH;

	// Original Extension.dll native wrapper behavior:
	// optional zero width/height means "use this window's current size".
	// PersonaScreenGoals relies on firstNoteWindow.AskParentToShowArea()
	// being equivalent to AskParentToShowArea(0, 0, Width, Height).
	if (areaWidth == 0.0f)
		areaWidth = width;

	if (areaHeight == 0.0f)
		areaHeight = height;

	AskParentToShowArea(areaX, areaY, areaWidth, areaHeight);
	unguardexec;
}

void XWindow::execConvertScriptString(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execConvertScriptString);
	P_GET_STR(oldStr);
	P_FINISH;
	*(FString*)Result = ConvertScriptString(*oldStr);
	unguardexec;
}

void XWindow::execIsKeyDown(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execIsKeyDown);
	P_GET_BYTE(key);
	P_FINISH;
	*(UBOOL*)Result = IsKeyDown(key);
	unguardexec;
}

void XWindow::execIsPointInWindow(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execIsPointInWindow);
	P_GET_FLOAT(pointX);
	P_GET_FLOAT(pointY);
	P_FINISH;
	*(UBOOL*)Result = IsPointInWindow(pointX, pointY);
	unguardexec;
}

void XWindow::execFindWindow(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execFindWindow);
	P_GET_FLOAT(pointX);
	P_GET_FLOAT(pointY);
	P_GET_FLOAT_REF(relativeX);
	P_GET_FLOAT_REF(relativeY);
	P_FINISH;
	*(XWindow**)Result = FindWindow(pointX, pointY, relativeX, relativeY);
	unguardexec;
}

void XWindow::execPlaySound(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execPlaySound);
	P_GET_OBJECT(USound,newsound);
	P_GET_FLOAT_OPTX(volume,-1.0f);
	P_GET_FLOAT_OPTX(pitch,1.0f);
	P_GET_FLOAT_OPTX(posX,width * 0.5f);
	P_GET_FLOAT_OPTX(posY,height * 0.5f);
	P_FINISH;
	PlaySound(newsound, volume, pitch, posX, posY);
	unguardexec;
}

void XWindow::execSetSoundVolume(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetSoundVolume);
	P_GET_FLOAT(newVolume);
	P_FINISH;
	SetSoundVolume(newVolume);
	unguardexec;
}

void XWindow::execSetTileColor(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetTileColor);
	P_GET_STRUCT(FColor,newColor);
	P_FINISH;
	SetTileColor(newColor);
	unguardexec;
}

void XWindow::execSetTextColor(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetTextColor);
	P_GET_STRUCT(FColor,newColor);
	P_FINISH;
	SetTextColor(newColor);
	unguardexec;
}

void XWindow::execSetFont(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetFont);
	P_GET_OBJECT(UFont,fn);
	P_FINISH;
	SetFont(fn);
	unguardexec;
}

void XWindow::execSetFonts(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetFonts);
	P_GET_OBJECT(UFont,nFont);
	P_GET_OBJECT(UFont,bFont);
	P_FINISH;
	SetFonts(nFont, bFont);
	unguardexec;
}

void XWindow::execSetNormalFont(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetNormalFont);
	P_GET_OBJECT(UFont,fn);
	P_FINISH;
	SetNormalFont(fn);
	unguardexec;
}

void XWindow::execSetBoldFont(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetBoldFont);
	P_GET_OBJECT(UFont,fn);
	P_FINISH;
	SetBoldFont(fn);
	unguardexec;
}

void XWindow::execEnableSpecialText(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execEnableSpecialText);
	P_GET_UBOOL_OPTX(bEnable,TRUE);
	P_FINISH;
	EnableSpecialText(bEnable);
	unguardexec;
}

void XWindow::execCarriageReturn(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execCarriageReturn);
	P_FINISH;
	*(FString*)Result = FString(TEXT("\n"));
	unguardexec;
}

void XWindow::execEnableTranslucentText(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execEnableTranslucentText);
	P_GET_UBOOL_OPTX(bEnable,TRUE);
	P_FINISH;
	EnableTranslucentText(bEnable);
	unguardexec;
}

void XWindow::execSetBaselineData(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetBaselineData);
	P_GET_FLOAT_OPTX(newBaselineOffset,2.0f);
	P_GET_FLOAT_OPTX(newUnderlineHeight,1.0f);
	P_FINISH;
	SetBaselineData(newBaselineOffset, newUnderlineHeight);
	unguardexec;
}

void XWindow::execGetGC(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetGC);
	P_FINISH;
	*(XGC**)Result = GetGC();
	unguardexec;
}

void XWindow::execReleaseGC(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execReleaseGC);
	P_GET_OBJECT(XGC,gc);
	P_FINISH;
	ReleaseGC(gc);
	unguardexec;
}

void XWindow::execSetClientObject(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetClientObject);
	P_GET_OBJECT(UObject,newClientObject);
	P_FINISH;
	SetClientObject(newClientObject);
	unguardexec;
}

void XWindow::execGetClientObject(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetClientObject);
	P_FINISH;
	*(UObject**)Result = GetClientObject();
	unguardexec;
}

void XWindow::execConvertVectorToCoordinates(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execConvertVectorToCoordinates);
	P_GET_VECTOR(location);
	P_GET_FLOAT_REF(relativeX);
	P_GET_FLOAT_REF(relativeY);
	P_FINISH;
	*(UBOOL*)Result = ConvertVectorToCoordinates(location, relativeX, relativeY);
	unguardexec;
}

void XWindow::execAddTimer(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execAddTimer);
	P_GET_FLOAT(timeout);
	P_GET_UBOOL_OPTX(bLoop,FALSE);
	P_GET_INT_OPTX(clientData,0);
	P_GET_NAME_OPTX(functionName,NAME_None);
	P_FINISH;
	*(INT*)Result = AddTimer(timeout, bLoop, clientData, functionName);
	unguardexec;
}

void XWindow::execRemoveTimer(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execRemoveTimer);
	P_GET_INT(timerId);
	P_FINISH;
	RemoveTimer(timerId);
	unguardexec;
}

void XWindow::execGetTickOffset(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execGetTickOffset);
	P_FINISH;
	*(FLOAT*)Result = GetTickOffset();
	unguardexec;
}

void XWindow::execChangeStyle(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execChangeStyle);
	P_FINISH;
	ChangeStyle();
	unguardexec;
}

void XWindow::execSetFocusSounds(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetFocusSounds);
	P_GET_OBJECT_OPTX(USound,focusSound,NULL);
	P_GET_OBJECT_OPTX(USound,unfocusSound,NULL);
	P_FINISH;
	SetFocusSounds(focusSound, unfocusSound);
	unguardexec;
}

void XWindow::execSetVisibilitySounds(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execSetVisibilitySounds);
	P_GET_OBJECT_OPTX(USound,visSound,NULL);
	P_GET_OBJECT_OPTX(USound,invisSound,NULL);
	P_FINISH;
	SetVisibilitySounds(visSound, invisSound);
	unguardexec;
}

void XWindow::execAddActorRef(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execAddActorRef);
	P_GET_OBJECT(AActor,refActor);
	P_FINISH;
	AddActorRef(refActor);
	unguardexec;
}

void XWindow::execRemoveActorRef(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execRemoveActorRef);
	P_GET_OBJECT(AActor,refActor);
	P_FINISH;
	RemoveActorRef(refActor);
	unguardexec;
}

void XWindow::execIsActorValid(FFrame& Stack, RESULT_DECL)
{
	guard(XWindow::execIsActorValid);
	P_GET_OBJECT(AActor,refActor);
	P_FINISH;
	*(UBOOL*)Result = IsActorValid(refActor);
	unguardexec;
}

void XRootWindow::execSetDefaultEditCursor(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execSetDefaultEditCursor);
	P_GET_OBJECT_OPTX(UTexture,newEditCursor,NULL);
	P_FINISH;
	SetDefaultEditCursor(newEditCursor);
	unguardexec;
}

void XRootWindow::execSetDefaultMovementCursors(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execSetDefaultMovementCursors);
	P_GET_OBJECT_OPTX(UTexture,newMovementCursor,NULL);
	P_GET_OBJECT_OPTX(UTexture,newHorizontalMovementCursor,NULL);
	P_GET_OBJECT_OPTX(UTexture,newVerticalMovementCursor,NULL);
	P_GET_OBJECT_OPTX(UTexture,newTopLeftMovementCursor,NULL);
	P_GET_OBJECT_OPTX(UTexture,newTopRightMovementCursor,NULL);
	P_FINISH;
	SetDefaultMovementCursors(newMovementCursor, newHorizontalMovementCursor, newVerticalMovementCursor, newTopLeftMovementCursor, newTopRightMovementCursor);
	unguardexec;
}

void XRootWindow::execEnableRendering(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execEnableRendering);
	P_GET_UBOOL_OPTX(bRender,TRUE);
	P_FINISH;
	EnableRendering(bRender);
	unguardexec;
}

void XRootWindow::execIsRenderingEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execIsRenderingEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsRenderingEnabled();
	unguardexec;
}

void XRootWindow::execSetRenderViewport(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execSetRenderViewport);
	P_GET_FLOAT(newX);
	P_GET_FLOAT(newY);
	P_GET_FLOAT(newWidth);
	P_GET_FLOAT(newHeight);
	P_FINISH;
	SetRenderViewport(newX, newY, newWidth, newHeight);
	unguardexec;
}

void XRootWindow::execResetRenderViewport(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execResetRenderViewport);
	P_FINISH;
	ResetRenderViewport();
	unguardexec;
}

void XRootWindow::execSetRawBackground(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execSetRawBackground);
	P_GET_OBJECT_OPTX(UTexture,newTexture,NULL);
	P_GET_STRUCT_OPTX(FColor,newColor,FColor(255,255,255));
	P_FINISH;
	SetRawBackground(newTexture, newColor);
	unguardexec;
}

void XRootWindow::execSetRawBackgroundSize(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execSetRawBackgroundSize);
	P_GET_FLOAT(newWidth);
	P_GET_FLOAT(newHeight);
	P_FINISH;
	SetRawBackgroundSize(newWidth, newHeight);
	unguardexec;
}

void XRootWindow::execStretchRawBackground(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execStretchRawBackground);
	P_GET_UBOOL_OPTX(bStretch,TRUE);
	P_FINISH;
	StretchRawBackground(bStretch);
	unguardexec;
}

void XRootWindow::execEnablePositionalSound(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execEnablePositionalSound);
	P_GET_UBOOL_OPTX(bEnable,TRUE);
	P_FINISH;
	EnablePositionalSound(bEnable);
	unguardexec;
}

void XRootWindow::execIsPositionalSoundEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execIsPositionalSoundEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsPositionalSoundEnabled();
	unguardexec;
}

void XRootWindow::execLockMouse(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execLockMouse);
	P_GET_UBOOL_OPTX(bLockMove,TRUE);
	P_GET_UBOOL_OPTX(bLockButton,TRUE);
	P_FINISH;
	LockMouse(bLockMove, bLockButton);
	unguardexec;
}

void XRootWindow::execShowCursor(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execShowCursor);
	P_GET_UBOOL_OPTX(bShow,TRUE);
	P_FINISH;
	ShowCursor(bShow);
	unguardexec;
}

void XRootWindow::execSetSnapshotSize(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execSetSnapshotSize);
	P_GET_FLOAT(newWidth);
	P_GET_FLOAT(newHeight);
	P_FINISH;
	SetSnapshotSize(newWidth, newHeight);
	unguardexec;
}

void XRootWindow::execGenerateSnapshot(FFrame& Stack, RESULT_DECL)
{
	guard(XRootWindow::execGenerateSnapshot);
	P_GET_UBOOL_OPTX(bFilter,0);
	P_FINISH;
	*(UTexture**)Result = GenerateSnapshot(NULL, bFilter, FALSE, FALSE);
	unguardexec;
}

void XBorderWindow::execSetBorders(FFrame& Stack, RESULT_DECL)
{
	guard(XBorderWindow::execSetBorders);
	P_GET_OBJECT_OPTX(UTexture,bordTL,NULL);
	P_GET_OBJECT_OPTX(UTexture,bordTR,NULL);
	P_GET_OBJECT_OPTX(UTexture,bordBL,NULL);
	P_GET_OBJECT_OPTX(UTexture,bordBR,NULL);
	P_GET_OBJECT_OPTX(UTexture,bordL,NULL);
	P_GET_OBJECT_OPTX(UTexture,bordR,NULL);
	P_GET_OBJECT_OPTX(UTexture,bordT,NULL);
	P_GET_OBJECT_OPTX(UTexture,bordB,NULL);
	P_GET_OBJECT_OPTX(UTexture,center,NULL);
	P_FINISH;
	SetBorders(bordTL, bordTR, bordBL, bordBR, bordL, bordR, bordT, bordB, center);
	unguardexec;
}

void XBorderWindow::execSetBorderMargins(FFrame& Stack, RESULT_DECL)
{
	guard(XBorderWindow::execSetBorderMargins);
	P_GET_FLOAT_OPTX(newLeft,0);
	P_GET_FLOAT_OPTX(newRight,0);
	P_GET_FLOAT_OPTX(newTop,0);
	P_GET_FLOAT_OPTX(newBottom,0);
	P_FINISH;
	SetBorderMargins(newLeft, newRight, newTop, newBottom);
	unguardexec;
}

void XBorderWindow::execBaseMarginsFromBorder(FFrame& Stack, RESULT_DECL)
{
	guard(XBorderWindow::execBaseMarginsFromBorder);
	P_GET_UBOOL_OPTX(bBorder,1);
	P_FINISH;
	BaseMarginsFromBorder(bBorder);
	unguardexec;
}

void XBorderWindow::execEnableResizing(FFrame& Stack, RESULT_DECL)
{
	guard(XBorderWindow::execEnableResizing);
	P_GET_UBOOL_OPTX(bResize,TRUE);
	P_FINISH;
	EnableResizing(bResize);
	unguardexec;
}

void XBorderWindow::execSetMoveCursors(FFrame& Stack, RESULT_DECL)
{
	guard(XBorderWindow::execSetMoveCursors);
	P_GET_OBJECT_OPTX(UTexture,move,NULL);
	P_GET_OBJECT_OPTX(UTexture,hMove,NULL);
	P_GET_OBJECT_OPTX(UTexture,vMove,NULL);
	P_GET_OBJECT_OPTX(UTexture,tlMove,NULL);
	P_GET_OBJECT_OPTX(UTexture,trMove,NULL);
	P_FINISH;
	SetMoveCursors(move, hMove, vMove, tlMove, trMove);
	unguardexec;
}

void XTileWindow::execSetMargins(FFrame& Stack, RESULT_DECL)
{
	guard(XTileWindow::execSetMargins);
	P_GET_FLOAT(newHMargin);
	P_GET_FLOAT(newVMargin);
	P_FINISH;
	SetMargins(newHMargin, newVMargin);
	unguardexec;
}

void XTileWindow::execSetOrientation(FFrame& Stack, RESULT_DECL)
{
	guard(XTileWindow::execSetOrientation);
	P_GET_BYTE(newOrientation);
	P_FINISH;
	SetOrientation((EOrientation)newOrientation);
	unguardexec;
}

void XTileWindow::execSetDirections(FFrame& Stack, RESULT_DECL)
{
	guard(XTileWindow::execSetDirections);
	P_GET_BYTE(newHDir);
	P_GET_BYTE(newVDir);
	P_FINISH;
	SetDirections((EHDirection)newHDir, (EVDirection)newVDir);
	unguardexec;
}

void XTileWindow::execSetOrder(FFrame& Stack, RESULT_DECL)
{
	guard(XTileWindow::execSetOrder);
	P_GET_BYTE(newOrder);
	P_FINISH;
	SetOrder((EOrder)newOrder);
	unguardexec;
}

void XTileWindow::execSetMinorSpacing(FFrame& Stack, RESULT_DECL)
{
	guard(XTileWindow::execSetMinorSpacing);
	P_GET_FLOAT(newSpacing);
	P_FINISH;
	SetMinorSpacing(newSpacing);
	unguardexec;
}

void XTileWindow::execSetMajorSpacing(FFrame& Stack, RESULT_DECL)
{
	guard(XTileWindow::execSetMajorSpacing);
	P_GET_FLOAT(newSpacing);
	P_FINISH;
	SetMajorSpacing(newSpacing);
	unguardexec;
}

void XTileWindow::execSetChildAlignments(FFrame& Stack, RESULT_DECL)
{
	guard(XTileWindow::execSetChildAlignments);
	P_GET_BYTE(newHAlign);
	P_GET_BYTE(newVAlign);
	P_FINISH;
	SetChildAlignments((EHAlign)newHAlign, (EVAlign)newVAlign);
	unguardexec;
}

void XTileWindow::execEnableWrapping(FFrame& Stack, RESULT_DECL)
{
	guard(XTileWindow::execEnableWrapping);
	P_GET_UBOOL(bWrapOn);
	P_FINISH;
	EnableWrapping(bWrapOn);
	unguardexec;
}

void XTileWindow::execFillParent(FFrame& Stack, RESULT_DECL)
{
	guard(XTileWindow::execFillParent);
	P_GET_UBOOL(bFillParent);
	P_FINISH;
	FillParent(bFillParent);
	unguardexec;
}

void XTileWindow::execMakeWidthsEqual(FFrame& Stack, RESULT_DECL)
{
	guard(XTileWindow::execMakeWidthsEqual);
	P_GET_UBOOL(bEqual);
	P_FINISH;
	MakeWidthsEqual(bEqual);
	unguardexec;
}

void XTileWindow::execMakeHeightsEqual(FFrame& Stack, RESULT_DECL)
{
	guard(XTileWindow::execMakeHeightsEqual);
	P_GET_UBOOL(bEqual);
	P_FINISH;
	MakeHeightsEqual(bEqual);
	unguardexec;
}

void XTextWindow::execSetText(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execSetText);
	P_GET_STR(newText);
	P_FINISH;
	SetText(ConvertScriptString(*newText));
	unguardexec;
}

void XTextWindow::execAppendText(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execAppendText);
	P_GET_STR(newText);
	P_FINISH;
	AppendText(ConvertScriptString(*newText));
	unguardexec;
}

void XTextWindow::execGetText(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execGetText);
	P_FINISH;
	*(FString*)Result = GetText();
	unguardexec;
}

void XTextWindow::execGetTextLength(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execGetTextLength);
	P_FINISH;
	*(INT*)Result = ExtNativeTextLength(GetText());
	unguardexec;
}

void XTextWindow::execGetTextPart(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execGetTextPart);
	P_GET_INT(startPos);
	P_GET_INT(count);
	P_GET_STR_REF(outText);
	P_FINISH;
	*(INT*)Result = ExtNativeTextWindowGetTextPart(GetText(), startPos, count, outText);
	unguardexec;
}

void XTextWindow::execSetWordWrap(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execSetWordWrap);
	P_GET_UBOOL(bNewWordWrap);
	P_FINISH;
	SetWordWrap(bNewWordWrap);
	unguardexec;
}

void XTextWindow::execSetTextAlignments(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execSetTextAlignments);
	P_GET_BYTE(newHAlign);
	P_GET_BYTE(newVAlign);
	P_FINISH;
	SetTextAlignments((EHAlign)newHAlign, (EVAlign)newVAlign);
	unguardexec;
}

void XTextWindow::execSetTextMargins(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execSetTextMargins);
	P_GET_FLOAT(newHMargin);
	P_GET_FLOAT(newVMargin);
	P_FINISH;
	SetTextMargins(newHMargin, newVMargin);
	unguardexec;
}

void XTextWindow::execSetLines(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execSetLines);
	P_GET_INT(newMinLines);
	P_GET_INT(newMaxLines);
	P_FINISH;
	SetLines(newMinLines, newMaxLines);
	unguardexec;
}

void XTextWindow::execSetMinLines(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execSetMinLines);
	P_GET_INT(newMinLines);
	P_FINISH;
	SetMinLines(newMinLines);
	unguardexec;
}

void XTextWindow::execSetMaxLines(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execSetMaxLines);
	P_GET_INT(newMaxLines);
	P_FINISH;
	SetMaxLines(newMaxLines);
	unguardexec;
}

void XTextWindow::execResetLines(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execResetLines);
	P_FINISH;
	ResetLines();
	unguardexec;
}

void XTextWindow::execSetMinWidth(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execSetMinWidth);
	P_GET_FLOAT(newMinWidth);
	P_FINISH;
	SetMinWidth(newMinWidth);
	unguardexec;
}

void XTextWindow::execResetMinWidth(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execResetMinWidth);
	P_FINISH;
	ResetMinWidth();
	unguardexec;
}

void XTextWindow::execEnableTextAsAccelerator(FFrame& Stack, RESULT_DECL)
{
	guard(XTextWindow::execEnableTextAsAccelerator);
	P_GET_UBOOL_OPTX(bEnable,TRUE);
	P_FINISH;
	EnableTextAsAccelerator(bEnable);
	unguardexec;
}

void XButtonWindow::execActivateButton(FFrame& Stack, RESULT_DECL)
{
	guard(XButtonWindow::execActivateButton);
	P_GET_BYTE(key);
	P_FINISH;
	ActivateButton((EInputKey)key);
	unguardexec;
}

void XButtonWindow::execSetActivateDelay(FFrame& Stack, RESULT_DECL)
{
	guard(XButtonWindow::execSetActivateDelay);
	P_GET_FLOAT_OPTX(newDelay,0.30000001f);
	P_FINISH;
	SetActivateDelay(newDelay);
	unguardexec;
}

void XButtonWindow::execSetButtonTextures(FFrame& Stack, RESULT_DECL)
{
	guard(XButtonWindow::execSetButtonTextures);
	P_GET_OBJECT_OPTX(UTexture,normal,NULL);
	P_GET_OBJECT_OPTX(UTexture,pressed,NULL);
	P_GET_OBJECT_OPTX(UTexture,normalFocus,NULL);
	P_GET_OBJECT_OPTX(UTexture,pressedFocus,NULL);
	P_GET_OBJECT_OPTX(UTexture,normalInsensitive,NULL);
	P_GET_OBJECT_OPTX(UTexture,pressedInsensitive,NULL);
	P_FINISH;
	SetButtonTextures(normal, pressed, normalFocus, pressedFocus, normalInsensitive, pressedInsensitive);
	unguardexec;
}

void XButtonWindow::execSetButtonColors(FFrame& Stack, RESULT_DECL)
{
	guard(XButtonWindow::execSetButtonColors);
	P_GET_STRUCT_OPTX(FColor,normal,FColor(255,255,255));
	P_GET_STRUCT_OPTX(FColor,pressed,FColor(255,255,255));
	P_GET_STRUCT_OPTX(FColor,normalFocus,FColor(255,255,255));
	P_GET_STRUCT_OPTX(FColor,pressedFocus,FColor(255,255,255));
	P_GET_STRUCT_OPTX(FColor,normalInsensitive,FColor(255,255,255));
	P_GET_STRUCT_OPTX(FColor,pressedInsensitive,FColor(255,255,255));
	P_FINISH;
	SetButtonColors(normal, pressed, normalFocus, pressedFocus, normalInsensitive, pressedInsensitive);
	unguardexec;
}

void XButtonWindow::execSetTextColors(FFrame& Stack, RESULT_DECL)
{
	guard(XButtonWindow::execSetTextColors);
	P_GET_STRUCT_OPTX(FColor,normal,FColor(0,255,0));
	P_GET_STRUCT_OPTX(FColor,pressed,FColor(0,255,0));
	P_GET_STRUCT_OPTX(FColor,normalFocus,FColor(0,255,0));
	P_GET_STRUCT_OPTX(FColor,pressedFocus,FColor(0,255,0));
	P_GET_STRUCT_OPTX(FColor,normalInsensitive,FColor(0,255,0));
	P_GET_STRUCT_OPTX(FColor,pressedInsensitive,FColor(0,255,0));
	P_FINISH;
	SetTextColors(normal, pressed, normalFocus, pressedFocus, normalInsensitive, pressedInsensitive);
	unguardexec;
}

void XButtonWindow::execEnableAutoRepeat(FFrame& Stack, RESULT_DECL)
{
	guard(XButtonWindow::execEnableAutoRepeat);
	P_GET_UBOOL_OPTX(bEnable,TRUE);
	P_GET_FLOAT_OPTX(initialDelay,0.5f);
	P_GET_FLOAT_OPTX(repeatRate,0.1f);
	P_FINISH;
	EnableAutoRepeat(bEnable, initialDelay, repeatRate);
	unguardexec;
}

void XButtonWindow::execEnableRightMouseClick(FFrame& Stack, RESULT_DECL)
{
	guard(XButtonWindow::execEnableRightMouseClick);
	P_GET_UBOOL_OPTX(bEnable,TRUE);
	P_FINISH;
	EnableRightMouseClick(bEnable);
	unguardexec;
}

void XButtonWindow::execSetButtonSounds(FFrame& Stack, RESULT_DECL)
{
	guard(XButtonWindow::execSetButtonSounds);
	P_GET_OBJECT_OPTX(USound,pressSound,NULL);
	P_GET_OBJECT_OPTX(USound,clickSound,NULL);
	P_FINISH;
	SetButtonSounds(pressSound, clickSound);
	unguardexec;
}

void XButtonWindow::execPressButton(FFrame& Stack, RESULT_DECL)
{
	guard(XButtonWindow::execPressButton);
	P_GET_BYTE_OPTX(key,32);
	P_FINISH;
	PressButton((EInputKey)key);
	unguardexec;
}

void XToggleWindow::execChangeToggle(FFrame& Stack, RESULT_DECL)
{
	guard(XToggleWindow::execChangeToggle);
	P_FINISH;
	ChangeToggle();
	unguardexec;
}

void XToggleWindow::execSetToggle(FFrame& Stack, RESULT_DECL)
{
	guard(XToggleWindow::execSetToggle);
	P_GET_UBOOL(bNewToggle);
	P_FINISH;
	SetToggle(bNewToggle);
	unguardexec;
}

void XToggleWindow::execGetToggle(FFrame& Stack, RESULT_DECL)
{
	guard(XToggleWindow::execGetToggle);
	P_FINISH;
	*(UBOOL*)Result = GetToggle();
	unguardexec;
}

void XToggleWindow::execSetToggleSounds(FFrame& Stack, RESULT_DECL)
{
	guard(XToggleWindow::execSetToggleSounds);
	P_GET_OBJECT_OPTX(USound,enableSound,NULL);
	P_GET_OBJECT_OPTX(USound,disableSound,NULL);
	P_FINISH;
	SetToggleSounds(enableSound, disableSound);
	unguardexec;
}

void XScaleWindow::execSetScaleOrientation(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetScaleOrientation);
	P_GET_BYTE(newOrientation);
	P_FINISH;
	SetOrientation((EOrientation)newOrientation);
	unguardexec;
}

void XScaleWindow::execSetScaleTexture(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetScaleTexture);
	P_GET_OBJECT(UTexture,newTexture);
	P_GET_FLOAT_OPTX(newWidth,0);
	P_GET_FLOAT_OPTX(newHeight,0);
	P_GET_FLOAT_OPTX(newStart,0);
	P_GET_FLOAT_OPTX(newEnd,0);
	P_FINISH;
	SetScaleTexture(newTexture, newWidth, newHeight, newStart, newEnd);
	unguardexec;
}

void XScaleWindow::execSetThumbTexture(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetThumbTexture);
	P_GET_OBJECT(UTexture,newTexture);
	P_GET_FLOAT_OPTX(newWidth,0);
	P_GET_FLOAT_OPTX(newHeight,0);
	P_FINISH;
	SetThumbTexture(newTexture, newWidth, newHeight);
	unguardexec;
}

void XScaleWindow::execSetTickTexture(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetTickTexture);
	P_GET_OBJECT(UTexture,tickTexture);
	P_GET_UBOOL_OPTX(bDrawEndTicks,TRUE);
	P_GET_FLOAT_OPTX(newWidth,0);
	P_GET_FLOAT_OPTX(newHeight,0);
	P_FINISH;
	SetTickTexture(tickTexture, bDrawEndTicks, newWidth, newHeight);
	unguardexec;
}

void XScaleWindow::execSetThumbCaps(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetThumbCaps);
	P_GET_OBJECT(UTexture,preCap);
	P_GET_OBJECT(UTexture,postCap);
	P_GET_FLOAT_OPTX(preCapWidth,0);
	P_GET_FLOAT_OPTX(preCapHeight,0);
	P_GET_FLOAT_OPTX(postCapWidth,0);
	P_GET_FLOAT_OPTX(postCapHeight,0);
	P_FINISH;
	SetThumbCaps(preCap, postCap, preCapWidth, preCapHeight, postCapWidth, postCapHeight);
	unguardexec;
}

void XScaleWindow::execEnableStretchedScale(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execEnableStretchedScale);
	P_GET_UBOOL_OPTX(bNewStretch,TRUE);
	P_FINISH;
	EnableStretchedScale(bNewStretch);
	unguardexec;
}

void XScaleWindow::execSetBorderPattern(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetBorderPattern);
	P_GET_OBJECT(UTexture,newTexture);
	P_FINISH;
	SetBorderPattern(newTexture);
	unguardexec;
}

void XScaleWindow::execSetScaleBorder(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetScaleBorder);
	P_GET_FLOAT_OPTX(newBorderSize,0);
	P_GET_STRUCT_OPTX(FColor,newColor,FColor(255,255,255));
	P_FINISH;
	SetScaleBorder(newBorderSize, newColor);
	unguardexec;
}

void XScaleWindow::execSetThumbBorder(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetThumbBorder);
	P_GET_FLOAT_OPTX(newBorderSize,0);
	P_GET_STRUCT_OPTX(FColor,newColor,FColor(255,255,255));
	P_FINISH;
	SetThumbBorder(newBorderSize, newColor);
	unguardexec;
}

void XScaleWindow::execSetScaleStyle(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetScaleStyle);
	P_GET_BYTE(newStyle);
	P_FINISH;
	SetScaleStyle(newStyle);
	unguardexec;
}

void XScaleWindow::execSetThumbStyle(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetThumbStyle);
	P_GET_BYTE(newStyle);
	P_FINISH;
	SetThumbStyle(newStyle);
	unguardexec;
}

void XScaleWindow::execSetTickStyle(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetTickStyle);
	P_GET_BYTE(newStyle);
	P_FINISH;
	SetTickStyle(newStyle);
	unguardexec;
}

void XScaleWindow::execSetScaleColor(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetScaleColor);
	P_GET_STRUCT(FColor,newColor);
	P_FINISH;
	SetScaleColor(newColor);
	unguardexec;
}

void XScaleWindow::execSetThumbColor(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetThumbColor);
	P_GET_STRUCT(FColor,newColor);
	P_FINISH;
	SetThumbColor(newColor);
	unguardexec;
}

void XScaleWindow::execSetTickColor(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetTickColor);
	P_GET_STRUCT(FColor,newColor);
	P_FINISH;
	SetTickColor(newColor);
	unguardexec;
}

void XScaleWindow::execSetScaleMargins(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetScaleMargins);
	P_GET_FLOAT_OPTX(marginWidth,0);
	P_GET_FLOAT_OPTX(marginHeight,0);
	P_FINISH;
	SetScaleMargins(marginWidth, marginHeight);
	unguardexec;
}

void XScaleWindow::execSetNumTicks(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetNumTicks);
	P_GET_INT(newNumTicks);
	P_FINISH;
	SetNumTicks(newNumTicks);
	unguardexec;
}

void XScaleWindow::execGetNumTicks(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execGetNumTicks);
	P_FINISH;
	*(INT*)Result = GetNumTicks();
	unguardexec;
}

void XScaleWindow::execSetThumbSpan(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetThumbSpan);
	P_GET_INT_OPTX(newRange,1);
	P_FINISH;
	SetThumbSpan(newRange);
	unguardexec;
}

void XScaleWindow::execGetThumbSpan(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execGetThumbSpan);
	P_FINISH;
	*(INT*)Result = GetThumbSpan();
	unguardexec;
}

void XScaleWindow::execSetTickPosition(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetTickPosition);
	P_GET_INT(newPosition);
	P_FINISH;
	SetTickPosition(newPosition);
	unguardexec;
}

void XScaleWindow::execGetTickPosition(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execGetTickPosition);
	P_FINISH;
	*(INT*)Result = GetTickPosition();
	unguardexec;
}

void XScaleWindow::execSetValueRange(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetValueRange);
	P_GET_FLOAT(newFrom);
	P_GET_FLOAT(newTo);
	P_FINISH;
	SetValueRange(newFrom, newTo);
	unguardexec;
}

void XScaleWindow::execSetValue(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetValue);
	P_GET_FLOAT(newValue);
	P_FINISH;
	SetValue(newValue);
	unguardexec;
}

void XScaleWindow::execGetValue(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execGetValue);
	P_FINISH;
	*(FLOAT*)Result = GetValue();
	unguardexec;
}

void XScaleWindow::execGetValues(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execGetValues);
	P_GET_FLOAT_REF(fromValue);
	P_GET_FLOAT_REF(toValue);
	P_FINISH;
	GetValues(fromValue, toValue);
	unguardexec;
}

void XScaleWindow::execSetValueFormat(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetValueFormat);
	P_GET_STR(newFmt);
	P_FINISH;
	SetValueFormat(*newFmt);
	unguardexec;
}

void XScaleWindow::execGetValueString(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execGetValueString);
	P_FINISH;
	*(FString*)Result = GetValueString();
	unguardexec;
}

void XScaleWindow::execSetEnumeration(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetEnumeration);
	P_GET_INT(tickPos);
	P_GET_STR(newStr);
	P_FINISH;
	SetEnumeration(tickPos, *newStr);
	unguardexec;
}

void XScaleWindow::execClearAllEnumerations(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execClearAllEnumerations);
	P_FINISH;
	ClearAllEnumerations();
	unguardexec;
}

void XScaleWindow::execMoveThumb(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execMoveThumb);
	P_GET_BYTE(moveThumb);
	P_FINISH;
	MoveThumb((EMoveThumb)moveThumb);
	unguardexec;
}

void XScaleWindow::execSetThumbStep(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetThumbStep);
	P_GET_INT(newStep);
	P_FINISH;
	SetThumbStep(newStep);
	unguardexec;
}

void XScaleWindow::execSetScaleSounds(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execSetScaleSounds);
	P_GET_OBJECT_OPTX(USound,setSound,NULL);
	P_GET_OBJECT_OPTX(USound,clickSound,NULL);
	P_GET_OBJECT_OPTX(USound,dragSound,NULL);
	P_FINISH;
	SetScaleSounds(setSound, clickSound, dragSound);
	unguardexec;
}

void XScaleWindow::execPlayScaleSound(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleWindow::execPlayScaleSound);
	P_GET_OBJECT(USound,newSound);
	P_GET_FLOAT_OPTX(volume,-1.0f);
	P_GET_FLOAT_OPTX(pitch,1.0f);
	P_FINISH;
	PlayScaleSound(newSound, volume, pitch);
	unguardexec;
}

void XScaleManagerWindow::execSetScaleButtons(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleManagerWindow::execSetScaleButtons);
	P_GET_OBJECT(XButtonWindow,newDecButton);
	P_GET_OBJECT(XButtonWindow,newIncButton);
	P_FINISH;
	SetScaleButtons(newDecButton, newIncButton);
	unguardexec;
}

void XScaleManagerWindow::execSetValueField(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleManagerWindow::execSetValueField);
	P_GET_OBJECT(XTextWindow,newValueField);
	P_FINISH;
	SetValueField(newValueField);
	unguardexec;
}

void XScaleManagerWindow::execSetScale(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleManagerWindow::execSetScale);
	P_GET_OBJECT(XScaleWindow,newScale);
	P_FINISH;
	SetScale(newScale);
	unguardexec;
}

void XScaleManagerWindow::execSetManagerOrientation(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleManagerWindow::execSetManagerOrientation);
	P_GET_BYTE(newOrientation);
	P_FINISH;
	SetOrientation((EOrientation)newOrientation);
	unguardexec;
}

void XScaleManagerWindow::execStretchScaleField(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleManagerWindow::execStretchScaleField);
	P_GET_UBOOL_OPTX(bNewStretch,TRUE);
	P_FINISH;
	StretchScaleField(bNewStretch);
	unguardexec;
}

void XScaleManagerWindow::execStretchValueField(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleManagerWindow::execStretchValueField);
	P_GET_UBOOL_OPTX(bNewStretch,TRUE);
	P_FINISH;
	StretchValueField(bNewStretch);
	unguardexec;
}

void XScaleManagerWindow::execSetManagerMargins(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleManagerWindow::execSetManagerMargins);
	P_GET_FLOAT_OPTX(newMarginWidth,0);
	P_GET_FLOAT_OPTX(newMarginHeight,0);
	P_FINISH;
	SetManagerMargins(newMarginWidth, newMarginHeight);
	unguardexec;
}

void XScaleManagerWindow::execSetManagerSpacing(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleManagerWindow::execSetManagerSpacing);
	P_GET_FLOAT_OPTX(newSpacing,0);
	P_FINISH;
	SetManagerSpacing(newSpacing);
	unguardexec;
}

void XScaleManagerWindow::execSetManagerAlignments(FFrame& Stack, RESULT_DECL)
{
	guard(XScaleManagerWindow::execSetManagerAlignments);
	P_GET_BYTE(newHAlign);
	P_GET_BYTE(newVAlign);
	P_FINISH;
	SetManagerAlignments((EHAlign)newHAlign, (EVAlign)newVAlign);
	unguardexec;
}

void XClipWindow::execSetChildPosition(FFrame& Stack, RESULT_DECL)
{
	guard(XClipWindow::execSetChildPosition);
	P_GET_INT(newX);
	P_GET_INT(newY);
	P_FINISH;
	SetChildPosition(newX, newY);
	unguardexec;
}

void XClipWindow::execGetChildPosition(FFrame& Stack, RESULT_DECL)
{
	guard(XClipWindow::execGetChildPosition);
	P_GET_INT_REF(pNewX);
	P_GET_INT_REF(pNewY);
	P_FINISH;
	GetChildPosition(pNewX, pNewY);
	unguardexec;
}

void XClipWindow::execSetUnitSize(FFrame& Stack, RESULT_DECL)
{
	guard(XClipWindow::execSetUnitSize);
	P_GET_INT(hUnits);
	P_GET_INT(vUnits);
	P_FINISH;
	SetUnitSize(hUnits, vUnits);
	unguardexec;
}

void XClipWindow::execSetUnitWidth(FFrame& Stack, RESULT_DECL)
{
	guard(XClipWindow::execSetUnitWidth);
	P_GET_INT(hUnits);
	P_FINISH;
	SetUnitWidth(hUnits);
	unguardexec;
}

void XClipWindow::execSetUnitHeight(FFrame& Stack, RESULT_DECL)
{
	guard(XClipWindow::execSetUnitHeight);
	P_GET_INT(vUnits);
	P_FINISH;
	SetUnitHeight(vUnits);
	unguardexec;
}

void XClipWindow::execResetUnitSize(FFrame& Stack, RESULT_DECL)
{
	guard(XClipWindow::execResetUnitSize);
	P_FINISH;
	ResetUnitSize();
	unguardexec;
}

void XClipWindow::execResetUnitWidth(FFrame& Stack, RESULT_DECL)
{
	guard(XClipWindow::execResetUnitWidth);
	P_FINISH;
	ResetUnitWidth();
	unguardexec;
}

void XClipWindow::execResetUnitHeight(FFrame& Stack, RESULT_DECL)
{
	guard(XClipWindow::execResetUnitHeight);
	P_FINISH;
	ResetUnitHeight();
	unguardexec;
}

void XClipWindow::execGetUnitSize(FFrame& Stack, RESULT_DECL)
{
	guard(XClipWindow::execGetUnitSize);
	P_GET_INT_REF(pAreaHSize);
	P_GET_INT_REF(pAreaVSize);
	P_GET_INT_REF(pChildHSize);
	P_GET_INT_REF(ChildVSize);
	P_FINISH;
	GetUnitSize(pAreaHSize, pAreaVSize, pChildHSize, ChildVSize);
	unguardexec;
}

void XClipWindow::execForceChildSize(FFrame& Stack, RESULT_DECL)
{
	guard(XClipWindow::execForceChildSize);
	P_GET_UBOOL_OPTX(bNewForceChildWidth,TRUE);
	P_GET_UBOOL_OPTX(bNewForceChildHeight,TRUE);
	P_FINISH;
	ForceChildSize(bNewForceChildWidth, bNewForceChildHeight);
	unguardexec;
}

void XClipWindow::execEnableSnapToUnits(FFrame& Stack, RESULT_DECL)
{
	guard(XClipWindow::execEnableSnapToUnits);
	P_GET_UBOOL_OPTX(bNewSnapToUnits,TRUE);
	P_FINISH;
	EnableSnapToUnits(bNewSnapToUnits);
	unguardexec;
}

void XClipWindow::execGetChild(FFrame& Stack, RESULT_DECL)
{
	guard(XClipWindow::execGetChild);
	P_FINISH;
	*(XWindow**)Result = GetChild();
	unguardexec;
}

void XScrollAreaWindow::execEnableScrolling(FFrame& Stack, RESULT_DECL)
{
	guard(XScrollAreaWindow::execEnableScrolling);
	P_GET_UBOOL_OPTX(bHScrolling,TRUE);
	P_GET_UBOOL_OPTX(bVScrolling,TRUE);
	P_FINISH;
	EnableScrolling(bHScrolling, bVScrolling);
	unguardexec;
}

void XScrollAreaWindow::execSetScrollbarDistance(FFrame& Stack, RESULT_DECL)
{
	guard(XScrollAreaWindow::execSetScrollbarDistance);
	P_GET_FLOAT(newDistance);
	P_FINISH;
	SetScrollbarDistance(newDistance);
	unguardexec;
}

void XScrollAreaWindow::execSetAreaMargins(FFrame& Stack, RESULT_DECL)
{
	guard(XScrollAreaWindow::execSetAreaMargins);
	P_GET_FLOAT(newMarginWidth);
	P_GET_FLOAT(newMarginHeight);
	P_FINISH;
	SetMargins(newMarginWidth, newMarginHeight);
	unguardexec;
}

void XScrollAreaWindow::execAutoHideScrollbars(FFrame& Stack, RESULT_DECL)
{
	guard(XScrollAreaWindow::execAutoHideScrollbars);
	P_GET_UBOOL_OPTX(bHide,TRUE);
	P_FINISH;
	AutoHideScrollbars(bHide);
	unguardexec;
}

void XListWindow::execIndexToRowId(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execIndexToRowId);
	P_GET_INT(index);
	P_FINISH;
	*(INT*)Result = IndexToRowId(index);
	unguardexec;
}

void XListWindow::execRowIdToIndex(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execRowIdToIndex);
	P_GET_INT(rowId);
	P_FINISH;
	*(INT*)Result = RowIdToIndex(rowId);
	unguardexec;
}

void XListWindow::execSetRowClientInt(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetRowClientInt);
	P_GET_INT(rowId);
	P_GET_INT(clientInt);
	P_FINISH;
	SetClientData(rowId, clientInt);
	unguardexec;
}

void XListWindow::execGetRowClientInt(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetRowClientInt);
	P_GET_INT(rowId);
	P_FINISH;
	*(INT*)Result = GetClientData(rowId);
	unguardexec;
}

void XListWindow::execSetRowClientObject(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetRowClientObject);
	P_GET_INT(rowId);
	P_GET_OBJECT(UObject,clientObj);
	P_FINISH;
	SetClientData(rowId, (INT)clientObj);
	unguardexec;
}

void XListWindow::execGetRowClientObject(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetRowClientObject);
	P_GET_INT(rowId);
	P_FINISH;
	*(UObject**)Result = (UObject*)GetClientData(rowId);
	unguardexec;
}

void XListWindow::execAddRow(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execAddRow);
	P_GET_STR(rowStr);
	P_GET_INT_OPTX(clientData,0);
	P_FINISH;
	*(INT*)Result = AddRow(*rowStr, clientData);
	unguardexec;
}

void XListWindow::execDeleteRow(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execDeleteRow);
	P_GET_INT(rowId);
	P_FINISH;
	DeleteRow(rowId);
	unguardexec;
}

void XListWindow::execModifyRow(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execModifyRow);
	P_GET_INT(rowId);
	P_GET_STR(rowStr);
	P_FINISH;
	ModifyRow(rowId, *rowStr);
	unguardexec;
}

void XListWindow::execDeleteAllRows(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execDeleteAllRows);
	P_FINISH;
	DeleteAllRows();
	unguardexec;
}

void XListWindow::execSetField(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetField);
	P_GET_INT(rowId);
	P_GET_INT(colIndex);
	P_GET_STR(fieldStr);
	P_FINISH;
	SetField(rowId, colIndex, *fieldStr);
	unguardexec;
}

void XListWindow::execGetField(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetField);
	P_GET_INT(rowId);
	P_GET_INT(colIndex);
	P_FINISH;
	*(FString*)Result = GetField(rowId, colIndex);
	unguardexec;
}

void XListWindow::execSetFieldValue(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetFieldValue);
	P_GET_INT(rowId);
	P_GET_INT(colIndex);
	P_GET_FLOAT(newValue);
	P_FINISH;
	SetFieldValue(rowId, colIndex, newValue);
	unguardexec;
}

void XListWindow::execGetFieldValue(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetFieldValue);
	P_GET_INT(rowId);
	P_GET_INT(colIndex);
	P_FINISH;
	*(FLOAT*)Result = GetFieldValue(rowId, colIndex);
	unguardexec;
}

void XListWindow::execGetNumRows(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetNumRows);
	P_FINISH;
	*(INT*)Result = GetNumRows();
	unguardexec;
}

void XListWindow::execGetNumSelectedRows(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetNumSelectedRows);
	P_FINISH;
	*(INT*)Result = GetNumSelectedRows();
	unguardexec;
}

void XListWindow::execSelectRow(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSelectRow);
	P_GET_INT(rowId);
	P_GET_UBOOL_OPTX(bSelect,TRUE);
	P_FINISH;
	SelectRow(rowId, bSelect);
	unguardexec;
}

void XListWindow::execSelectAllRows(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSelectAllRows);
	P_GET_UBOOL_OPTX(bSelect,TRUE);
	P_FINISH;
	SelectAllRows(bSelect);
	unguardexec;
}

void XListWindow::execSelectToRow(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSelectToRow);
	P_GET_INT(rowId);
	P_GET_UBOOL_OPTX(bClearRows,TRUE);
	P_GET_UBOOL_OPTX(bInvert,FALSE);
	P_GET_UBOOL_OPTX(bSpanRows,FALSE);
	P_FINISH;
	SelectToRow(rowId, bClearRows, bInvert, bSpanRows);
	unguardexec;
}

void XListWindow::execToggleRowSelection(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execToggleRowSelection);
	P_GET_INT(rowId);
	P_FINISH;
	ToggleRowSelection(rowId);
	unguardexec;
}

void XListWindow::execIsRowSelected(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execIsRowSelected);
	P_GET_INT(rowId);
	P_FINISH;
	*(UBOOL*)Result = IsRowSelected(rowId);
	unguardexec;
}

void XListWindow::execGetSelectedRow(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetSelectedRow);
	P_FINISH;
	*(INT*)Result = GetSelectedRow();
	unguardexec;
}

void XListWindow::execMoveRow(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execMoveRow);
	P_GET_BYTE(move);
	P_GET_UBOOL_OPTX(bSelect,TRUE);
	P_GET_UBOOL_OPTX(bClearRows,TRUE);
	P_GET_UBOOL_OPTX(bDrag,FALSE);
	P_FINISH;
	MoveRow((EMoveList)move, bSelect, bClearRows, bDrag);
	unguardexec;
}

void XListWindow::execSetRow(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetRow);
	P_GET_INT(rowId);
	P_GET_UBOOL_OPTX(bSelect,TRUE);
	P_GET_UBOOL_OPTX(bClearRows,TRUE);
	P_GET_UBOOL_OPTX(bDrag,FALSE);
	P_FINISH;
	SetRow(rowId, bSelect, bClearRows, bDrag);
	unguardexec;
}

void XListWindow::execSetFocusRow(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetFocusRow);
	P_GET_INT(rowId);
	P_GET_UBOOL_OPTX(bMoveTo,TRUE);
	P_GET_UBOOL_OPTX(bAnchor,TRUE);
	P_FINISH;
	SetFocusRow(rowId, bMoveTo, bAnchor);
	unguardexec;
}

void XListWindow::execGetFocusRow(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetFocusRow);
	P_FINISH;
	*(INT*)Result = GetFocusRow();
	unguardexec;
}

void XListWindow::execSetNumColumns(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetNumColumns);
	P_GET_INT(newCols);
	P_FINISH;
	SetNumColumns(newCols);
	unguardexec;
}

void XListWindow::execGetNumColumns(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetNumColumns);
	P_FINISH;
	*(INT*)Result = GetNumColumns();
	unguardexec;
}

void XListWindow::execResizeColumns(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execResizeColumns);
	P_GET_UBOOL_OPTX(bExpandOnly,FALSE);
	P_FINISH;
	ResizeColumns(bExpandOnly);
	unguardexec;
}

void XListWindow::execSetColumnTitle(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetColumnTitle);
	P_GET_INT(colIndex);
	P_GET_STR(title);
	P_FINISH;
	SetColumnTitle(colIndex, *title);
	unguardexec;
}

void XListWindow::execGetColumnTitle(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetColumnTitle);
	P_GET_INT(colIndex);
	P_FINISH;
	*(FString*)Result = GetColumnTitle(colIndex);
	unguardexec;
}

void XListWindow::execSetColumnWidth(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetColumnWidth);
	P_GET_INT(colIndex);
	P_GET_FLOAT(newWidth);
	P_FINISH;
	SetColumnWidth(colIndex, newWidth);
	unguardexec;
}

void XListWindow::execGetColumnWidth(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetColumnWidth);
	P_GET_INT(colIndex);
	P_FINISH;
	*(FLOAT*)Result = GetColumnWidth(colIndex);
	unguardexec;
}

void XListWindow::execSetColumnAlignment(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetColumnAlignment);
	P_GET_INT(colIndex);
	P_GET_BYTE(newAlign);
	P_FINISH;
	SetColumnAlignment(colIndex, (EHAlign)newAlign);
	unguardexec;
}

void XListWindow::execGetColumnAlignment(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetColumnAlignment);
	P_GET_INT(colIndex);
	P_FINISH;
	*(BYTE*)Result = GetColumnAlignment(colIndex);
	unguardexec;
}

void XListWindow::execSetColumnColor(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetColumnColor);
	P_GET_INT(colIndex);
	P_GET_STRUCT(FColor,newColor);
	P_FINISH;
	SetColumnColor(colIndex, newColor);
	unguardexec;
}

void XListWindow::execGetColumnColor(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetColumnColor);
	P_GET_INT(colIndex);
	P_GET_STRUCT_REF(FColor,colColor);
	P_FINISH;
	*colColor = GetColumnColor(colIndex);
	unguardexec;
}

void XListWindow::execSetColumnFont(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetColumnFont);
	P_GET_INT(colIndex);
	P_GET_OBJECT(UFont,newFont);
	P_FINISH;
	SetColumnFont(colIndex, newFont);
	unguardexec;
}

void XListWindow::execGetColumnFont(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetColumnFont);
	P_GET_INT(colIndex);
	P_FINISH;
	*(UFont**)Result = GetColumnFont(colIndex);
	unguardexec;
}

void XListWindow::execSetColumnType(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetColumnType);
	P_GET_INT(colIndex);
	P_GET_BYTE(newType);
	P_GET_STR_OPTX(newFmt,TEXT(""));
	P_FINISH;
	SetColumnType(colIndex, (EColumnType)newType, *newFmt);
	unguardexec;
}

void XListWindow::execGetColumnType(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetColumnType);
	P_GET_INT(colIndex);
	P_FINISH;
	*(BYTE*)Result = GetColumnType(colIndex);
	unguardexec;
}

void XListWindow::execHideColumn(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execHideColumn);
	P_GET_INT(colIndex);
	P_GET_UBOOL_OPTX(bHide,TRUE);
	P_FINISH;
	HideColumn(colIndex, bHide);
	unguardexec;
}

void XListWindow::execIsColumnHidden(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execIsColumnHidden);
	P_GET_INT(colIndex);
	P_FINISH;
	*(UBOOL*)Result = IsColumnHidden(colIndex);
	unguardexec;
}

void XListWindow::execSetSortColumn(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetSortColumn);
	P_GET_INT(colIndex);
	P_GET_UBOOL_OPTX(bReverse,0);
	P_GET_UBOOL_OPTX(bCaseSensitive,0);
	P_FINISH;
	SetSortColumn(colIndex, bReverse, bCaseSensitive);
	unguardexec;
}

void XListWindow::execAddSortColumn(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execAddSortColumn);
	P_GET_INT(colIndex);
	P_GET_UBOOL_OPTX(bReverse,0);
	P_GET_UBOOL_OPTX(bCaseSensitive,0);
	P_FINISH;
	AddSortColumn(colIndex, bReverse, bCaseSensitive);
	unguardexec;
}

void XListWindow::execRemoveSortColumn(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execRemoveSortColumn);
	P_GET_INT(colIndex);
	P_FINISH;
	RemoveSortColumn(colIndex);
	unguardexec;
}

void XListWindow::execResetSortColumns(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execResetSortColumns);
	P_GET_UBOOL_OPTX(bSort,TRUE);
	P_FINISH;
	ResetSortColumns(bSort);
	unguardexec;
}

void XListWindow::execSort(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSort);
	P_FINISH;
	Sort();
	unguardexec;
}

void XListWindow::execEnableHotKeys(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execEnableHotKeys);
	P_GET_UBOOL_OPTX(bEnable,TRUE);
	P_FINISH;
	EnableHotKeys(bEnable);
	unguardexec;
}

void XListWindow::execSetHotKeyColumn(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetHotKeyColumn);
	P_GET_INT(colIndex);
	P_FINISH;
	SetHotKeyColumn(colIndex);
	unguardexec;
}

void XListWindow::execEnableAutoSort(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execEnableAutoSort);
	P_GET_UBOOL_OPTX(bAutoSort,TRUE);
	P_FINISH;
	EnableAutoSort(bAutoSort);
	unguardexec;
}

void XListWindow::execIsAutoSortEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execIsAutoSortEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsAutoSortEnabled();
	unguardexec;
}

void XListWindow::execEnableAutoExpandColumns(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execEnableAutoExpandColumns);
	P_GET_UBOOL_OPTX(bAutoExpand,TRUE);
	P_FINISH;
	EnableAutoExpandColumns(bAutoExpand);
	unguardexec;
}

void XListWindow::execIsAutoExpandColumnsEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execIsAutoExpandColumnsEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsAutoExpandColumnsEnabled();
	unguardexec;
}

void XListWindow::execEnableMultiSelect(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execEnableMultiSelect);
	P_GET_UBOOL_OPTX(bEnableMultiSelect,TRUE);
	P_FINISH;
	EnableMultiSelect(bEnableMultiSelect);
	unguardexec;
}

void XListWindow::execIsMultiSelectEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execIsMultiSelectEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsMultiSelectEnabled();
	unguardexec;
}

void XListWindow::execSetFieldMargins(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetFieldMargins);
	P_GET_FLOAT(newMarginWidth);
	P_GET_FLOAT(newMarginHeight);
	P_FINISH;
	SetFieldMargins(newMarginWidth, newMarginHeight);
	unguardexec;
}

void XListWindow::execGetFieldMargins(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetFieldMargins);
	P_GET_FLOAT_REF(marginWidth);
	P_GET_FLOAT_REF(marginHeight);
	P_FINISH;
	GetFieldMargins(marginWidth, marginHeight);
	unguardexec;
}

void XListWindow::execGetPageSize(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execGetPageSize);
	P_FINISH;
	*(INT*)Result = GetPageSize();
	unguardexec;
}

void XListWindow::execSetDelimiter(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetDelimiter);
	P_GET_STR(newDelimiter);
	P_FINISH;
	const TCHAR* DelimText = *newDelimiter;
	SetDelimiter((DelimText != NULL && DelimText[0] != 0) ? DelimText[0] : 0);
	unguardexec;
}

void XListWindow::execSetHighlightTextColor(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetHighlightTextColor);
	P_GET_STRUCT(FColor,newColor);
	P_FINISH;
	SetHighlightTextColor(newColor);
	unguardexec;
}

void XListWindow::execSetHighlightTexture(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetHighlightTexture);
	P_GET_OBJECT(UTexture,newTexture);
	P_FINISH;
	SetHighlightTexture(newTexture);
	unguardexec;
}

void XListWindow::execSetHighlightColor(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetHighlightColor);
	P_GET_STRUCT(FColor,newColor);
	P_FINISH;
	SetHighlightColor(newColor);
	unguardexec;
}

void XListWindow::execSetFocusTexture(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetFocusTexture);
	P_GET_OBJECT(UTexture,newTexture);
	P_FINISH;
	SetFocusTexture(newTexture);
	unguardexec;
}

void XListWindow::execSetFocusColor(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetFocusColor);
	P_GET_STRUCT(FColor,newColor);
	P_FINISH;
	SetFocusColor(newColor);
	unguardexec;
}

void XListWindow::execSetFocusThickness(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetFocusThickness);
	P_GET_FLOAT(newThickness);
	P_FINISH;
	SetFocusThickness(newThickness);
	unguardexec;
}

void XListWindow::execShowFocusRow(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execShowFocusRow);
	P_FINISH;
	ShowFocusRow();
	unguardexec;
}

void XListWindow::execSetListSounds(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execSetListSounds);
	P_GET_OBJECT_OPTX(USound,activateSound,NULL);
	P_GET_OBJECT_OPTX(USound,moveSound,NULL);
	P_FINISH;
	SetListSounds(activateSound, moveSound);
	unguardexec;
}

void XListWindow::execPlayListSound(FFrame& Stack, RESULT_DECL)
{
	guard(XListWindow::execPlayListSound);
	P_GET_OBJECT(USound,listSound);
	P_GET_FLOAT_OPTX(volume,-1.0f);
	P_GET_FLOAT_OPTX(pitch,1.0f);
	P_FINISH;
	PlayListSound(listSound, volume, pitch);
	unguardexec;
}

void XCheckboxWindow::execSetCheckboxTextures(FFrame& Stack, RESULT_DECL)
{
	guard(XCheckboxWindow::execSetCheckboxTextures);
	P_GET_OBJECT_OPTX(UTexture,toggleOff,NULL);
	P_GET_OBJECT_OPTX(UTexture,toggleOn,NULL);
	P_GET_FLOAT_OPTX(textureWidth,0);
	P_GET_FLOAT_OPTX(textureHeight,0);
	P_FINISH;
	SetCheckboxTextures(toggleOff, toggleOn, textureWidth, textureHeight);
	unguardexec;
}

void XCheckboxWindow::execSetCheckboxSpacing(FFrame& Stack, RESULT_DECL)
{
	guard(XCheckboxWindow::execSetCheckboxSpacing);
	P_GET_FLOAT(newSpacing);
	P_FINISH;
	SetCheckboxSpacing(newSpacing);
	unguardexec;
}

void XCheckboxWindow::execShowCheckboxOnRightSide(FFrame& Stack, RESULT_DECL)
{
	guard(XCheckboxWindow::execShowCheckboxOnRightSide);
	P_GET_UBOOL_OPTX(bRight,TRUE);
	P_FINISH;
	ShowCheckboxOnRightSide(bRight);
	unguardexec;
}

void XCheckboxWindow::execSetCheckboxStyle(FFrame& Stack, RESULT_DECL)
{
	guard(XCheckboxWindow::execSetCheckboxStyle);
	P_GET_BYTE(newStyle);
	P_FINISH;
	SetCheckboxStyle(newStyle);
	unguardexec;
}

void XCheckboxWindow::execSetCheckboxColor(FFrame& Stack, RESULT_DECL)
{
	guard(XCheckboxWindow::execSetCheckboxColor);
	P_GET_STRUCT(FColor,newColor);
	P_FINISH;
	SetCheckboxColor(newColor);
	unguardexec;
}

void XEditWindow::execMoveInsertionPoint(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execMoveInsertionPoint);
	P_GET_BYTE(moveInsert);
	P_GET_UBOOL_OPTX(bDrag,0);
	P_FINISH;
	MoveInsertionPoint((EMoveInsert)moveInsert, bDrag);
	unguardexec;
}

void XEditWindow::execSetInsertionPoint(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execSetInsertionPoint);
	P_GET_INT(newPos);
	P_GET_UBOOL_OPTX(bDrag,0);
	P_FINISH;
	SetInsertionPoint(newPos, bDrag);
	unguardexec;
}

void XEditWindow::execGetInsertionPoint(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execGetInsertionPoint);
	P_FINISH;
	*(INT*)Result = GetInsertionPoint();
	unguardexec;
}

void XEditWindow::execSetSelectedArea(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execSetSelectedArea);
	P_GET_INT(startPos);
	P_GET_INT(count);
	P_FINISH;
	SetSelectedArea(startPos, count);
	unguardexec;
}

void XEditWindow::execGetSelectedArea(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execGetSelectedArea);
	P_GET_INT_REF(startPos);
	P_GET_INT_REF(count);
	P_FINISH;
	GetSelectedArea(startPos, count);
	unguardexec;
}

void XEditWindow::execEnableEditing(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execEnableEditing);
	P_GET_UBOOL_OPTX(bEdit,TRUE);
	P_FINISH;
	EnableEditing(bEdit);
	unguardexec;
}

void XEditWindow::execIsEditingEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execIsEditingEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsEditingEnabled();
	unguardexec;
}

void XEditWindow::execEnableSingleLineEditing(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execEnableSingleLineEditing);
	P_GET_UBOOL_OPTX(bSingle,TRUE);
	P_FINISH;
	EnableSingleLineEditing(bSingle);
	unguardexec;
}

void XEditWindow::execIsSingleLineEditingEnabled(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execIsSingleLineEditingEnabled);
	P_FINISH;
	*(UBOOL*)Result = IsSingleLineEditingEnabled();
	unguardexec;
}

void XEditWindow::execEnableUppercaseOnly(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execEnableUppercaseOnly);
	P_GET_UBOOL_OPTX(bUppercase,TRUE);
	P_FINISH;
	EnableUppercaseOnly(bUppercase);
	unguardexec;
}

void XEditWindow::execClearTextChangedFlag(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execClearTextChangedFlag);
	P_FINISH;
	ClearTextChangedFlag();
	unguardexec;
}

void XEditWindow::execSetTextChangedFlag(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execSetTextChangedFlag);
	P_GET_UBOOL_OPTX(bSet,TRUE);
	P_FINISH;
	SetTextChangedFlag(bSet);
	unguardexec;
}

void XEditWindow::execHasTextChanged(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execHasTextChanged);
	P_FINISH;
	*(UBOOL*)Result = HasTextChanged();
	unguardexec;
}

void XEditWindow::execSetMaxSize(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execSetMaxSize);
	P_GET_INT(newMaxSize);
	P_FINISH;
	SetMaxSize(newMaxSize);
	unguardexec;
}

void XEditWindow::execSetMaxUndos(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execSetMaxUndos);
	P_GET_INT(newMaxUndos);
	P_FINISH;
	SetMaxUndos(newMaxUndos);
	unguardexec;
}

void XEditWindow::execInsertText(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execInsertText);
	P_GET_STR_OPTX(insertText,TEXT(""));
	P_GET_UBOOL_OPTX(bUndo,0);
	P_GET_UBOOL_OPTX(bSelect,FALSE);
	P_FINISH;
	*(UBOOL*)Result = InsertText(ConvertScriptString(*insertText), bUndo, bSelect);
	unguardexec;
}

void XEditWindow::execDeleteChar(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execDeleteChar);
	P_GET_UBOOL_OPTX(bBefore,0);
	P_GET_UBOOL_OPTX(bUndo,0);
	P_FINISH;
	DeleteChar(bBefore, bUndo);
	unguardexec;
}

void XEditWindow::execSetInsertionPointBlinkRate(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execSetInsertionPointBlinkRate);
	P_GET_FLOAT_OPTX(blinkStart,0.75f);
	P_GET_FLOAT_OPTX(blinkPeriod,1.0f);
	P_FINISH;
	SetInsertionPointBlinkRate(blinkStart, blinkPeriod);
	unguardexec;
}

void XEditWindow::execSetInsertionPointTexture(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execSetInsertionPointTexture);
	P_GET_OBJECT_OPTX(UTexture,newTexture,NULL);
	P_GET_STRUCT_OPTX(FColor,newColor,FColor(255,255,255));
	P_FINISH;
	SetInsertionPointTexture(newTexture, newColor);
	unguardexec;
}

void XEditWindow::execSetInsertionPointType(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execSetInsertionPointType);
	P_GET_BYTE(newType);
	P_GET_FLOAT_OPTX(prefWidth,0);
	P_GET_FLOAT_OPTX(prefHeight,0);
	P_FINISH;
	SetInsertionPointType((EInsertionPointType)newType, prefWidth, prefHeight);
	unguardexec;
}

void XEditWindow::execSetSelectedAreaTexture(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execSetSelectedAreaTexture);
	P_GET_OBJECT_OPTX(UTexture,newTexture,NULL);
	P_GET_STRUCT_OPTX(FColor,newColor,FColor(192,192,192));
	P_FINISH;
	SetSelectedAreaTexture(newTexture, newColor);
	unguardexec;
}

void XEditWindow::execSetSelectedAreaTextColor(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execSetSelectedAreaTextColor);
	P_GET_STRUCT_OPTX(FColor,newColor,FColor(0,0,0));
	P_FINISH;
	SetSelectedAreaTextColor(newColor);
	unguardexec;
}

void XEditWindow::execSetEditCursor(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execSetEditCursor);
	P_GET_OBJECT_OPTX(UTexture,newCursor,NULL);
	P_GET_OBJECT_OPTX(UTexture,newCursorShadow,NULL);
	P_GET_STRUCT_OPTX(FColor,newColor,FColor(255,255,255));
	P_FINISH;
	SetEditCursor(newCursor, newCursorShadow, newColor);
	unguardexec;
}

void XEditWindow::execUndo(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execUndo);
	P_FINISH;
	Undo();
	unguardexec;
}

void XEditWindow::execRedo(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execRedo);
	P_FINISH;
	Redo();
	unguardexec;
}

void XEditWindow::execClearUndo(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execClearUndo);
	P_FINISH;
	ClearUndo();
	unguardexec;
}

void XEditWindow::execCopy(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execCopy);
	P_FINISH;
	Copy();
	unguardexec;
}

void XEditWindow::execCut(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execCut);
	P_FINISH;
	Cut();
	unguardexec;
}

void XEditWindow::execPaste(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execPaste);
	P_FINISH;
	Paste();
	unguardexec;
}

void XEditWindow::execSetEditSounds(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execSetEditSounds);
	P_GET_OBJECT_OPTX(USound,typeSound,NULL);
	P_GET_OBJECT_OPTX(USound,deleteSound,NULL);
	P_GET_OBJECT_OPTX(USound,enterSound,NULL);
	P_GET_OBJECT_OPTX(USound,moveSound,NULL);
	P_FINISH;
	SetEditSounds(typeSound, deleteSound, enterSound, moveSound);
	unguardexec;
}

void XEditWindow::execPlayEditSound(FFrame& Stack, RESULT_DECL)
{
	guard(XEditWindow::execPlayEditSound);
	P_GET_OBJECT(USound,playSound);
	P_GET_FLOAT_OPTX(volume,-1.0f);
	P_GET_FLOAT_OPTX(pitch,1.0f);
	P_FINISH;
	PlayEditSound(playSound, volume, pitch);
	unguardexec;
}

void XComputerWindow::execSetBackgroundTextures(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetBackgroundTextures);
	P_GET_OBJECT(UTexture,backTexture1);
	P_GET_OBJECT(UTexture,backTexture2);
	P_GET_OBJECT(UTexture,backTexture3);
	P_GET_OBJECT(UTexture,backTexture4);
	P_GET_OBJECT(UTexture,backTexture5);
	P_GET_OBJECT(UTexture,backTexture6);
	P_FINISH;
	SetBackgroundTextures(backTexture1, backTexture2, backTexture3, backTexture4, backTexture5, backTexture6);
	unguardexec;
}

void XComputerWindow::execSetTextSize(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetTextSize);
	P_GET_INT(newCols);
	P_GET_INT(newRows);
	P_FINISH;
	SetTextSize(newCols, newRows);
	unguardexec;
}

void XComputerWindow::execSetTextWindowPosition(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetTextWindowPosition);
	P_GET_INT(newX);
	P_GET_INT(newY);
	P_FINISH;
	SetTextWindowPosition(newX, newY);
	unguardexec;
}

void XComputerWindow::execSetTextFont(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetTextFont);
	P_GET_OBJECT(UFont,newFont);
	P_GET_INT(newFontWidth);
	P_GET_INT(newFontHeight);
	P_GET_STRUCT(FColor,newFontColor);
	P_FINISH;
	SetTextFont(newFont, newFontWidth, newFontHeight, newFontColor);
	unguardexec;
}

void XComputerWindow::execSetFontColor(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetFontColor);
	P_GET_STRUCT(FColor,newFontColor);
	P_FINISH;
	SetTextColor(newFontColor);
	unguardexec;
}

void XComputerWindow::execSetTextTiming(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetTextTiming);
	P_GET_FLOAT(newTiming);
	P_FINISH;
	SetTextTiming(newTiming);
	unguardexec;
}

void XComputerWindow::execSetFadeSpeed(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetFadeSpeed);
	P_GET_FLOAT(fadeSpeed);
	P_FINISH;
	SetFadeSpeed(fadeSpeed);
	unguardexec;
}

void XComputerWindow::execSetCursorTexture(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetCursorTexture);
	P_GET_OBJECT(UTexture,newCursorTexture);
	P_GET_INT_OPTX(newCursorWidth,cursorWidth);
	P_GET_INT_OPTX(newCursorHeight,2);
	P_FINISH;
	SetCursorTexture(newCursorTexture, newCursorWidth, newCursorHeight);
	unguardexec;
}

void XComputerWindow::execSetCursorColor(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetCursorColor);
	P_GET_STRUCT(FColor,newCursorColor);
	P_FINISH;
	SetCursorColor(newCursorColor);
	unguardexec;
}

void XComputerWindow::execSetCursorBlinkSpeed(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetCursorBlinkSpeed);
	P_GET_FLOAT(newBlinkSpeed);
	P_FINISH;
	SetCursorBlinkSpeed(newBlinkSpeed);
	unguardexec;
}

void XComputerWindow::execShowTextCursor(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execShowTextCursor);
	P_GET_UBOOL_OPTX(bShow,TRUE);
	P_FINISH;
	ShowTextCursor(bShow);
	unguardexec;
}

void XComputerWindow::execSetTextSound(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetTextSound);
	P_GET_OBJECT(USound,newTextSound);
	P_FINISH;
	SetTextSound(newTextSound);
	unguardexec;
}

void XComputerWindow::execSetTypingSound(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetTypingSound);
	P_GET_OBJECT(USound,newTypingSound);
	P_FINISH;
	SetTypingSound(newTypingSound);
	unguardexec;
}

void XComputerWindow::execSetComputerSoundVolume(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetComputerSoundVolume);
	P_GET_FLOAT(newSoundVolume);
	P_FINISH;
	SetComputerSoundVolume(newSoundVolume);
	unguardexec;
}

void XComputerWindow::execSetTypingSoundVolume(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetTypingSoundVolume);
	P_GET_FLOAT(newSoundVolume);
	P_FINISH;
	SetTypingSoundVolume(newSoundVolume);
	unguardexec;
}

void XComputerWindow::execClearScreen(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execClearScreen);
	P_FINISH;
	ClearScreen();
	unguardexec;
}

void XComputerWindow::execClearLine(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execClearLine);
	P_GET_INT(rowToClear);
	P_FINISH;
	ClearLine(rowToClear);
	unguardexec;
}

void XComputerWindow::execPrint(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execPrint);
	P_GET_STR(printText);
	P_GET_UBOOL_OPTX(bNewLine,TRUE);
	P_FINISH;
	FString localPrintText = *printText;
	Print(localPrintText, bNewLine);
	unguardexec;
}

void XComputerWindow::execPrintLn(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execPrintLn);
	P_FINISH;
	PrintLn();
	unguardexec;
}

void XComputerWindow::execGetInput(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execGetInput);
	P_GET_INT(maxLength);
	P_GET_STR(inputKey);
	P_GET_STR_OPTX(defaultInputString,TEXT(""));
	P_GET_STR_OPTX(inputMask,TEXT(""));
	P_FINISH;
	FString localInputKey = *inputKey;
	FString localDefaultInputString = *defaultInputString;
	FString localInputMask = *inputMask;
	GetInput(maxLength, localInputKey, localDefaultInputString, localInputMask);
	unguardexec;
}

void XComputerWindow::execGetChar(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execGetChar);
	P_GET_STR(inputKey);
	P_GET_UBOOL_OPTX(bEcho,TRUE);
	P_FINISH;
	FString localInputKey = *inputKey;
	GetChar(localInputKey, bEcho);
	unguardexec;
}

void XComputerWindow::execPrintGraphic(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execPrintGraphic);
	P_GET_OBJECT(UTexture,graphic);
	P_GET_INT(width);
	P_GET_INT(height);
	P_GET_INT_OPTX(posX,-1);
	P_GET_INT_OPTX(posY,-1);
	P_GET_UBOOL_OPTX(bStatic,0);
	P_GET_UBOOL_OPTX(bPixelPos,0);
	P_FINISH;
	PrintGraphic(graphic, width, height, posX, posY, bStatic, bPixelPos);
	unguardexec;
}

void XComputerWindow::execPlaySoundLater(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execPlaySoundLater);
	P_GET_OBJECT(USound,newSound);
	P_FINISH;
	PlaySoundLater(newSound);
	unguardexec;
}

void XComputerWindow::execSetTextPosition(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetTextPosition);
	P_GET_INT(posX);
	P_GET_INT(posY);
	P_FINISH;
	SetTextPosition(posX, posY);
	unguardexec;
}

void XComputerWindow::execIsBufferFlushed(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execIsBufferFlushed);
	P_FINISH;
	*(UBOOL*)Result = IsBufferFlushed();
	unguardexec;
}

void XComputerWindow::execPause(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execPause);
	P_GET_FLOAT_OPTX(pauseLength,-1.0f);
	P_FINISH;
	Pause(pauseLength);
	unguardexec;
}

void XComputerWindow::execResume(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execResume);
	P_FINISH;
	Resume();
	unguardexec;
}

void XComputerWindow::execIsPaused(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execIsPaused);
	P_FINISH;
	*(UBOOL*)Result = IsPaused();
	unguardexec;
}

void XComputerWindow::execSetThrottle(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execSetThrottle);
	P_GET_FLOAT(throttleModifier);
	P_FINISH;
	SetThrottle(throttleModifier);
	unguardexec;
}

void XComputerWindow::execGetThrottle(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execGetThrottle);
	P_FINISH;
	*(FLOAT*)Result = GetThrottle();
	unguardexec;
}

void XComputerWindow::execResetThrottle(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execResetThrottle);
	P_FINISH;
	ResetThrottle();
	unguardexec;
}

void XComputerWindow::execEnableWordWrap(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execEnableWordWrap);
	P_GET_UBOOL_OPTX(bNewWordWrap,TRUE);
	P_FINISH;
	EnableWordWrap(bNewWordWrap);
	unguardexec;
}

void XComputerWindow::execFadeOutText(FFrame& Stack, RESULT_DECL)
{
	guard(XComputerWindow::execFadeOutText);
	P_GET_FLOAT_OPTX(fadeDuration,2.0f);
	P_FINISH;
	FadeOutText(fadeDuration);
	unguardexec;
}
