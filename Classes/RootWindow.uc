//=============================================================================
// RootWindow.
//=============================================================================
class RootWindow extends ModalWindow
	native
	noexport;

// ----------------------------------------------------------------------
// Execs

#exec FONT    IMPORT FILE=Textures\TechMedium.bmp          NAME=TechMedium
#exec FONT    IMPORT FILE=Textures\TechMedium_DS.bmp       NAME=TechMedium_DS
#exec FONT    IMPORT FILE=Textures\TechMedium_B.bmp        NAME=TechMedium_B
#exec FONT    IMPORT FILE=Textures\TechSmall.bmp           NAME=TechSmall
#exec FONT    IMPORT FILE=Textures\TechSmall_DS.bmp        NAME=TechSmall_DS
#exec FONT    IMPORT FILE=Textures\TechTiny.bmp            NAME=TechTiny
#exec TEXTURE IMPORT FILE=Textures\DefaultCursor.bmp       NAME=DefaultCursor
#exec TEXTURE IMPORT FILE=Textures\DefaultTextCursor.bmp   NAME=DefaultTextCursor
#exec TEXTURE IMPORT FILE=Textures\DefaultMoveCursor.bmp   NAME=DefaultMoveCursor
#exec TEXTURE IMPORT FILE=Textures\DefaultHMoveCursor.bmp  NAME=DefaultHMoveCursor
#exec TEXTURE IMPORT FILE=Textures\DefaultVMoveCursor.bmp  NAME=DefaultVMoveCursor
#exec TEXTURE IMPORT FILE=Textures\DefaultTLMoveCursor.bmp NAME=DefaultTLMoveCursor
#exec TEXTURE IMPORT FILE=Textures\DefaultTRMoveCursor.bmp NAME=DefaultTRMoveCursor
#exec TEXTURE IMPORT FILE=Textures\Solid.bmp               NAME=Solid
#exec TEXTURE IMPORT FILE=Textures\SolidGreen.bmp          NAME=SolidGreen
#exec TEXTURE IMPORT FILE=Textures\SolidYellow.bmp         NAME=SolidYellow
#exec TEXTURE IMPORT FILE=Textures\SolidRed.bmp            NAME=SolidRed
#exec TEXTURE IMPORT FILE=Textures\VisionBlue.bmp          NAME=VisionBlue
#exec TEXTURE IMPORT FILE=Textures\Dithered.bmp            NAME=Dithered


// ----------------------------------------------------------------------
// Variables

var const PlayerPawnExt parentPawn;         // APlayerPawnExt which owns this win

var private RootWindow  nextRootWindow;     // Next root window in global list

// Cursor position info
var private float       mouseX;             // Cursor X pos
var private float       mouseY;             // Cursor Y pos
var private float       prevMouseX;         // Last cursor X pos
var private float       prevMouseY;         // Last cursor Y pos
var private window      lastMouseWindow;    // Last window the cursor was in
var private bool        bMouseMoved;        // TRUE if the mouse moved
var private bool        bMouseMoveLocked;   // TRUE if mouse movement is disabled
var private bool        bMouseButtonLocked; // TRUE if mouse buttons are disabled
var private bool        bCursorVisible;     // TRUE if the cursor is visible

// Default cursors
var private texture     defaultEditCursor;           // Cursor for edit widgets
var private texture     defaultMoveCursor;           // General movement cursor
var private texture     defaultHorizontalMoveCursor; // Horizontal movement cursor
var private texture     defaultVerticalMoveCursor;   // Vertical movement cursor
var private texture     defaultTopLeftMoveCursor;    // Upper left to lower right cursor
var private texture     defaultTopRightMoveCursor;   // Upper right to lower left cursor

// Sound options
var private bool        bPositionalSound;            // TRUE if positional sound is enabled

// Input windows
var const window        grabbedWindow;         // Recipient window for all mouse events
var const window        focusWindow;           // Recipient window for all keyboard events

// Reference counters
var int                 handleMouseRef;        // Should root handle mouse events?
var int                 handleKeyboardRef;     // Should root handle keyboard events?

// Initialization reference counter
var int                 initCount;             // Number of windows to be initialized this tick

// Rendered area information
var private bool        bRender;               // TRUE if 3D areas should be rendered
var private bool        bClipRender;           // TRUE if the 3D area is clipped
var private bool        bStretchRawBackground; // TRUE if raw background should be stretched
var private float       renderX;               // X offset of rendered area
var private float       renderY;               // Y offset of rendered area
var private float       renderWidth;           // Width of rendered area
var private float       renderHeight;          // Height of rendered area
var private texture     rawBackground;         // Background graphic drawn in unrendered areas
var private float       rawBackgroundWidth;    // Width of background graphic
var private float       rawBackgroundHeight;   // Height of background graphic
var private color       rawColor;              // Color of raw background texture

// Statistical variables
var const int           tickCycles;            // Number of cycles used during windows tick
var const int           paintCycles;           // Number of cycles used during PaintWindows call
var bool                bShowStats;            // Should statistics be shown on root window?
var bool                bShowFrames;           // Should we draw debugging frames around all windows?
var texture             debugTexture;          // Debugging texture
var float               frameTimer;            // Timer used for frames


// Button click information used to determine multiple clicks
var float               multiClickTimeout;     // Max amount of time between multiple button clicks
var private float       maxMouseDist;          // Maximum mouse distance for multi-click to work
var private int         clickCount;            // Current click number (zero-based)
var private int         lastButtonType;        // Last mouse button handled
var private float       lastButtonPress;       // Time remaining for last button press
var private window      lastButtonWindow;      // Last window clicked in
var private float       firstButtonMouseX;     // X position of initial button press
var private float       firstButtonMouseY;     // Y position of initial button press

// List of all current key states
var private byte        keyDownMap[0xFF];      // State is TRUE if key is pressed

var private int         hMultiplier;           // Horizontal multiplier
var private int         vMultiplier;           // Vertical multiplier

// Snapshot-related variables
var private int         snapshotWidth;        // Snapshot width
var private int         snapshotHeight;       // Snapshot height

var private transient int rootFrame;           // Transient frame

// ----------------------------------------------------------------------
// Intrinsics

native(1510) final function SetDefaultEditCursor(optional texture newEditCursor);
native(1511) final function SetDefaultMovementCursors(optional texture newMovementCursor,
                                                      optional texture newHorizontalMovementCursor,
                                                      optional texture newVerticalMovementCursor,
                                                      optional texture newTopLeftMovementCursor,
                                                      optional texture newTopRightMovementCursor);
native(1512) final function EnableRendering(optional bool bRender);
native(1513) final function Bool IsRenderingEnabled();
native(1514) final function SetRenderViewport(float newX, float newY,
                                              float newWidth, float newHeight);
native(1515) final function ResetRenderViewport();
native(1516) final function SetRawBackground(optional texture newTexture,
                                             optional color newColor);
native(1517) final function SetRawBackgroundSize(float newWidth, float newHeight);
native(1518) final function StretchRawBackground(optional bool bStretch);

native(1519) final function EnablePositionalSound(optional bool bEnable);
native(1520) final function bool IsPositionalSoundEnabled();

native(1521) final function LockMouse(optional bool bLockMove, optional bool bLockButton);
native(1522) final function ShowCursor(optional bool bShow);

native(1523) final function SetSnapshotSize(float newWidth, float newHeight);
native(1524) final function Texture GenerateSnapshot(optional bool bFilter);

// ----------------------------------------------------------------------
// InitWindow() : Called when this window is initialized

function InitWindow()
{
	Super.InitWindow();
	SetFont(Font'TechMedium');
	SetDefaultCursor(Texture'DefaultCursor');
	SetDefaultEditCursor(Texture'DefaultTextCursor');
	SetDefaultMovementCursors(Texture'DefaultMoveCursor',
	                          Texture'DefaultHMoveCursor',
	                          Texture'DefaultVMoveCursor',
	                          Texture'DefaultTLMoveCursor',
	                          Texture'DefaultTRMoveCursor');
}


// ----------------------------------------------------------------------
// Tick() : Work procedure, called periodically

function Tick(float deltaSeconds)
{
}


// ----------------------------------------------------------------------
// VirtualKeyPressed() : Default virtual key handler for the root window

function bool VirtualKeyPressed(EInputKey key, bool bRepeat)
{
	local bool retval;

	// Try the superclass first
	retval = Super.VirtualKeyPressed(key, bRepeat);

	// Didn't handle it -- do it ourselves
	if (!retval)
	{
		retval = true;

		// Handle arrow keys
		if      (key == IK_Left)
			MoveFocusLeft();
		else if (key == IK_Right)
			MoveFocusRight();
		else if (key == IK_Up)
			MoveFocusUp();
		else if (key == IK_Down)
			MoveFocusDown();

		// Handle tab key
		else if (key == IK_Tab)
		{
			if (IsKeyDown(IK_Shift))
				MoveTabGroupPrev();
			else
				MoveTabGroupNext();
		}

		// Handle zilch
		else
			retval = false;
	}

	// Return TRUE if we handled this event
	return (retval);
}


// ----------------------------------------------------------------------
// ClientMessage() : Called when the game wants to tell the player
//                   something

event bool ClientMessage(coerce string msg, optional name type,
                         optional bool bBeep)
{
	return (false);  // don't handle this
}


// ----------------------------------------------------------------------
// GrabKeyboardEvents() : Allows root window to process keyboard events;
//                        uses reference counting

function GrabKeyboardEvents()
{
	handleKeyboardRef++;
}


// ----------------------------------------------------------------------
// UngrabKeyboardEvents() : Prevents root window from processing
//                          keyboard events; uses reference counting

function UngrabKeyboardEvents()
{
	if (handleKeyboardRef > 0)
		handleKeyboardRef--;
}


// ----------------------------------------------------------------------
// GrabMouseEvents() : Allows root window to process mouse events; uses
//                     reference counting

function GrabMouseEvents()
{
	handleMouseRef++;
}


// ----------------------------------------------------------------------
// UngrabMouseEvents() : Prevents root window from processing mouse
//                       events; uses reference counting

function UngrabMouseEvents()
{
	if (handleMouseRef > 0)
		handleMouseRef--;
}


// ----------------------------------------------------------------------
// IsKeyboardGrabbed() : Returns TRUE if the root window can process
//                       keyboard events

function bool IsKeyboardGrabbed()
{
	if (handleKeyboardRef > 0)
		return true;
	else
		return false;
}


// ----------------------------------------------------------------------
// IsMouseGrabbed() : Returns TRUE if the root window can process mouse
//                    events

function bool IsMouseGrabbed()
{
	if (handleMouseRef > 0)
		return true;
	else
		return false;
}


// ----------------------------------------------------------------------
// ShowStats() : Turns the status display on or off

function ShowStats(bool bNewShowStats)
{
	bShowStats = bNewShowStats;
}


// ----------------------------------------------------------------------
// ShowFrames() : Turns window frames on or off

function ShowFrames(bool bNewShowFrames)
{
	bShowFrames = bNewShowFrames;
}

defaultproperties
{
     rawBackground=Texture'Extension.Solid'
     debugTexture=Texture'Extension.Solid'
}
