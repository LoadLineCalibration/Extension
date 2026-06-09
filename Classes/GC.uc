//=============================================================================
// GC.
//=============================================================================
class GC extends ExtensionObject
	native
	noexport;

// ----------------------------------------------------------------------
// Variables

var private native Canvas canvas;

var private ClipRect  gcClipRect;

var private byte      style;
var private bool      bSmoothed;
var private bool      bDrawEnabled;
var private bool      bMasked;
var private bool      bTranslucent;
var private bool      bModulated;
var private bool      bTextTranslucent;
var private int       polyFlags;
var private int       textPolyFlags;

var private color     tileColor;
var private plane     tilePlane;

var private color     textColor;
var private plane     textPlane;
var private font      normalFont;
var private font      boldFont;
var private texture   underlineTexture;
var private float     underlineHeight;
var private float     baselineOffset;
var private float     textVSpacing;
var private EHAlign   hAlign;
var private EVAlign   vAlign;
var private bool      bWordWrap;
var private bool      bParseMetachars;

var private int       hMultiplier;
var private int       vMultiplier;

var private bool      bFree;
var private int       gcCount;
var private GC        gcStack;
var private GC        gcFree;
var private GC        gcOwner;


// ----------------------------------------------------------------------
// Intrinsics

native(1200) final function Intersect(float clipX, float clipY,
                                      float clipWidth, float clipHeight);

native(1210) final function EnableSmoothing(bool bNewSmoothing);
native(1211) final function bool IsSmoothingEnabled();
native(1212) final function SetStyle(EDrawStyle newStyle);
native(1213) final function EDrawStyle GetStyle();
native(1214) final function EnableDrawing(bool bDrawEnabled);
native(1215) final function bool IsDrawingEnabled();
native(1216) final function EnableMasking(bool bNewMasking);
native(1217) final function bool IsMaskingEnabled();
native(1218) final function EnableTranslucency(bool bNewTranslucency);
native(1219) final function bool IsTranslucencyEnabled();
native(1220) final function EnableModulation(bool bNewModulation);
native(1221) final function bool IsModulationEnabled();

native(1230) final function SetTileColor(color newTileColor);
native(1231) final function GetTileColor(out color tileColor);

native(1240) final function SetTextColor(color newTextColor);
native(1241) final function GetTextColor(out color textColor);
native(1242) final function SetFont(font newFont);
native(1243) final function SetNormalFont(font newNormalFont);
native(1244) final function SetBoldFont(font newBoldFont);
native(1245) final function SetFonts(font newNormalFont, font newBoldFont);
native(1246) final function GetFonts(out font normalFont, out font boldFont);
native(1247) final function EnableTranslucentText(bool bNewTranslucency);
native(1248) final function bool IsTranslucentTextEnabled();

native(1250) final function SetTextVSpacing(float newVSpacing);
native(1251) final function float GetTextVSpacing();
native(1252) final function SetHorizontalAlignment(EHAlign newHAlign);
native(1253) final function EHAlign GetHorizontalAlignment();
native(1254) final function SetVerticalAlignment(EVAlign newVAlign);
native(1255) final function EVAlign GetVerticalAlignment();
native(1256) final function SetAlignments(EHAlign newHAlign, EVAlign newVAlign);
native(1257) final function GetAlignments(out EHAlign hAlign, out EVAlign vAlign);
native(1258) final function EnableWordWrap(bool bNewWordWrap);
native(1259) final function bool IsWordWrapEnabled();
native(1260) final function EnableSpecialText(bool bNewSpecialText);
native(1261) final function bool IsSpecialTextEnabled();
native(1262) final function SetBaselineData(optional float newBaselineOffset,
                                            optional float newUnderlineHeight);

native(1270) final function CopyGC(gc copy);
native(1271) final function int PushGC();
native(1272) final function PopGC(optional int gcNum);

native(1280) final function GetTextExtent(float destWidth,
                                          out float xExtent, out float yExtent,
                                          coerce string textStr);

native(1281) final function float GetFontHeight(optional bool bIncludeSpace);

native(1282) final function DrawText(float destX, float destY,
                                     float destWidth, float destHeight,
                                     coerce string textStr);

native(1283) final function DrawIcon(float destX, float destY,
                                     texture tx);
native(1284) final function DrawTexture(float destX, float destY,
                                        float destWidth, float destHeight,
                                        float srcX, float srcY,
                                        texture tx);
native(1285) final function DrawPattern(float destX, float destY,
                                        float destWidth, float destHeight,
                                        float orgX, float orgY,
                                        texture tx);

native(1286) final function DrawBox(float destX, float destY,
                                    float destWidth, float destHeight,
                                    float orgX, float orgY,
                                    float boxThickness,
                                    texture tx);

native(1287) final function DrawStretchedTexture(float destX, float destY,
                                                 float destWidth, float destHeight,
                                                 float srcX, float srcY,
                                                 float srcWidth, float srcHeight,
                                                 texture tx);

native(1288) final function DrawActor(Actor actor, optional bool bClearZ,
                                      optional bool bConstrain, optional bool bUnlit,
                                      optional float drawScale, optional float scaleGlow,
                                      optional texture skin);

native(1289) final function DrawBorders(float destX, float destY, float destWidth, float destHeight,
                                        float leftMargin, float rightMargin,
                                        float topMargin, float bottomMargin,
                                        texture borders[9],
                                        optional bool bStretchHorizontally, optional bool bStretchVertically);
// Order of border textures (0-8):
// top left, top right, bottom left, bottom right,
// left, right, top, bottom, center

native(1295) final function ClearZ();


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

defaultproperties
{
     underlineTexture=Texture'Extension.Solid'
}
