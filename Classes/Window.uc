//=============================================================================
// Window.
//=============================================================================
class Window extends ExtensionObject
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

// Quick class reference
var private byte     windowType;          // Contains window type

var private window   parentOwner;         // Window that "owns" this window

// Booleans
var const bool       bIsVisible;          // TRUE if the window is visible
var const bool       bIsSensitive;        // TRUE if the window can take input
var const bool       bIsSelectable;       // TRUE if the window can have keyboard focus
var const bool       bIsInitialized;      // TRUE if the window has been initialized

var const bool       bBeingDestroyed;     // TRUE if this window is going bye-bye
var const int        lockCount;           // Reference count of locks on this window

// Background drawing options
var const bool       bDrawRawBackground;  // TRUE if raw backgrounds should be drawn here
var const bool       bStretchBackground;  // TRUE=stretch background, FALSE=repeat tile
var const bool       bSmoothBackground;   // TRUE if the background should be smoothed
var const EDrawStyle backgroundStyle;     // Normal, masked or translucent

// Text/font drawing state variables
var const color      textColor;           // Color of text
var const plane      textPlane;           // Plane of text; converted from textColor
var const font       normalFont;          // Regular font
var const font       boldFont;            // Boldface font
var const float      textVSpacing;        // Space between lines of text
var const color      tileColor;           // Color of tile
var const plane      tilePlane;           // Plane of tile; converted from tileColor
var const bool       bSpecialText;        // TRUE if special text is enabled
var const bool       bTextTranslucent;    // TRUE if translucent text is enabled

// Configuration information
var const float      x;                   // X position of window relative to parent
var const float      y;                   // Y position of window relative to parent
var /*const*/ float      width;               // Width of window
var /*const*/ float      height;              // Height of window

// User-specified size preferences
var const EHAlign    winHAlign;           // User-specified horizontal alignment
var const EVAlign    winVAlign;           // User-specified vertical alignment
var const float      hMargin0;            // User-specified first horizontal margin
var const float      hMargin1;            // User-specified second horizontal margin (FULL only)
var const float      vMargin0;            // User-specified first vertical margin
var const float      vMargin1;            // User-specified second vertical margin (FULL only)
var /*const*/ float      hardcodedWidth;      // User-specified preferred width
var /*const*/ float      hardcodedHeight;     // User-specified preferred height

// Temporary variables; used by parents to configure children
var float            holdX;
var float            holdY;
var float            holdWidth;
var float            holdHeight;

// Accelerator key info
var int              acceleratorKey;         // Hotkey, cast to an INT (hack)

// Efficiency variables for geometry negotiation
var bool             bNeedsReconfigure;      // TRUE if this window must be reconfigured
var bool             bNeedsQuery;            // TRUE if this window must be requeried
var bool             bConfigured;            // TRUE if this window was reconfigured
var private bool     bLastWidthSpecified;    // Whether width was specified in last size query
var private bool     bLastHeightSpecified;   // Whether height was specified in last size query
var private float    lastSpecifiedWidth;     // Width specified in last size query
var private float    lastSpecifiedHeight;    // Height specified in last size query
var private float    lastQueryWidth;         // Width returned by last size query
var private float    lastQueryHeight;        // Height returned by last size query

// Callback options
var bool             bTickEnabled;        // TRUE if the UnrealScript Tick() is enabled

// Multiple click options
var int              maxClicks;           // Number of clicks handled by window; 0=any number

// Clipping rectangle
var const ClipRect   winClipRect;         // Clipping rectangle; maintained at all times

// Texture information
var const texture    background;          // Background textures

// Cursor information
var const texture    defaultCursor;       // Default cursor; can be overridden in CursorRequested
var const texture    defaultCursorShadow; // Default cursor shadow; can be overridden in CursorRequested
var const float      defaultHotX;         // Default hot X; can be overridden in CursorRequested
var const float      defaultHotY;         // Default hot Y; can be overridden in CursorRequested
var const color      defaultCursorColor;  // Default cursor color; can be overridden in CursorRequested

// Sound stuff
var const sound      focusSound;          // Played when this window obtains focus
var const sound      unfocusSound;        // Played when this window loses focus
var const sound      visibleSound;        // Played when this window becomes visible
var const sound      invisibleSound;      // Played when this window becomes invisible
var const float      soundVolume;         // Volume of sound

// Albert variables
var const object     clientObject;        // Pointer to an object associated with this window

// Traversal positions (used only when bIsSelectable is TRUE)
var private int      rowMajorIndex;       // Index into tab group's row-major sorted list
var private int      colMajorIndex;       // Index into tab group's column-major sorted list

// Attribute pointer
var native private int      att;                 // Pointer to first attribute; used by parent

// Timer pointers
var native private int      firstTimer;          // Pointer to this window's first timer
var native private int      freeTimer;           // Free list of timers

var private gc       wGc;
var private gc       gGc;

// Relatives
var const window     winParent;           // Parent window; NULL if this is root
var const window     firstChild;          // "Lowest" child (first one drawn)
var const window     lastChild;           // "Highest" child (last one drawn)
var const window     prevSibling;         // Next "lowest" sibling (previous one drawn)
var const window     nextSibling;         // Next "highest" sibling (next one drawn)


// ----------------------------------------------------------------------
// Intrinsics

native(1409) final function Destroy();
native(1410) final function window NewChild(class newClass, optional bool bShow);
native(1411) final function Raise();
native(1412) final function Lower();
native(1413) final function Show(optional bool bShow);
native(1414) final function Hide();
native(1415) final function bool IsVisible(optional bool bRecurse);
native(1416) final function SetSensitivity(bool newSensitivity);
native(1417) final function EnableWindow(optional bool bEnable);
native(1418) final function DisableWindow();
native(1419) final function bool IsSensitive(optional bool bRecurse);
native(1420) final function SetSelectability(bool newSelectability);
native(1421) final function SetBackground(texture newBackground);
native(1422) final function SetBackgroundStyle(EDrawStyle newStyle);
native(1423) final function SetBackgroundSmoothing(bool newSmoothing);
native(1424) final function SetBackgroundStretching(bool newStretching);

native(1425) final function RootWindow     GetRootWindow();
native(1426) final function ModalWindow    GetModalWindow();
native(1427) final function TabGroupWindow GetTabGroupWindow();
native(1428) final function Window         GetParent();
native(1429) final function PlayerPawnExt  GetPlayerPawn();

native(1430) final function SetConfiguration(float newX, float newY,
                                             float newWidth, float newHeight);
native(1431) final function SetSize(float newWidth, float newHeight);
native(1432) final function SetPos(float newX, float newY);
native(1433) final function SetWidth(float newWidth);
native(1434) final function SetHeight(float newHeight);
native(1435) final function ResetSize();
native(1436) final function ResetWidth();
native(1437) final function ResetHeight();
native(1438) final function SetWindowAlignments(EHAlign hAlign, EVAlign vAlign,
                                                optional float hMargin0,
                                                optional float vMargin0,
                                                optional float hMargin1,
                                                optional float vMargin1);

native(1439) final function SetAcceleratorText(string newStr);

native(1440) final function bool   SetFocusWindow(window newFocusWindow);
native(1441) final function window GetFocusWindow();
native(1442) final function window MoveFocusLeft();
native(1443) final function window MoveFocusRight();
native(1444) final function window MoveFocusUp();
native(1445) final function window MoveFocusDown();
native(1446) final function window MoveTabGroupNext();
native(1447) final function window MoveTabGroupPrev();
native(1448) final function bool   IsFocusWindow();

native(1449) final function ConvertCoordinates(window fromWin, float fromX, float fromY,
                                               window toWin, out float toX, out float toY);

native(1450) final function GrabMouse();
native(1451) final function UngrabMouse();

native(1452) final function GetCursorPos(out float mouseX, out float mouseY);
native(1453) final function SetCursorPos(float newMouseX, float newMouseY);

native(1454) final function SetDefaultCursor(Texture tx,
                                             optional Texture shadowTexture,
                                             optional float hotX,
                                             optional float hotY,
                                             optional color cursorColor);

native(1455) final function window GetTopChild(optional bool bVisibleOnly);
native(1456) final function window GetBottomChild(optional bool bVisibleOnly);
native(1457) final function window GetHigherSibling(optional bool bVisibleOnly);
native(1458) final function window GetLowerSibling(optional bool bVisibleOnly);
native(1459) final function DestroyAllChildren();

native(1460) final function AskParentForReconfigure();
native(1461) final function ConfigureChild(float newX, float newY, float newWidth, float newHeight);
native(1462) final function ResizeChild();
native(1463) final function float QueryPreferredWidth(float queryHeight);
native(1464) final function float QueryPreferredHeight(float queryWidth);
native(1465) final function QueryPreferredSize(out float preferredWidth, out float preferredHeight);
native(1466) final function QueryGranularity(out float hGranularity, out float vGranularity);
native(1467) final function SetChildVisibility(bool bNewVisibility);

native(1468) final function AskParentToShowArea(optional float areaX, optional float areaY,
                                                optional float areaWidth, optional float areaHeight);

native(1469) final function string ConvertScriptString(string oldStr);

native(1470) final function bool IsKeyDown(EInputKey key);
native(1471) final function bool IsPointInWindow(float pointX, float pointY);
native(1472) final function window FindWindow(float pointX, float pointY,
                                              out float relativeX, out float relativeY);

native(1473) final function PlaySound(Sound newsound,
                                      optional float volume, optional float pitch,
                                      optional float posX, optional float posY);
native(1474) final function SetSoundVolume(float newVolume);

native(1475) final function SetTileColor(color newColor);
native(1476) final function SetTextColor(color newColor);
native(1477) final function SetFont(Font fn);
native(1478) final function SetFonts(Font nFont, Font bFont);
native(1479) final function SetNormalFont(Font fn);
native(1480) final function SetBoldFont(Font fn);
native(1481) final function EnableSpecialText(optional bool bEnable);

native(1482) final function string CarriageReturn();

native(1483) final function EnableTranslucentText(optional bool bEnable);

native(1484) final function SetBaselineData(optional float newBaselineOffset,
                                            optional float newUnderlineHeight);

native(1485) final function GC GetGC();
native(1486) final function ReleaseGC(GC gc);

native(1487) final function SetClientObject(object newClientObject);
native(1488) final function object GetClientObject();

native(1489) final function bool ConvertVectorToCoordinates(vector location,
                                                            out float relativeX,
                                                            out float relativeY);

native(1490) final function int AddTimer(float timeout, optional bool bLoop, optional int clientData,
                                         optional name functionName);
native(1491) final function RemoveTimer(int timerId);
native(1492) final function float GetTickOffset();

native(1493) final function ChangeStyle();

native(1495) final function SetFocusSounds(optional sound focusSound, optional sound unfocusSound);
native(1496) final function SetVisibilitySounds(optional sound visSound, optional sound invisSound);

native(1497) final function AddActorRef(actor refActor);
native(1498) final function RemoveActorRef(actor refActor);
native(1499) final function bool IsActorValid(actor refActor);


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// SetTextColorRGB() : Sets the color of text based on individual RGB
//                     values

function SetTextColorRGB(INT red, INT green, INT blue)
{
	local color newColor;
	newColor.r = red;
	newColor.g = green;
	newColor.b = blue;
	SetTextColor(newColor);
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// SetTileColorRGB() : Sets the color of a tile based on individual RGB
//                     values

function SetTileColorRGB(INT red, INT green, INT blue)
{
	local color newColor;
	newColor.r = red;
	newColor.g = green;
	newColor.b = blue;
	SetTileColor(newColor);
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// GetColorScaled() : A useful little routine that generates a scaled
//                    color, based on a value from 0 to 1
//                    (Moved to Window.uc and modified slightly because
//                    it was being copied all over the place :)

function Color GetColorScaled(float percent)
{
	local float mult;
	local Color col;

	if (percent > 0.80)
	{
		col.r = 0;
		col.g = 255;
		col.b = 0;
	}
	else if (percent > 0.40)
	{
		mult = (percent-0.40)/(0.80-0.40);
		col.r = 255 + (0-255)*mult;
		col.g = 255;
		col.b = 0;
	}
	else if (percent > 0.10)
	{
		mult = (percent-0.10)/(0.40-0.10);
		col.r = 255;
		col.g = 0 + (255-0)*mult;
		col.b = 0;
	}
	else if (percent > 0)
	{
		col.r = 255;
		col.g = 0;
		col.b = 0;
	}
	else
	{
		col.r = 0;
		col.g = 0;
		col.b = 0;
	}

	return col;

}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// CR() : Shortened form of CarriageReturn()

function string CR()
{
	return CarriageReturn();
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// InitWindow() : Window constructor

event InitWindow()
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// DestroyWindow() : Window destructor

event DestroyWindow()
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// WindowReady() : Called just before a window is first drawn

event WindowReady()
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ParentRequestedPreferredSize() : Called when the parent wants to know
//                                  this window's preferred size

event ParentRequestedPreferredSize(bool bWidthSpecified, out float preferredWidth,
                                   bool bHeightSpecified, out float preferredHeight)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ParentRequestedGranularity() : Called when the parent wants to know
//                                this window's granularity

event ParentRequestedGranularity(out float hGranularity,
                                 out float vGranularity)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ChildRequestedVisibilityChange() : Called when a child wants to become
//                                    visible or invisible

event ChildRequestedVisibilityChange(window childWin, bool bNewVisibility)
{
	childWin.SetChildVisibility(bNewVisibility);
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ChildRequestedReconfiguration() : Called when a child wants to change
//                                   size

event bool ChildRequestedReconfiguration(window childWin)
{

	childWin.ResizeChild();

	return true;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ChildRequestedShowArea() : Called when a child wants to be seen

event ChildRequestedShowArea(window child, float showX, float showY,
                             float showWidth, float showHeight)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ConfigurationChanged() : Called when the parent changes this window's
//                          size

event ConfigurationChanged()
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// VisibilityChanged() : Called when the parent shows or hides this
//                       window

event VisibilityChanged(bool bNewVisibility)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// SensitivityChanged() : Called when this window becomes sensitive or
//                        insensitive

event SensitivityChanged(bool bNewSensitivity)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// MouseMoved() : Called when the mouse cursor moves over a window

event MouseMoved(float newX, float newY)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// RawMouseButtonPressed() : Raw event called when a mouse button is
//                           pressed

event bool RawMouseButtonPressed(float pointX, float pointY,
                                 EInputKey button, EInputState iState)
{
	return false;  // don't handle
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// RawKeyPressed() : Raw event called when a key is pressed

event bool RawKeyPressed(EInputKey key, EInputState iState, bool bRepeat)
{
	return false;  // don't handle
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// MouseButtonPressed() : Called when a mouse button is pressed

event bool MouseButtonPressed(float pointX, float pointY, EInputKey button,
                              int numClicks)
{
	return false;  // don't handle
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// MouseButtonReleased() : Called when a mouse button is released

event bool MouseButtonReleased(float pointX, float pointY, EInputKey button,
                               int numClicks)
{
	return false;  // don't handle
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// KeyPressed() : Called when a key is pressed; proves an ASCII keypress
//                value in the form of a string

event bool KeyPressed(string key)
{
	return false;  // don't handle
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// AcceleratorKeyPressed() : Called when this window's accelerator key
//                           is activated

event bool AcceleratorKeyPressed(string key)
{
	return false;  // don't handle
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// VirtualKeyPressed() : Called when a key is pressed; provides a virtual
//                       key value

event bool VirtualKeyPressed(EInputKey key, bool bRepeat)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// MouseEnteredWindow() : Called when the mouse enters a window

event MouseEnteredWindow()
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// MouseLeftWindow() : Called when the mouse leaves a window

event MouseLeftWindow()
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// FocusEnteredWindow() : Called when a window gets focus

event FocusEnteredWindow()
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// FocusLeftWindow() : Called when a window loses focus

event FocusLeftWindow()
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// FocusEnteredDescendant() : Called when a descendant window gets focus

event FocusEnteredDescendant(Window enterWindow)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// FocusLeftDescendant() : Called when a descendant window loses focus

event FocusLeftDescendant(Window leaveWindow)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ButtonActivated() : Called when an ancestor button is activated

event bool ButtonActivated(Window button)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ToggleChanged() : Called when an ancestor toggle is modified

event bool ToggleChanged(Window button, bool bNewToggle)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// BoxOptionSelected() : Called when an ancestor message box option is
//                       chosen

event bool BoxOptionSelected(Window box, int buttonNumber)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ScalePositionChanged() : Called when an ancestor scale window's
//                          position is moved

event bool ScalePositionChanged(Window scale, int newTickPosition,
                                float newValue, bool bFinal)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ScaleRangeChanged() : Called when an ancestor scale window's
//                       position + span is moved

event bool ScaleRangeChanged(Window scale, int fromTick, int toTick,
                             float fromValue, float toValue, bool bFinal)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ScaleAttributesChanged() : Called when an ancestor scale window's
//                            position, span or range is changed

event bool ScaleAttributesChanged(Window scale, int tickPosition,
                                  int tickSpan, int numTicks)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ClipAttributesChanged() : Called when an ancestor clip window's
//                           attributes have changed

event bool ClipAttributesChanged(Window scale,
                                 int newClipWidth,  int newClipHeight,
                                 int newChildWidth, int newChildHeight)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ListRowActivated() : Called when a list item has been activated

event bool ListRowActivated(window list, int rowId)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ListSelectionChanged() : Called when list items are selected or
//                          deselected

event bool ListSelectionChanged(window list, int numSelections, int focusRowId)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ClipPositionChanged() : Called when the position of a window within
//                         a clip window is changed

event bool ClipPositionChanged(window clip, int newCol, int newRow)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// TextChanged() : Called when the text in an edit window has changed

event bool TextChanged(window edit, bool bModified)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// EditActivated() : Called when the user hits the ENTER key in a single-
//                   line edit window

event bool EditActivated(window edit, bool bModified)
{
	return false;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// DrawWindow() : Draws the window

event DrawWindow(GC gc)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// PostDrawWindow() : Draws the window (AFTER all children are drawn)

event PostDrawWindow(GC gc)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ChildAdded() : Called when a child window is added

event ChildAdded(Window child)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ChildRemoved() : Called when a child window is removed

event ChildRemoved(Window child)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// DescendantAdded() : Called when a descendant window is added

event DescendantAdded(Window descendant)
{
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// DescendantRemoved() : Called when a descendant window is removed

event DescendantRemoved(Window descendant)
{
}

defaultproperties
{
}
