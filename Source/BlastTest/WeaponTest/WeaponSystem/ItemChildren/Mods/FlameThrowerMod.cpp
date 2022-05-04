        
// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTest/WeaponSystem/ItemChildren/Mods/FlameThrowerMod.h"
#include "NetworkChar.h"
#include "FlamethrowerProjectile.h"

AFlameThrowerMod::AFlameThrowerMod()
{
	/*** Defaults if needed
	 */
	this->ProjectileSpeed = 1000;
	this->bReadyToFire = false;

	this->RateOfFlames = 0.1f;
	this->RateOfFlamesReset = this->RateOfFlames;

	if(!FlameThrowerEffect)	// create the sub object for the vfx and set the system in the editor
	{
		FlameThrowerEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Flame Thrower Effect"));
	}
}

void AFlameThrowerMod::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if(ProjectileVfxNiagaraComponent)
		ProjectileVfxNiagaraComponent->DestroyInstance();
}

void AFlameThrowerMod::FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	if (OwningPlayer)	// this updates our fire state on the local machine and on the server
	{
		OwningPlayer->UpdateFireState(this, true);
		FireState = true;
		UpdateFiringState();
		OwningPlayer->SpawnWeaponSound(this, FireSound, 1);
	}
	
	/*** Save the initial values that were passed into this function
	 */
	this->SavedCameraComponent = CameraComponent;
	this->SavedMuzzleLocation = MuzzleLocation;

	/*** Turn on the spawning of the projectils and tell the tick to begin counting.
	 */
	bReadyToFire = true;
	bHoldingFireDown = true;

	/***Sound effect on fire
	 */
	SimulateFire();
}

void AFlameThrowerMod::SimulateFire()
{
	Super::SimulateFire();
}

void AFlameThrowerMod::ActiveModRelease(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	bReadyToFire = false;

	if (OwningPlayer)
	{
		OwningPlayer->UpdateFireState(this, false);
		FireState = false;
		UpdateFiringState();
		OwningPlayer->SpawnWeaponSound(this, ReleaseSound, 1);
	}
	
	/*** Halt projectile spawning
	 */
	bHoldingFireDown = false;

	/*** Sound effect on release
	 */
	SimulateFireRelease();
	
}

void AFlameThrowerMod::SimulateFireRelease()
{
	Super::SimulateFireRelease();
}


void AFlameThrowerMod::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// TODO: this does not need to happen continuously. This can happen just once really to attaching the
	// vfx to the location
	if (!bIsVfxClamped && SavedMuzzleLocation && OwningPlayer)
	{
		OwningPlayer->UpdateActiveVfxLocation(this, SavedMuzzleLocation);
		bIsVfxClamped = true;
	}

	if(bHoldingFireDown)
		OwningPlayer->DecreaseModAmmo(this, DeltaSeconds);
	
	if(bReadyToFire && bHoldingFireDown)
	{
		this->PlayerCameraShake(ModFireShake, 1.0f);
		if(GetWorld())
		{
			if(this->SavedCameraComponent)
			{
				if(OwningPlayer)
				{
					OwningPlayer->FireProjectile(this,SavedCameraComponent,SavedMuzzleLocation);
					
				}
			}
		}
		
		bReadyToFire = false;
	}
	else if(!bReadyToFire && bHoldingFireDown)
	{
		if(RateOfFlames > 0.0)
		{
			RateOfFlames -= DeltaSeconds;
		}
		else
		{
			bReadyToFire = true;
		}
	}


	/*** When the ammo has run out
	 */
	if(this->AmmoCount <= 0)
	{
		if(OwningPlayer)
		{
			OwningPlayer->SpawnWeaponSound(this, nullptr, 1);
			OwningPlayer->SpawnWeaponSound(this, OutOfAmmoSound, 2);
		}
		this->bReadyToDestroy = true;
	}
}

FString AFlameThrowerMod::GetModTypeName()
{
	return FString("Flamethrower");
}

void AFlameThrowerMod::OnPickup(UStaticMeshComponent* MuzzleLocation)
{
	SavedMuzzleLocation = MuzzleLocation;
	if (OwningPlayer)	// On pickup we have to make sure to set the states to false for client and server
	{
		OwningPlayer->UpdateFireState(this, false);
		FireState = false;
		UpdateFiringState();
	}
}

void AFlameThrowerMod::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FlameThrowerEffect->Deactivate();

}

void AFlameThrowerMod::OnRep_FireState()
{
	UpdateFiringState();
}

void AFlameThrowerMod::UpdateFiringState()
{
	if (FireState)
	{
		FlameThrowerEffect->Activate();
	}
	else
	{
		FlameThrowerEffect->Deactivate();
	}
}
    