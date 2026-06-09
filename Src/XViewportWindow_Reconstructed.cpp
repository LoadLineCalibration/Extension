/*=============================================================================
	XViewportWindow_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 10.

	Focus: ViewportWindow behaviour: actor/location camera sources, watch target,
	relative offsets, fallback texture drawing, temporary player/actor render flags,
	FSceneNode creation, Canvas frame swapping, and world-to-viewport projection.

	This is reconstruction-first source. It is intentionally explicit and
	behavioural; it is not yet a final VC98 drop-in translation unit.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

struct XReconViewportCalcViewParms
{
	AActor* OriginActor;
	AActor* WatchActor;
	FVector FrameLocation;
	FRotator FrameRotation;
};

IMPLEMENT_CLASS(XViewportWindow);

static UBOOL ReconIsActorA(UObject* Object, UClass* Class)
{
	if (Object == NULL)
		return FALSE;

	for (UClass* TestClass = Object->GetClass(); TestClass != NULL; TestClass = TestClass->GetSuperClass())
	{
		if (TestClass == Class)
			return TRUE;
	}

	return FALSE;
}

static FVector ReconZeroVector(void)
{
	return FVector(0.0f, 0.0f, 0.0f);
}

static FRotator ReconZeroRotator(void)
{
	return FRotator(0, 0, 0);
}

static FVector ReconRotatedXAxis(const FRotator& Rotation)
{
	FCoords Coords = GMath.UnitCoords / Rotation;
	return Coords.XAxis;
}

static FVector ReconTransformForwardOffset(const FVector& RelativeLocation, const FRotator& BaseRotation)
{
	FLOAT Distance = appSqrt(RelativeLocation.X * RelativeLocation.X + RelativeLocation.Y * RelativeLocation.Y + RelativeLocation.Z * RelativeLocation.Z);
	FVector RelativeLocationCopy = RelativeLocation;
	FRotator RelativeDirection = RelativeLocationCopy.Rotation();
	FRotator FinalRotation = BaseRotation + RelativeDirection;
	FVector Forward = ReconRotatedXAxis(FinalRotation);

	return Forward * Distance;
}

/*-----------------------------------------------------------------------------
	Structors and simple state setters.
-----------------------------------------------------------------------------*/

XViewportWindow::XViewportWindow(XWindow* Parent)
: XWindow(Parent)
{
	guard(XViewportWindow::XViewportWindow);
	unguard;
}

void XViewportWindow::Init(XWindow* Parent)
{
	guard(XViewportWindow::Init);

	XWindow::Init(Parent);

	bDrawRawBackground = FALSE;

	bEnableViewport = TRUE;
	bClearZ = TRUE;
	bShowActor = FALSE;
	bShowWeapons = FALSE;
	bUseViewRotation = TRUE;
	bUseEyeHeight = TRUE;
	bWatchEyeHeight = TRUE;

	fov = 90.0f;
	defaultColor = FColor(0, 0, 0);

	originActor = NULL;
	watchActor = NULL;
	location = ReconZeroVector();
	relLocation = ReconZeroVector();
	rotation = ReconZeroRotator();
	relRotation = ReconZeroRotator();

	bOriginActorDestroyed = FALSE;
	lastLocation = ReconZeroVector();
	lastRotation = ReconZeroRotator();
	viewportFrame = NULL;

	SetViewportActor(GetPlayerPawn(), TRUE, TRUE);

	unguard;
}

void XViewportWindow::CleanUp(void)
{
	guard(XViewportWindow::CleanUp);

	SetViewportActor(NULL, TRUE, TRUE);
	XWindow::CleanUp();

	unguard;
}

void XViewportWindow::Serialize(FArchive& Ar)
{
	guard(XViewportWindow::Serialize);

	UObject::Serialize(Ar);

	unguardobj;
}

void XViewportWindow::SetViewportActor(AActor* NewOriginActor, UBOOL bEyeLevel, UBOOL bEnable)
{
	guard(XViewportWindow::SetViewportActor);

	bEnableViewport = bEnable;
	bUseEyeHeight = bEyeLevel;

	location = ReconZeroVector();
	relLocation = ReconZeroVector();

	if (originActor != NewOriginActor)
	{
		APlayerPawnExt* PlayerPawn = GetPlayerPawn();
		if (PlayerPawn != NULL)
		{
			PlayerPawn->RemoveAdditionalView(originActor);

			if (PlayerPawn != NewOriginActor)
				PlayerPawn->AddAdditionalView(NewOriginActor);
		}

		AddActorRef(NewOriginActor);
		RemoveActorRef(originActor);
		originActor = NewOriginActor;
	}

	if (originActor != NULL)
		lastLocation = originActor->Location;
	else
		lastLocation = location;

	unguard;
}

void XViewportWindow::SetViewportLocation(FVector NewLocation, UBOOL bEnable)
{
	guard(XViewportWindow::SetViewportLocation);

	bEnableViewport = bEnable;
	location = NewLocation;
	relLocation = ReconZeroVector();

	if (originActor != NULL)
	{
		RemoveActorRef(originActor);
		originActor = NULL;
	}

	lastLocation = location;

	unguard;
}

void XViewportWindow::SetWatchActor(AActor* NewWatchActor, UBOOL bEyeLevel)
{
	guard(XViewportWindow::SetWatchActor);

	bWatchEyeHeight = bEyeLevel;
	rotation = ReconZeroRotator();
	relRotation = ReconZeroRotator();

	if (watchActor != NewWatchActor)
	{
		AddActorRef(NewWatchActor);
		RemoveActorRef(watchActor);
		watchActor = NewWatchActor;
	}

	if (watchActor != NULL)
	{
		FVector WatchDelta = watchActor->Location - lastLocation;
		lastRotation = WatchDelta.Rotation();
	}
	else
	{
		lastRotation = rotation;
	}

	unguard;
}

void XViewportWindow::SetRotation(FRotator NewRotation)
{
	guard(XViewportWindow::SetRotation);

	rotation = NewRotation;
	relRotation = ReconZeroRotator();

	if (watchActor != NULL)
	{
		RemoveActorRef(watchActor);
		watchActor = NULL;
	}

	lastRotation = rotation;

	unguard;
}

void XViewportWindow::EnableViewport(UBOOL bEnabled)
{
	bEnableViewport = bEnabled;
}

void XViewportWindow::SetFOVAngle(FLOAT NewAngle)
{
	if (NewAngle > 179.0f)
		NewAngle = 179.0f;

	if (NewAngle < 0.1f)
		NewAngle = 0.1f;

	fov = NewAngle;
}

void XViewportWindow::ShowViewportActor(UBOOL bNewShowActor)
{
	bShowActor = bNewShowActor;
}

void XViewportWindow::ShowWeapons(UBOOL bNewShowWeapons)
{
	bShowWeapons = bNewShowWeapons;
}

void XViewportWindow::SetRelativeLocation(FVector NewRelLocation)
{
	relLocation = NewRelLocation;
}

void XViewportWindow::SetRelativeRotation(FRotator NewRelRotation)
{
	relRotation = NewRelRotation;
}

void XViewportWindow::SetDefaultTexture(UTexture* NewTexture, FColor NewColor)
{
	defaultTexture = NewTexture;
	defaultColor = NewColor;
}

void XViewportWindow::ClearZBuffer(UBOOL bClear)
{
	bClearZ = bClear;
}

/*-----------------------------------------------------------------------------
	Draw pipeline.
-----------------------------------------------------------------------------*/

void XViewportWindow::Draw(XGC* GC)
{
	guard(XViewportWindow::Draw);

	XRootWindow* RootWindow = GetRootWindow();
	if (RootWindow == NULL)
		return;

	/*
		If the origin actor disappeared, the original code freezes the camera at
		the last known location/rotation, removes the actor mode, and raises a
		one-frame bOriginActorDestroyed latch. The latch is cleared at the start
		of the next Draw().
	*/
	if (originActor != NULL && IsActorValid(originActor) == FALSE)
	{
		originActor = NULL;
		location = lastLocation;
		relLocation = ReconZeroVector();
		rotation = lastRotation - relRotation;
		bOriginActorDestroyed = TRUE;
	}

	if (watchActor != NULL && IsActorValid(watchActor) == FALSE)
	{
		watchActor = NULL;
		rotation = lastRotation;
		relRotation = ReconZeroRotator();
	}

	if (bOriginActorDestroyed == TRUE)
		bOriginActorDestroyed = FALSE;

	UBOOL bRenderedViewport = FALSE;

	if (bEnableViewport == TRUE)
	{
		UCanvas* Canvas = NULL;
		UViewport* Viewport = NULL;

		if (GC != NULL)
			Canvas = GC->GetCanvas();

		if (Canvas != NULL)
			Viewport = Canvas->Viewport;

		URenderBase* Renderer = GetRenderer();

		AActor* EffectiveOriginPawn = NULL;
		if (originActor != NULL)
		{
			if (ReconIsActorA(originActor, APawn::StaticClass()) == TRUE)
				EffectiveOriginPawn = originActor;
		}

		APlayerPawnExt* PlayerPawn = GetPlayerPawn();

		FVector FrameLocation;
		FRotator FrameRotation;

		if (originActor != NULL)
		{
			FrameLocation = originActor->Location;

			if (EffectiveOriginPawn != NULL && bUseEyeHeight == TRUE)
				FrameLocation.Z += ((APawn*)EffectiveOriginPawn)->EyeHeight;

			APlayerPawn* OriginPlayerPawn = NULL;
			if (ReconIsActorA(originActor, APlayerPawn::StaticClass()) == TRUE)
				OriginPlayerPawn = (APlayerPawn*)originActor;

			if (OriginPlayerPawn != NULL && bUseViewRotation == TRUE)
				FrameRotation = OriginPlayerPawn->ViewRotation;
			else
				FrameRotation = originActor->Rotation;
		}
		else
		{
			FrameLocation = location;
			FrameRotation = rotation;
		}

		FrameLocation += ReconTransformForwardOffset(relLocation, FrameRotation);

		if (watchActor != NULL)
		{
			FVector WatchLocation = watchActor->Location;

			if (bWatchEyeHeight == TRUE && ReconIsActorA(watchActor, APawn::StaticClass()) == TRUE)
				WatchLocation.Z += ((APawn*)watchActor)->EyeHeight;

			FrameRotation = (WatchLocation - FrameLocation).Rotation();
		}

		FrameRotation += relRotation;

		CalcView(originActor, watchActor, FrameLocation, FrameRotation);

		lastLocation = FrameLocation;
		lastRotation = FrameRotation;

		if (Renderer != NULL && Viewport != NULL)
		{
			FSceneNode* NewFrame = Renderer->CreateMasterFrame(Viewport, FrameLocation, FrameRotation, NULL);
			if (NewFrame != NULL)
			{
				viewportFrame = NewFrame;

				NewFrame->XB = (INT)((clipRect.clipX + clipRect.originX) * RootWindow->hMultiplier);
				NewFrame->YB = (INT)((clipRect.clipY + clipRect.originY) * RootWindow->vMultiplier);
				NewFrame->X = (INT)(clipRect.clipWidth * RootWindow->hMultiplier);
				NewFrame->Y = (INT)(clipRect.clipHeight * RootWindow->vMultiplier);

				if (Canvas != NULL && Canvas->Frame != NULL)
				{
					NewFrame->XB += Canvas->Frame->XB;
					NewFrame->YB += Canvas->Frame->YB;
				}

				UBOOL bSavedBehindView = FALSE;
				FLOAT SavedFOV = 0.0f;
				UBOOL bSavedOriginHidden = FALSE;

				if (PlayerPawn != NULL)
				{
					bSavedBehindView = PlayerPawn->bBehindView;
					SavedFOV = PlayerPawn->FovAngle;

					if (bShowWeapons == TRUE && PlayerPawn == originActor)
						PlayerPawn->bBehindView = FALSE;
					else
						PlayerPawn->bBehindView = TRUE;

					PlayerPawn->FovAngle = fov;
				}

				if (originActor != NULL)
				{
					bSavedOriginHidden = originActor->bHidden;

					if (bShowActor == TRUE)
						originActor->bHidden = FALSE;
					else
						originActor->bHidden = TRUE;
				}

				NewFrame->ComputeRenderSize();

				if (NewFrame->X > 0 && NewFrame->Y > 0)
				{
					FSceneNode* SavedCanvasFrame = Canvas->Frame;
					FLOAT SavedClipX = Canvas->ClipX;
					FLOAT SavedClipY = Canvas->ClipY;
					INT SavedCanvasX = Canvas->X;
					INT SavedCanvasY = Canvas->Y;

					Canvas->Frame = NewFrame;
					Canvas->X = NewFrame->X;
					Canvas->Y = NewFrame->Y;
					Canvas->ClipX = (FLOAT)NewFrame->X;
					Canvas->ClipY = (FLOAT)NewFrame->Y;

					if (bClearZ == TRUE)
						Viewport->RenDev->ClearZ(NewFrame);

					RenderFrame(Renderer, NewFrame);
					bRenderedViewport = TRUE;

					Canvas->Frame = SavedCanvasFrame;
					Canvas->ClipX = SavedClipX;
					Canvas->ClipY = SavedClipY;
					Canvas->X = SavedCanvasX;
					Canvas->Y = SavedCanvasY;
				}

				if (originActor != NULL)
					originActor->bHidden = bSavedOriginHidden;

				if (PlayerPawn != NULL)
				{
					PlayerPawn->FovAngle = SavedFOV;
					PlayerPawn->bBehindView = bSavedBehindView;
				}
			}
		}
	}

	if (bRenderedViewport == FALSE)
	{
		GC->PushGC();
		GC->SetStyle(STY_Normal);
		GC->SetTileColor(defaultColor);
		GC->DrawPattern(0.0f, 0.0f, width, height, 0.0f, 0.0f, defaultTexture);
		GC->PopGC();
	}

	XWindow::Draw(GC);

	unguard;
}

void XViewportWindow::PostDraw(XGC* GC)
{
	guard(XViewportWindow::PostDraw);

	XWindow::PostDraw(GC);

	if (viewportFrame != NULL)
	{
		URenderBase* Renderer = GetRenderer();
		if (XReconGetCompatibilityBool(TEXT("bUseOriginalViewportPostDrawNullRendererBug"), TRUE) == TRUE)
		{
			Renderer->FinishMasterFrame();
		}
		else if (Renderer != NULL)
		{
			Renderer->FinishMasterFrame();
		}

		viewportFrame = NULL;
	}

	unguard;
}

UBOOL XViewportWindow::ConvertVectorToCoordinates(FVector WorldLocation, FLOAT* OutRelativeX, FLOAT* OutRelativeY)
{
	guard(XViewportWindow::ConvertVectorToCoordinates);

	check(viewportFrame);

	FVector Delta = WorldLocation - viewportFrame->Coords.Origin;

	FLOAT LocalX = Delta | viewportFrame->Coords.XAxis;
	FLOAT LocalY = Delta | viewportFrame->Coords.YAxis;
	FLOAT LocalZ = Delta | viewportFrame->Coords.ZAxis;

	FLOAT PixelX = viewportFrame->FX2;
	FLOAT PixelY = viewportFrame->FY2;
	UBOOL bInFront = FALSE;

	if (LocalZ > 1.0f)
	{
		FLOAT Scale = viewportFrame->Proj.Z / LocalZ;
		PixelX = (FLOAT)((INT)(LocalX * Scale + viewportFrame->FX2));
		PixelY = (FLOAT)((INT)(LocalY * Scale + viewportFrame->FY2));
		bInFront = TRUE;
	}
	else
	{
		PixelX = (FLOAT)((INT)viewportFrame->FX2);
		PixelY = (FLOAT)((INT)viewportFrame->FY2);
	}

	if (OutRelativeX != NULL)
		*OutRelativeX = PixelX;

	if (OutRelativeY != NULL)
		*OutRelativeY = PixelY;

	return bInFront;

	unguard;
}

void XViewportWindow::CalcView(AActor* InOriginActor, AActor* InWatchActor, FVector& FrameLocation, FRotator& FrameRotation)
{
	guard(XViewportWindow::CalcView);

	XReconViewportCalcViewParms Parms;

	Parms.OriginActor = InOriginActor;
	Parms.WatchActor = InWatchActor;
	Parms.FrameLocation = FrameLocation;
	Parms.FrameRotation = FrameRotation;

	ProcessScript(EXTENSION_CalcView, &Parms, FALSE);

	FrameLocation = Parms.FrameLocation;
	FrameRotation = Parms.FrameRotation;

	unguard;
}

void XViewportWindow::RenderFrame(URenderBase* Render, FSceneNode* Frame)
{
	guard(XViewportWindow::RenderFrame);

	if (Frame->X > 0 && Frame->Y > 0)
		Render->DrawWorld(Frame);

	unguard;
}

URenderBase* XViewportWindow::GetRenderer(void)
{
	guard(XViewportWindow::GetRenderer);

	APlayerPawnExt* PlayerPawn = GetPlayerPawn();
	if (PlayerPawn != NULL)
	{
		ULevel* Level = PlayerPawn->XLevel;
		if (Level != NULL)
		{
			UEngine* Engine = Level->Engine;
			if (Engine != NULL)
				return Engine->Render;
		}
	}

	return NULL;

	unguard;
}

/*-----------------------------------------------------------------------------
	Native wrappers.
-----------------------------------------------------------------------------*/

void XViewportWindow::execSetViewportActor(FFrame& Stack, RESULT_DECL)
{
	guard(XViewportWindow::execSetViewportActor);

	P_GET_OBJECT_OPTX(AActor, NewOriginActor, NULL);
	P_GET_UBOOL_OPTX(bEyeLevel, TRUE);
	P_GET_UBOOL_OPTX(bEnable, TRUE);
	P_FINISH;

	SetViewportActor(NewOriginActor, bEyeLevel, bEnable);

	unguardexec;
}

void XViewportWindow::execSetViewportLocation(FFrame& Stack, RESULT_DECL)
{
	guard(XViewportWindow::execSetViewportLocation);

	P_GET_VECTOR(NewLocation);
	P_GET_UBOOL_OPTX(bEnable, TRUE);
	P_FINISH;

	SetViewportLocation(NewLocation, bEnable);

	unguardexec;
}

void XViewportWindow::execSetWatchActor(FFrame& Stack, RESULT_DECL)
{
	guard(XViewportWindow::execSetWatchActor);

	P_GET_OBJECT_OPTX(AActor, NewWatchActor, NULL);
	P_GET_UBOOL_OPTX(bEyeLevel, TRUE);
	P_FINISH;

	SetWatchActor(NewWatchActor, bEyeLevel);

	unguardexec;
}

void XViewportWindow::execSetRotation(FFrame& Stack, RESULT_DECL)
{
	guard(XViewportWindow::execSetRotation);

	P_GET_ROTATOR_OPTX(NewRotation, FRotator(0, 0, 0));
	P_FINISH;

	SetRotation(NewRotation);

	unguardexec;
}

void XViewportWindow::execEnableViewport(FFrame& Stack, RESULT_DECL)
{
	guard(XViewportWindow::execEnableViewport);

	P_GET_UBOOL_OPTX(bEnable, TRUE);
	P_FINISH;

	EnableViewport(bEnable);

	unguardexec;
}

void XViewportWindow::execSetFOVAngle(FFrame& Stack, RESULT_DECL)
{
	guard(XViewportWindow::execSetFOVAngle);

	P_GET_FLOAT_OPTX(NewAngle, 90.0f);
	P_FINISH;

	SetFOVAngle(NewAngle);

	unguardexec;
}

void XViewportWindow::execShowViewportActor(FFrame& Stack, RESULT_DECL)
{
	guard(XViewportWindow::execShowViewportActor);

	P_GET_UBOOL_OPTX(bShow, TRUE);
	P_FINISH;

	ShowViewportActor(bShow);

	unguardexec;
}

void XViewportWindow::execShowWeapons(FFrame& Stack, RESULT_DECL)
{
	guard(XViewportWindow::execShowWeapons);

	P_GET_UBOOL_OPTX(bShow, TRUE);
	P_FINISH;

	ShowWeapons(bShow);

	unguardexec;
}

void XViewportWindow::execSetRelativeLocation(FFrame& Stack, RESULT_DECL)
{
	guard(XViewportWindow::execSetRelativeLocation);

	P_GET_VECTOR_OPTX(NewRelLocation, FVector(0.0f, 0.0f, 0.0f));
	P_FINISH;

	SetRelativeLocation(NewRelLocation);

	unguardexec;
}

void XViewportWindow::execSetRelativeRotation(FFrame& Stack, RESULT_DECL)
{
	guard(XViewportWindow::execSetRelativeRotation);

	P_GET_ROTATOR_OPTX(NewRelRotation, FRotator(0, 0, 0));
	P_FINISH;

	SetRelativeRotation(NewRelRotation);

	unguardexec;
}

void XViewportWindow::execSetDefaultTexture(FFrame& Stack, RESULT_DECL)
{
	guard(XViewportWindow::execSetDefaultTexture);

	P_GET_OBJECT_OPTX(UTexture, NewTexture, NULL);
	P_GET_STRUCT_OPTX(FColor, NewColor, FColor(255, 255, 255));
	P_FINISH;

	SetDefaultTexture(NewTexture, NewColor);

	unguardexec;
}

void XViewportWindow::execClearZBuffer(FFrame& Stack, RESULT_DECL)
{
	guard(XViewportWindow::execClearZBuffer);

	P_GET_UBOOL_OPTX(bClear, TRUE);
	P_FINISH;

	ClearZBuffer(bClear);

	unguardexec;
}
