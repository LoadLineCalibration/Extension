//=============================================================================
// TabGroupWindow.
//=============================================================================
class TabGroupWindow extends Window
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

// Sorted list of selectable children
var native private DynamicArray rowMajorWindowList;
var native private DynamicArray colMajorWindowList;

// Variables that control window sizing
var bool                 bSizeParentToChildren;
var bool                 bSizeChildrenToParent;

// Our index into our modal's list of tab groups
var private int          tabGroupIndex;

// Absolute position of first child in tab group
var private float        firstAbsX;
var private float        firstAbsY;

defaultproperties
{
}
