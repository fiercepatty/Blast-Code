// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTest/WeaponSystem/ItemChildren/Mods/DefaultAmmo.h"

#include "LobbyPC.h"
#include "NetworkChar.h"

ADefaultAmmo::ADefaultAmmo()
{
	//set up defaults(can be overriden in blueprints)
	bReadyToFire = true;
	ProjectileDamage= 15;
	bHasInfiniteAmmo =true;
	RateOfFire = 0.1f;
	RateOfFireReset =RateOfFire;
	ProjectileSpeed = 3000;
	ProjectileLifeTime = 1.0f;
	ProjectileImpulse = 100;
}

void ADefaultAmmo::Tick(float DeltaTime)
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
}

void ADefaultAmmo::FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	if(bReadyToFire)	// if ready to fire
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

void ADefaultAmmo::SimulateFire()
{
	// play our fire sound
	/*
	if(Cast<ANetworkChar>(GetInstigator())->AudioComponent)
	{
		if(FireSound)
		{
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Purple,"Firing");
			Cast<ANetworkChar>(GetInstigator())->AudioComponent->SetWorldLocation(GetInstigator()->GetActorLocation());
			Cast<ANetworkChar>(GetInstigator())->AudioComponent->SetSound(FireSound);
			Cast<ANetworkChar>(GetInstigator())->AudioComponent->FadeIn(0.1f);
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Purple,"No FireSound");
		}
	}
	*/
	// play our screen shake
	PlayerCameraShake(ModFireShake, 1.0f);
}

FString ADefaultAmmo::GetModTypeName()
{
	return FString("Default");
}



