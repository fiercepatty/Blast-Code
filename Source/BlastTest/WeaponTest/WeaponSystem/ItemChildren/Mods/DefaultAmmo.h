// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTest/WeaponSystem/ItemChildren/Mods/ModParent.h"
#include "DefaultAmmo.generated.h"

/**
 * 
 */
UCLASS()
class ADefaultAmmo : public AModParent
{
	GENERATED_BODY()
	public:
	ADefaultAmmo();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/**
	 * Plays VFX in the direction the cameras facing
	 * @param CameraComponent direction and location the camera is facing
	 * @param MuzzleLocation location of where the VFX is played from.
	 */
	virtual void FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation ) override;

	/* This is responsible for running all the local events when the player fires. Like sound and screen shake */
	virtual void SimulateFire() override;
	
	/** Returns the name of the move for the user interface
	* @return - the name of the mod that will appear in the player's UI
	*/
	virtual FString GetModTypeName() override;
	


	

	
};
