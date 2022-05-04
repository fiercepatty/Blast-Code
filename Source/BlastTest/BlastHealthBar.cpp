// Fill out your copyright notice in the Description page of Project Settings.

#include "BlastHealthBar.h"

#include <string>

#include "NetworkChar.h"


void UBlastHealthBar::NativeConstruct()
{
	Super::NativeConstruct();

	//ActiveMod->SetText(FText::FromString(ActiveModName));
}

void UBlastHealthBar::Update(int playerID)
{
	if(playerID >= 0)
	{
		if(IsValid(UGameplayStatics::GetPlayerCharacter(GetWorld(), playerID)))
		{
			Player = Cast<ANetworkChar>(UGameplayStatics::GetPlayerCharacter(GetWorld(), playerID));
			
			if(Player->HasAuthority())
			{
				if (Player)
				{
					CHealth = Player->PlayerInventory->GetCurrentPlayerHealth();
					MHealth = Player->PlayerInventory->GetMaxPlayerHealth();
					ModAmmo = Player->PlayerInventory->GetActiveModAmmoAmount();
					MaxModAmmo = Player->PlayerInventory->GetActiveModMaxAmmoCount();
					UpdateHealth(CHealth, MHealth);
					UpdateProgressBarAmmo(ModAmmo, MaxModAmmo);
				}
			}
			else
			{
				if (Player)
				{
					CHealth = Player->PlayerInventory->GetCurrentPlayerHealth();
					MHealth = Player->PlayerInventory->GetMaxPlayerHealth();
					ModAmmo = Player->PlayerInventory->GetActiveModAmmoAmount();
					MaxModAmmo = Player->PlayerInventory->GetActiveModMaxAmmoCount();
					UpdateClientHealth(CHealth, MHealth);
					UpdateProgressBarAmmoClient(ModAmmo, MaxModAmmo);
				}
			}
			ActiveMod->SetText(FText::FromString(this->GetCurrentMod()));
			//ActiveModAmmo->SetText(FText::AsNumber(this->GetAmmoAmount()));
			SecondaryMod->SetText(FText::FromString(this->GetSecondaryMod()));
			//SecondaryModAmmo->SetText(FText::AsNumber(this->GetSecondaryModAmmo()));
			//MaxAmmo->SetText(FText::AsNumber(this->GetMaxAmmoAmount()));
			Points->SetText(FText::AsNumber(this->GetCurrentPoints()));
		}
	}
}

void UBlastHealthBar::UpdateHealth(float currentHealth, float maxHealth)
{
	CHealth = currentHealth;
	MHealth = maxHealth;
	PercentHealth = currentHealth / maxHealth;
	HealthBar->SetPercent(PercentHealth);
}

void UBlastHealthBar::UpdateClientHealth(float currentHealth, float maxHealth)
{
	CHealth = currentHealth;
	MHealth = maxHealth;
	PercentHealth = currentHealth / maxHealth;
	HealthBar->SetPercent(PercentHealth);
}

void UBlastHealthBar::UpdateProgressBarAmmo(float currentAmmo, float maxAmmo)
{
	PercentModAmmo = currentAmmo / maxAmmo;
	ModAmmoBar->SetPercent(PercentModAmmo);
}

void UBlastHealthBar::UpdateProgressBarAmmoClient(float currentAmmo, float maxAmmo)
{
	PercentModAmmo = currentAmmo / maxAmmo;
	ModAmmoBar->SetPercent(PercentModAmmo);
}


FString UBlastHealthBar::GetCurrentMod()
{
	ActiveModName = Player->PlayerInventory->GetActiveModName();
	return ActiveModName;
}

FString UBlastHealthBar::GetSecondaryMod()
{
	SecondaryModName = Player->PlayerInventory->GetSecondaryModName();
	return SecondaryModName;
}

float UBlastHealthBar::GetAmmoAmount()
{
	ModAmmo = Player->PlayerInventory->GetActiveModAmmoAmount();
	return ModAmmo;
}

float UBlastHealthBar::GetMaxAmmoAmount()
{
	MaxModAmmo = Player->PlayerInventory->GetActiveModMaxAmmoCount();
	return MaxModAmmo;
}

float UBlastHealthBar::GetSecondaryModAmmo()
{
	SecondaryModAmmoCount = Player->PlayerInventory->GetSecondaryModAmmoAmount();
	return SecondaryModAmmoCount;
}

int UBlastHealthBar::GetCurrentPoints()
{
	CurrentPoints = Player->PlayerInventory->GetCurrentNumOfGeneratorPoints();
	return CurrentPoints;
}
