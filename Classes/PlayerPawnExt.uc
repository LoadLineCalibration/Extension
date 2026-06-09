//=============================================================================
// PlayerPawnExt.
//=============================================================================
class PlayerPawnExt extends PlayerPawn
	native
	config
	noexport;

// ----------------------------------------------------------------------
// Structure used for actor references (PRIVATE!)

struct ActorRef
{
	var Actor actor;
	var int   refCount;
};


// ----------------------------------------------------------------------
// Variables

var const travel FlagBase flagBase;     // Knowledge base for the player

var const transient RootWindow rootWindow; // Root window for window manager

var private ActorRef actorList[32];  // List of valid actors
var private int      actorCount;     // Count of valid actors


// ----------------------------------------------------------------------
// Intrinsics

native(1050) final function PreRenderWindows(canvas Canvas);
native(1051) final function PostRenderWindows(canvas Canvas);
native(1052) final function InitRootWindow();

// ----------------------------------------------------------------------
// Network replication

replication
{
	
}

// ----------------------------------------------------------------------
// PreRender() : Called before all 3D rendering is done for the
//               specified canvas

simulated event PreRender(canvas Canvas)
{
	Super.PreRender(Canvas);
	PreRenderWindows(Canvas);
}


// ----------------------------------------------------------------------
// RenderOverlays() : Called when weapons and other items must be drawn

simulated event RenderOverlays(canvas Canvas)
{
	Super.RenderOverlays(Canvas);
}


// ----------------------------------------------------------------------
// PostRenderFlash() : Called when after the 3D rendering is done for the
//                     specified canvas, but before the screen flash is
//                     completed

simulated event PostRenderFlash(canvas Canvas)
{
	PostRenderWindows(Canvas);
	Super.PostRenderFlash(Canvas);
}


// ----------------------------------------------------------------------
// PostRender() : Called after all 3D rendering has been done for the
//                specified canvas

simulated event PostRender(canvas Canvas)
{
	Super.PostRender(Canvas);
}


// ----------------------------------------------------------------------
// Possess() : Called when a player possesses a pawn

function Possess()
{
	Super.Possess();
	InitRootWindow();
}


// ----------------------------------------------------------------------
// Destroyed() : Called when the PlayerPawn has been destroyed

simulated event Destroyed()
{
	Super.Destroyed();
}


// ----------------------------------------------------------------------
// ClientMessage() : Handles log messages for the client

function ClientMessage(coerce string msg, optional Name type, optional bool bBeep)
{
	local bool bHandled;

	// See if the root window can use this event
	bHandled = false;
	if (rootWindow != None)
		if (rootWindow.ClientMessage(msg, type, bBeep))
			bHandled = true;

	// If not, call the superclass
	if (!bHandled)
		Super.ClientMessage(msg, type, bBeep);
}

// The player wants to switch to weapon group numer I.
exec function SwitchWeapon (byte F )
{
	local weapon newWeapon;

	if ( Inventory == None )
		return;
	if ( (Weapon != None) && (Weapon.Inventory != None) )
		newWeapon = Weapon.Inventory.WeaponChange(F);
	else
		newWeapon = None;	
	if ( newWeapon == None )
		newWeapon = Inventory.WeaponChange(F);
	if ( newWeapon == None )
		return;

	if ( Weapon == None )
	{
		PendingWeapon = newWeapon;
		ChangedWeapon();
	}
	else if ( (Weapon != newWeapon) && Weapon.PutDown() )
		PendingWeapon = newWeapon;
}

defaultproperties
{
}
