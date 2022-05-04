// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTest/WeaponSystem/ItemChildren/Mods/LaserMod.h"

#include "BlastTestCharacter.h"
#include "DrawDebugHelpers.h"
#include "NetworkChar.h"
#include "Kismet/KismetSystemLibrary.h"

ALaserMod::ALaserMod()
{
	
	if(!LaserBeamEffect)	// create the sub object for the vfx and set the system in the editor
	{
		LaserBeamEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Laser Beam Effect"));
	}
}

/*void ALaserMod::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALaserMod, CanDealDamage);	// replicate can deal damage since its changed on the server
}*/

void ALaserMod::BeginPlay()
{
	Super::BeginPlay();
	LaserDamageCooldownReset = LaserDamageCooldown;
}

void ALaserMod::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if(ProjectileVfxNiagaraComponent)
		ProjectileVfxNiagaraComponent->DestroyInstance();
}

void ALaserMod::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	LaserBeamEffect->Deactivate();
}
void ALaserMod::OnRep_FireState()
{
	UpdateFiringState();
}
void ALaserMod::UpdateFiringState()
{
	
	if (FireState)
	{
		LaserBeamEffect->SetVisibility(true); // when turning it on set visibility to true
		LaserBeamEffect->Activate();	// then re activate it
	}
	else
	{
		LaserBeamEffect->SetVisibility(false); // when turning it off set visibility to false
		LaserBeamEffect->Deactivate();	// then deactivate it
	}
	
	
}

void ALaserMod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!CanDealDamage)		// if we've dealt damage and now have to wait for damage cooldown
	{
		LaserDamageCooldown -= DeltaTime;
		if (LaserDamageCooldown <= 0.f)
		{
			LaserDamageCooldown = LaserDamageCooldownReset;
			if (OwningPlayer)
			{
				if (HasAuthority())
					OwningPlayer->UpdateCanDealDamage(this, true);	// set our can deal damage to true on server
			}
		}
	}
	if(bLaserFiring)
	{
		if(OwningPlayer)	// decrease our ammo while were firing
		{
			OwningPlayer->DecreaseModAmmo(this, DeltaTime);
		}
		
		this->PlayerCameraShake(ModFireShake, 1.0f);	// apply our camera shake as well
		if(PlayerCameraComponent && PlayerMuzzleComponent)	// if we successfully retrieved the components from fire
		{
			// TODO: this does not need to happen continuously. This can happen just once really to attaching the
			// vfx to the location
			if (PlayerMuzzleComponent && OwningPlayer)	// this keeps our vfx updated to our location
			{
				OwningPlayer->UpdateActiveVfxLocation(this, PlayerMuzzleComponent);	

				// this is what actually will do the damage since it all has to be done on the server
				// this will also update our vfx laser end point
				OwningPlayer->CastRayFromMod(this,
					GetFireDirection(PlayerCameraComponent, PlayerMuzzleComponent),
					PlayerMuzzleComponent);
			}
		}
	}
	if (AmmoCount <= 0.0)
	{
		if(OwningPlayer)
		{
			OwningPlayer->SpawnWeaponSound(this, OutOfAmmoSound, 2);
		}
		bReadyToDestroy = true;
		bLaserFiring = false;
	
		if (OwningPlayer)
		{
			OwningPlayer->UpdateFireState(this, false);
			FireState = false;
			UpdateFiringState();
			OwningPlayer->SpawnWeaponSound(this, nullptr, 1);
		}
	}
}

void ALaserMod::FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	if (OwningPlayer)	// this updates our fire state on the local machine and on the server
		{
		OwningPlayer->UpdateFireState(this, true);
		FireState = true;
		UpdateFiringState();
		OwningPlayer->SpawnWeaponSound(this, FireSound, 1);
		}
	// GRAB OUT CAMERA AND MUZZLE SO WE CAN REFERENCE THEM IN THE TICK
	if(!PlayerCameraComponent)
		PlayerCameraComponent=CameraComponent;
	if(!PlayerMuzzleComponent)
		PlayerMuzzleComponent=MuzzleLocation;
	bLaserFiring = true;	// Tick can know that
	SimulateFire();
}


void ALaserMod::SimulateFire()
{
	Super::SimulateFire();
	
}


void ALaserMod::ActiveModRelease(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
	
	bLaserFiring=false;
	if (OwningPlayer)
	{
		OwningPlayer->UpdateFireState(this, false);
		FireState = false;
		UpdateFiringState();
		OwningPlayer->SpawnWeaponSound(this, ReleaseSound, 1);
	}
	SimulateFireRelease();
}


void ALaserMod::SimulateFireRelease()
{
	Super::SimulateFireRelease();
}

FString ALaserMod::GetModTypeName()
{
	return FString("Laser Beam");
}

void ALaserMod::OnPickup(UStaticMeshComponent* MuzzleLocation)
{
	Super::OnPickup(MuzzleLocation);
	PlayerMuzzleComponent = MuzzleLocation;
	if (OwningPlayer)	// On pickup we have to make sure to set the states to false for client and server
		{
		OwningPlayer->UpdateFireState(this, false);
		FireState = false;
		UpdateFiringState();
		}
}


