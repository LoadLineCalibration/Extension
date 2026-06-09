/*=============================================================================
	ExtensionPackage_Reconstructed.cpp: package/name bootstrap scaffold.
	Pass17.

	This file is a compile-oriented reconstruction layer.  It is kept separate
	from the behavioural reconstructed files so the final VC98 pass can choose
	which implementation units are enabled without duplicating package globals.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_PACKAGE(Extension);


static UBOOL GReconDiagnosticLogging = FALSE;
static UBOOL GReconDiagnosticLoggingLoaded = FALSE;

const TCHAR* XReconGetInterfaceConfigFilename()
{
	// Generic System.ini is translated by FConfigCacheIni to the active game ini
	// passed to the engine, e.g. DeusEx.ini or a mod-specific system ini.
	return TEXT("System.ini");
}

UBOOL XReconShouldLogDiagnostics()
{
	guard(XReconShouldLogDiagnostics);

	if (GReconDiagnosticLoggingLoaded == FALSE)
	{
		UBOOL bEnabled = FALSE;
		const TCHAR* ConfigFilename = XReconGetInterfaceConfigFilename();

		if (GConfig != NULL)
		{
			if (GConfig->GetBool(TEXT("Extension.UI"), TEXT("bLogDiagnostics"), bEnabled, ConfigFilename) == FALSE)
				GConfig->SetBool(TEXT("Extension.UI"), TEXT("bLogDiagnostics"), bEnabled, ConfigFilename);
		}

		GReconDiagnosticLogging = bEnabled;
		GReconDiagnosticLoggingLoaded = TRUE;
	}

	return GReconDiagnosticLogging;

	unguard;
}

UBOOL XReconGetCompatibilityBool(const TCHAR* Key, UBOOL bDefault)
{
	guard(XReconGetCompatibilityBool);

	UBOOL bValue = bDefault;
	const TCHAR* ConfigFilename = XReconGetInterfaceConfigFilename();

	if (GConfig != NULL)
	{
		if (GConfig->GetBool(TEXT("Extension.Compatibility"), Key, bValue, ConfigFilename) == FALSE)
			GConfig->SetBool(TEXT("Extension.Compatibility"), Key, bValue, ConfigFilename);
	}

	return bValue;

	unguard;
}

FName EXTENSION_InitWindow;
FName EXTENSION_DestroyWindow;
FName EXTENSION_WindowReady;
FName EXTENSION_DrawWindow;
FName EXTENSION_PostDrawWindow;
FName EXTENSION_ParentRequestedPreferredSize;
FName EXTENSION_ParentRequestedGranularity;
FName EXTENSION_ChildRequestedVisibilityChange;
FName EXTENSION_ChildRequestedReconfiguration;
FName EXTENSION_ChildRequestedShowArea;
FName EXTENSION_ConfigurationChanged;
FName EXTENSION_VisibilityChanged;
FName EXTENSION_SensitivityChanged;
FName EXTENSION_ChildAdded;
FName EXTENSION_ChildRemoved;
FName EXTENSION_DescendantAdded;
FName EXTENSION_DescendantRemoved;
FName EXTENSION_CursorRequested;
FName EXTENSION_MouseMoved;
FName EXTENSION_MouseEnteredWindow;
FName EXTENSION_MouseLeftWindow;
FName EXTENSION_FocusEnteredWindow;
FName EXTENSION_FocusLeftWindow;
FName EXTENSION_FocusEnteredDescendant;
FName EXTENSION_FocusLeftDescendant;
FName EXTENSION_Tick;
FName EXTENSION_ButtonActivated;
FName EXTENSION_ButtonActivatedRight;
FName EXTENSION_ToggleChanged;
FName EXTENSION_BoxOptionSelected;
FName EXTENSION_ScalePositionChanged;
FName EXTENSION_ScaleRangeChanged;
FName EXTENSION_ScaleAttributesChanged;
FName EXTENSION_ClipAttributesChanged;
FName EXTENSION_ListRowActivated;
FName EXTENSION_ListSelectionChanged;
FName EXTENSION_ClipPositionChanged;
FName EXTENSION_FilterChar;
FName EXTENSION_TextChanged;
FName EXTENSION_EditActivated;
FName EXTENSION_CalcView;
FName EXTENSION_RawMouseButtonPressed;
FName EXTENSION_MouseButtonPressed;
FName EXTENSION_MouseButtonReleased;
FName EXTENSION_RawKeyPressed;
FName EXTENSION_VirtualKeyPressed;
FName EXTENSION_KeyPressed;
FName EXTENSION_AcceleratorKeyPressed;
FName EXTENSION_StyleChanged;
FName EXTENSION_ComputerStart;
FName EXTENSION_ComputerInputFinished;
FName EXTENSION_ComputerFadeOutCompleted;

void ExtInitNames_Reconstructed()
{
	guard(ExtInitNames_Reconstructed);

	EXTENSION_InitWindow = FName(TEXT("InitWindow"), FNAME_Intrinsic);
	EXTENSION_DestroyWindow = FName(TEXT("DestroyWindow"), FNAME_Intrinsic);
	EXTENSION_WindowReady = FName(TEXT("WindowReady"), FNAME_Intrinsic);
	EXTENSION_DrawWindow = FName(TEXT("DrawWindow"), FNAME_Intrinsic);
	EXTENSION_PostDrawWindow = FName(TEXT("PostDrawWindow"), FNAME_Intrinsic);
	EXTENSION_ParentRequestedPreferredSize = FName(TEXT("ParentRequestedPreferredSize"), FNAME_Intrinsic);
	EXTENSION_ParentRequestedGranularity = FName(TEXT("ParentRequestedGranularity"), FNAME_Intrinsic);
	EXTENSION_ChildRequestedVisibilityChange = FName(TEXT("ChildRequestedVisibilityChange"), FNAME_Intrinsic);
	EXTENSION_ChildRequestedReconfiguration = FName(TEXT("ChildRequestedReconfiguration"), FNAME_Intrinsic);
	EXTENSION_ChildRequestedShowArea = FName(TEXT("ChildRequestedShowArea"), FNAME_Intrinsic);
	EXTENSION_ConfigurationChanged = FName(TEXT("ConfigurationChanged"), FNAME_Intrinsic);
	EXTENSION_VisibilityChanged = FName(TEXT("VisibilityChanged"), FNAME_Intrinsic);
	EXTENSION_SensitivityChanged = FName(TEXT("SensitivityChanged"), FNAME_Intrinsic);
	EXTENSION_ChildAdded = FName(TEXT("ChildAdded"), FNAME_Intrinsic);
	EXTENSION_ChildRemoved = FName(TEXT("ChildRemoved"), FNAME_Intrinsic);
	EXTENSION_DescendantAdded = FName(TEXT("DescendantAdded"), FNAME_Intrinsic);
	EXTENSION_DescendantRemoved = FName(TEXT("DescendantRemoved"), FNAME_Intrinsic);
	EXTENSION_CursorRequested = FName(TEXT("CursorRequested"), FNAME_Intrinsic);
	EXTENSION_MouseMoved = FName(TEXT("MouseMoved"), FNAME_Intrinsic);
	EXTENSION_MouseEnteredWindow = FName(TEXT("MouseEnteredWindow"), FNAME_Intrinsic);
	EXTENSION_MouseLeftWindow = FName(TEXT("MouseLeftWindow"), FNAME_Intrinsic);
	EXTENSION_FocusEnteredWindow = FName(TEXT("FocusEnteredWindow"), FNAME_Intrinsic);
	EXTENSION_FocusLeftWindow = FName(TEXT("FocusLeftWindow"), FNAME_Intrinsic);
	EXTENSION_FocusEnteredDescendant = FName(TEXT("FocusEnteredDescendant"), FNAME_Intrinsic);
	EXTENSION_FocusLeftDescendant = FName(TEXT("FocusLeftDescendant"), FNAME_Intrinsic);
	EXTENSION_Tick = FName(TEXT("Tick"), FNAME_Intrinsic);
	EXTENSION_ButtonActivated = FName(TEXT("ButtonActivated"), FNAME_Intrinsic);
	EXTENSION_ButtonActivatedRight = FName(TEXT("ButtonActivatedRight"), FNAME_Intrinsic);
	EXTENSION_ToggleChanged = FName(TEXT("ToggleChanged"), FNAME_Intrinsic);
	EXTENSION_BoxOptionSelected = FName(TEXT("BoxOptionSelected"), FNAME_Intrinsic);
	EXTENSION_ScalePositionChanged = FName(TEXT("ScalePositionChanged"), FNAME_Intrinsic);
	EXTENSION_ScaleRangeChanged = FName(TEXT("ScaleRangeChanged"), FNAME_Intrinsic);
	EXTENSION_ScaleAttributesChanged = FName(TEXT("ScaleAttributesChanged"), FNAME_Intrinsic);
	EXTENSION_ClipAttributesChanged = FName(TEXT("ClipAttributesChanged"), FNAME_Intrinsic);
	EXTENSION_ListRowActivated = FName(TEXT("ListRowActivated"), FNAME_Intrinsic);
	EXTENSION_ListSelectionChanged = FName(TEXT("ListSelectionChanged"), FNAME_Intrinsic);
	EXTENSION_ClipPositionChanged = FName(TEXT("ClipPositionChanged"), FNAME_Intrinsic);
	EXTENSION_FilterChar = FName(TEXT("FilterChar"), FNAME_Intrinsic);
	EXTENSION_TextChanged = FName(TEXT("TextChanged"), FNAME_Intrinsic);
	EXTENSION_EditActivated = FName(TEXT("EditActivated"), FNAME_Intrinsic);
	EXTENSION_CalcView = FName(TEXT("CalcView"), FNAME_Intrinsic);
	EXTENSION_RawMouseButtonPressed = FName(TEXT("RawMouseButtonPressed"), FNAME_Intrinsic);
	EXTENSION_MouseButtonPressed = FName(TEXT("MouseButtonPressed"), FNAME_Intrinsic);
	EXTENSION_MouseButtonReleased = FName(TEXT("MouseButtonReleased"), FNAME_Intrinsic);
	EXTENSION_RawKeyPressed = FName(TEXT("RawKeyPressed"), FNAME_Intrinsic);
	EXTENSION_VirtualKeyPressed = FName(TEXT("VirtualKeyPressed"), FNAME_Intrinsic);
	EXTENSION_KeyPressed = FName(TEXT("KeyPressed"), FNAME_Intrinsic);
	EXTENSION_AcceleratorKeyPressed = FName(TEXT("AcceleratorKeyPressed"), FNAME_Intrinsic);
	EXTENSION_StyleChanged = FName(TEXT("StyleChanged"), FNAME_Intrinsic);
	EXTENSION_ComputerStart = FName(TEXT("ComputerStart"), FNAME_Intrinsic);
	EXTENSION_ComputerInputFinished = FName(TEXT("ComputerInputFinished"), FNAME_Intrinsic);
	EXTENSION_ComputerFadeOutCompleted = FName(TEXT("ComputerFadeOutCompleted"), FNAME_Intrinsic);

	unguard;
}

void RegisterExtensionNatives_Reconstructed()
{
	guard(RegisterExtensionNatives_Reconstructed);

	// Pass17 keeps the actual table in ExtensionNativeRegistration_CompileSkeleton.cpp.
	// Enable EXTENSION_ENABLE_NATIVE_REGISTRATION there once wrappers are enabled.

	unguard;
}
