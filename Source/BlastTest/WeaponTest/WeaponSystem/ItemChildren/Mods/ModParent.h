// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTest/WeaponSystem/ItemParent.h"
#include "EBlueprintEnums.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "WeaponTest/WeaponSystem/ProjectileParent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/AIEnemyParent.h"
#include "ModParent.generated.h"

/**
 * 
 */
UCLASS()
class AModParent : public AItemParent
{
	GENERATED_BODY()

public:
	AModParent();

	/** Property replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/**
	 * @param CameraComponent - Reference to the owning players camera component
	 * @param MuzzleLocation Location that the actual VFX will play from
	 * Needs to be override to allow for the implementation of different effects when a fire function is called for separate mods
	 */
	virtual void FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation);
	
	/* This is responsible for running all the local events when the player fires. Like sound and screen shake */
	virtual void SimulateFire();
	
	/**Inventory checks this boolean to see if the mod needs to be destroyed*/
	bool bReadyToDestroy = false;
	/**
	* @param CameraComponent - Reference to the owning players camera component
	* @param MuzzleLocation Location that the actual VFX will play from
	* Needs to be override to allow for the implementation of different effects when a release function is called for separate mods
	*/
	virtual void ActiveModRelease(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation);

	/* This is responsible for running all the local events when the player releases the fire button. Like sound and screen shake.
	* only used by the Mods that use the ActiveModRelease function */
	virtual void SimulateFireRelease();

	/** called when projectile overlaps something 
	* @param OverlappedComponent component that overlapped
	* @param OtherActor other actor that was overlapped
	* @param OtherComp other component that overlapped
	* @param OtherBodyIndex index the other object is
	* @param bFromSweep if theres a sweep
	* @param SweepResult hit result from the sweep
	*/
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Resets the mod ammo back to max ammo count */
	inline void ResetAmmo() { AmmoCount = MaxAmmoCount; }
	
	/** calculates the needed direction to fire vfx so that it goes towards the center of the screen
	*  @param CameraComponent - player camera component
	*  @param Muzzle - where the vfx will come out of
	*  @param bReturnHitLocation - if true the the location of the hit will be returned
	*  @return The direction from muzzle to where the vfx should go as a unit vector
	*/
	FVector GetFireDirection(UCameraComponent* CameraComponent, UStaticMeshComponent* Muzzle, bool bReturnHitLocation=false) const;
	
	/** String that is what the mod type is
	 * @return String will always be Camel Case
	 */
	virtual FString GetModTypeName();

	/** retrieves the current ammo count of the mod
	 * @return the ammo count
	 */
	inline float GetCurrentAmmoCount() const { return AmmoCount; }

	/**
	 *Get the max ammo value
	 * @return the max ammo count
	 */
	inline float GetMaxAmmoCount() const {return MaxAmmoCount;}

	/**Set the max ammo value
	 */
	inline void SetMaxAmmoCount() {this->MaxAmmoCount = this->AmmoCount; }

	/** Method called by the inventory after a mod is picked up for any logic right after pickup
	 * @param MuzzleLocation - The muzzle of the player that picked up the mod
	 */
	virtual void OnPickup(UStaticMeshComponent* MuzzleLocation);
	
	/** The visual aspect of our mod */
	UPROPERTY(EditAnywhere, Category = "Item Settings", meta= (AllowProtectedAccess= "true"))
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UNiagaraComponent* DropEffect = nullptr;

	/** boolean used to determine weather the mod is default. If so we dont take away from its ammo */
	UPROPERTY(Replicated)
	bool bIsDefaultMod = false;
	
	/* owning inventory reference*/
	UPROPERTY(Replicated)
	class ANetworkChar* OwningPlayer;
	
	virtual void BeginPlay() override;

	/** Camera Shake Used When Firing the Mod */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings | Camera Shake", meta = (AllowProtectedAccess = "true"))
	TSubclassOf<UMatineeCameraShake> ModFireShake;
	
	/**
	 * Boolean to Tell if the mod is ready to be fired
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	bool bReadyToFire = false;

	/**
	 * Amount of Damage the projectile fired from the weapon will do
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	int ProjectileDamage;
	/**
	* Impulse power the projectile fired from the weapon will do
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	int ProjectileImpulse;
	
	/**
	 * The amount of shots that a mod will have before it will disappear
	 * NOTE: FOR MODS THAT USE THIS AS A TIMER THE AMMO COUNT WILL REPRESENT ABOUT HALF OF THE TRUE TIMER
	 * EXAMPLE: AmmoCount = 5.0f --> roughly 10 seconds worth of ammo
	 */
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	float AmmoCount;

	/**
	 *The max ammo count
	 *Never Changes.
	 */
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	float MaxAmmoCount;
	
	//UFUNCTION()
	//void OnRep_MaxAmmo();
	/**
	* Boolean to tell if the mod doesnt use AmmoCount
	*/
	bool bHasInfiniteAmmo = false;
	
	/**
	 * Cooldown for firing bullets
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	float RateOfFire;
	
	/**
	* Used to reset cooldown for firing bullets
	*/
	float RateOfFireReset;
	
	/**
	 * The speed of the projectile
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	int ProjectileSpeed;

	/**
	 * The amount of time a projectile will last before they destroy themselves
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	float ProjectileLifeTime;
	
	/**
	* The amount that the collision box "lags" behind the VFX
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	float ProjectileMuzzleOffset;
	
	/**
	 * For later implementation for status effect if needed
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	EWeaponStatusEffects WeaponStatusEffects;
	/**
	* [Replicated] Niagara component for projectiles
	*/
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	UNiagaraComponent* ProjectileVfxNiagaraComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	UNiagaraComponent* MuzzleVfxNiagaraComponent;
	/**
	* Niagara System for projectiles
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	UNiagaraSystem* ProjectileVFXNiagaraSystem;
	
	/**
	* Niagara system for muzzle effects
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	UNiagaraSystem* MuzzleVFXNiagaraSystem;
	
	/**
	* Niagara system for collision
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	UNiagaraSystem* CollisionVFXNiagaraSystem;

	/**
	 * Projectile that the mod spawns in
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	TSubclassOf<AProjectileParent> ProjectileClass;

	/**
	* Fire Sound to play
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	USoundBase* FireSound;

	/**
	* Release Sound to play
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	USoundBase* ReleaseSound;

	/**
	* OutOfAmmo Sound to play
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	USoundBase* OutOfAmmoSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	float MinimumTraceThreshold = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings | Mod Settings", meta = (AllowProtectedAccess = "true"))
	float MaxTraceThreshold = 5000.f;

	/** Name of the character actor that owns this mod and is wanting to fire it right now */
	FString OwnerName = "None";

	/** Method that will call this given camera shake to play on the owning player
	*@param Shake - The matinee shake that needs to be set in blueprints
	*@param Scale - 1.0 is default, >1 will scale the shake up and <1 will scale down. Can be used to give a kind of charge up effect.
	*/
	void PlayerCameraShake(TSubclassOf<UMatineeCameraShake> Shake, float Scale) const;

	/** This variable is used to determine if a mod that gets held down needs its vfx on or off */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_FireState)
	bool FireState = true;

	UPROPERTY(ReplicatedUsing=OnRep_CanDealDamage)
	bool CanDealDamage = true;

	UFUNCTION()
	void OnRep_CanDealDamage();
	
	/** Function called after Fire State gets replicated, simply calls the UpdateFireState() */
	UFUNCTION()
	virtual void OnRep_FireState();

	/** Responsible for handling the logic for after the fire state is updated, mainly turning vfx on and off */
	virtual void UpdateFiringState();
	
};
