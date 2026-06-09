//=============================================================================
// BorderWindow.
//=============================================================================
class BorderWindow extends Window
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

// Border textures
var const texture borderLeft;
var const texture borderTopLeft;
var const texture borderTop;
var const texture borderTopRight;
var const texture borderRight;
var const texture borderBottomRight;
var const texture borderBottom;
var const texture borderBottomLeft;

var const texture center;

// Cursors
var const texture moveCursor;
var const texture hMoveCursor;
var const texture vMoveCursor;
var const texture tlMoveCursor;
var const texture trMoveCursor;

// Drawing options
var EDrawStyle    borderStyle;     // Solid, translucent or masked
var bool          bSmoothBorder;   // TRUE if the borders should be smoothed
var bool          bStretchBorder;  // TRUE=stretched, FALSE=repeated tile

// Window options
var const bool    bResizeable;     // TRUE if the user can resize by dragging
var const bool    bMarginsFromBorder; // TRUE if child margins are based on borders

// Margins for child windows
var float         childLeftMargin;
var float         childRightMargin;
var float         childTopMargin;
var float         childBottomMargin;

// Private
var private float leftMargin;
var private float rightMargin;
var private float topMargin;
var private float bottomMargin;

var private bool  bLeftDrag;
var private bool  bRightDrag;
var private bool  bUpDrag;
var private bool  bDownDrag;

var private float lastMouseX;
var private float lastMouseY;
var private float dragX;
var private float dragY;
var private float dragWidth;
var private float dragHeight;


// ----------------------------------------------------------------------
// Intrinsics

native(1530) function final SetBorders(optional texture bordTL,
                                       optional texture bordTR,
                                       optional texture bordBL,
                                       optional texture bordBR,
                                       optional texture bordL,
                                       optional texture bordR,
                                       optional texture bordT,
                                       optional texture bordB,
                                       optional texture center);
native(1531) function final SetBorderMargins(optional float newLeft,
                                             optional float newRight,
                                             optional float newTop,
                                             optional float newBottom);
native(1532) function final BaseMarginsFromBorder(optional bool bBorder);
native(1533) function final EnableResizing(optional bool bResize);
native(1534) function final SetMoveCursors(optional texture move,
                                           optional texture hMove,
                                           optional texture vMove,
                                           optional texture tlMove,
                                           optional texture trMove);


// ----------------------------------------------------------------------
// SetBorderStyle() : Makes the border solid, translucent or masked

function SetBorderStyle(EDrawStyle newBorderStyle)
{
	borderStyle = newBorderStyle;
}


// ----------------------------------------------------------------------
// SmoothBorder() : Draws the border smoothly, or not

function SmoothBorder(bool bNewSmoothBorder)
{
	bSmoothBorder = bNewSmoothBorder;
}


// ----------------------------------------------------------------------
// StretchBorder() : Draws the border sides as stretched images (if TRUE)
//                   or repeating patterns (if FALSE)

function StretchBorder(bool bNewStretchBorder)
{
	bStretchBorder = bNewStretchBorder;
}

defaultproperties
{
}
