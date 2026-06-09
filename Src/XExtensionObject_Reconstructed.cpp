/*=============================================================================
	XExtensionObject_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 01.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XExtensionObject);

void XExtensionObject::execStringToName(FFrame& Stack, RESULT_DECL)
{
	guard(XExtensionObject::execStringToName);

	P_GET_STR(Source);
	P_FINISH;

	const TCHAR* Text = *Source;
	if (Text == NULL)
	{
		Text = TEXT("");
	}

	INT BadIndex = 0;
	while (Text[BadIndex] != 0)
	{
		const TCHAR Ch = Text[BadIndex];

		UBOOL bLower = FALSE;
		UBOOL bUpper = FALSE;
		UBOOL bDigit = FALSE;
		UBOOL bExtra = FALSE;

		if (Ch >= TEXT('a') && Ch <= TEXT('z'))
		{
			bLower = TRUE;
		}
		if (Ch >= TEXT('A') && Ch <= TEXT('Z'))
		{
			bUpper = TRUE;
		}
		if (Ch >= TEXT('0') && Ch <= TEXT('9'))
		{
			bDigit = TRUE;
		}
		if (Ch == TEXT('_') || Ch == TEXT('-'))
		{
			bExtra = TRUE;
		}

		if (bLower == FALSE && bUpper == FALSE && bDigit == FALSE && bExtra == FALSE)
		{
			break;
		}

		BadIndex++;
	}

	if (Text[BadIndex] != 0)
	{
		GError->Logf(TEXT("StringToName: Invalid character 0x%04x in name \"%s\""), Text[BadIndex], Text);
	}

	*(FName*)Result = FName(Text, FNAME_Add);

	unguardexec;
}
