// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTest/WeaponSystem/ItemChildren/Mods/RocketLauncherMod.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponTest/WeaponSystem/RocketProjectile.h"
#include "NetworkChar.h"
#include "Kismet/KismetSystemLibrary.h"

ARocketLauncherMod::ARocketLauncherMod()
{
	//set up defaults(can be overriden in blueprints)
	bReadyToFire = true;
	ProjectileDamage= 50;
	AmmoCount = 10;
	RateOfFire = 0.6f;
	RateOfFireReset =RateOfFire;
	ProjectileSpeed = 4000;
	ProjectileLifeTime = 1.5f;
}

void ARocketLauncherMod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!bReadyToFire)
	{
		if(RateOfFire>0.0f)
		{
			RateOfFire-=DeltaTime;
		}
		else
		{
			bReadyToFire=true;
			RateOfFire=RateOfFireReset;
		}
	}
	if(AmmoCount<=0)
	{
		if(OwningPlayer)
		{
			OwningPlayer->SpawnWeaponSound(this, OutOfAmmoSound, 2);
		}
		bReadyToDestroy = true;
	}
	
}

void ARocketLauncherMod::FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	if(bReadyToFire)
	{
		if (OwningPlayer)	// if the owning player is set correctly
		{
			// fire the projectile then spawn the vfx from the character
			OwningPlayer->FireProjectile(this, CameraComponent, MuzzleLocation);
			OwningPlayer->SpawnWeaponVfx(this, CameraComponent, MuzzleLocation);
			if(FireSound)
			{
				OwningPlayer->SpawnWeaponSound(this, FireSound, 1);
			}
			if (!bIsDefaultMod)
			{
				OwningPlayer->DecreaseModAmmo(this);	//decrease our ammo if this isn't our default mod
			}
		}
		bReadyToFire = false;	// no longer ready to fire
		SimulateFire();
		//PlaySound(FireSound);
		MuzzleVfxNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleVFXNiagaraSystem,
			MuzzleLocation,
			NAME_None,
			FVector::ZeroVector,
			GetFireDirection(CameraComponent, MuzzleLocation).Rotation(),
			EAttachLocation::KeepRelativeOffset,
			true);
	}
}

void ARocketLauncherMod::SimulateFire()
{
	Super::SimulateFire();
	
	// play our screen shake
	PlayerCameraShake(ModFireShake, 1.0f);
}

FString ARocketLauncherMod::GetModTypeName()
{
	return FString("Rocket Launcher");
}



