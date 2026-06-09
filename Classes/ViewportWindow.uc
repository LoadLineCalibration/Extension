//=============================================================================
// ViewportWindow.
//=============================================================================
class ViewportWindow extends Window
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

var const bool    bEnableViewport;
var const bool    bClearZ;
var const bool    bShowActor;
var const bool    bShowWeapons;
var const bool    bUseViewRotation;
var const bool    bUseEyeHeight;
var const bool    bWatchEyeHeight;

var const float   fov;

var const texture defaultTexture;
var const color   defaultColor;

var const actor   originActor;
var const actor   watchActor;
var const vector  location;
var const vector  relLocation;
var const rotator rotation;
var const rotator relRotation;

var private bool    bOriginActorDestroyed;
var private vector  lastLocation;
var private rotator lastRotation;

var private transient int viewportFrame;


// ----------------------------------------------------------------------
// Intrinsics

native(1940) final function SetViewportActor(optional actor newOriginActor,
                                             optional bool bEyeLevel,
                                             optional bool bEnable);
native(1941) final function SetViewportLocation(Vector newLocation,
                                                optional bool bEnable);

native(1942) final function SetWatchActor(optional actor newWatchActor,
                                          optional bool bEyeLevel);
native(1943) final function SetRotation(optional rotator newRotation);

native(1945) final function EnableViewport(optional bool bEnable);
native(1946) final function SetFOVAngle(optional float newAngle);
native(1947) final function ShowViewportActor(optional bool bShow);
native(1948) final function ShowWeapons(optional bool bShow);

native(1950) final function SetRelativeLocation(optional vector relLoc);
native(1951) final function SetRelativeRotation(optional rotator relRot);

native(1955) final function SetDefaultTexture(optional texture newTexture,
                                              optional color newColor);
native(1956) final function ClearZBuffer(optional bool bClear);


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// CalcView() : Overrideable callback used to modify camera position and
//              rotation

event CalcView(actor originActor, actor watchActor,
               out vector frameLocation, out rotator frameRotation)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// SetViewportLocationXYZ() - Convenience function

function SetViewportLocationXYZ(float x, float y, float z, bool bEnable)
{
	local Vector vector;

	vector.x = x;
	vector.y = y;
	vector.z = z;

	SetViewportLocation(vector, bEnable);
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// SetRotationPYR() - Convenience function

function SetRotationPYR(int pitch, int yaw, int roll)
{
	local rotator rotation;

	rotation.roll  = roll;
	rotation.pitch = pitch;
	rotation.yaw   = yaw;

	SetRotation(rotation);
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// SetRelativeLocationXYZ() - Convenience function

function SetRelativeLocationXYZ(float x, float y, float z)
{
	local Vector vector;

	vector.x = x;
	vector.y = y;
	vector.z = z;

	SetRelativeLocation(vector);
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// SetRelativeRotationPYR() - Convenience function

function SetRelativeRotationPYR(int pitch, int yaw, int roll)
{
	local rotator rotation;

	rotation.roll  = roll;
	rotation.pitch = pitch;
	rotation.yaw   = yaw;

	SetRelativeRotation(rotation);
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// SetDefaultTextureRGB() - Convenience function

function SetDefaultTextureRGB(texture newTexture, INT red, INT green, INT blue)
{
	local color newColor;

	newColor.r = red;
	newColor.g = green;
	newColor.b = blue;

	SetDefaultTexture(newTexture, newColor);
}

defaultproperties
{
     DefaultTexture=Texture'Extension.Solid'
}
