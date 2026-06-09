//=============================================================================
// Flag.
//=============================================================================
class Flag extends ExtensionObject
	native
	noexport;

var private travel name      flagName;
var private travel int       flagHash;
var private travel FlagBase  flagBase;
var private travel Flag      nextFlag;
var private travel EFlagType flagType;
var private travel int       expiration;

defaultproperties
{
}
