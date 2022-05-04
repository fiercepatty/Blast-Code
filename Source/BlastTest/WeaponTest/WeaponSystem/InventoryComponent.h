// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemParent.h"
#include "WeaponTest/WeaponSystem/ItemChildren/Mods/ModParent.h"
#include "WeaponTest/WeaponSystem/ItemChildren/GeneratorPoints.h"
#include "EBlueprintEnums.h"
#include "Net/UnrealNetwork.h"
#include "InventoryComponent.generated.h"

struct ModInfo
{
	/** General all purpose mod infos */
	FString Name;
	float Damage;
	float AmmoCount, CurrentAmmoCount;
	bool HasInfiniteAmmo;
	float RateOfFire;
	EWeaponStatusEffects StatusEffect;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual ~UInventoryComponent() override;
	
	/** Property replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** This is the method to call when there is an item to be picked up into the inventory
	 * @param ItemToPickup - This is the actual item were picking up to be handled by the method
	 */
	UFUNCTION(BlueprintCallable)
	void PickupItem(AItemParent* ItemToPickup);

	/** [Local] calls the active mods fire method
	 * @param CameraComponent - reference to the players camera component to get info out of for mods
	 * @param MuzzleLocation - the muzzle location of the players weapon
	 */
	UFUNCTION()
	void FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation);
	
	/** calls the active mods released method (important for things like the rail gun
	 * @param CameraComponent - reference to the players camera component to get info out of for mods
	 * @param MuzzleLocation - the muzzle location of the players weapon
	 */
	UFUNCTION(BlueprintCallable)
	void ActiveModReleased(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation);

	/** [Local] function called by the player when wanting to fire to get the ball rolling on shooting
	 * @param CameraComponent - players camera
	 * @param MuzzleLocation - players muzzle location
	 */
	UFUNCTION(BlueprintCallable)
	void StartModFire(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation);
	
	/** Called when the player wants to switch between mods
	 * @return - The new active mod slot (NOTE: this can still be the old active mod slot aka the player had nothing to swap to)
	*/
	UFUNCTION(BlueprintCallable)
	void SwapWeapons(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation);

	/** Will primarily be called when the player dies.
	 *  All this does is makes sure both mod slots are empty and sets the active mod slot to DEFAULT
	 */
	UFUNCTION(BlueprintCallable)
	void ResetMods();
	
	/** Called when a GeneratorPoint is picked up and we want to add to our generator points */
	inline void AddGeneratorPoint() { CurrentNumOfGeneratorPoints++; }
	
	virtual void BeginPlay() override;

	/** ----------------- GETTERS ---------------- */
	/** returns us the current number of Generator Points in the inventory *
	 * @return - The current number of generator points
	 */
	inline int GetCurrentNumOfGeneratorPoints() const { return CurrentNumOfGeneratorPoints; }

	/** For when we want to know what mod we currently have active
	* @return FString of name of mod NOTE: THE NAMES WILL BE CammalCase
	*/
	UFUNCTION(BlueprintCallable)
	FString GetActiveModName() const;

	FString GetSecondaryModName() const;
	
	/** Gets the current mod's ammo amount */
	inline float GetActiveModAmmoAmount() const { return ActiveModAmmoCount; }
	
	/** Get max ammo
	*/
	inline float GetActiveModMaxAmmoCount() const {return MaxActiveModAmmoCount;}

	/** Gets the current mod's ammo amount */
	inline float GetSecondaryModAmmoAmount() const { return SecondaryModAmmoCount; }
	
	/** Get max ammo
	*/
	inline float GetSecondaryModMaxAmmoCount() const {return MaxSecondaryModAmmoCount;}

	/* Returns the DefaultModClass of this inventory
	 * @return - The TSubclassOf<AModParent> reference
	 */
	inline TSubclassOf<AModParent> GetDefaultModClass() const { return DefaultModClass; }

	/** --------------- HEALTH -------------------- */
	
	/** [Local + Server] Calls our SetCurrentPlayer health using the damage amount
	* @param DamageAmount - How much to decrement by
	*/
	void PlayerTakeDamage(const float DamageAmount);

	/** What is the players current health
	 * @return - The players current health
	 */
	inline float GetCurrentPlayerHealth() const { return CurrentPlayerHealth; }

	/** Gives the max player health */
	inline float GetMaxPlayerHealth() const { return MaxPlayerHealth; }

	/** ---------------- SETTERS ----------------- */
	/** sets what the players max health can be
	 * @param NewMax - What the new max is allowed to be
	 */
	inline void SetMaxPlayerHealth(const float NewMax) { MaxPlayerHealth = NewMax; }

	/** [Server] Sets the current player health to the amount given on the server
	 * Current Health is replicated to all connected clients' copies of this character
	 * @param NewHealth - the amount the new health will be set to
	 */
	void SetCurrentPlayerHealth(float NewHealth);

	/** Handling the internal regeneration timer
	 *	Avoids adding too much logic into the tickcomponent
	 */
	void HandleRegeneration(float DeltaTime);

	/** Resets the regeneration timer
	 */
	inline void ResetHealthRegenerationTimer() {this->HealthRegenerationTimer = this->MaxHealthRegenerationTimer;}

	/** Resets the secondary regeneration timer
	 */
	inline void ResetHealthRegenerationSecondaryTimer() {this->HealthRegenerationScaleTimer = this->MaxHealthRegenerationScaleTimer;}

	/** Handle healh resets
	 */
	void HandleRegenerationTimerReset();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(ReplicatedUsing=OnRep_ControllerUpdated)
	AController* OwningController;

	UFUNCTION()
	void OnRep_ControllerUpdated();

	/**Set max ammo count
	 */
	void SetMaxAmmoCount(EModSlots ModSlot);
	
protected:
	/** ----------------- PROTECTED SETTERS ---------------- */
	/** sets the ActiveModSlot in the inventory to the given NewActiveSlot
	* @param NewActiveSlot - the new slot to set to
	*/
	inline void SetActiveModSlot(EModSlots NewActiveSlot) { ActiveModSlot = NewActiveSlot; }
	
	/** This function is called primarily when a mod is picked up or weapon fired. It simply looks at the active mod
	 * slot and checks to see if we need to update what the current mod slot is
	 */
	void UpdateActiveMod();

	UFUNCTION(Server, Reliable)
	void ServerSwitchMod();

	/** If there is an empty mod slot then store the new mod in that
	 * @param NewMod - The new mod that we want to try and add to our inventory
	 */
	void PickupMod(AModParent* NewMod);

	/** This method checks if NewMod is of the same type as any mods in our inventory
	 * @param NewMod - The mod being picked up
	 * @return True if mod type is held in inventory, False otherwise
	 */
	bool CheckIfModContained(AModParent* NewMod);

	/** This mod will look at the mod type name and find the mod in the inventory that matches it and increase the ammo
	 * @param ModTypeName - the name of the mod type we want to pick up
	 */
	void AddAmmoToHeldModOfName(const FString ModTypeName);
	
	/**
	* If true the mod will destroy when picked up if the player already holds a mod of that type
	* If False the mod type the player picks up that is in their inventory will get its ammo refilled
	*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "Inventory")
	bool bDestroyDuplicateModOnPickup = false;

	/** this is essentially our win condition, how many generator points the player has */
	//UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "Inventory")
	UPROPERTY(Replicated,VisibleAnywhere,BlueprintReadWrite,Category= "Inventory")
	int CurrentNumOfGeneratorPoints = 0;

	/**
	 * The class of Default mod so that we can spawn in a version from the blueprint inside the constructor
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "Inventory")
	TSubclassOf<AModParent> DefaultModClass;
	
	/** The mod slot that never changes and holds our default mod type to shoot when needed */
	UPROPERTY(ReplicatedUsing=OnRep_DefaultModUpdate,EditAnywhere,BlueprintReadWrite,Category= "Inventory")
	AModParent* DefaultMod;

	UFUNCTION()
	void OnRep_DefaultModUpdate();
	
	/** The mod that is in our slot one */
	UPROPERTY(ReplicatedUsing=OnRep_Mod1Update, EditAnywhere,BlueprintReadWrite,Category= "Inventory")
	AModParent* ModSlot1;

	UFUNCTION()
	void OnRep_Mod1Update();
	
	/** The mod that is in our slot two */
	UPROPERTY(ReplicatedUsing=OnRep_Mod2Update, EditAnywhere,BlueprintReadWrite,Category= "Inventory")
	AModParent* ModSlot2;

	UFUNCTION()
	void OnRep_Mod2Update();

	/** The enum that represents what we currently have active */
	UPROPERTY(Replicated,EditAnywhere,BlueprintReadWrite,Category= "Inventory")
	EModSlots ActiveModSlot = EModSlots::Ve_DEFAULT;

	/** The players current health
	 * when changed the OnRep_CurrentHealth function is called*/
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth,EditAnywhere,BlueprintReadWrite,Category= "Inventory | Player Settings")
	float CurrentPlayerHealth;

	/** Se the player's Regen logic max values
	 */
	void SetUpHealthRegen();

	/** The players rate of regeneration per interval
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= "Inventory | Player Settings | Health Regeneration")
	float HealthRegenerationRate = 3.0;

	/** The players secondary rate of regeneration per interval
	 * The regeneration rate will scale to this value after a specified amount of time.
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= "Inventory | Player Settings | Health Regeneration")
	float HealthRegenerationRateSecondary = 6.0;

	/** The players max Health regeneration timer
	 *	The amount that we reset to.
	 */
	float MaxHealthRegenerationTimer;

	/** The players timer before regeneration starts. Resets upon taking damage.
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= "Inventory | Player Settings | Health Regeneration")
	float HealthRegenerationTimer = 4.0;

	/** The players timer before the regeneration scale begins. Resets upon taking damage.
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= "Inventory | Player Settings | Health Regeneration")
	float HealthRegenerationScaleTimer = 7.0;

	/** The players max health regeneration scale timer
	 *	The amount that we reset to.
	 */
	float MaxHealthRegenerationScaleTimer;

	/** The player interval of regeneration
	 * Speed of regen.
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= "Inventory | Player Settings | Health Regeneration")
	float HealthIncreaseInterval = 0.5;

	/** The players max interval of regeneration
	 */
	float MaxHealthIncreaseInterval;

	/** Function that gets called when CurrenPlayerHealth is changed
	 * in this case the function calls OnHealthUpdate
	 */
	UFUNCTION()
	void OnRep_CurrentHealth();

	/** Response to health being updated. Called on the server immediately
	 *  after modification, and on clients in response to a RepNotify*/
	void OnHealthUpdate();

	/** The players max health */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "Inventory | Player Settings")
	float MaxPlayerHealth = 100.f;

	/** So that we can see in editor what our active mod's ammo is */
	UPROPERTY(Replicated,VisibleAnywhere,BlueprintReadWrite,Category= "Inventory")
	float ActiveModAmmoCount;

	/**Ammo max for hud calculations
	 */
	UPROPERTY(Replicated,VisibleAnywhere,BlueprintReadWrite,Category= "Inventory")
	float MaxActiveModAmmoCount;

	/** So that we can see in editor what our active mod's ammo is */
	UPROPERTY(Replicated,VisibleAnywhere,BlueprintReadWrite,Category= "Inventory")
	float SecondaryModAmmoCount;

	/**Ammo max for hud calculations
	*/
	float MaxSecondaryModAmmoCount;
	
	/** A pickup sound for when a valid mod gets added to the inventory
	 *Potential for various different sounds. Only one at the moment.
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= "Inventory | Sound Settings")
	USoundBase* SuccessfulWeaponPickupSound;

	/** A pickup sound for when we have a full inventory. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Inventory | Sound Settings")
	USoundBase* FailedWeaponPickupSound;

	/** A pickup sound for when we pickup a generator point. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Inventory | Sound Settings")
	USoundBase* GeneratorPointPickupSound;

	UStaticMeshComponent* StoredMuzzleLocation;

};
