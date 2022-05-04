// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemParent.generated.h"

UCLASS()
class AItemParent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemParent();

	/** Property replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// It may become necessary to add an enum to each item inherited in case the logic i have in
	// mind for casting each item to a gen or mod to determine which item it is doesnt work out in pickup item.
	//enum ItemTypes { GENERATOR, MOD};
	//virtual ItemTypes GetItemType() = 0;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings ")
	bool bNeverDestroy=false;
	
	/**
	* Life time of the item
	* How long it will last in second before it destroys itself
	*/
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item Settings ", meta = (AllowPrivateAccess = "true"))
	float LifeTime;
	
protected:
	virtual void BeginPlay() override;
	
	

	/** Contains the initial lifetime for the item */
	float MaxLifeTime;

	/** How long the drop turns on for when blinking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings ", meta = (AllowPrivateAccess = "true"))
	float DropEffectBlinkTimerOn = 0.5;

	/** Reset value for the on timer */
	float ResetDropEffectBlinkTimerOn;

	/** How long the Drop turns off for when blinking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings ", meta = (AllowPrivateAccess = "true"))
	float DropEffectBlinkTimerOff = 0.3;

	/** Reset value for the off timer */
	float ResetDropEffectBlinkTimerOff;

	bool bHasSpeedUpOnce;

	bool bHasSpeedUpTwice;

};
