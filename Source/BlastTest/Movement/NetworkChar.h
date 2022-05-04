// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "InventoryComponent.h"
#include "LaserMod.h"
#include "GameFramework/Character.h"
#include "ZoneTool/ZoneToolCon.h"
#include "Components/AudioComponent.h"
#include "NetworkChar.generated.h"


UCLASS(Blueprintable)
class ANetworkChar : public ACharacter
{
	GENERATED_BODY()
	
public:

	/** Property replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;
	
	/** reference to the player inventory component */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	class UInventoryComponent* PlayerInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAudioComponent* AudioComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UAudioComponent* SecondaryAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAudioComponent* InventoryAudioComponent;

	//The sound for the hit markers, aka, damage sound
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Hitmarker Sound Settings")
	UAudioComponent* HitMarkerAudioComponent;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool FoundGenerator;

	inline bool GetFoundGenerator() { return FoundGenerator; }

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Hitmarker Sound Settings")
	float HitMarkerTickLimiter;

	float MaxHitMarkerTickLimiter;
	

	/**Force the player to respawn by clicking button in editor*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Respawn")
	void ForceRespawnPlayer();
	
	/**
	 *Sets default values for this character's properties
	 * @param ObjectInitializer 
	 */
	ANetworkChar(const class FObjectInitializer& ObjectInitializer);
	
	/**
	 * Called every frame
	 * @param DeltaTime The change in time
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * method to call when the player dies and is responsible for Reset the player location and stats
	 */
	UFUNCTION(BlueprintCallable)
	void RespawnPlayer();

	/** decreases the current health by damage to deal amount
	 * @return True if the damage dealt killed the player, false if not
	 */
	UFUNCTION(BlueprintCallable)
	bool DealDamageToPlayer(float DamageToDeal);

	/** method called by projectiles when they overlap with anything holding the enemy tag */
	void OnNotifyProjectileHitEnemy();

	/** Helper function used to call from the mod to call the server fire so the server RPC happens within the NetworkChar
	* @param Mod - reference to the mod that called this method
	* @param CameraComponent - Players camera component
	* @param MuzzleLocation - Players muzzle location
	* @param Offset - used to offset the fire direction of the projectile, defaults to zero vectors for not offsetting at all
	*/
	void FireProjectile(AModParent* Mod, UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation, FVector Offset=FVector::ZeroVector);

	/** Helper function used to call from the mod to call the multicast vfx
	*  @param Mod - reference to the mod that called this method
	*  @param CameraComponent - Players camera component
	*  @param MuzzleLocation - Players muzzle location
	 */
	void SpawnWeaponVfx(AModParent* Mod, UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation);

	/** Helper function used to call from the mod to call the multicast sound
	*  @param Mod - reference to the mod that called this method
	*/
	void SpawnWeaponSound(AModParent* Mod, USoundBase* Sound, int AudioComponentNum);
	
	/** Helper function used to call from the mod to decrease its ammo on the server
	 * @param Mod - reference to the mod that called this method
	 * @param DecreaseAmount - how much to decrease the ammo by, default to 1. Available so we can pass in a delta time
	 */
	void DecreaseModAmmo(AModParent* Mod, float DecreaseAmount=1.0f);

	/** Used for mods that are held down over time to determine the fire state for them which is used for the vfx
	 * @param Mod - reference to the mod in question
	 * @param NewFireState - what to set the new fire state to
	 */
	void UpdateFireState(AModParent* Mod, bool NewFireState);

	/** Used for updating the location of the mod by attaching it to the location
	 * @param Mod - the mod in question
	 * @param Location - the muzzle the vfx will get attached to
	 */
	void UpdateActiveVfxLocation(AModParent* Mod, UStaticMeshComponent* Location);

	/** Used for casting a ray from the mod to deal damage, pretty much just unique to the laser mod
	 * @param Mod - the mod calling this function which is pretty much just the laser
	 * @param Direction - which direction the ray will be shot in
	 * @param Location - Where the ray will get shot from
	 */
	void CastRayFromMod(AModParent* Mod, const FVector Direction, UStaticMeshComponent* Location);

	/** Used for updating the CanDealDamage in the mods on the server since its replicated
	 * @param Mod - the mod calling this function
	 * @param NewState - the state to set the variable to
	 */
	void UpdateCanDealDamage(AModParent* Mod, bool NewState);

	/** Used to play hitmarker sound.
	 * @param DamageAmount - The amount of damage that the hit did.
	 */
	void PlayHitMarkerSound(float DamageAmount);

protected:
	
	/** [Server + Client] Method ran from ServerCastRayFromMod to update the end point on the server and client
	* @param Laser - Reference to the laser mod ultimately calling this function
	* @param EndPoint - what the set the end point to
	* @param 
	*/
	UFUNCTION(NetMulticast, Unreliable)
	void MultiSetLaserEndPoint(ALaserMod* Laser, const FVector EndPoint);

	/** [Server] Responsible for changing a mods CanDealDamage on server when needed
	* @param Mod - reference to the mod that called this method
	* @param NewState - the new state for CanDealDamage
	*/
	UFUNCTION(Server, Reliable)
	void ServerUpdateCanDealDamage(AModParent* Mod, bool NewState);

	/** [Server] Responsible for casting the desired ray for the given mod to inflict possible damage
	* @param Mod - reference to the mod that called this method
	* @param Direction - the direction to shoot the ray in
	* @param Location - the muzzle location to shoot the ray from
	*/
	UFUNCTION(Server, Reliable)
	void ServerCastRayFromMod(AModParent* Mod, const FVector Direction, UStaticMeshComponent* Location);
	
	/** [Server] Responsible for spawning the mod projectile based on the information given to it
	* @param Mod - reference to the mod that called this method
	* @param Location - where we are setting the vfx to
	*/
	UFUNCTION(Server, Reliable)
	void ServerUpdateActiveVfxLocation(AModParent* Mod, UStaticMeshComponent* Location);
	
	/** [Server + Client] Method ran to set and spawn the vfx for the server and client
	* @param Mod - reference to the mod that called this method
	* @param Location - where we are setting the vfx to
	*/
	UFUNCTION(NetMulticast, Unreliable)
	void MultiUpdateActiveVfxLocation(AModParent* Mod, UStaticMeshComponent* Location);
	
	/** [Server] Responsible for spawning the mod projectile based on the information given to it
	* @param Mod - reference to the mod that called this method
	* @param NewFireState - what the new fire state will be
	*/
	UFUNCTION(Server, Reliable)
	void ServerUpdateFireState(AModParent* Mod, bool NewFireState);

	/** [Server] Responsible for spawning the mod projectile based on the information given to it
	* @param Mod - reference to the mod that called this method
	* @param CameraComponent - Players camera component
	* @param MuzzleLocation - Players muzzle location
	* @param Offset - used to offset the fire direction of the projectile, defaults to zero vectors for not offsetting at all
	*/
	UFUNCTION(Server, Reliable)
	void ServerFireProjectile(AModParent* Mod, UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation, FVector Offset);

	/** [Server] Responsible calling the multicast vfx. Needed because multicasts must be ran from server
	* @param Mod - reference to the mod that called this method
	* @param Location - where to spawn vfx
	* @param Direction - what direction to shoot the vfx
	*/
	UFUNCTION(Server, Unreliable)
	void ServerFireVfx(AModParent* Mod, FVector Location, FVector Direction);
	
	/** [Server + Client] Method ran to set and spawn the vfx for the server and client
	* @param Mod - reference to the mod that called this method
	* @param Location - where we are spawning the vfx at
	* @param Direction - what direction the vfx needs to be shot in
	*/
	UFUNCTION(NetMulticast, Unreliable)
	void MultiFireVfx(AModParent* Mod, FVector Location, FVector Direction);

	/** [Server] Responsible calling the multicast sound. Needed because multicasts must be ran from server
	* @param Mod - reference to the mod that called this method
	*/
	UFUNCTION(Server, Unreliable)
	void ServerFireSound(AModParent* Mod, USoundBase* Sound, int AudioComponentNum);
	
	/** [Server + Client] Method ran to set and spawn the sound for the server and client
	* @param Mod - reference to the mod that called this method
	*/
	UFUNCTION(NetMulticast, Unreliable)
	void MultiFireSound(AModParent* Mod, USoundBase* Sound, int AudioComponentNum);
	
	/** [Server] Decreases the mods ammo
	 * @param Mod - reference to the mod that called this method
	 * @param DecreaseAmount - how much to decrease the ammo of Mod by
	 */
	UFUNCTION(Server, Reliable)
	void ServerDecreaseAmmo(AModParent* Mod, float DecreaseAmount);
	
	/* Called when the game starts or when spawned **/
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
	
	/**Class that the projectiles are fired using*/
	UPROPERTY(EditAnywhere, Category = "Network Char | Player Start", meta= (AllowPrivateAccess= "true"))
	TSubclassOf<class AActor> PlayerStart;

	UFUNCTION(BlueprintCallable)
	void RemoveFromZone();
	
private:
	
	/**Reference to the ZoneToolCon*/
	AZoneToolCon* ZoneCon;
	
};






