//=============================================================================
// TextLogWindow.
//=============================================================================
class TextLogWindow extends TextWindow
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

// Log entry timeout
var float                       textTimeout;  // Time before a log entry disappears

// Internal info
var native private DynamicArray lines;        // Individual log entries
var private bool                bTooTall;     // TRUE if the log won't fit in the window
var private bool                bPaused;      // TRUE if the log is currently paused


// ----------------------------------------------------------------------
// Intrinsics

native(1570) final function AddLog(string newText, color linecol);
native(1571) final function ClearLog();
native(1572) final function SetTextTimeout(float newTimeout);
native(1573) final function PauseLog(bool bNewPauseState);

defaultproperties
{
}
