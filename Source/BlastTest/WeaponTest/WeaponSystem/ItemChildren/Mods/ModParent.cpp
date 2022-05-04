// Fill out your copyright notice in the Description page of Project Settings.


#include "ModParent.h"
#include "NetworkChar.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InventoryComponent.h"

AModParent::AModParent()
{
	if(!MeshComponent)
	{	
		MeshComponent= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
		MeshComponent->bHiddenInGame=true;
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RootComponent = MeshComponent;		// TODO: check if this fixes the no root component being set in the mods
	}
	
	if(!DropEffect)	// create the sub object for the vfx and set the system in the editor
	{
		DropEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Drop Effect"));
		DropEffect->Activate();
		
		DropEffect->SetupAttachment(MeshComponent);
	}
	
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AModParent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AModParent, OwningPlayer);	// replicate our owning player
	DOREPLIFETIME(AModParent, ProjectileVfxNiagaraComponent);	// replicate the niagara component
	DOREPLIFETIME(AModParent, AmmoCount);	// replicate the ammo count of the mod
	DOREPLIFETIME(AModParent, MaxAmmoCount); // replicate the max ammo of the mod
	DOREPLIFETIME(AModParent, bIsDefaultMod) // replicate if this mod is default mod
	DOREPLIFETIME(AModParent, FireState) // replicate if this mod is default mod
	DOREPLIFETIME(AModParent, CanDealDamage) // replicate if this mod is default mod
}

void AModParent::FireActiveMod(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
}

void AModParent::SimulateFire()
{
}

void AModParent::ActiveModRelease(UCameraComponent* CameraComponent, UStaticMeshComponent* MuzzleLocation)
{
}

void AModParent::SimulateFireRelease()
{
}

void AModParent::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

FVector AModParent::GetFireDirection(UCameraComponent* CameraComponent, UStaticMeshComponent* Muzzle, bool bReturnHitLocation) const
{
	// set some variables that will be needed
	FVector MuzzleLocation = Muzzle->GetComponentLocation();
	FVector CameraLookDirection = CameraComponent->GetForwardVector().GetSafeNormal();
	FVector CameraLocation = CameraComponent->GetComponentLocation();
	FVector ReturnDirection;
	FVector HitLocation;
	float Distance;

	//start of ray
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation + CameraLookDirection * MaxTraceThreshold;
	//ignore player that is holding the mod
	const AActor* ActorToIgnore = GetInstigator();
	//set up trace parameters
	FCollisionQueryParams TraceParams(FName(TEXT("RayTracing")), true, ActorToIgnore);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	FHitResult HitResult;
	
	//ray trace check for first collision 
	bool HadHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
				
	if (HadHit)
	{
		// get new location to move to since we would have been running into a wall
		FVector VectorToHitLocation = HitResult.Location - CameraLocation;
		float HitDistance = (VectorToHitLocation).Size();
		HitLocation = HitResult.Location;	// set our hit location in case we need that info
		if (HitDistance<=MinimumTraceThreshold)		// we are closer than our threshold allows
			{
			Distance = MinimumTraceThreshold;
			// return unit vector from muzzle location to location at min distance in front of camera
			ReturnDirection = ((CameraLocation + (CameraLookDirection * MinimumTraceThreshold)) - MuzzleLocation).GetSafeNormal();
			} else		// hit location is far enough away
				{
				Distance = HitDistance;
				// return unit vector from muzzle location to hit location
				ReturnDirection = (HitResult.Location - MuzzleLocation).GetSafeNormal();	// return unit vector from muzzle location to hit location
				}
	} else
	{
		HitLocation = FVector(0, 0, 50000);	// this way we will know if this vector gets returned there was no hit
		Distance = MaxTraceThreshold;
		// we have not hit so return default as such
		// return unit vector from muzzle location to location at min distance in front of camera
		ReturnDirection = (TraceEnd - MuzzleLocation).GetSafeNormal();
	}
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::SanitizeFloat(Distance));

	//DrawDebugLine(GetWorld(), CameraLocation, MuzzleLocation + ReturnDirection * Distance, FColor::Red, false, 1.5f, 0, 5);
	//DrawDebugLine(GetWorld(), MuzzleLocation, MuzzleLocation + ReturnDirection * Distance, FColor::Red, false, 1.5f, 0, 5);
	//DrawDebugCircle(GetWorld(), MuzzleLocation + ReturnDirection * Distance, 200, 50, FColor::Red, false, 30.f);
	if (bReturnHitLocation)
		return HitLocation;
	return ReturnDirection;
}

FString AModParent::GetModTypeName()
{
	return FString("Mod Does Not Contain An Override for GetModTypeName");
}

void AModParent::OnPickup(UStaticMeshComponent* MuzzleLocation)
{
}

void AModParent::BeginPlay()
{
	Super::BeginPlay();
	//Set max ammo count
	MaxAmmoCount = AmmoCount;
}

/*void AModParent::OnRep_MaxAmmo()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString("Max Ammo Replicated"));
}*/

void AModParent::PlayerCameraShake(const TSubclassOf<UMatineeCameraShake> Shake, const float Scale=1.0f) const
{
	if (!Shake)
		return;		// if an invalid camera shake is given dont proceed

	// for loop to go through the player controllers in the world
	for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator )
	{
		if(Iterator)
		{
			const ANetworkChar* Player = Cast<ANetworkChar>(Iterator->Get()->GetCharacter());	// get reference to player controlled by controller
			if (!Player || !OwningPlayer)
				return;
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,  FString("Player from iterator is ") + Player->GetName());
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,  FString("Owning Player Firing Mod is ") + OwningPlayer->GetName());
			if (Player->GetName() == OwningPlayer->GetName())	// if its the same as the owning character 
			{
				Iterator->Get()->PlayerCameraManager->StartMatineeCameraShake(Shake, Scale);	// play shake
				return;
			}
		}
	}
}

void AModParent::OnRep_CanDealDamage()
{
 	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Magenta, FString("Can Deal Damage Replicated to ") + FString::SanitizeFloat(CanDealDamage));
}

void AModParent::OnRep_FireState()
{
}

void AModParent::UpdateFiringState()
{
}


