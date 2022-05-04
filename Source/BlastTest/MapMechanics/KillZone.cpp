// Fill out your copyright notice in the Description page of Project Settings.


#include "MapMechanics/KillZone.h"

#define COLLISION_PROJECTILE	ECC_GameTraceChannel1


// Sets default values
AKillZone::AKillZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setup details
	ZoneBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("KillZoneBounds"));

	//Most of the stuff here is the same implementation as the ZoneTool
	ZoneBounds->OnComponentBeginOverlap.AddDynamic(this,&AKillZone::OnOverLap);

	ZoneBounds->SetCollisionObjectType(ECC_Pawn);

	ZoneBounds->CanCharacterStepUpOn = ECB_No;
	
	//Wont collide with Projectiles
	ZoneBounds->SetCollisionResponseToChannel(COLLISION_PROJECTILE,ECR_Ignore);

}

// Called when the game starts or when spawned
void AKillZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKillZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKillZone::OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->Tags.Contains("Player"))
	{
		ANetworkChar* Player = Cast<ANetworkChar>(OtherActor);
		if(Player)
		{
			Player->DealDamageToPlayer(600);
		}
	}
	else if (OtherActor->Tags.Contains("Enemy"))
	{
		AAIEnemyParent* Enemy = Cast<AAIEnemyParent>(OtherActor);
		if (Enemy)
		{
			Enemy->DealDamageToEnemy(Enemy->GetCurrentHealth()*6);
			
		}	
	}
}

