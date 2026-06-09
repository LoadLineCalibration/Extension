/*=============================================================================
	XFlagBase_Flags_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 12.

	Focus: Flag / FlagBase behaviour: hash table ordering, replacement policy,
	typed flag storage, expiration, iterators, and UnrealScript native bridge
	semantics.

	This is reconstruction-first source. It is intentionally explicit and
	behavioural; it is not yet a final VC98 drop-in translation unit.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(XFlag);
IMPLEMENT_CLASS(XFlagBool);
IMPLEMENT_CLASS(XFlagByte);
IMPLEMENT_CLASS(XFlagInt);
IMPLEMENT_CLASS(XFlagFloat);
IMPLEMENT_CLASS(XFlagName);
IMPLEMENT_CLASS(XFlagVector);
IMPLEMENT_CLASS(XFlagRotator);
IMPLEMENT_CLASS(XFlagBase);

/*-----------------------------------------------------------------------------
	Hash helpers.
-----------------------------------------------------------------------------*/

static TCHAR ReconFlagUpperChar(TCHAR Ch)
{
	guard(ReconFlagUpperChar);

	if ((Ch >= TEXT('a')) && (Ch <= TEXT('z')))
		return Ch - 32;

	return Ch;

	unguard;
}

static INT ReconCalcFlagHash(FName FlagName)
{
	guard(ReconCalcFlagHash);

	// The original constructor and FindName both calculate a CRC over the FName
	// text. Lowercase ASCII letters are folded to uppercase before feeding the
	// UTF-16 bytes through GCRCTable. This makes the hash case-insensitive for
	// ordinary latin flag names while FName equality still remains the final name
	// identity check.
	DWORD Hash = 0;
	const TCHAR* Text = *FlagName;

	if (Text == NULL)
		Text = TEXT("");

	for (const TCHAR* Scan = Text; *Scan != 0; Scan++)
	{
		TCHAR Ch = ReconFlagUpperChar(*Scan);

		DWORD First = (Hash >> 8) ^ GCRCTable[((BYTE)Ch) ^ ((BYTE)Hash)];
		Hash = (First >> 8) ^ GCRCTable[((BYTE)First) ^ ((BYTE)(Ch >> 8))];
	}

	return (INT)Hash;

	unguard;
}

static INT ReconFlagBucket(INT FlagHash)
{
	guard(ReconFlagBucket);
	return FlagHash & FLAG_HASH_MASK;
	unguard;
}

static INT ReconResolveFlagExpiration(INT DefaultExpiration, INT Expiration)
{
	guard(ReconResolveFlagExpiration);

	if (Expiration == -1)
		return DefaultExpiration;

	return Expiration;

	unguard;
}

static void ReconSetFlagExpiration(XFlag* Flag, INT Expiration)
{
	guard(ReconSetFlagExpiration);

	if (Flag != NULL || XReconGetCompatibilityBool(TEXT("bUseOriginalFlagMissingSetExpirationBug"), TRUE) == TRUE)
		Flag->SetExpiration(Expiration);

	unguard;
}


/*-----------------------------------------------------------------------------
	XFlag.
-----------------------------------------------------------------------------*/

XFlag::XFlag(XFlagBase* Base, FName FlagName, BYTE FlagType, INT Expiration)
: XExtensionObject()
, flagName(FlagName)
, flagHash(0)
, flagBase(NULL)
, nextFlag(NULL)
, flagType(FlagType)
, expiration(Expiration)
{
	guard(XFlag::XFlag);

	flagHash = ReconCalcFlagHash(flagName);

	if (Base != NULL)
		Base->AddFlag(this);

	unguard;
}

void XFlag::Destroy()
{
	guard(XFlag::Destroy);

	XExtensionObject::Destroy();

	XFlagBase* OldBase = flagBase;
	if (OldBase != NULL)
		OldBase->RemoveFlag(this);

	unguard;
}

void XFlag::SetExpiration(INT NewExpiration)
{
	guard(XFlag::SetExpiration);
	expiration = NewExpiration;
	unguard;
}

/*-----------------------------------------------------------------------------
	XFlagBase lifetime.
-----------------------------------------------------------------------------*/

XFlagBase::XFlagBase()
: XExtensionObject()
, defaultFlagExpiration(0)
{
	guard(XFlagBase::XFlagBase);

	for (INT i = 0; i < FLAG_HASH_SIZE; i++)
		flagHashTable[i] = NULL;

	unguard;
}

void XFlagBase::Destroy()
{
	guard(XFlagBase::Destroy);

	XExtensionObject::Destroy();

	for (INT i = 0; i < FLAG_HASH_SIZE; i++)
	{
		while (flagHashTable[i] != NULL)
		{
			XFlag* Flag = flagHashTable[i];
			RemoveFlag(Flag);
		}
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Name validation and lookup.
-----------------------------------------------------------------------------*/

void XFlagBase::CheckName(FName& FlagName)
{
	guard(XFlagBase::CheckName);

	const TCHAR* Text = *FlagName;
	if (Text == NULL)
		Text = TEXT("");

	for (INT i = 0; Text[i] != 0; i++)
	{
		TCHAR Ch = Text[i];

		UBOOL bLetter = ((Ch >= TEXT('a')) && (Ch <= TEXT('z'))) || ((Ch >= TEXT('A')) && (Ch <= TEXT('Z')));
		UBOOL bDigit = (Ch >= TEXT('0')) && (Ch <= TEXT('9'));
		UBOOL bUnderscore = Ch == TEXT('_');

		if ((bLetter == FALSE) && (bDigit == FALSE) && (bUnderscore == FALSE))
		{
			// Original calls GError->Logf here, not GWarn. This is intentionally
			// severe: invalid flag names are treated as programmer errors.
			GError->Logf(TEXT("CheckName: Invalid character 0x%04x in name \"%s\""), Ch, Text);
			break;
		}
	}

	unguard;
}

XFlag* XFlagBase::FindName(FName& FlagName, BYTE FlagType, XFlag**& ppPrevFlag)
{
	guard(XFlagBase::FindName);

	INT Hash = ReconCalcFlagHash(FlagName);
	XFlag** Link = &flagHashTable[ReconFlagBucket(Hash)];
	XFlag* Previous = NULL;
	XFlag* Current = *Link;
	XFlag* Found = NULL;

	while (Current != NULL)
	{
		// Buckets are sorted by full CRC hash ascending. For equal names, entries
		// are sorted by flag type. This lets lookup stop early.
		if (Current->flagHash > Hash)
			break;

		if ((Current->flagHash == Hash) && (Current->flagName == FlagName))
		{
			if (Current->flagType > FlagType)
				break;

			if (Current->flagType == FlagType)
			{
				Found = Current;
				break;
			}
		}

		Previous = Current;
		Current = Current->nextFlag;
	}

	if (Previous != NULL)
		ppPrevFlag = &Previous->nextFlag;
	else
		ppPrevFlag = Link;

	return Found;

	unguard;
}

UBOOL XFlagBase::FindFlag(XFlag* Flag, XFlag**& ppPrevFlag)
{
	guard(XFlagBase::FindFlag);

	if (Flag == NULL)
	{
		ppPrevFlag = NULL;
		return FALSE;
	}

	XFlag** Link = &flagHashTable[ReconFlagBucket(Flag->flagHash)];
	XFlag* Previous = NULL;
	XFlag* Current = *Link;

	while (Current != NULL)
	{
		if (Current == Flag)
		{
			if (Previous != NULL)
				ppPrevFlag = &Previous->nextFlag;
			else
				ppPrevFlag = Link;

			return TRUE;
		}

		Previous = Current;
		Current = Current->nextFlag;
	}

	if (Previous != NULL)
		ppPrevFlag = &Previous->nextFlag;
	else
		ppPrevFlag = Link;

	return FALSE;

	unguard;
}

XFlag* XFlagBase::GetFlag(FName FlagName, BYTE FlagType)
{
	guard(XFlagBase::GetFlag);

	XFlag** ppPrevFlag = NULL;
	return FindName(FlagName, FlagType, ppPrevFlag);

	unguard;
}

void XFlagBase::AddFlag(XFlag* NewFlag)
{
	guard(XFlagBase::AddFlag);

	if (NewFlag == NULL)
		return;

	if (NewFlag->flagBase == this)
		return;

	if (NewFlag->flagBase != NULL)
		NewFlag->flagBase->RemoveFlag(NewFlag);

	while (TRUE)
	{
		XFlag** ppPrevFlag = NULL;
		XFlag* Existing = FindName(NewFlag->flagName, NewFlag->flagType, ppPrevFlag);

		if (Existing == NULL)
		{
			NewFlag->nextFlag = *ppPrevFlag;
			*ppPrevFlag = NewFlag;
			NewFlag->flagBase = this;
			return;
		}

		if (Existing == NewFlag)
			return;

		// Original destroys an existing same-name/same-type flag before inserting
		// the new one. This makes AddFlag a replacement operation.
		delete Existing;
	}

	unguard;
}

void XFlagBase::RemoveFlag(XFlag* OldFlag)
{
	guard(XFlagBase::RemoveFlag);

	if (OldFlag == NULL)
		return;

	if (OldFlag->flagBase != this)
		return;

	XFlag** ppPrevFlag = NULL;
	if (FindFlag(OldFlag, ppPrevFlag) == TRUE)
	{
		*ppPrevFlag = OldFlag->nextFlag;
		OldFlag->nextFlag = NULL;
		OldFlag->flagBase = NULL;
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Typed setters.
-----------------------------------------------------------------------------*/

UBOOL XFlagBase::SetBool(FName FlagName, UBOOL NewValue, UBOOL bAdd, INT Expiration)
{
	guard(XFlagBase::SetBool);

	CheckName(FlagName);

	XFlagBool* Flag = (XFlagBool*)GetFlag(FlagName, FLAG_Bool);
	if (Flag != NULL)
	{
		Flag->SetValue(NewValue);
	}
	else if (bAdd == TRUE)
	{
		Flag = new(this, NAME_None, 0) XFlagBool(this, FlagName, NewValue, 0);
	}

	// Original decompile calls SetExpiration after the create/update branch.
	// It does not visibly guard the NULL case. Therefore SetX(..., bAdd=False)
	// on a missing flag looks like an original unsafe edge.
	ReconSetFlagExpiration(Flag, ReconResolveFlagExpiration(defaultFlagExpiration, Expiration));

	return Flag != NULL;

	unguard;
}

UBOOL XFlagBase::SetByte(FName FlagName, BYTE NewValue, UBOOL bAdd, INT Expiration)
{
	guard(XFlagBase::SetByte);

	CheckName(FlagName);

	XFlagByte* Flag = (XFlagByte*)GetFlag(FlagName, FLAG_Byte);
	if (Flag != NULL)
	{
		Flag->SetValue(NewValue);
	}
	else if (bAdd == TRUE)
	{
		Flag = new(this, NAME_None, 0) XFlagByte(this, FlagName, NewValue, 0);
	}

	ReconSetFlagExpiration(Flag, ReconResolveFlagExpiration(defaultFlagExpiration, Expiration));

	return Flag != NULL;

	unguard;
}

UBOOL XFlagBase::SetInt(FName FlagName, INT NewValue, UBOOL bAdd, INT Expiration)
{
	guard(XFlagBase::SetInt);

	CheckName(FlagName);

	XFlagInt* Flag = (XFlagInt*)GetFlag(FlagName, FLAG_Int);
	if (Flag != NULL)
	{
		Flag->SetValue(NewValue);
	}
	else if (bAdd == TRUE)
	{
		Flag = new(this, NAME_None, 0) XFlagInt(this, FlagName, NewValue, 0);
	}

	ReconSetFlagExpiration(Flag, ReconResolveFlagExpiration(defaultFlagExpiration, Expiration));

	return Flag != NULL;

	unguard;
}

UBOOL XFlagBase::SetFloat(FName FlagName, FLOAT NewValue, UBOOL bAdd, INT Expiration)
{
	guard(XFlagBase::SetFloat);

	CheckName(FlagName);

	XFlagFloat* Flag = (XFlagFloat*)GetFlag(FlagName, FLAG_Float);
	if (Flag != NULL)
	{
		Flag->SetValue(NewValue);
	}
	else if (bAdd == TRUE)
	{
		Flag = new(this, NAME_None, 0) XFlagFloat(this, FlagName, NewValue, 0);
	}

	ReconSetFlagExpiration(Flag, ReconResolveFlagExpiration(defaultFlagExpiration, Expiration));

	return Flag != NULL;

	unguard;
}

UBOOL XFlagBase::SetName(FName FlagName, FName NewValue, UBOOL bAdd, INT Expiration)
{
	guard(XFlagBase::SetName);

	CheckName(FlagName);

	XFlagName* Flag = (XFlagName*)GetFlag(FlagName, FLAG_Name);
	if (Flag != NULL)
	{
		Flag->SetValue(NewValue);
	}
	else if (bAdd == TRUE)
	{
		Flag = new(this, NAME_None, 0) XFlagName(this, FlagName, NewValue, 0);
	}

	ReconSetFlagExpiration(Flag, ReconResolveFlagExpiration(defaultFlagExpiration, Expiration));

	return Flag != NULL;

	unguard;
}

UBOOL XFlagBase::SetVector(FName FlagName, FVector NewValue, UBOOL bAdd, INT Expiration)
{
	guard(XFlagBase::SetVector);

	CheckName(FlagName);

	XFlagVector* Flag = (XFlagVector*)GetFlag(FlagName, FLAG_Vector);
	if (Flag != NULL)
	{
		Flag->SetValue(NewValue);
	}
	else if (bAdd == TRUE)
	{
		Flag = new(this, NAME_None, 0) XFlagVector(this, FlagName, NewValue, 0);
	}

	ReconSetFlagExpiration(Flag, ReconResolveFlagExpiration(defaultFlagExpiration, Expiration));

	return Flag != NULL;

	unguard;
}

UBOOL XFlagBase::SetRotator(FName FlagName, FRotator NewValue, UBOOL bAdd, INT Expiration)
{
	guard(XFlagBase::SetRotator);

	CheckName(FlagName);

	XFlagRotator* Flag = (XFlagRotator*)GetFlag(FlagName, FLAG_Rotator);
	if (Flag != NULL)
	{
		Flag->SetValue(NewValue);
	}
	else if (bAdd == TRUE)
	{
		Flag = new(this, NAME_None, 0) XFlagRotator(this, FlagName, NewValue, 0);
	}

	ReconSetFlagExpiration(Flag, ReconResolveFlagExpiration(defaultFlagExpiration, Expiration));

	return Flag != NULL;

	unguard;
}

/*-----------------------------------------------------------------------------
	Typed getters.
-----------------------------------------------------------------------------*/

UBOOL XFlagBase::GetBool(FName FlagName, UBOOL& Value)
{
	guard(XFlagBase::GetBool);

	XFlagBool* Flag = (XFlagBool*)GetFlag(FlagName, FLAG_Bool);
	if (Flag != NULL)
		Value = Flag->GetValue();

	return Flag != NULL;

	unguard;
}

UBOOL XFlagBase::GetByte(FName FlagName, BYTE& Value)
{
	guard(XFlagBase::GetByte);

	XFlagByte* Flag = (XFlagByte*)GetFlag(FlagName, FLAG_Byte);
	if (Flag != NULL)
		Value = Flag->GetValue();

	return Flag != NULL;

	unguard;
}

UBOOL XFlagBase::GetInt(FName FlagName, INT& Value)
{
	guard(XFlagBase::GetInt);

	XFlagInt* Flag = (XFlagInt*)GetFlag(FlagName, FLAG_Int);
	if (Flag != NULL)
		Value = Flag->GetValue();

	return Flag != NULL;

	unguard;
}

UBOOL XFlagBase::GetFloat(FName FlagName, FLOAT& Value)
{
	guard(XFlagBase::GetFloat);

	XFlagFloat* Flag = (XFlagFloat*)GetFlag(FlagName, FLAG_Float);
	if (Flag != NULL)
		Value = Flag->GetValue();

	return Flag != NULL;

	unguard;
}

UBOOL XFlagBase::GetName(FName FlagName, FName& Value)
{
	guard(XFlagBase::GetName);

	XFlagName* Flag = (XFlagName*)GetFlag(FlagName, FLAG_Name);
	if (Flag != NULL)
		Value = Flag->GetValue();

	return Flag != NULL;

	unguard;
}

UBOOL XFlagBase::GetVector(FName FlagName, FVector& Value)
{
	guard(XFlagBase::GetVector);

	XFlagVector* Flag = (XFlagVector*)GetFlag(FlagName, FLAG_Vector);
	if (Flag != NULL)
		Value = Flag->GetValue();

	return Flag != NULL;

	unguard;
}

UBOOL XFlagBase::GetRotator(FName FlagName, FRotator& Value)
{
	guard(XFlagBase::GetRotator);

	XFlagRotator* Flag = (XFlagRotator*)GetFlag(FlagName, FLAG_Rotator);
	if (Flag != NULL)
		Value = Flag->GetValue();

	return Flag != NULL;

	unguard;
}

/*-----------------------------------------------------------------------------
	Existence / deletion / expiration.
-----------------------------------------------------------------------------*/

UBOOL XFlagBase::CheckFlag(FName FlagName, BYTE FlagType)
{
	guard(XFlagBase::CheckFlag);
	return GetFlag(FlagName, FlagType) != NULL;
	unguard;
}

UBOOL XFlagBase::DeleteFlag(FName FlagName, BYTE FlagType)
{
	guard(XFlagBase::DeleteFlag);

	XFlag* Flag = GetFlag(FlagName, FlagType);
	if (Flag == NULL)
		return FALSE;

	delete Flag;
	return TRUE;

	unguard;
}

void XFlagBase::DeleteAllFlags()
{
	guard(XFlagBase::DeleteAllFlags);

	for (INT i = 0; i < FLAG_HASH_SIZE; i++)
	{
		while (flagHashTable[i] != NULL)
		{
			XFlag* Flag = flagHashTable[i];
			delete Flag;
		}
	}

	unguard;
}

void XFlagBase::SetExpiration(FName FlagName, BYTE FlagType, INT Expiration)
{
	guard(XFlagBase::SetExpiration);

	XFlag* Flag = GetFlag(FlagName, FlagType);
	if (Flag != NULL)
		Flag->expiration = Expiration;

	unguard;
}

INT XFlagBase::GetExpiration(FName FlagName, BYTE FlagType)
{
	guard(XFlagBase::GetExpiration);

	XFlag* Flag = GetFlag(FlagName, FlagType);
	if (Flag != NULL)
		return Flag->expiration;

	return -1;

	unguard;
}

void XFlagBase::SetDefaultExpiration(INT Expiration)
{
	guard(XFlagBase::SetDefaultExpiration);
	defaultFlagExpiration = Expiration;
	unguard;
}

void XFlagBase::DeleteExpiredFlags(INT Criteria)
{
	guard(XFlagBase::DeleteExpiredFlags);

	XFlagIterator Iterator = CreateIterator(MAX_FLAG_TYPE);
	if (Iterator == 0)
		return;

	FName FlagName = NAME_None;
	EFlagType FlagType = MAX_FLAG_TYPE;

	UBOOL bHasNext = GetNextFlag(Iterator, &FlagName, &FlagType);
	while (bHasNext == TRUE)
	{
		// Original advances the iterator before deleting the current flag. That is
		// the whole trick that prevents deletion from invalidating iteration.
		FName DeleteName = FlagName;
		EFlagType DeleteType = FlagType;

		bHasNext = GetNextFlag(Iterator, &FlagName, &FlagType);

		XFlag* Flag = GetFlag(DeleteName, DeleteType);
		if (Flag != NULL)
		{
			if (Flag->expiration != 0)
			{
				if (Flag->expiration <= Criteria)
					delete Flag;
			}
		}
	}

	DestroyIterator(Iterator);

	unguard;
}

/*-----------------------------------------------------------------------------
	Iterators.
-----------------------------------------------------------------------------*/

XFlagIterator XFlagBase::CreateIterator(EFlagType FlagType)
{
	guard(XFlagBase::CreateIterator);

	XFlagIteratorStruct* Iterator = (XFlagIteratorStruct*)appMalloc(sizeof(XFlagIteratorStruct), TEXT("New"));
	if (Iterator != NULL)
	{
		Iterator->flagType = FlagType;
		Iterator->curFlag = NULL;
		Iterator->hash = -1;
	}

	return (XFlagIterator)Iterator;

	unguard;
}

UBOOL XFlagBase::GetNextFlag(XFlagIterator IteratorId, FName* pName, EFlagType* pFlagType)
{
	guard(XFlagBase::GetNextFlag);

	FName ResultName = NAME_None;
	EFlagType ResultType = FLAG_Bool;
	UBOOL bResult = FALSE;

	XFlagIteratorStruct* Iterator = (XFlagIteratorStruct*)IteratorId;
	if (Iterator != NULL)
	{
		XFlag* Current = Iterator->curFlag;
		EFlagType WantedType = Iterator->flagType;
		INT Hash = Iterator->hash;

		if (Current != NULL)
			Current = Current->nextFlag;

		if (Hash < FLAG_HASH_SIZE)
		{
			XFlag** Bucket = &flagHashTable[Hash];

			while (Current == NULL)
			{
				Hash++;
				Bucket++;

				if (Hash >= FLAG_HASH_SIZE)
				{
					Current = NULL;
					bResult = FALSE;
					break;
				}

				Current = *Bucket;
			}

			while (Current != NULL)
			{
				if ((WantedType > FLAG_Rotator) || (Current->flagType == WantedType))
				{
					ResultName = Current->flagName;
					ResultType = (EFlagType)Current->flagType;
					bResult = TRUE;
					break;
				}

				Current = Current->nextFlag;
				if (Current == NULL)
				{
					while (Current == NULL)
					{
						Hash++;
						Bucket++;

						if (Hash >= FLAG_HASH_SIZE)
						{
							bResult = FALSE;
							break;
						}

						Current = *Bucket;
					}
				}

				if (Hash >= FLAG_HASH_SIZE)
					break;
			}
		}

		Iterator->hash = Hash;
		Iterator->curFlag = Current;
	}

	if (pName != NULL)
		*pName = ResultName;

	if (pFlagType != NULL)
		*pFlagType = ResultType;

	return bResult;

	unguard;
}

void XFlagBase::DestroyIterator(XFlagIterator IteratorId)
{
	guard(XFlagBase::DestroyIterator);

	if (IteratorId != 0)
		appFree((void*)IteratorId);

	unguard;
}

/*-----------------------------------------------------------------------------
	Native wrappers summary.
-----------------------------------------------------------------------------*/

// Natives 1100..1130 are thin P_GET wrappers around the methods above.
// They are intentionally not expanded here line-by-line yet; the behaviour is
// completely represented in the direct C++ methods. Important wrapper defaults:
//   SetX(Name, Value, optional bAdd, optional Expiration): bAdd=True, Expiration=-1
//   CreateIterator(optional EFlagType): default MAX_FLAG_TYPE
//   GetNextFlagName: same as GetNextFlag but discards the type out-param
//   GetX(Name): returns default zero/None/vector(0) if absent, because the out
//               return storage starts zeroed and GetX only writes on success.

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
