/*=============================================================================
	XPlayerInputGameBridge_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 09.

	Focus: APlayerPawnExt, XInputExt, and XGameEngineExt. These classes are the
	router layer around XRootWindow. They do not own UI behaviour themselves; they
	create the root, feed it render/input events, and fall back to stock engine
	behaviour whenever the root refuses an event.

	This is reconstruction-first source. It is not yet a final VC98 drop-in
	translation unit.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

IMPLEMENT_CLASS(APlayerPawnExt);
IMPLEMENT_CLASS(XInputExt);
IMPLEMENT_CLASS(XGameEngineExt);

APlayerPawnExt::APlayerPawnExt()
: APlayerPawn()
{
	guard(APlayerPawnExt::APlayerPawnExt);

	appMemzero(actorList, sizeof(actorList));
	actorCount = 0;
	rootWindow = NULL;

	if (flagBase == NULL)
		flagBase = new(this) XFlagBase;

	unguard;
}

void APlayerPawnExt::Destroy()
{
	guard(APlayerPawnExt::Destroy);

	if (UObject::GObjInGarbageCollection != 0 || flagBase == NULL)
	{
		flagBase = NULL;
	}
	else
	{
		flagBase->ConditionalDestroy();
		flagBase = NULL;
	}

	if (rootWindow != NULL)
	{
		rootWindow->SafeDestroy();
		rootWindow = NULL;
	}

	AActor::Destroy();

	unguard;
}

void APlayerPawnExt::InitRootWindow()
{
	guard(APlayerPawnExt::InitRootWindow);

	if (rootWindow == NULL)
		ConstructRootWindow();

	unguard;
}

void APlayerPawnExt::ConstructRootWindow()
{
	guard(APlayerPawnExt::ConstructRootWindow);

	// Stock Extension.dll loads the actual root class from [Engine.Engine] Root=...
	// Deus Ex sets that to DeusEx.DeusExRootWindow. Instantiating the plain
	// Extension.RootWindow leaves menu/HUD scripts with the wrong root layer.
	UClass* RootClass = UObject::StaticLoadClass(
		XRootWindow::StaticClass(),
		NULL,
		TEXT("ini:Engine.Engine.Root"),
		NULL,
		LOAD_NoFail,
		NULL);

	rootWindow = CastChecked<XRootWindow>(StaticConstructObject(RootClass, this, NAME_None, 0, NULL, GError));
	rootWindow->Init(this);
	rootWindow->ProcessScript(EXTENSION_InitWindow, NULL, TRUE);
	rootWindow->SetVisibility(TRUE);

	unguard;
}

INT APlayerPawnExt::FindActor(AActor* Actor)
{
	guard(APlayerPawnExt::FindActor);

	INT FoundIndex = INDEX_NONE;
	INT WriteIndex = 0;

	if (actorCount <= 0)
	{
		actorCount = 0;
		return INDEX_NONE;
	}

	for (INT ReadIndex = 0; ReadIndex < actorCount; ReadIndex++)
	{
		AActor* TestActor = actorList[ReadIndex].actor;

		if (TestActor != NULL && TestActor->bDeleteMe == TRUE)
			TestActor = NULL;

		if (TestActor == Actor)
			FoundIndex = ReadIndex;

		if (TestActor != NULL)
		{
			actorList[WriteIndex].actor = TestActor;
			actorList[WriteIndex].refCount = actorList[ReadIndex].refCount;
			WriteIndex++;
		}
	}

	actorCount = WriteIndex;
	return FoundIndex;

	unguard;
}

void APlayerPawnExt::AddActorRef(AActor* Actor)
{
	guard(APlayerPawnExt::AddActorRef);

	if (Actor == NULL)
		return;

	INT Index = FindActor(Actor);
	if (Index >= 0)
	{
		actorList[Index].refCount++;
		return;
	}

	if (actorCount >= MAX_ACTOR_REF)
	{
		for (INT i = 1; i < actorCount; i++)
			actorList[i - 1] = actorList[i];

		actorCount--;
	}

	if (actorCount < MAX_ACTOR_REF)
	{
		actorList[actorCount].actor = Actor;
		actorList[actorCount].refCount = 1;
		actorCount++;
	}

	unguard;
}

void APlayerPawnExt::RemoveActorRef(AActor* Actor)
{
	guard(APlayerPawnExt::RemoveActorRef);

	if (Actor == NULL)
		return;

	INT Index = FindActor(Actor);
	if (Index < 0)
		return;

	actorList[Index].refCount--;

	if (actorList[Index].refCount < 1)
	{
		for (INT i = Index + 1; i < actorCount; i++)
			actorList[i - 1] = actorList[i];

		actorCount--;
	}

	unguard;
}

UBOOL APlayerPawnExt::IsActorValid(AActor* Actor)
{
	guard(APlayerPawnExt::IsActorValid);

	if (Actor == NULL)
		return FALSE;

	return FindActor(Actor) >= 0;

	unguard;
}

UBOOL APlayerPawnExt::SetMousePosition(FLOAT NewX, FLOAT NewY)
{
	guard(APlayerPawnExt::SetMousePosition);

	if (rootWindow != NULL)
		return rootWindow->SetMousePosition(NewX, NewY);

	return FALSE;

	unguard;
}

UBOOL APlayerPawnExt::SetMouseDelta(FLOAT DeltaX, FLOAT DeltaY)
{
	guard(APlayerPawnExt::SetMouseDelta);

	if (rootWindow != NULL)
		return rootWindow->SetMouseDelta(DeltaX, DeltaY);

	return FALSE;

	unguard;
}

void APlayerPawnExt::PreRenderWindows(UCanvas* Canvas)
{
	guard(APlayerPawnExt::PreRenderWindows);

	if (rootWindow != NULL)
		rootWindow->PreRender(Canvas);

	unguard;
}

void APlayerPawnExt::PostRenderWindows(UCanvas* Canvas)
{
	guard(APlayerPawnExt::PostRenderWindows);

	if (rootWindow != NULL)
		rootWindow->PostRender(Canvas);

	unguard;
}

void APlayerPawnExt::execPreRenderWindows(FFrame& Stack, RESULT_DECL)
{
	guard(APlayerPawnExt::execPreRenderWindows);

	P_GET_OBJECT(UCanvas, Canvas);
	P_FINISH;

	PreRenderWindows(Canvas);

	unguardexec;
}

void APlayerPawnExt::execPostRenderWindows(FFrame& Stack, RESULT_DECL)
{
	guard(APlayerPawnExt::execPostRenderWindows);

	P_GET_OBJECT(UCanvas, Canvas);
	P_FINISH;

	PostRenderWindows(Canvas);

	unguardexec;
}

void APlayerPawnExt::execInitRootWindow(FFrame& Stack, RESULT_DECL)
{
	guard(APlayerPawnExt::execInitRootWindow);

	P_FINISH;
	InitRootWindow();

	unguardexec;
}

XInputExt::XInputExt()
: UInput()
{
	guard(XInputExt::XInputExt);

	// Match the stock Extension constructor.  In the game path the original DLL
	// reloads the XInputExt class default object first, then the live viewport
	// input object, both with Propagate=TRUE.  A plain LoadConfig() here can
	// leave Bindings[] empty on the live object, which makes every key/mouse
	// event fall through with no ExecInputCommands() call.
	if (GIsEditor)
	{
		LoadConfig(TRUE, NULL, NULL);
	}
	else
	{
		UClass* ThisClass = XInputExt::StaticClass();
		if (ThisClass != NULL)
		{
			UObject* DefaultObject = ThisClass->GetDefaultObject();
			if (DefaultObject != NULL)
				DefaultObject->LoadConfig(TRUE, NULL, NULL);
		}

		LoadConfig(TRUE, NULL, NULL);
	}

	// Native constructor also runs the common Extension name/object bootstrap.
	ExtInitNames_Reconstructed();
	RegisterExtensionNatives_Reconstructed();

	unguard;
}

UBOOL XInputExt::Process(FOutputDevice& Out, EInputKey Key, EInputAction Action, FLOAT Delta)
{
	guard(XInputExt::Process);

	if ((INT)Key < 0 || (INT)Key >= IK_MAX)
		return FALSE;

	APlayerPawnExt* Player = NULL;
	if (Viewport != NULL)
		Player = Cast<APlayerPawnExt>(Viewport->Actor);

	if (Player != NULL && Player->rootWindow != NULL)
	{
		if (Player->rootWindow->Process(Key, Action, Delta) == TRUE)
		{
			for (INT i = 0; i < IK_MAX; i++)
			{
				if (KeyDownTable[i] != 0)
				{
					KeyDownTable[i] = 0;

					if (Bindings[i].Len() > 0)
					{
						Action = IST_Release;
						Delta = 0.0f;
						ExecInputCommands(*Bindings[i], Out);
						Action = IST_None;
						Delta = 0.0f;
					}
				}
			}

			return TRUE;
		}
	}

	if (Action == IST_Press)
	{
		if (KeyDownTable[Key] != 0)
			return FALSE;

		KeyDownTable[Key] = 1;
	}
	else if (Action == IST_Release)
	{
		if (KeyDownTable[Key] == 0)
			return FALSE;

		KeyDownTable[Key] = 0;
	}

	if (Bindings[Key].Len() > 0)
	{
		this->Action = Action;
		this->Delta = Delta;
		ExecInputCommands(*Bindings[Key], Out);
		this->Action = IST_None;
		this->Delta = 0.0f;
		return TRUE;
	}

	return FALSE;

	unguard;
}

UBOOL XInputExt::Key(EInputKey Key)
{
	guard(XInputExt::Key);

	APlayerPawnExt* Player = NULL;
	if (Viewport != NULL)
		Player = Cast<APlayerPawnExt>(Viewport->Actor);

	if (Player == NULL)
		return FALSE;

	if (Player->rootWindow == NULL)
		return FALSE;

	return Player->rootWindow->Key(Key);

	unguard;
}

XGameEngineExt::XGameEngineExt()
: UGameEngine()
{
	guard(XGameEngineExt::XGameEngineExt);
	unguard;
}

void XGameEngineExt::Init()
{
	guard(XGameEngineExt::Init);

	UGameEngine::Init();
	ExtInitNames_Reconstructed();
	RegisterExtensionNatives_Reconstructed();

	unguard;
}

void XGameEngineExt::Destroy()
{
	guard(XGameEngineExt::Destroy);

	XRootWindow::DestroyAllWindows();
	UGameEngine::Destroy();

	unguard;
}

void XGameEngineExt::Serialize(FArchive& Ar)
{
	guard(XGameEngineExt::Serialize);
	UGameEngine::Serialize(Ar);
	unguard;
}

UBOOL XGameEngineExt::Browse(FURL URL, const TMap<FString,FString>* TravelInfo, FString& Error)
{
	guard(XGameEngineExt::Browse);

	UBOOL bResult = UGameEngine::Browse(URL, TravelInfo, Error);

	if (bResult == TRUE && Client != NULL && Client->Viewports.Num() > 0)
	{
		UViewport* Viewport = Client->Viewports(0);
		APlayerPawnExt* Player = NULL;

		if (Viewport != NULL)
			Player = Cast<APlayerPawnExt>(Viewport->Actor);

		if (Player != NULL)
			Player->InitRootWindow();
	}

	return bResult;

	unguard;
}

void XGameEngineExt::MousePosition(UViewport* Viewport, DWORD Buttons, FLOAT X, FLOAT Y)
{
	guard(XGameEngineExt::MousePosition);

	UBOOL bHandled = FALSE;
	APlayerPawnExt* Player = NULL;

	if (Viewport != NULL)
		Player = Cast<APlayerPawnExt>(Viewport->Actor);

	if (Player != NULL)
		bHandled = Player->SetMousePosition(X, Y);

	if (bHandled == FALSE)
		UGameEngine::MousePosition(Viewport, Buttons, X, Y);

	unguard;
}

void XGameEngineExt::MouseDelta(UViewport* Viewport, DWORD Buttons, FLOAT DeltaX, FLOAT DeltaY)
{
	guard(XGameEngineExt::MouseDelta);

	UBOOL bHandled = FALSE;
	APlayerPawnExt* Player = NULL;

	if (Viewport != NULL)
		Player = Cast<APlayerPawnExt>(Viewport->Actor);

	if (Player != NULL)
		bHandled = Player->SetMouseDelta(DeltaX, DeltaY);

	if (bHandled == FALSE)
		UGameEngine::MouseDelta(Viewport, Buttons, DeltaX, DeltaY);

	unguard;
}

INT XGameEngineExt::Key(UViewport* Viewport, EInputKey Key)
{
	guard(XGameEngineExt::Key);

	INT Result = UEngine::Key(Viewport, Key);

	if (Result == 0 && Viewport != NULL)
	{
		XInputExt* Input = Cast<XInputExt>(Viewport->Input);
		if (Input != NULL)
			Result = Input->Key(Key);
	}

	return Result;

	unguard;
}

void XGameEngineExt::Tick(FLOAT DeltaSeconds)
{
	guard(XGameEngineExt::Tick);

	XRootWindow::TickWindows(DeltaSeconds);
	UGameEngine::Tick(DeltaSeconds);

	unguard;
}
