// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTest/WeaponSystem/ItemChildren/Mods/ModParent.h"
#include "FlameThrowerMod.generated.h"

/**
 * 
 */
UCLASS()
class AFlameThrowerMod : public AModParent
{
	GENERATED_BODY()

	/*** Constructor
	 */
	AFlameThrowerMod();

	/**End Play
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Plays VFX in the direction the cameras facing
	 * @param CameraComponent direction and location the camera is facing
	 * @param MuzzleLocation location of where the VFX is played from.
	 */
	virtual void FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation) override;
	
	/* This is responsible for running all the local events when the player fires. Like sound and screen shake */
	virtual void SimulateFire() override;
	
	/**
	* Plays VFX in the direction the cameras facing
	* @param CameraComponent direction and location the camera is facing
	* @param MuzzleLocation location of where the VFX is played from.
	*/
	virtual void ActiveModRelease(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation) override;
	
	/* This is responsible for running all the local events when the player releases the fire button. Like sound and screen shake.
	* only used by the Mods that use the ActiveModRelease function */
	virtual void SimulateFireRelease() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
	/** Returns the name of the move for the user interface
	* @return - the name of the mod that will appear in the player's UI
	*/
	virtual FString GetModTypeName() override;

public:
	/** ----------------- GETTERS ---------------- */
	/**
	 *A getter for the bool of whether or not the fire button is being held down.
	 * @return - The bool, bHoldingFireDown.
	 */
	inline bool IsHoldingFireDown() const {return bHoldingFireDown;}
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UNiagaraComponent* FlameThrowerEffect = nullptr;

	/*** The saved reference of the CameraComponent and MuzzleLocation of the owning player.
	*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Player Components")
	UCameraComponent* SavedCameraComponent;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Player Components")
	UStaticMeshComponent* SavedMuzzleLocation;

	/** Function that gets called by the inventory after a mod has been picked up */
	virtual void OnPickup(UStaticMeshComponent* MuzzleLocation) override;
	
protected:

	bool bIsVfxClamped = false;
	
	/** When the components initialize, use this to deactivate the flamethrower */
	virtual void PostInitializeComponents() override;

	/** Function called when the FireState gets replicated */
	virtual void OnRep_FireState() override;

	/** Logic for what do after the fire state has been updated */
	virtual void UpdateFiringState() override;
	
	/***To keep the mod from spawning projectiles when button not held down.
	 *	Will set true on button down
	 *	Will set false on button up.
	 */
	bool bHoldingFireDown = false;

	/*** A timer of sorts to give intervals to the flame projectile spawn.
	 * NOTE: The base class rate of fire value isn't working for some reason.
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Firing Properties")
	float RateOfFlames;

	/***The reset value of our Flame's rate of fire.
	 */
	float RateOfFlamesReset;
};
