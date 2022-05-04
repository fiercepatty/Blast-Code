// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTest/WeaponSystem/RocketProjectile.h"
//#include "Classes/Kismet/KismetSystemLibrary.h"
#include "AIEnemyParent.h"
#include "BlastTestCharacter.h"
#include "DrawDebugHelpers.h"
#include "NetworkChar.h"
//#include "Kismet/KismetSystemLibrary.h"


ARocketProjectile::ARocketProjectile()
{
	if (!SphereCollider)
	{
		SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
		SphereCollider->SetupAttachment(RootComponent); 
	}
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.Clear();
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARocketProjectile::OnOverlap);
	}
	//bounce implementation?
	//ProjectileMovementComponent->bShouldBounce=true;
	//ProjectileMovementComponent->Bounciness=1;
}

void ARocketProjectile::OnConstruction(const FTransform& MovieSceneBlends)
{
	Super::OnConstruction(MovieSceneBlends);
}

void ARocketProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())	// if we arent the server then dont continue
		return;

	if (!OwningPlayer)		// make sure we have our owning player set
	{
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString("Projectile Owning Player not set!"));
		return;
	}
	if (OtherActor->Tags.Contains("Player") && OtherActor->GetName() == OwningPlayer->GetName())
	{
		return;	// we are hitting ourselves when the projectile spawns
	}
	if (!OtherActor->Tags.Contains("Player") && !OtherActor->Tags.Contains("Environment") && !OtherActor->Tags.Contains("Enemy") )
	{
		return; // were overlapping with something we dont care about
	}
	// make sure to also cull out the hit actors we dont care about
	if (OtherActor->Tags.Contains("Projectile") || OtherActor->GetName() == OwnerName)
	{
		return;
	}


	
	if(ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(),ExplosionSound,GetActorLocation());
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString("Explosion"));
	}
	
	//SphereCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	//get list of all actors affected by the aoe 
	TArray<AActor*> OverlappedActors;
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes ;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType((ECollisionChannel::ECC_Pawn)));
	

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(),SphereCollider->GetComponentLocation(),ExplosionRadius,ObjectTypes,nullptr,IgnoredActors,OverlappedActors);
	
	if(bDrawExplosion)
	{
		//draw sphere for debugging
		DrawDebugSphere(GetWorld(),SphereCollider->GetComponentLocation()/*-FVector(0,0,100)*/,350,12,FColor::Red,false,20.0f,SDPG_World,2.0);
	}
	//go through that list and damage all enemies
	for (AActor* OverlappedActor : OverlappedActors)
	{
		
		if (OverlappedActor->Tags.Contains("Enemy"))
		{

			// notify the player that we hit an enemy
			if (GetInstigator())
				Cast<ANetworkChar>(GetInstigator())->OnNotifyProjectileHitEnemy();
			
			AAIEnemyParent* Enemy = Cast<AAIEnemyParent>(OverlappedActor);
			if (Enemy)
			{
				Enemy->DealDamageToEnemy(DamageAmount);
				if(Enemy->GetCurrentHealth()<=0){
					Enemy->GetMesh()->SetSimulatePhysics(true);
					Enemy->GetMesh()->AddRadialImpulse(SphereCollider->GetComponentLocation()-FVector(0,0,100),ExplosionRadius,ExplosionStrength,ERadialImpulseFalloff::RIF_Constant,true);
				}
				OwningPlayer->PlayHitMarkerSound(DamageAmount);
			}
		}
		if (OverlappedActor->Tags.Contains("Player"))
		{
			// notify the player that we hit an enemy
			if (GetInstigator())
				Cast<ANetworkChar>(OwningPlayer)->OnNotifyProjectileHitEnemy();
			// This is an enemy player
			ANetworkChar* Player = Cast<ANetworkChar>(OverlappedActor);
			Player->DealDamageToPlayer(DamageAmount);
			OwningPlayer->PlayHitMarkerSound(DamageAmount);
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, FString("Hit Player"));
		}
	}
	// destroy self after dealing with first collision
	CollisionComponent->Deactivate();
	SphereCollider->Deactivate();
	Destroy();
}

void ARocketProjectile::SetExplosionRadius(float NewExplosionRadius)
{
	ExplosionRadius = NewExplosionRadius;
}

float ARocketProjectile::GetExplosionRadius() const
{
	return ExplosionRadius;
}

void ARocketProjectile::SetExplosionStrength(float NewExplosionStrength)
{
	ExplosionStrength = NewExplosionStrength;
}

float ARocketProjectile::GetExplosionStrength() const
{
	return ExplosionStrength;
}

void ARocketProjectile::BeginPlay()
{
	Super::BeginPlay();
	//SphereCollider->SetSphereRadius(ExplosionRadius);
}


