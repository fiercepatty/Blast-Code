// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTest/WeaponSystem/ItemChildren/Mods/RailGunMod.h"

#include "DrawDebugHelpers.h"
#include "NetworkChar.h"

ARailGunMod::ARailGunMod()
{
	//sniper stats?
	bReadyToFire = false;

	if(!ChargeEffect)	// create the sub object for the vfx and set the system in the editor
	{
		ChargeEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Charge Effect"));
	}
}

void ARailGunMod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsCharging)
	{
		// TODO: this does not need to happen continuously. This can happen just once really to attaching the
		// vfx to the location
		if (SavedMuzzleLocation && OwningPlayer)
		{
			OwningPlayer->UpdateActiveVfxLocation(this, SavedMuzzleLocation);
		}
	}
	if(!bReadyToFire && bIsCharging)
	{
		
		//GEngine->AddOnScreenDebugMessage(-1,1.5f,FColor::Emerald,FString::SanitizeFloat(RateOfFire));
		if(ChargeScale<1.0f)
		{
			ChargeScale += DeltaTime * ChargeScreenShakeRampUpScale;
		}
		if(RateOfFire>0.0f)
		{
			RateOfFire -= DeltaTime;
			
		}
		else
		{
			if(OwningPlayer)
			{
				OwningPlayer->SpawnWeaponSound(this, ChargedSound, 2);
			}
			bReadyToFire = true;
			RateOfFire = RateOfFireReset;
		}
		
		if (Cast<ANetworkChar>(GetInstigator()))	// only run this when we have a valid instigator (aka the player is holding it)
			PlayerCameraShake(ModChargeShake, ChargeScale);
	}
	if(AmmoCount<=0.0f)
	{
		if(OwningPlayer)
		{
			OwningPlayer->SpawnWeaponSound(this, OutOfAmmoSound, 2);
		}
		bReadyToDestroy=true;
	}
}

void ARailGunMod::FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	SimulateFire();
	bIsCharging=true;

	this->SavedMuzzleLocation = MuzzleLocation;

	if (OwningPlayer)	// this updates our fire state on the local machine and on the server
	{
		OwningPlayer->UpdateFireState(this, true);
		FireState = true;
		UpdateFiringState();
		OwningPlayer->SpawnWeaponSound(this, FireSound, 1);
	}
}

void ARailGunMod::SimulateFire()
{
	Super::SimulateFire();
}

void ARailGunMod::ActiveModRelease(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	// reset variables
	bIsCharging=false;
	RateOfFire = RateOfFireReset;
	
	if (OwningPlayer)
	{
		OwningPlayer->UpdateFireState(this, false);
		FireState = false;
		UpdateFiringState();
	}
	
	if(bReadyToFire)
	{
		bReadyToFire = false;
		if (OwningPlayer)		// if the owning player is set correctly
		{
			// fire the projectile then spawn the vfx from the character and decrease our ammo
			OwningPlayer->FireProjectile(this, CameraComponent, MuzzleLocation);
			OwningPlayer->SpawnWeaponVfx(this, CameraComponent, MuzzleLocation);
			OwningPlayer->DecreaseModAmmo(this);
			OwningPlayer->SpawnWeaponSound(this, ReleaseSound, 1);
		}
		// play our fire sound
		SimulateFireRelease();
		PlayerCameraShake(ModFireShake, 1.0);
	}
	else
	{
		if(OwningPlayer)
		{
			OwningPlayer->SpawnWeaponSound(this, nullptr, 1);
		}
	}
}

void ARailGunMod::SimulateFireRelease()
{
	Super::SimulateFireRelease();
	
}



FString ARailGunMod::GetModTypeName()
{
	return FString("RailGun");
}

void ARailGunMod::BeginPlay()
{
	Super::BeginPlay();

	RateOfFireReset = RateOfFire;
}

void ARailGunMod::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ChargeEffect->SetVisibility(false);
}

void ARailGunMod::OnRep_FireState()
{
	UpdateFiringState();
}

void ARailGunMod::UpdateFiringState()
{
	if (FireState)
	{
		ChargeEffect->SetVisibility(true);
		ChargeEffect->ReinitializeSystem();
	}
	else
	{
		ChargeEffect->SetVisibility(false);
	}
}
