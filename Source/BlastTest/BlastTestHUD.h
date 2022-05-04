// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "CoreMinimal.h"
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlastTestHUD.generated.h"

UCLASS(Abstract)
class ABlastTestHUD : public AHUD
{
	GENERATED_BODY()

	/** Primary draw call for the HUD */
	//UFUNCTION(BlueprintCallable)
	virtual void ShowInGameHUD();

protected:
	// Following benui.ca
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> FPDisplayClass;

	UPROPERTY()
		class UUserWidget* FPDisplay;
};

