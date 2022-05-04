// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTest/WeaponSystem/ItemChildren/Mods/ShotgunMod.h"

#include "NetworkChar.h"
#include "WeaponTest/WeaponSystem/ShotgunProjectile.h"

AShotgunMod::AShotgunMod()
{
	//set up defaults(can be overriden in blueprints)
	bReadyToFire = true;
	ProjectileDamage= 10;
	AmmoCount = 20;
	RateOfFire = 0.2f;
	RateOfFireReset =RateOfFire;
	ProjectileSpeed = 3000;
	ProjectileLifeTime = 0.5f;
	
}

void AShotgunMod::Tick(float DeltaTime)
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

void AShotgunMod::FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	if(bReadyToFire)
	{
		if (OwningPlayer)	// if the owning player is set correctly
		{
			// fire the vfx from the character
			OwningPlayer->SpawnWeaponVfx(this, CameraComponent, MuzzleLocation);
			OwningPlayer->SpawnWeaponSound(this, FireSound, 1);

			for (int i = 0; i < projectilesToSpawn; i++)
			{
				//calculate fire direction vector for current Projectile
				const float x = FMath::RandRange(MinXSpread/6,MaxXSpread/6);
				const float y = FMath::RandRange(MinYSpread/6,MaxYSpread/6);
				const float z = FMath::RandRange(MinZSpread/6,MaxZSpread/6);
				const FVector Spread = FVector( x, y,z );
			
				OwningPlayer->FireProjectile(this, CameraComponent, MuzzleLocation, Spread);
			}
			
			// make sure to decrement our ammo count if not our default mod
			if (!bIsDefaultMod)
			{
				OwningPlayer->DecreaseModAmmo(this);
			}
			bReadyToFire = false;
			SimulateFire();
			MuzzleVfxNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleVFXNiagaraSystem,
				MuzzleLocation,
				NAME_None,
				FVector::ZeroVector,
				GetFireDirection(CameraComponent, MuzzleLocation).Rotation(),
				EAttachLocation::KeepRelativeOffset,
				true);
		}
	}
}

FString AShotgunMod::GetModTypeName()
{
	return FString("Shotgun");
}

void AShotgunMod::SimulateFire()
{

	// play the camera shake
	PlayerCameraShake(ModFireShake, 1.0f);
}
