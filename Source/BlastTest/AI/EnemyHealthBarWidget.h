// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/ProgressBar.h"
#include "EnemyHealthBarWidget.generated.h"

/*
 * 
 */
UCLASS()
class BLASTTEST_API UEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetBarValuePercent(float Value);

	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* HealthValue = nullptr;

	
};
