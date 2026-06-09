/*=============================================================================
	XGC_TilesAndActors_Reconstructed.cpp
	Reconstructed from Extension.dll.c, pass 02.

	Focus: clipping tiles, texture drawing, borders and actor drawing.
=============================================================================*/

#include "ExtensionPrivate_Reconstructed.h"

static inline INT XReconRoundTile(FLOAT Value)
{
	return INT(Value + 0.1f);
}

/*-----------------------------------------------------------------------------
	Reconstruction helper.

	Original Extension has two internal draw paths:
		1. XGC::DrawTile() uses tilePlane/polyFlags.
		2. XGC::DrawChar() uses textPlane/textPolyFlags.

	Hex-Rays exposes the text path inline inside DrawChar, so this helper is not an
	original exported symbol. It keeps the reconstructed source readable.
-----------------------------------------------------------------------------*/

void XGC::DrawTileRaw(UTexture* texture,
	FLOAT destX, FLOAT destY, FLOAT destWidth, FLOAT destHeight,
	FLOAT srcX, FLOAT srcY, FLOAT srcWidth, FLOAT srcHeight,
	UBOOL bWrapHorizontal, UBOOL bWrapVertical,
	FPlane& drawPlane, DWORD drawPolyFlags)
{
	guard(XGC::DrawTileRaw);

	if (bDrawEnabled == FALSE)
		return;

	if (canvas == NULL || texture == NULL)
		return;

	ClipTile(texture,
		destX,
		destY,
		destWidth,
		destHeight,
		srcX,
		srcY,
		srcWidth,
		srcHeight,
		bWrapHorizontal,
		bWrapVertical);

	if (destWidth <= 0.0f || destHeight <= 0.0f || srcWidth <= 0.0f || srcHeight <= 0.0f)
		return;

	/*
		Do not reuse the color plane as temporary storage for UL/VL/Z.
		Hex-Rays shows the original compiler reusing one stack slot around the
		UCanvas::DrawTile call, so v24.X/v24.Y look like source dimensions in
		the decompiled output. In real C++ DrawTile takes UL/VL as separate
		arguments and the FPlane argument must remain the actual RGB draw color.
		Overwriting X/Y with srcWidth/srcHeight clamps to red+green and makes
		every UI element bright yellow.
	*/
	FPlane DrawColor = drawPlane;

	INT CanvasX = hMultiplier * XReconRoundTile(destX);
	INT CanvasY = vMultiplier * XReconRoundTile(destY);
	INT CanvasW = hMultiplier * XReconRoundTile(destWidth);
	INT CanvasH = vMultiplier * XReconRoundTile(destHeight);
	INT SourceX = XReconRoundTile(srcX);
	INT SourceY = XReconRoundTile(srcY);
	INT SourceW = XReconRoundTile(srcWidth);
	INT SourceH = XReconRoundTile(srcHeight);

	canvas->DrawTile(texture,
		CanvasX,
		CanvasY,
		CanvasW,
		CanvasH,
		SourceX,
		SourceY,
		SourceW,
		SourceH,
		NULL,
		canvas->Z,
		DrawColor,
		FPlane(0,0,0,0),
		drawPolyFlags);

	unguard;
}

void XGC::ClipTile(UTexture* texture,
	FLOAT& destX, FLOAT& destY, FLOAT& destWidth, FLOAT& destHeight,
	FLOAT& srcX, FLOAT& srcY, FLOAT& srcWidth, FLOAT& srcHeight,
	UBOOL bWrapHorizontal, UBOOL bWrapVertical)
{
	guard(XGC::ClipTile);

	destX += clipRect.originX;
	destY += clipRect.originY;

	FLOAT ClipAbsX = clipRect.originX + clipRect.clipX;
	FLOAT ClipAbsY = clipRect.originY + clipRect.clipY;

	if (bWrapHorizontal == TRUE)
	{
		if (destX < ClipAbsX)
		{
			FLOAT Delta = destX - ClipAbsX;
			srcX -= Delta;
			destWidth += Delta;
			destX = ClipAbsX;
		}

		FLOAT ClipRight = ClipAbsX + clipRect.clipWidth;
		if (destX + destWidth > ClipRight)
			destWidth = ClipRight - destX;

		srcWidth = destWidth;
	}
	else
	{
		if (destWidth > 0.0f && destX < ClipAbsX)
		{
			FLOAT Delta = destX - ClipAbsX;
			FLOAT SourceDelta = Delta * srcWidth / destWidth;
			srcWidth += SourceDelta;
			srcX -= SourceDelta;
			destWidth += Delta;
			destX = ClipAbsX;
		}

		if (destWidth > 0.0f)
		{
			FLOAT DestRight = destX + destWidth;
			FLOAT ClipRight = ClipAbsX + clipRect.clipWidth;
			if (DestRight > ClipRight)
			{
				srcWidth += (ClipRight - DestRight) * srcWidth / destWidth;
				destWidth = ClipRight - destX;
			}
		}

		if (srcWidth > 0.0f && srcX < 0.0f)
		{
			FLOAT SourceDelta = srcX;
			FLOAT DestDelta = SourceDelta * destWidth / srcWidth;
			destWidth += DestDelta;
			destX -= DestDelta;
			srcWidth += SourceDelta;
			srcX = 0.0f;
		}

		if (srcWidth > 0.0f && texture != NULL)
		{
			FLOAT SourceRight = srcX + srcWidth;
			if (SourceRight > FLOAT(texture->USize))
			{
				destWidth += (FLOAT(texture->USize) - SourceRight) * destWidth / srcWidth;
				srcWidth = FLOAT(texture->USize) - srcX;
			}
		}
	}

	if (bWrapVertical == TRUE)
	{
		if (destY < ClipAbsY)
		{
			FLOAT Delta = destY - ClipAbsY;
			srcY -= Delta;
			destHeight += Delta;
			destY = ClipAbsY;
		}

		FLOAT ClipBottom = ClipAbsY + clipRect.clipHeight;
		if (destY + destHeight > ClipBottom)
			destHeight = ClipBottom - destY;

		srcHeight = destHeight;
	}
	else
	{
		if (destHeight > 0.0f && destY < ClipAbsY)
		{
			FLOAT Delta = destY - ClipAbsY;
			FLOAT SourceDelta = Delta * srcHeight / destHeight;
			srcHeight += SourceDelta;
			srcY -= SourceDelta;
			destHeight += Delta;
			destY = ClipAbsY;
		}

		if (destHeight > 0.0f)
		{
			FLOAT DestBottom = destY + destHeight;
			FLOAT ClipBottom = ClipAbsY + clipRect.clipHeight;
			if (DestBottom > ClipBottom)
			{
				srcHeight += (ClipBottom - DestBottom) * srcHeight / destHeight;
				destHeight = ClipBottom - destY;
			}
		}

		if (srcHeight > 0.0f && srcY < 0.0f)
		{
			FLOAT SourceDelta = srcY;
			FLOAT DestDelta = SourceDelta * destHeight / srcHeight;
			destHeight += DestDelta;
			destY -= DestDelta;
			srcHeight += SourceDelta;
			srcY = 0.0f;
		}

		if (srcHeight > 0.0f && texture != NULL)
		{
			FLOAT SourceBottom = srcY + srcHeight;
			if (SourceBottom > FLOAT(texture->VSize))
			{
				destHeight += (FLOAT(texture->VSize) - SourceBottom) * destHeight / srcHeight;
				srcHeight = FLOAT(texture->VSize) - srcY;
			}
		}
	}

	unguard;
}

void XGC::DrawTile(UTexture* texture,
	FLOAT destX, FLOAT destY, FLOAT destWidth, FLOAT destHeight,
	FLOAT srcX, FLOAT srcY, FLOAT srcWidth, FLOAT srcHeight,
	UBOOL bWrapHorizontal, UBOOL bWrapVertical)
{
	guard(XGC::DrawTile);

	DrawTileRaw(texture,
		destX,
		destY,
		destWidth,
		destHeight,
		srcX,
		srcY,
		srcWidth,
		srcHeight,
		bWrapHorizontal,
		bWrapVertical,
		tilePlane,
		polyFlags);

	unguard;
}

void XGC::DrawIconPattern(FLOAT destX, FLOAT destY, FLOAT destWidth, FLOAT destHeight,
	FLOAT srcX, FLOAT srcY, FLOAT srcWidth, FLOAT srcHeight, UTexture* texture)
{
	guard(XGC::DrawIconPattern);

	if (texture != NULL && bDrawEnabled == TRUE)
	{
		UBOOL bWrapHorizontal = FALSE;
		UBOOL bWrapVertical = FALSE;

		if (srcWidth == 0.0f)
			bWrapHorizontal = TRUE;

		if (srcHeight == 0.0f)
			bWrapVertical = TRUE;

		DrawTile(texture, destX, destY, destWidth, destHeight, srcX, srcY, srcWidth, srcHeight, bWrapHorizontal, bWrapVertical);
	}

	unguard;
}

void XGC::DrawStretchedIcon(FLOAT destX, FLOAT destY, FLOAT destWidth, FLOAT destHeight, UTexture* texture)
{
	guard(XGC::DrawStretchedIcon);

	if (texture != NULL && bDrawEnabled == TRUE)
		DrawTile(texture, destX, destY, destWidth, destHeight, 0.0f, 0.0f, FLOAT(texture->USize), FLOAT(texture->VSize), FALSE, FALSE);

	unguard;
}

void XGC::DrawScaledIcon(FLOAT destX, FLOAT destY, FLOAT scaleX, FLOAT scaleY, UTexture* texture)
{
	guard(XGC::DrawScaledIcon);

	if (texture != NULL && bDrawEnabled == TRUE)
		DrawTile(texture, destX, destY, FLOAT(texture->USize) * scaleX, FLOAT(texture->VSize) * scaleY, 0.0f, 0.0f, FLOAT(texture->USize), FLOAT(texture->VSize), FALSE, FALSE);

	unguard;
}

void XGC::DrawIcon(FLOAT destX, FLOAT destY, UTexture* texture)
{
	guard(XGC::DrawIcon);

	if (texture != NULL && bDrawEnabled == TRUE)
		DrawStretchedIcon(destX, destY, FLOAT(texture->USize), FLOAT(texture->VSize), texture);

	unguard;
}

void XGC::DrawPattern(FLOAT destX, FLOAT destY, FLOAT destWidth, FLOAT destHeight,
	FLOAT orgX, FLOAT orgY, UTexture* texture)
{
	guard(XGC::DrawPattern);

	if (texture != NULL && bDrawEnabled == TRUE)
		DrawTile(texture, destX, destY, destWidth, destHeight, orgX, orgY, 0.0f, 0.0f, TRUE, TRUE);

	unguard;
}

void XGC::DrawTexture(FLOAT destX, FLOAT destY, FLOAT destWidth, FLOAT destHeight,
	FLOAT srcX, FLOAT srcY, UTexture* texture)
{
	guard(XGC::DrawTexture);

	if (texture != NULL && bDrawEnabled == TRUE)
		DrawIconPattern(destX, destY, destWidth, destHeight, srcX, srcY, destWidth, destHeight, texture);

	unguard;
}

void XGC::DrawStretchedTexture(FLOAT destX, FLOAT destY, FLOAT destWidth, FLOAT destHeight,
	FLOAT srcX, FLOAT srcY, FLOAT srcWidth, FLOAT srcHeight, UTexture* texture)
{
	guard(XGC::DrawStretchedTexture);

	if (texture != NULL && bDrawEnabled == TRUE)
		DrawTile(texture, destX, destY, destWidth, destHeight, srcX, srcY, srcWidth, srcHeight, FALSE, FALSE);

	unguard;
}

void XGC::DrawBox(FLOAT destX, FLOAT destY, FLOAT destWidth, FLOAT destHeight,
	FLOAT orgX, FLOAT orgY, FLOAT thickness, UTexture* texture)
{
	guard(XGC::DrawBox);

	if (texture == NULL || bDrawEnabled == FALSE || thickness <= 0.0f)
		return;

	FLOAT Twice = thickness + thickness;

	if (destWidth <= Twice || destHeight <= Twice)
	{
		DrawPattern(destX, destY, destWidth, destHeight, orgX, orgY, texture);
		return;
	}

	DrawPattern(destX, destY, destWidth, thickness, orgX, orgY, texture);
	DrawPattern(destX, destY + destHeight - thickness, destWidth, thickness, orgX, orgY + destHeight - thickness, texture);
	DrawPattern(destX, destY + thickness, thickness, destHeight - Twice, orgX, orgY + thickness, texture);
	DrawPattern(destX + destWidth - thickness, destY + thickness, thickness, destHeight - Twice, orgX + destWidth - thickness, orgY + thickness, texture);

	unguard;
}

void XGC::DrawBorders(FLOAT destX, FLOAT destY, FLOAT destWidth, FLOAT destHeight,
	FLOAT leftMargin, FLOAT rightMargin, FLOAT topMargin, FLOAT bottomMargin,
	UTexture* bordTL, UTexture* bordTR, UTexture* bordBL, UTexture* bordBR,
	UTexture* bordL, UTexture* bordR, UTexture* bordT, UTexture* bordB,
	UTexture* center, UBOOL bStretchHorizontally, UBOOL bStretchVertically)
{
	guard(XGC::DrawBorders);

	if (bDrawEnabled == FALSE || destWidth <= 0.0f || destHeight <= 0.0f)
		return;

	FLOAT Left = 0.0f;
	FLOAT Right = 0.0f;
	FLOAT Top = 0.0f;
	FLOAT Bottom = 0.0f;

	if (bordTL != NULL)
	{
		if (FLOAT(bordTL->USize) > Left)
			Left = FLOAT(bordTL->USize);
		if (FLOAT(bordTL->VSize) > Top)
			Top = FLOAT(bordTL->VSize);
	}
	if (bordTR != NULL)
	{
		if (FLOAT(bordTR->USize) > Right)
			Right = FLOAT(bordTR->USize);
		if (FLOAT(bordTR->VSize) > Top)
			Top = FLOAT(bordTR->VSize);
	}
	if (bordBL != NULL)
	{
		if (FLOAT(bordBL->USize) > Left)
			Left = FLOAT(bordBL->USize);
		if (FLOAT(bordBL->VSize) > Bottom)
			Bottom = FLOAT(bordBL->VSize);
	}
	if (bordBR != NULL)
	{
		if (FLOAT(bordBR->USize) > Right)
			Right = FLOAT(bordBR->USize);
		if (FLOAT(bordBR->VSize) > Bottom)
			Bottom = FLOAT(bordBR->VSize);
	}
	if (bordL != NULL && FLOAT(bordL->USize) > Left)
		Left = FLOAT(bordL->USize);
	if (bordR != NULL && FLOAT(bordR->USize) > Right)
		Right = FLOAT(bordR->USize);
	if (bordT != NULL && FLOAT(bordT->VSize) > Top)
		Top = FLOAT(bordT->VSize);
	if (bordB != NULL && FLOAT(bordB->VSize) > Bottom)
		Bottom = FLOAT(bordB->VSize);

	if (leftMargin > 0.0f)
		Left = leftMargin;
	if (rightMargin > 0.0f)
		Right = rightMargin;
	if (topMargin > 0.0f)
		Top = topMargin;
	if (bottomMargin > 0.0f)
		Bottom = bottomMargin;

	FLOAT CenterWidth = destWidth - (Left + Right);
	FLOAT CenterHeight = destHeight - (Top + Bottom);

	if (CenterWidth < 0.0f)
	{
		FLOAT Scale = destWidth / (Left + Right);
		CenterWidth = 0.0f;
		Left *= Scale;
		Right *= Scale;
	}

	if (CenterHeight < 0.0f)
	{
		FLOAT Scale = destHeight / (Top + Bottom);
		CenterHeight = 0.0f;
		Top *= Scale;
		Bottom *= Scale;
	}

	FLOAT XLeft = destX;
	FLOAT XCenter = destX + Left;
	FLOAT XRight = XCenter + CenterWidth;
	FLOAT YTop = destY;
	FLOAT YCenter = destY + Top;
	FLOAT YBottom = YCenter + CenterHeight;

	if (bordTL != NULL)
		DrawIconPattern(XLeft, YTop, Left, Top, FLOAT(bordTL->USize) - Left, FLOAT(bordTL->VSize) - Top, Left, Top, bordTL);
	if (bordTR != NULL)
		DrawIconPattern(XRight, YTop, Right, Top, 0.0f, FLOAT(bordTR->VSize) - Top, Right, Top, bordTR);
	if (bordBL != NULL)
		DrawIconPattern(XLeft, YBottom, Left, Bottom, FLOAT(bordBL->USize) - Left, 0.0f, Left, Bottom, bordBL);
	if (bordBR != NULL)
		DrawIconPattern(XRight, YBottom, Right, Bottom, 0.0f, 0.0f, Right, Bottom, bordBR);

	if (bordL != NULL)
	{
		FLOAT SourceHeight = 0.0f;
		if (bStretchVertically == TRUE)
			SourceHeight = FLOAT(bordL->VSize);
		DrawIconPattern(XLeft, YCenter, Left, CenterHeight, FLOAT(bordL->USize) - Left, 0.0f, Left, SourceHeight, bordL);
	}
	if (bordR != NULL)
	{
		FLOAT SourceHeight = 0.0f;
		if (bStretchVertically == TRUE)
			SourceHeight = FLOAT(bordR->VSize);
		DrawIconPattern(XRight, YCenter, Right, CenterHeight, 0.0f, 0.0f, Right, SourceHeight, bordR);
	}
	if (bordT != NULL)
	{
		FLOAT SourceWidth = 0.0f;
		if (bStretchHorizontally == TRUE)
			SourceWidth = FLOAT(bordT->USize);
		DrawIconPattern(XCenter, YTop, CenterWidth, Top, 0.0f, FLOAT(bordT->VSize) - Top, SourceWidth, Top, bordT);
	}
	if (bordB != NULL)
	{
		FLOAT SourceWidth = 0.0f;
		if (bStretchHorizontally == TRUE)
			SourceWidth = FLOAT(bordB->USize);
		DrawIconPattern(XCenter, YBottom, CenterWidth, Bottom, 0.0f, 0.0f, SourceWidth, Bottom, bordB);
	}
	if (center != NULL)
	{
		FLOAT SourceWidth = 0.0f;
		FLOAT SourceHeight = 0.0f;
		if (bStretchHorizontally == TRUE)
			SourceWidth = FLOAT(center->USize);
		if (bStretchVertically == TRUE)
			SourceHeight = FLOAT(center->VSize);
		DrawIconPattern(XCenter, YCenter, CenterWidth, CenterHeight, 0.0f, 0.0f, SourceWidth, SourceHeight, center);
	}

	unguard;
}

void XGC::DrawActor(AActor* actor, UBOOL bClearZ, UBOOL bConstrain,
	UBOOL bUnlit, FLOAT drawScale, FLOAT scaleGlow, UTexture* newSkin)
{
	guard(XGC::DrawActor);

	if (bDrawEnabled == FALSE || canvas == NULL || actor == NULL)
		return;

	FSceneNode* Frame = canvas->Frame;

	UBOOL bOldHidden = actor->bHidden;
	UBOOL bOldUnlit = actor->bUnlit;
	BYTE OldStyle = actor->Style;
	FLOAT OldDrawScale = actor->DrawScale;
	FLOAT OldScaleGlow = actor->ScaleGlow;
	UTexture* OldSkin = actor->Skin;
	UTexture* OldMultiSkins[8];

	for (INT i = 0; i < 8; i++)
		OldMultiSkins[i] = actor->MultiSkins[i];

	INT OldXB = Frame->XB;
	INT OldYB = Frame->YB;
	INT OldX = Frame->X;
	INT OldY = Frame->Y;

	if (bConstrain == TRUE)
	{
		FLOAT FrameLeft = FLOAT(Frame->XB);
		FLOAT FrameTop = FLOAT(Frame->YB);
		FLOAT FrameRight = FLOAT(Frame->XB + Frame->X);
		FLOAT FrameBottom = FLOAT(Frame->YB + Frame->Y);

		FLOAT ClipLeft = hMultiplier * (clipRect.originX + clipRect.clipX);
		FLOAT ClipTop = vMultiplier * (clipRect.originY + clipRect.clipY);
		FLOAT ClipRight = ClipLeft + hMultiplier * clipRect.clipWidth;
		FLOAT ClipBottom = ClipTop + vMultiplier * clipRect.clipHeight;

		if (FrameLeft < ClipLeft)
			FrameLeft = ClipLeft;
		if (FrameTop < ClipTop)
			FrameTop = ClipTop;
		if (FrameRight > ClipRight)
			FrameRight = ClipRight;
		if (FrameBottom > ClipBottom)
			FrameBottom = ClipBottom;

		Frame->XB = INT(FrameLeft - FLOAT(OldXB));
		Frame->YB = INT(FrameTop - FLOAT(OldYB));
		Frame->X = INT(FrameRight - FrameLeft);
		Frame->Y = INT(FrameBottom - FrameTop);
		Frame->ComputeRenderSize();
	}

	if (newSkin != NULL)
	{
		for (INT j = 0; j < 8; j++)
			actor->MultiSkins[j] = newSkin;
		actor->Skin = newSkin;
	}

	actor->bHidden = FALSE;
	actor->bUnlit = bUnlit;
	actor->Style = style;
	actor->ScaleGlow = scaleGlow;
	actor->DrawScale = actor->DrawScale * drawScale;

	if (bClearZ == TRUE)
		canvas->Viewport->RenDev->ClearZ(Frame);

	canvas->Render->DrawActor(Frame, actor);

	actor->bHidden = bOldHidden;
	actor->bUnlit = bOldUnlit;
	actor->Style = OldStyle;
	actor->ScaleGlow = OldScaleGlow;
	actor->DrawScale = OldDrawScale;

	if (newSkin != NULL)
	{
		for (INT k = 0; k < 8; k++)
			actor->MultiSkins[k] = OldMultiSkins[k];
		actor->Skin = OldSkin;
	}

	if (bConstrain == TRUE)
	{
		Frame->XB = OldXB;
		Frame->YB = OldYB;
		Frame->X = OldX;
		Frame->Y = OldY;
		Frame->ComputeRenderSize();
	}

	unguard;
}

void XGC::ClearZ(void)
{
	guard(XGC::ClearZ);

	if (canvas != NULL)
		canvas->Viewport->RenDev->ClearZ(canvas->Frame);

	unguard;
}

