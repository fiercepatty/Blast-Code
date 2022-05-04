// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "NetworkChar.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "LobbyPC.h"
#include "Generator.h"
#include "Components/TextBlock.h"
#include "BlastInteratableMessage.generated.h"

/**
 * 
 */
UCLASS()
class BLASTTEST_API UBlastInteratableMessage : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void Update(int PlayerID);

protected:
	ANetworkChar* Player;

	AGenerator* Generator;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InteractMessage;
};
