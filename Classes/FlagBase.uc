//=============================================================================
// FlagBase.
//=============================================================================
class FlagBase extends ExtensionObject
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

// Quick class reference
var private travel flag hashTable[64];					// Hash table containing all flags
var private travel int  defaultFlagExpiration;          // Default flag expiration


// ----------------------------------------------------------------------

function FubarCheat()
{
	local FlagBool    flag1;
	local FlagByte    flag2;
	local FlagInt     flag3;
	local FlagFloat   flag4;
	local FlagName    flag5;
	local FlagVector  flag6;
	local FlagRotator flag7;

	// Do absolutely nothing -- the above variables are only included so
	// UnrealScript will load the associated class information from the
	// Extension package.  Mega-Barf.
}


// ----------------------------------------------------------------------
// Intrinsics

native(1100) final function bool SetBool(Name flagName, bool newValue, optional bool bAdd, optional int expiration);
native(1101) final function bool SetByte(Name flagName, byte newValue, optional bool bAdd, optional int expiration);
native(1102) final function bool SetInt(Name flagName, int newValue, optional bool bAdd, optional int expiration);
native(1103) final function bool SetFloat(Name flagName, float newValue, optional bool bAdd, optional int expiration);
native(1104) final function bool SetName(Name flagName, name newValue, optional bool bAdd, optional int expiration);
native(1105) final function bool SetVector(Name flagName, vector newValue, optional bool bAdd, optional int expiration);
native(1106) final function bool SetRotator(Name flagName, rotator newValue, optional bool bAdd, optional int expiration);

native(1110) final function bool    GetBool(Name flagName);
native(1111) final function byte    GetByte(Name flagName);
native(1112) final function int     GetInt(Name flagName);
native(1113) final function float   GetFloat(Name flagName);
native(1114) final function name    GetName(Name flagName);
native(1115) final function vector  GetVector(Name flagName);
native(1116) final function rotator GetRotator(Name flagName);

native(1120) final function bool CheckFlag(Name flagName, EFlagType flagType);
native(1121) final function bool DeleteFlag(Name flagName, EFlagType flagType);
native(1122) final function      SetExpiration(Name flagName, EFlagType flagType, int expiration);
native(1123) final function int  GetExpiration(Name flagName, EFlagType flagType);
native(1124) final function      DeleteExpiredFlags(int criteria);
native(1125) final function      SetDefaultExpiration(int expiration);

native(1126) final function int  CreateIterator(optional EFlagType flagType);
native(1127) final function bool GetNextFlagName(int iterator, out name flagName);
native(1128) final function bool GetNextFlag(int iterator,
                                             out name flagName, out EFlagType flagType);
native(1129) final function      DestroyIterator(int iterator);

native(1130) final function      DeleteAllFlags();

defaultproperties
{
}
