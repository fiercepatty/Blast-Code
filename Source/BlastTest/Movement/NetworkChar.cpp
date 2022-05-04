// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkChar.h"

#include <string>

#include "EnemyProjectile.h"
#include "FlameThrowerMod.h"
#include "FlamethrowerProjectile.h"
#include "InGameUI.h"
#include "InventoryComponent.h"
#include "RailGunMod.h"
#include "LaserMod.h"
#include "RocketLauncherMod.h"
#include "RocketProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ANetworkChar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetworkChar, PlayerInventory);
	DOREPLIFETIME(ANetworkChar, FoundGenerator);
}

void ANetworkChar::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ACharacter* CurPlayer = NewController->GetCharacter();
	if (PlayerInventory)
	{
		PlayerInventory->OwningController = NewController;
	}
}

void ANetworkChar::ForceRespawnPlayer()
{
	RespawnPlayer();
}

// Sets default values
ANetworkChar::ANetworkChar(const class FObjectInitializer& ObjectInitializer) 
{
	PrimaryActorTick.bCanEverTick = true;


	// get reference to our inventory component
	if(!PlayerInventory)
	{
		PlayerInventory= CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
		PlayerInventory->SetIsReplicated(true);
	}

	if(!AudioComponent)
	{
		AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

	}
	if(!SecondaryAudioComponent)
	{
		SecondaryAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SecondaryAudioComponent"));
	}
	if(!InventoryAudioComponent)
	{
		InventoryAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("InventoryAudioComponent"));
	}

	bReplicates = true;
	bAlwaysRelevant = true;


	//Set up our hit marker
	if(!HitMarkerAudioComponent)
	{
		HitMarkerAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("HitMarkerAudioComponent"));
	}

	//hit marker Defaults
	HitMarkerTickLimiter = 0.0f;
	MaxHitMarkerTickLimiter = 0.0f;
}

// Called when the game starts or when spawned
void ANetworkChar::BeginPlay()
{
	Super::BeginPlay();	

	TArray<AActor*> NewZoneCon;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AZoneToolCon::StaticClass(), NewZoneCon);
	//Adding this player to the ZoneToolCons Array of Players so AI spawned from it can target player
	if(NewZoneCon.Num() != 0)
		ZoneCon = Cast<AZoneToolCon>(NewZoneCon[0]);

	if(ZoneCon != nullptr)
		ZoneCon->AddPlayer(this);

	//Hitmarker logic set up
	MaxHitMarkerTickLimiter = HitMarkerTickLimiter;

}

void ANetworkChar::Destroyed()
{
	Super::Destroyed();
}

bool ANetworkChar::DealDamageToPlayer(float DamageToDeal)
{
	PlayerInventory->PlayerTakeDamage(DamageToDeal);
	return PlayerInventory->GetCurrentPlayerHealth()<=0.f;
}

void ANetworkChar::OnNotifyProjectileHitEnemy()
{
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString("Hit"));
}

void ANetworkChar::FireProjectile(AModParent* Mod, UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation, FVector Offset)
{
	ServerFireProjectile(Mod, CameraComponent, MuzzleLocation, Offset);	// spawn our projectile onto the server
}

void ANetworkChar::ServerFireProjectile_Implementation(AModParent* Mod, UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation, FVector Offset)
{
	//Note for Flamethrowers: No additional casting is needed at this time.
	// calculate some needed variabls
	const FVector OffsetVector = Mod->GetFireDirection(CameraComponent, MuzzleLocation)*-1 * Mod->ProjectileMuzzleOffset;
	const FTransform CollisionTransform =FTransform(FRotator(0,0,0),OffsetVector,FVector(0,0,0)) + MuzzleLocation->GetComponentTransform();
	const FVector Direction = Mod->GetFireDirection(CameraComponent, MuzzleLocation);

	// try and spawn the projectile (MUST BE DEFERRED!)
	AProjectileParent* ProjectileParent = GetWorld()->SpawnActorDeferred<AProjectileParent>(Mod->ProjectileClass,CollisionTransform, Mod, GetInstigator());
	if(ProjectileParent)
	{
		// set prespawn variables
		ProjectileParent->SetProjectileLifespan(Mod->ProjectileLifeTime);
		ProjectileParent->SetDamageAmount(Mod->ProjectileDamage);
		ProjectileParent->SetImpulsePower(Mod->ProjectileImpulse);
		ProjectileParent->OwningPlayer = Mod->OwningPlayer;	// let the projectile know what the owning inventory is
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::SanitizeFloat());

		if(Mod->GetModTypeName()=="Rocket Launcher")
		{
			ARocketLauncherMod* RocketMod = Cast<ARocketLauncherMod>(Mod);
			if(RocketMod)
			{
				ARocketProjectile* RocketProjectile = Cast<ARocketProjectile>(ProjectileParent);
				if(RocketProjectile)
				{
					RocketProjectile->SetExplosionRadius(RocketMod->ExplosionRadius);
					RocketProjectile->SetExplosionStrength(RocketMod->ExplosionStrength);
				}
			}
		}
		
		// Finish spawning actor now and fire in desired direction
		UGameplayStatics::FinishSpawningActor(ProjectileParent, CollisionTransform);
		ProjectileParent->FireInDirection(Direction + Offset);
	}
}

void ANetworkChar::SpawnWeaponVfx(AModParent* Mod, UCameraComponent* CameraComponent,
	UStaticMeshComponent* MuzzleLocation)
{
	const FVector Location = MuzzleLocation->GetComponentLocation();	// calculate the location
	const FVector Direction = Mod->GetFireDirection(CameraComponent, MuzzleLocation);	// calculate the direction
	ServerFireVfx(Mod, Location, Direction);	// run the server function for the vfx
}

void ANetworkChar::ServerFireVfx_Implementation(AModParent* Mod, FVector Location, FVector Direction)
{
	MultiFireVfx(Mod, Location, Direction);	// run the multicast from the server function to correctly spawn vfx
}

void ANetworkChar::MultiFireVfx_Implementation(AModParent* Mod, FVector Location, FVector Direction)
{
	if (Mod && Mod->ProjectileVFXNiagaraSystem)
	{
		Mod->ProjectileVfxNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),Mod->ProjectileVFXNiagaraSystem,Location,Direction.Rotation());
		Mod->ProjectileVfxNiagaraComponent->SetFloatParameter("User.Projectile_Lifetime",Mod->ProjectileLifeTime);
		Mod->ProjectileVfxNiagaraComponent->SetVectorParameter("User.Velocity",FVector(Mod->ProjectileSpeed, 0.f, 0.f));
	}
}

void ANetworkChar::SpawnWeaponSound(AModParent* Mod, USoundBase* Sound, int AudioComponentNum)
{
	ServerFireSound(Mod, Sound, AudioComponentNum);	// run the server function for the vfx
}

void ANetworkChar::ServerFireSound_Implementation(AModParent* Mod, USoundBase* Sound, int AudioComponentNum)
{
	MultiFireSound(Mod, Sound, AudioComponentNum);	// run the multicast from the server function to correctly spawn sound
}

void ANetworkChar::MultiFireSound_Implementation(AModParent* Mod, USoundBase* Sound, int AudioComponentNum)
{
	UAudioComponent*  ChosenAudioComp = nullptr; 
	switch(AudioComponentNum)
	{
		case 1: ChosenAudioComp = AudioComponent;
		break;
		case 2: ChosenAudioComp = SecondaryAudioComponent;
		break;
		case 3: ChosenAudioComp = InventoryAudioComponent;
		break;
		default: break;
	}
	if(ChosenAudioComp)
	{
		
		if(ChosenAudioComp->IsPlaying())
		{
			ChosenAudioComp->FadeOut(0.1,0);
		}
		
		if(Sound)
		{
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Purple,"Firing");
			ChosenAudioComp->SetWorldLocation(GetInstigator()->GetActorLocation());
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Purple,GetInstigator()->GetActorLocation().ToString());
			ChosenAudioComp->SetSound(Sound);
			ChosenAudioComp->FadeIn(0.1f);
			
			//UGameplayStatics::SpawnSoundAttached(Sound, GetInstigator()->GetRootComponent());
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Purple,"No FireSound");
		}
	}
}

void ANetworkChar::DecreaseModAmmo(AModParent* Mod, float DecreaseAmount)
{
	if(Mod)
		ServerDecreaseAmmo(Mod, DecreaseAmount);	// run server version
}

void ANetworkChar::UpdateFireState(AModParent* Mod, bool NewFireState)
{
	ServerUpdateFireState(Mod, NewFireState);
}

void ANetworkChar::ServerUpdateFireState_Implementation(AModParent* Mod, bool NewFireState)
{
	if (Mod)
	{
		Mod->FireState = NewFireState;
		//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Purple,NewFireState ? "True" : "False");
		Mod->UpdateFiringState();
	}
}

void ANetworkChar::UpdateCanDealDamage(AModParent* Mod, bool NewState)
{
	ServerUpdateCanDealDamage(Mod, NewState);
}

void ANetworkChar::ServerUpdateCanDealDamage_Implementation(AModParent* Mod, bool NewState)
{
	Mod->CanDealDamage = NewState;	// set variable on server
	Mod->OnRep_CanDealDamage();		// in case there is any logic for after can deal damage is changed
}

void ANetworkChar::CastRayFromMod(AModParent* Mod, const FVector Direction, UStaticMeshComponent* Location)
{
	ServerCastRayFromMod(Mod, Direction, Location);
}

void ANetworkChar::ServerCastRayFromMod_Implementation(AModParent* Mod, const FVector Direction, UStaticMeshComponent* Location)
{
	ALaserMod* LaserRef = Cast<ALaserMod>(Mod);
	if (LaserRef)	// if the laser called this method
	{
		// trace params required for line trace, ignore actor prevents from hitting itself
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);	// owning player of mod is ignored
		// initialize hit info
		FHitResult HitResult;
		// do trace to muzzle to fire direction * laser range
		bool HadHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Location->GetComponentLocation(),
			Location->GetComponentLocation() + Direction*LaserRef->LaserRange,
			ECC_Visibility,
			TraceParams);
		
		if(HadHit)
		{
			MultiSetLaserEndPoint(LaserRef, HitResult.ImpactPoint);	// set the end point in a multicast
			if (!LaserRef->CanDealDamage) // dont continue we cant deal damage
				return;
			
			if (HitResult.Actor->ActorHasTag("Enemy") && LaserRef->CanDealDamage)	// enemy hit 
			{
				// notify the player that we hit an enemy
				if (GetInstigator())
					Cast<ANetworkChar>(GetInstigator())->OnNotifyProjectileHitEnemy();
					
				AAIEnemyParent* Enemy = Cast<AAIEnemyParent>(HitResult.Actor);
				if (Enemy)
				{
					Enemy->DealDamageToEnemy(LaserRef->ProjectileDamage);
					PlayHitMarkerSound(LaserRef->ProjectileDamage);
					UpdateCanDealDamage(Mod, false);
				}
			}
			if (HitResult.Actor->Tags.Contains("Player"))	// player hit
			{
				// This is an enemy player
				ANetworkChar* Player = Cast<ANetworkChar>(HitResult.Actor);
				Player->DealDamageToPlayer(LaserRef->ProjectileDamage);
				//GEngine->AddOnScreenDebugMessage(1, 0.05f, FColor::Orange, FString("Hit Player"));
				UpdateCanDealDamage(Mod, false);
			}
		}
		else
		{
			//DrawDebugCircle(GetWorld(), PlayerMuzzleComponent->GetComponentLocation()+GetFireDirection(PlayerCameraComponent, PlayerMuzzleComponent)*LaserRange, 200, 50, FColor::Blue, true, -1, 0, 10);
			MultiSetLaserEndPoint(LaserRef, HitResult.TraceEnd);	// set the end point in a multicast
		}
	}
}

void ANetworkChar::MultiSetLaserEndPoint_Implementation(ALaserMod* Laser, const FVector EndPoint)
{
	if (Laser && Laser->LaserBeamEffect) 
		Laser->LaserBeamEffect->SetVectorParameter("User.End", EndPoint);
}

void ANetworkChar::UpdateActiveVfxLocation(AModParent* Mod, UStaticMeshComponent* Location)
{
	ServerUpdateActiveVfxLocation(Mod, Location);
}

void ANetworkChar::ServerUpdateActiveVfxLocation_Implementation(AModParent* Mod, UStaticMeshComponent* Location)
{
	MultiUpdateActiveVfxLocation(Mod, Location);
}

void ANetworkChar::MultiUpdateActiveVfxLocation_Implementation(AModParent* Mod, UStaticMeshComponent* Location)
{
	if (Location && Cast<AFlameThrowerMod>(Mod) && Cast<AFlameThrowerMod>(Mod)->FlameThrowerEffect)
	{
		const FAttachmentTransformRules Rules = FAttachmentTransformRules( EAttachmentRule::SnapToTarget, true );
		Cast<AFlameThrowerMod>(Mod)->FlameThrowerEffect->AttachToComponent(Location, Rules);
	}
	else if (Location && Cast<ARailGunMod>(Mod) && Cast<ARailGunMod>(Mod)->ChargeEffect)
	{
		const FAttachmentTransformRules Rules = FAttachmentTransformRules( EAttachmentRule::SnapToTarget, true );
		Cast<ARailGunMod>(Mod)->ChargeEffect->AttachToComponent(Location, Rules);
		//Cast<ARailGunMod>(Mod)->ChargeEffect->AddWorldRotation(FRotator(0, 0, 90));
	}
	else if (Location && Cast<ALaserMod>(Mod) && Cast<ALaserMod>(Mod)->LaserBeamEffect)
	{
		const FAttachmentTransformRules Rules = FAttachmentTransformRules( EAttachmentRule::KeepRelative, true );
		Cast<ALaserMod>(Mod)->LaserBeamEffect->AttachToComponent(Location, Rules);
	}
}

void ANetworkChar::ServerDecreaseAmmo_Implementation(AModParent* Mod, float DecreaseAmount)
{
	Mod->AmmoCount -= DecreaseAmount;	// decrease ammo on server so it replicates to client
}

// Called every frame
void ANetworkChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1,.01,FColor::Red,TEXT("I'm ticking"));
	const float CurHealth = PlayerInventory->GetCurrentPlayerHealth();
	//if (HasAuthority())
	//	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Blue,  GetName() + FString(": Health = ") + FString::SanitizeFloat(CurHealth));
	if(PlayerInventory->GetCurrentPlayerHealth()<=0)
	{
		//GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::Red,TEXT("I'm a dead bitch"));
		// our health has dropped below zero so wer respawn now
		RespawnPlayer();
	}

	//Hitmarker timer logic
	HitMarkerTickLimiter -= DeltaTime;

	//Clamp to zero
	if(HitMarkerTickLimiter < 0.0)
	{
		HitMarkerTickLimiter = 0.0f;
	}

}

void ANetworkChar::RespawnPlayer()
{
	// find all player start actors in the world
	TArray<AActor*> PlayerStartArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerStart, PlayerStartArray);

	if (PlayerStartArray.Num() == 0)	// make sure we actually have something in our player start array
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("No player starts found in level"));
		return;
	}
	// select a spawn point at random
	// TODO: i imagine that once we have a larger game flow in place this will need more than just a random index
	const AActor* ChosenSpawn = PlayerStartArray[FMath::FRandRange(0, PlayerStartArray.Num()-1)];

	// reset our health
	PlayerInventory->SetCurrentPlayerHealth(PlayerInventory->GetMaxPlayerHealth());

	// Reset the player weapon back to spawning state
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString("Call respawn"));
	PlayerInventory->ResetMods();
	
	// set our actor location to the spawn point
	SetActorLocation(ChosenSpawn->GetActorLocation());

	if(AudioComponent->IsPlaying())
	{
		AudioComponent->FadeOut(0.1f,0.0f);
	}
	if(SecondaryAudioComponent->IsPlaying())
	{
		SecondaryAudioComponent->FadeOut(0.1f,0.0f);
	}
	if(InventoryAudioComponent->IsPlaying())
	{
		InventoryAudioComponent->FadeOut(0.1f,0.0f);
	}
}

void ANetworkChar::RemoveFromZone()
{
	ZoneCon->RemovePlayer(this);
}

void ANetworkChar::PlayHitMarkerSound(float DamageAmount)
{
	//HitMarkerAudioComponent->SetPitchMultiplier(DamageAmount);

	if(HitMarkerTickLimiter <= 0.0f)
	{
		if(HitMarkerAudioComponent->IsActive())
		{
			HitMarkerAudioComponent->Deactivate();
			HitMarkerAudioComponent->Activate();
			HitMarkerTickLimiter = MaxHitMarkerTickLimiter;
			return;
		}
		HitMarkerTickLimiter = MaxHitMarkerTickLimiter;
		HitMarkerAudioComponent->Activate();
	}
	
}
