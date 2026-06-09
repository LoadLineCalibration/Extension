//=============================================================================
// ClipWindow.
//=============================================================================
class ClipWindow extends TabGroupWindow
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

var const int childH;
var const int childV;

var const int prefHUnits;
var const int prefVUnits;

var const bool bForceChildWidth;
var const bool bForceChildHeight;

var const bool bSnapToUnits;
var const bool bFillWindow;

var private int areaHSize;
var private int areaVSize;
var private int childHSize;
var private int childVSize;

var private float hMult;
var private float vMult;


// ----------------------------------------------------------------------
// Intrinsics

native(1680) final function SetChildPosition(int newX, int newY);
native(1681) final function GetChildPosition(out int pNewX, out int pNewY);

native(1682) final function SetUnitSize(int hUnits, int vUnits);
native(1683) final function SetUnitWidth(int hUnits);
native(1684) final function SetUnitHeight(int vUnits);
native(1685) final function ResetUnitSize();
native(1686) final function ResetUnitWidth();
native(1687) final function ResetUnitHeight();
native(1688) final function GetUnitSize(out int pAreaHSize,  out int pAreaVSize,
                                        out int pChildHSize, out int ChildVSize);

native(1689) final function ForceChildSize(optional bool bNewForceChildWidth,
                                           optional bool bNewForceChildHeight);

native(1690) final function EnableSnapToUnits(optional bool bNewSnapToUnits);

native(1691) final function window GetChild();

defaultproperties
{
}
