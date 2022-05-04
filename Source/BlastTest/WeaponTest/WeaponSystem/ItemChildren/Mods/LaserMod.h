// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphColor/Private/appconst.h"
#include "WeaponTest/WeaponSystem/ItemChildren/Mods/ModParent.h"
#include "LaserMod.generated.h"

/**
 * 
 */
UCLASS()
class ALaserMod : public AModParent
{
	GENERATED_BODY()

public:
	ALaserMod();

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** How far the laser can extend */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings")
	float LaserRange;

	/** how often the laser can inflict damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings")
	float LaserDamageCooldown;

	/** the stored reset value for the damage cooldown */
	float LaserDamageCooldownReset = LaserDamageCooldown;

	/** This works with the damage cooldown so we know if we are allowed to deal damage */
	//UPROPERTY(Replicated)
	//bool CanDealDamage = true;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
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
	
	bool bLaserFiring=false;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UNiagaraComponent* LaserBeamEffect = nullptr;
	/*** The saved reference of the CameraComponent and MuzzleLocation of the owning player.
	*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Player Components")
	UCameraComponent* PlayerCameraComponent;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Player Components")
	UStaticMeshComponent* PlayerMuzzleComponent;
	/** Returns the name of the move for the user interface
	* @return - the name of the mod that will appear in the player's UI
	*/
	virtual FString GetModTypeName() override;
	
	/** Function that gets called by the inventory after a mod has been picked up */
	virtual void OnPickup(UStaticMeshComponent* MuzzleLocation) override;

protected:
	
	
	virtual void BeginPlay() override;

	
	
	/** When the components initialize, use this to deactivate the flamethrower */
	virtual void PostInitializeComponents() override;
	
	/** Function called when the FireState gets replicated */
	virtual void OnRep_FireState() override;
	
	/** Logic for what do after the fire state has been updated */
	virtual void UpdateFiringState() override;
	

};
