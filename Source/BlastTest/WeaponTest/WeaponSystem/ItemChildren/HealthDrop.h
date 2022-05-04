// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "WeaponTest/WeaponSystem/ItemParent.h"
#include "HealthDrop.generated.h"

/**
 * 
 */
UCLASS()
class BLASTTEST_API AHealthDrop : public AItemParent
{
	GENERATED_BODY()

public:	
	AHealthDrop();

	virtual void BeginPlay() override;
	
	/** The visual aspect of our mod */
	UPROPERTY(EditAnywhere, Category = "Item Settings", meta= (AllowProtectedAccess= "true"))
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UNiagaraComponent* DropEffect = nullptr;

	/** getter for if this drop is a full heal */
	inline bool GetIsFullHeal() const { return isFullHeal; }

	/** getter for how much to heal the player */
	inline float GetAmountToHeal() const { return HealthAmount; }

protected:	
	/** boolean to determine if the health drop fully heals the player */
	UPROPERTY(EditAnywhere, Category = "Item Settings", meta= (AllowProtectedAccess= "true"))
	bool isFullHeal = true;

	/** if isFullHeal is false this is how much the player will get healed by */
	UPROPERTY(EditAnywhere, Category = "Item Settings", meta= (AllowProtectedAccess= "true"))
	float HealthAmount = 50.f;
	
};


