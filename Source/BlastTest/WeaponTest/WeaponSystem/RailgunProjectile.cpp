// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTest/WeaponSystem/RailgunProjectile.h"

#include "AIEnemyParent.h"
#include "BlastTestCharacter.h"
#include "NetworkChar.h"

ARailgunProjectile::ARailgunProjectile()
{
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentBeginOverlap.Clear();
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARailgunProjectile::OnOverlap);
	}
}

void ARailgunProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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
	if (OtherActor->Tags.Contains("Player") && OtherActor->GetName() == GetInstigator()->GetName())
	{
		return;	// we are hitting ourselves when the projectile spawns
	}
	if (!OtherActor->Tags.Contains("Player") && !OtherActor->Tags.Contains("Environment") && !OtherActor->Tags.Contains("Enemy") )
	{
		return; // were overlapping with something we dont care about
	}
	//if overlap with environment
	if(OtherActor->Tags.Contains("Environment"))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString("Hit Non-Enemy"));
		Destroy();
	}

	//if overlap with enemy
	if(OtherActor->Tags.Contains("Enemy"))
	{

		// notify the player that we hit an enemy
		if (GetInstigator())
			Cast<ANetworkChar>(GetInstigator())->OnNotifyProjectileHitEnemy();
		
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString("Hit"));
		
		AAIEnemyParent* Enemy = Cast<AAIEnemyParent>(OtherActor);
		if (Enemy)
		{
			Enemy->DealDamageToEnemy(DamageAmount);
			if(Enemy->GetCurrentHealth()<=0)
			{
				Enemy->GetMesh()->SetSimulatePhysics(true);
				Enemy->GetMesh()->AddImpulse(FVector(ProjectileMovementComponent->Velocity*ImpulsePower));
			}
			OwningPlayer->PlayHitMarkerSound(DamageAmount);
		}		
	}
	if (OtherActor->Tags.Contains("Player"))
	{
		// This is an enemy player
		ANetworkChar* Player = Cast<ANetworkChar>(OtherActor);
		Player->DealDamageToPlayer(DamageAmount);
		OwningPlayer->PlayHitMarkerSound(DamageAmount);
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, FString("Hit Player"));
	}
	Destroy();
}

void ARailgunProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ARailgunProjectile::BeginPlay()
{
	Super::BeginPlay();
}
