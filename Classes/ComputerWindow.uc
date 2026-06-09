//=============================================================================
// ComputerWindow
//
// Interface that allows computer-like displays (computer screens, terminals,
// ATMs, etc.)
//=============================================================================

class ComputerWindow extends Window
	native
	noexport;

var Float	eventTimeInterval;			// Timing between each event displayed
var Float	timeNextEvent;				// Time of next event
var Float	timeLastEvent;				// Time of last event added
var Float	timeCurrent;				// Current event time
var Float	fadeSpeed;					// Fade Speed
var Float	throttle;					// Throttle (speed modifier)
var Float   fadeOutTimer;				// Used to fade the entire screen out
var Float   fadeOutStart;				// Starting fade out value
var int		textCols;					// Number of text cols (for example, 80)
var int		textRows;					// Number of text rows (for example, 25)
var Font	textFont;					// Font used to display text
var int     fontWidth;			
var int		fontHeight;
var Color	fontColor;					// Current Font Color, used when adding text events
var Sound	textSound;					// Sound to play for each character
var Sound   typingSound;				// Sound when user types
var Float	computerSoundVolume;		// Sound Volume
var Bool	bWordWrap;					// True if Word Wrap is enabled
var Bool	bLastLineWrapped;			// True if the last line was wrapped
var Bool	bInvokeComputerStart;		// True if we need to invoke ComputerStart
var Bool	bComputerStartInvoked;		// True if we've invoked ComputerStart event
var Bool	bFirstTick;					// True until we've had our first Tick event

var Texture backgroundTextures[6];		// Background textures

var Texture cursorTexture;				// Cursor texture, drawn ahead of text
var Color	cursorColor;				// Cursor Color
var int     cursorWidth;				// Cursor Width
var int     cursorHeight;				// Cursor Height
var Float	cursorBlinkSpeed;			// Cursor Blink Speed
var Bool    bCursorVisible;				// True if Cursor Visible
var Bool	bShowCursor;				// Set to False to hide cursor
var Float   cursorNextEvent;			// Countdown until 0 for next cursor blink change
var Color	colGraphicTile;				// Color used to draw graphics

var Window  textWindow;					// Window that text is displayed in
var PlayerPawnExt player;				// Pointer to player pawn

var const native DynamicArray displayBuffer;				// Pointer to display info
var const native DynamicArray queuedBuffer;				// Queued characters

var int		queuedBufferStart;			// Start of QueuedBuffer we're interested in
var int		textX;						// Current text X position
var int		textY;						// Current text Y position

// Input related stuff
var	String		inputKey;				// Input Key
var String		inputMask;				// Input Mask Character
var EditWindow  inputWindow;			// Input Window
var Bool		bWaitingForKey;			// True if we're waiting for a single key
var Bool		bEchoKey;				// True if echo keypress
var Bool		bPauseProcessing;		// True if we're pausing processing
var Bool		bIgnoreTick;			// True if ignoring TICK event
var Bool		bGamePaused;			// True if the Game is paused
var Bool		bIgnoreGamePaused;		// True if we're going to ignore the game being paused

// native functions
native(1970) final function SetBackgroundTextures(
	Texture backTexture1, Texture backTexture2, Texture backTexture3, 
	Texture backTexture4, Texture backTexture5, Texture backTexture6);
native(1971) final function SetTextSize( int newCols, int newRows);
native(1972) final function SetTextWindowPosition( int newX, int newY );
native(1973) final function SetTextFont( Font newFont, int newFontWidth, int newFontHeight,	Color newFontColor);
native(1974) final function SetFontColor( Color newFontColor );
native(1975) final function SetTextTiming( Float newTiming );
native(1976) final function SetFadeSpeed( Float fadeSpeed );
native(1977) final function SetCursorTexture( 
	Texture newCursorTexture, 
	optional int newCursorWidth, 
	optional int newCursorHeight);
native(1978) final function SetCursorColor( Color newCursorColor );
native(1979) final function SetCursorBlinkSpeed( Float newBlinkSpeed );
native(1980) final function ShowTextCursor( optional Bool bShow );
native(1981) final function SetTextSound( Sound newTextSound );
native(1982) final function SetTypingSound( Sound newTypingSound );
native(1983) final function SetComputerSoundVolume( Float newSoundVolume );
native(1984) final function SetTypingSoundVolume( Float newSoundVolume );
native(1985) final function ClearScreen();
native(1986) final function ClearLine( int rowToClear );
native(1987) final function Print( String printText, optional bool bNewLine );
native(1988) final function PrintLn();

native(1989) final function GetInput( 
	int maxLength, 
	String inputKey, 
	optional String defaultInputString,
	optional String inputMask );

native(1990) final function GetChar(String inputKey, optional bool bEcho);

native(1991) final function PrintGraphic( 
	Texture graphic, 
	int width, 
	int height, 
	optional int posX, 
	optional int posY, 
	optional Bool bStatic, 
	optional Bool bPixelPos);

native(1992) final function PlaySoundLater( Sound newSound );
native(1993) final function SetTextPosition( int posX, int posY );
native(1994) final function Bool IsBufferFlushed();
native(1995) final function Pause( optional Float pauseLength );
native(1996) final function Resume();
native(1997) final function Bool IsPaused();
native(1998) final function SetThrottle(float throttleModifier);
native(1999) final function float GetThrottle();
native(2000) final function ResetThrottle();
native(2001) final function EnableWordWrap( optional Bool bNewWordWrap );
native(2002) final function FadeOutText( optional Float fadeDuration );

// ----------------------------------------------------------------------
// ComputerStart()
// ----------------------------------------------------------------------

event Bool ComputerStart()
{
	return False;
}

// ----------------------------------------------------------------------
// ComputerInputFinished()
// ----------------------------------------------------------------------

event Bool ComputerInputFinished( String inputKey, String inputValue )
{
	return False;
}

// ----------------------------------------------------------------------
// ComputerFadeOutCompleted()
// ----------------------------------------------------------------------

event ComputerFadeOutCompleted()
{
}

// ----------------------------------------------------------------------
// SetGraphicTileColor()
// ----------------------------------------------------------------------

function SetGraphicTileColor(Color newColor)
{
	colGraphicTile = newColor;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

defaultproperties
{
     throttle=1.000000
     textCols=80
     textRows=24
     fontWidth=8
     fontHeight=20
     FontColor=(R=255,G=255,B=255)
     computerSoundVolume=0.500000
     bWordWrap=True
     bFirstTick=True
     cursorTexture=Texture'Extension.Solid'
     cursorColor=(R=255,G=255,B=255)
     cursorWidth=8
     cursorHeight=2
     cursorBlinkSpeed=0.500000
     bShowCursor=True
     colGraphicTile=(R=255,G=255,B=255)
}
