// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlastTestHUD.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


void ABlastTestHUD::ShowInGameHUD()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	FPDisplay = CreateWidget<UUserWidget>(PC, FPDisplayClass);

	FPDisplay->AddToViewport();

	// Show the HUD
	/*APlayerController* PC2 = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABlastTestHUD* HUD = PC2->GetHUD<ABlastTestHUD>();
	HUD->DrawHUD();*/
}
