//=============================================================================
// ExtensionObject.
//=============================================================================
class ExtensionObject extends Object
	native
	noexport;

// ----------------------------------------------------------------------
// Enumerations
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// EFlagType - Flag types

enum EFlagType
{
	FLAG_Bool,
	FLAG_Byte,
	FLAG_Int,
	FLAG_Float,
	FLAG_Name,
	FLAG_Vector,
	FLAG_Rotator,
};


// ----------------------------------------------------------------------
// EHAlign - Horizontal alignments

enum EHAlign
{
	HALIGN_Left,
	HALIGN_Center,
	HALIGN_Right,
	HALIGN_Full
};


// ----------------------------------------------------------------------
// EVAlign - Vertical alignments

enum EVAlign
{
	VALIGN_Top,
	VALIGN_Center,
	VALIGN_Bottom,
	VALIGN_Full
};


// ----------------------------------------------------------------------
// EHDirection - Horizontal directions

enum EHDirection
{
	HDIR_LeftToRight,
	HDIR_RightToLeft
};


// ----------------------------------------------------------------------
// EVDirection - Vertical directions

enum EVDirection
{
	VDIR_TopToBottom,
	VDIR_BottomToTop
};


// ----------------------------------------------------------------------
// EOrder - Horizontal and vertical order

enum EOrder
{
	ORDER_Right,
	ORDER_Left,
	ORDER_Down,
	ORDER_Up,
	ORDER_RightThenDown,
	ORDER_RightThenUp,
	ORDER_LeftThenDown,
	ORDER_LeftThenUp,
	ORDER_DownThenRight,
	ORDER_DownThenLeft,
	ORDER_UpThenRight,
	ORDER_UpThenLeft
};


// ----------------------------------------------------------------------
// EMouseFocusMode - How mouse actions affect focus

enum EMouseFocusMode
{
	MFOCUS_None,
	MFOCUS_Click,
	MFOCUS_Enter,
	MFOCUS_EnterLeave
};


// ----------------------------------------------------------------------
// EMove - Movement direction

enum EMove
{
	MOVE_Left,
	MOVE_Right,
	MOVE_Up,
	MOVE_Down
};


// ----------------------------------------------------------------------
// EOrientation - Orientation (duh)

enum EOrientation
{
	ORIENT_Horizontal,
	ORIENT_Vertical
};


// ----------------------------------------------------------------------
// EMoveList - List window movement enumeration

enum EMoveList
{
	MOVELIST_Up,
	MOVELIST_Down,
	MOVELIST_PageUp,
	MOVELIST_PageDown,
	MOVELIST_Home,
	MOVELIST_End
};


// ----------------------------------------------------------------------
// EMoveInsert - Insertion point movement enumeration

enum EMoveInsert
{
	MOVEINSERT_Up,
	MOVEINSERT_Down,
	MOVEINSERT_Left,
	MOVEINSERT_Right,
	MOVEINSERT_WordLeft,
	MOVEINSERT_WordRight,
	MOVEINSERT_StartOfLine,
	MOVEINSERT_EndOfLine,
	MOVEINSERT_PageUp,
	MOVEINSERT_PageDown,
	MOVEINSERT_Home,
	MOVEINSERT_End
};


// ----------------------------------------------------------------------
// EInsertionPointType - Insertion point cursor type

enum EInsertionPointType
{
	INSTYPE_Insert,
	INSTYPE_Underscore,
	INSTYPE_Block,
	INSTYPE_RawInsert,
	INSTYPE_RawOverlay
};


// ----------------------------------------------------------------------
// EColumnType - Type of data contained in a column

enum EColumnType
{
	COLTYPE_String,
	COLTYPE_Float,
	COLTYPE_Time
};


// ----------------------------------------------------------------------
// EMoveThumb - Enumeration for thumb movement in a scrollbar

enum EMoveThumb
{
	MOVETHUMB_Home,
	MOVETHUMB_End,
	MOVETHUMB_Prev,
	MOVETHUMB_Next,
	MOVETHUMB_StepUp,
	MOVETHUMB_StepDown,
	MOVETHUMB_PageUp,
	MOVETHUMB_PageDown
};


// ----------------------------------------------------------------------
// EDrawStyle - Types of drawing modes

enum EDrawStyle
{
	DSTY_None,
	DSTY_Normal,
	DSTY_Masked,
	DSTY_Translucent,
	DSTY_Modulated
};


// ----------------------------------------------------------------------
// EInputState - Different types of input

enum EInputState
{
	IST_None,
	IST_Press,
	IST_Hold,
	IST_Release,
	IST_Axis
};


// ----------------------------------------------------------------------
// EInputKey - Virtual keypress enumeration

enum EInputKey
{
/*00*/	IK_None			,IK_LeftMouse	,IK_RightMouse	,IK_Cancel		,
/*04*/	IK_MiddleMouse	,IK_Unknown05	,IK_Unknown06	,IK_Unknown07	,
/*08*/	IK_Backspace	,IK_Tab         ,IK_Unknown0A	,IK_Unknown0B	,
/*0C*/	IK_Unknown0C	,IK_Enter	    ,IK_Unknown0E	,IK_Unknown0F	,
/*10*/	IK_Shift		,IK_Ctrl	    ,IK_Alt			,IK_Pause       ,
/*14*/	IK_CapsLock		,IK_Unknown15	,IK_Unknown16	,IK_Unknown17	,
/*18*/	IK_Unknown18	,IK_Unknown19	,IK_Unknown1A	,IK_Escape		,
/*1C*/	IK_Unknown1C	,IK_Unknown1D	,IK_Unknown1E	,IK_Unknown1F	,
/*20*/	IK_Space		,IK_PageUp      ,IK_PageDown    ,IK_End         ,
/*24*/	IK_Home			,IK_Left        ,IK_Up          ,IK_Right       ,
/*28*/	IK_Down			,IK_Select      ,IK_Print       ,IK_Execute     ,
/*2C*/	IK_PrintScrn	,IK_Insert      ,IK_Delete      ,IK_Help		,
/*30*/	IK_0			,IK_1			,IK_2			,IK_3			,
/*34*/	IK_4			,IK_5			,IK_6			,IK_7			,
/*38*/	IK_8			,IK_9			,IK_Unknown3A	,IK_Unknown3B	,
/*3C*/	IK_Unknown3C	,IK_Unknown3D	,IK_Unknown3E	,IK_Unknown3F	,
/*40*/	IK_Unknown40	,IK_A			,IK_B			,IK_C			,
/*44*/	IK_D			,IK_E			,IK_F			,IK_G			,
/*48*/	IK_H			,IK_I			,IK_J			,IK_K			,
/*4C*/	IK_L			,IK_M			,IK_N			,IK_O			,
/*50*/	IK_P			,IK_Q			,IK_R			,IK_S			,
/*54*/	IK_T			,IK_U			,IK_V			,IK_W			,
/*58*/	IK_X			,IK_Y			,IK_Z			,IK_Unknown5B	,
/*5C*/	IK_Unknown5C	,IK_Unknown5D	,IK_Unknown5E	,IK_Unknown5F	,
/*60*/	IK_NumPad0		,IK_NumPad1     ,IK_NumPad2     ,IK_NumPad3     ,
/*64*/	IK_NumPad4		,IK_NumPad5     ,IK_NumPad6     ,IK_NumPad7     ,
/*68*/	IK_NumPad8		,IK_NumPad9     ,IK_GreyStar    ,IK_GreyPlus    ,
/*6C*/	IK_Separator	,IK_GreyMinus	,IK_NumPadPeriod,IK_GreySlash   ,
/*70*/	IK_F1			,IK_F2          ,IK_F3          ,IK_F4          ,
/*74*/	IK_F5			,IK_F6          ,IK_F7          ,IK_F8          ,
/*78*/	IK_F9           ,IK_F10         ,IK_F11         ,IK_F12         ,
/*7C*/	IK_F13			,IK_F14         ,IK_F15         ,IK_F16         ,
/*80*/	IK_F17			,IK_F18         ,IK_F19         ,IK_F20         ,
/*84*/	IK_F21			,IK_F22         ,IK_F23         ,IK_F24         ,
/*88*/	IK_Unknown88	,IK_Unknown89	,IK_Unknown8A	,IK_Unknown8B	,
/*8C*/	IK_Unknown8C	,IK_Unknown8D	,IK_Unknown8E	,IK_Unknown8F	,
/*90*/	IK_NumLock		,IK_ScrollLock  ,IK_Unknown92	,IK_Unknown93	,
/*94*/	IK_Unknown94	,IK_Unknown95	,IK_Unknown96	,IK_Unknown97	,
/*98*/	IK_Unknown98	,IK_Unknown99	,IK_Unknown9A	,IK_Unknown9B	,
/*9C*/	IK_Unknown9C	,IK_Unknown9D	,IK_Unknown9E	,IK_Unknown9F	,
/*A0*/	IK_LShift		,IK_RShift      ,IK_LControl    ,IK_RControl    ,
/*A4*/	IK_UnknownA4	,IK_UnknownA5	,IK_UnknownA6	,IK_UnknownA7	,
/*A8*/	IK_UnknownA8	,IK_UnknownA9	,IK_UnknownAA	,IK_UnknownAB	,
/*AC*/	IK_UnknownAC	,IK_UnknownAD	,IK_UnknownAE	,IK_UnknownAF	,
/*B0*/	IK_UnknownB0	,IK_UnknownB1	,IK_UnknownB2	,IK_UnknownB3	,
/*B4*/	IK_UnknownB4	,IK_UnknownB5	,IK_UnknownB6	,IK_UnknownB7	,
/*B8*/	IK_UnknownB8	,IK_UnknownB9	,IK_Semicolon	,IK_Equals		,
/*BC*/	IK_Comma		,IK_Minus		,IK_Period		,IK_Slash		,
/*C0*/	IK_Tilde		,IK_UnknownC1	,IK_UnknownC2	,IK_UnknownC3	,
/*C4*/	IK_UnknownC4	,IK_UnknownC5	,IK_UnknownC6	,IK_UnknownC7	,
/*C8*/	IK_Joy1	        ,IK_Joy2	    ,IK_Joy3	    ,IK_Joy4	    ,
/*CC*/	IK_Joy5	        ,IK_Joy6	    ,IK_Joy7	    ,IK_Joy8	    ,
/*D0*/	IK_Joy9	        ,IK_Joy10	    ,IK_Joy11	    ,IK_Joy12		,
/*D4*/	IK_Joy13		,IK_Joy14	    ,IK_Joy15	    ,IK_Joy16	    ,
/*D8*/	IK_UnknownD8	,IK_UnknownD9	,IK_UnknownDA	,IK_LeftBracket	,
/*DC*/	IK_Backslash	,IK_RightBracket,IK_SingleQuote	,IK_UnknownDF	,
/*E0*/  IK_JoyX			,IK_JoyY		,IK_JoyZ		,IK_JoyR		,
/*E4*/	IK_MouseX		,IK_MouseY		,IK_MouseZ		,IK_MouseW		,
/*E8*/	IK_JoyU			,IK_JoyV		,IK_UnknownEA	,IK_UnknownEB	,
/*EC*/	IK_MouseWheelUp ,IK_MouseWheelDown,IK_Unknown10E,UK_Unknown10F  ,
/*F0*/	IK_JoyPovUp     ,IK_JoyPovDown	,IK_JoyPovLeft	,IK_JoyPovRight	,
/*F4*/	IK_UnknownF4	,IK_UnknownF5	,IK_Attn		,IK_CrSel		,
/*F8*/	IK_ExSel		,IK_ErEof		,IK_Play		,IK_Zoom		,
/*FC*/	IK_NoName		,IK_PA1			,IK_OEMClear
};


// ----------------------------------------------------------------------
// Structures
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// UnrealScript version of TArray/FArray (originally in Object.uc; moved
// here after removal in version 224)

struct DynamicArray
{
	var const int Num, Max, Ptr;
};


// ----------------------------------------------------------------------
// Clip rectangle

struct ClipRect
{
	var float originX,   originY;
	var float clipX,     clipY;
	var float clipWidth, clipHeight;
};


// ----------------------------------------------------------------------
// Intrinsics

native(1024) final function Name StringToName(coerce string str);

defaultproperties
{
}
