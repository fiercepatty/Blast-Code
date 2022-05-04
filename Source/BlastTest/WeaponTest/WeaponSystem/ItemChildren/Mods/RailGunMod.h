// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTest/WeaponSystem/ItemChildren/Mods/ModParent.h"
#include "RailGunMod.generated.h"

/**
 * 
 */
UCLASS()
class ARailGunMod : public AModParent
{
	GENERATED_BODY()

public:
	ARailGunMod();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/**
	* @param CameraComponent - The camera component of the player holding the inventory
	* @param MuzzleLocation - Location that the actual VFX will play from
	* Needs to be override to allow for the implementation of different effects when a fire function is called for separate mods
	*/
	virtual void FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation) override;
	/* This is responsible for running all the local events when the player fires. Like sound and screen shake */
	virtual void SimulateFire() override;
	/**
	* @param CameraComponent - The camera component of the player holding the inventory
	* @param MuzzleLocation - Location that the actual VFX will play from
	* Needs to be override to allow for the implementation of different effects when a release function is called for separate mods
	*/
	virtual void ActiveModRelease(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation) override;

	/* This is responsible for running all the local events when the player releases the fire button. Like sound and screen shake.
	* only used by the Mods that use the ActiveModRelease function */
	virtual void SimulateFireRelease() override;
	
	/** Returns the name of the move for the user interface
	 * @return - the name of the mod that will appear in the player's UI
	 */
	virtual FString GetModTypeName() override;
	
	/** sets the reset to the player rate of fire */
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UNiagaraComponent* ChargeEffect = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Player Components")
	UStaticMeshComponent* SavedMuzzleLocation;
	
protected:

	/** When the components initialize, use this to deactivate the flamethrower */
	virtual void PostInitializeComponents() override;

	/** Function called when the FireState gets replicated */
	virtual void OnRep_FireState() override;

	/** Logic for what do after the fire state has been updated */
	virtual void UpdateFiringState() override;
	
	/**
	 *  Boolean used to make sure the mod only fires if fully charged 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	bool bIsCharging = false;

	/** attribute used to increase overtime up to one for the scale of the charge screen shake */
	float ChargeScale = 0.0f;

	/** Value used to make the time needed for charge up screen shake to reach max shake either faster or slower */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings | Camera Shake", meta = (AllowProtectedAccess = "true"))
	float ChargeScreenShakeRampUpScale = 2.0f;

	/** Camera Shake Used When Charging the Mod */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings | Camera Shake", meta = (AllowProtectedAccess = "true"))
	TSubclassOf<UMatineeCameraShake> ModChargeShake;

	/**
	* Charged Sound to play
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	USoundBase* ChargedSound;
};
