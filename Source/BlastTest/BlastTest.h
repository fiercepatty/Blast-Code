// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// The direction the crosshair is placed in player's view
namespace ECrosshairDirection
{
	enum Type
	{
		Left = 0,
		Right = 1,
		Top = 2,
		Bottom = 3,
		Center = 4
	};
}

// What the game is currently in
namespace EShooterMatchState
{
	enum Type
	{
		Playing,
		Won,
		Lost
	};
}

// The positions for the in-game HUD split into 8 parts of the screen
namespace EShooterHUDPosition
{
	enum Type
	{
		Left=0,
		FrontLeft=1,
		Front=2,
		FrontRight=3,
		Right=4,
		BackRight=5,
		Back=6,
		BackLeft=7,
	};
}