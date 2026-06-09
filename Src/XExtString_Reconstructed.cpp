/*=============================================================================
	XExtString_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 25 compile cleanup.

	Focus: ExtString behaviour: native serialized FStringNoInit storage,
	append capacity growth, 239-character paging, and script out-string slices.

	This is reconstruction-first source. It is intentionally explicit and
	behavioural; it is not yet a final VC98 drop-in translation unit.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XExtString);

static const INT ReconTextPartSize = TEXT_PART_SIZE;

static void ReconAssignString(FStringNoInit& Dest, const TCHAR* Source)
{
	guard(ReconAssignString);

	const TCHAR* SafeSource = Source;
	if (SafeSource == NULL)
		SafeSource = TEXT("");

	Dest = SafeSource;

	unguard;
}

static void ReconAppendString(FStringNoInit& Dest, const TCHAR* Source)
{
	guard(ReconAppendString);

	const TCHAR* SafeSource = Source;
	if (SafeSource == NULL)
		SafeSource = TEXT("");

	Dest += SafeSource;

	unguard;
}

/*-----------------------------------------------------------------------------
	XExtString.
-----------------------------------------------------------------------------*/

XExtString::XExtString()
: UObject()
, speechPage(0)
{
	guard(XExtString::XExtString);
	unguard;
}

void XExtString::Serialize(FArchive& Ar)
{
	guard(XExtString::Serialize);

	UObject::Serialize(Ar);
	Ar << text;

	unguard;
}

void XExtString::SetText(const TCHAR* NewText)
{
	guard(XExtString::SetText);
	ReconAssignString(text, NewText);
	unguard;
}

void XExtString::AppendText(const TCHAR* NewText)
{
	guard(XExtString::AppendText);
	ReconAppendString(text, NewText);
	unguard;
}

/*-----------------------------------------------------------------------------
	Native-facing behavioural helpers.
-----------------------------------------------------------------------------*/

#if 0
// Documentation-only helpers from the reconstruction notes.  They intentionally
// access protected members and are not part of the compile smoke-test.
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
