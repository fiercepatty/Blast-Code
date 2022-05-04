// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "NetworkChar.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "BlastHealthBar.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BLASTTEST_API UBlastHealthBar : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInventoryComponent* InventoryComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PercentHealth = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ActiveModName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SecondaryModName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ModAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxModAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SecondaryModAmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SecondaryMaxModAmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PercentModAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RailgunChargeValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentPoints;

	/** Updates the UI */
	UFUNCTION(BlueprintCallable)
	void Update(int playerID);

	/** Updates the Flamethrower */
	void UpdateProgressBarAmmo(float currentAmmo, float maxAmmo);

	/** Updates the Flamethrower */
	void UpdateProgressBarAmmoClient(float currentAmmo, float maxAmmo);

	/** Update the Health on the server*/
	void UpdateHealth(float currentHealth, float maxHealth);

	/** Update the client health */
	void UpdateClientHealth(float currentHealth, float maxHealth);

protected:
	ANetworkChar* Player;
	
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
		class UImage* Crosshair;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ActiveMod;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* SecondaryMod;
	/*UPROPERTY(meta = (BindWidget))
		class UTextBlock* ActiveModAmmo;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* SecondaryModAmmo;*/
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* ModAmmoBar;
	//UPROPERTY(meta = (BindWidget))
	//	class UProgressBar* RailgunCharge;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Points;
	
	/** Gives the current active mod
	* @return The current active mod
	*/ 
	FString GetCurrentMod();

	/** Gets the ammo amount from player
	 * @return An FString of the Ammo amount
	 */
	float GetAmmoAmount();

	/** 
	 * @return Gets the Max ammo for the mod
	 */
	float GetMaxAmmoAmount();

	/**
	 * @return Gets the secondary mod name
	 */
	FString GetSecondaryMod();

	/**
	 * @return The secondary mod ammo
	 */
	float GetSecondaryModAmmo();

	//float GetSecondaryMaxModAmmo();

	/**
	 * @return The player's current number of generator parts
	 */
	int GetCurrentPoints();
};
