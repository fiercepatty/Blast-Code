// Fill out your copyright notice in the Description page of Project Settings.


#include "ZoneTool.h"
#include "ZoneToolCon.h"
#include "GameFramework/Character.h"

#define COLLISION_PROJECTILE	ECC_GameTraceChannel1

// Sets default values
AZoneTool::AZoneTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	
	/**setup ZoneBounds details*/
	ZoneBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBounds"));
	ZoneBounds->OnComponentBeginOverlap.AddDynamic(this, &AZoneTool::OnOverLap);
	ZoneBounds->OnComponentEndOverlap.AddDynamic(this,&AZoneTool::EndOverLap);
	ZoneBounds->SetCollisionObjectType(ECC_Pawn);
	ZoneBounds->CanCharacterStepUpOn = ECB_No;
	//Wont collide with Projectiles
	ZoneBounds->SetCollisionResponseToChannel(COLLISION_PROJECTILE,ECR_Ignore);

	//setting ZoneBounds as RootComponent
	RootComponent = ZoneBounds;

	//**Setting defaults*/
	EnemyPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	DespawnTime = 10.0f;
	SpawnTimer = 5.0f;
	SpawnerRadius = 5.0f;
	
	bIsPlayerInZone = false;
	CurrEnemyCount = 0;
	numNeighborZones = 2;
	AICanChangeZone = false;

	SuicderSpawnProb = 1.0f;
	FlyingSpawnProb = 1.0f;
	EngineerSpawnProb = 1.0f;
	GruntSpawnProb = 1.0f;

}

// Called when the game starts or when spawned
void AZoneTool::BeginPlay()
{
	Super::BeginPlay();
	FillEnemyToSpawn();
	CreateSpawners();
	
}

// Called every frame
void AZoneTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurrEnemyCount = EnemiesInZone.Num();
	if(bIsActive)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Emerald,
		//GetName() + " spawning enemies with authority " + FString::SanitizeFloat(HasAuthority()) + "  Spawn Time: "+ FString::SanitizeFloat(Time));
		if(!bIsPaused)
		{
			Time+=DeltaTime;
			DespawnTimer = 0.0f;
			//Spawning enemy's into zone
			if(Time>=SpawnTimer)
			{

				if(EnemiesInZone.Num() < MaxEnemyCount)
				{
					if(EnemysToSpawn.Num() != 0)
					{
						if(ZoneControler != nullptr)
						{
							int RandWaveAmount = FMath::RandRange(ZoneControler->MinEnemyPerWave,ZoneControler->MaxEnemyPerWave);
							 
							int i = 0;
							while(RandWaveAmount>i )
							{
								//breaks loop if the amount of enemies reaches the MaxEnemyCount to stop from going over limit 
								if(EnemiesInZone.Num() == MaxEnemyCount)
									break;
								int32 RandSpawner = FMath::RandRange(0,Spawners.Num()-1);
								//GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Spawning Enemy"));
								//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT(" %d"), RandEnemy));
								// If the spawner only spawns Engineer then it will check weather it should spawn a Engineer 
								i += SpawnerActive(Spawners[RandSpawner]);
							}
						}
						else
						{
							GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("ZoneControler is a nullptr"));
						}
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("No Enemy in spawn array"));
					}
				}
				//checking to see if any enemies are being destroyed and removes them if they have been
				for(int i = 0; EnemiesInZone.Num()>i;i++)
				{
					if(EnemiesInZone[i] != nullptr)
						if(EnemiesInZone[i]->IsActorBeingDestroyed())
						{
							EnemiesInZone.RemoveAt(i);
						}
				}
				Time = 0.0f;
			}
		}
	}
	else
	{
		//if there is not a total of 2 players in all of the zones then check to see if a player is in the zone

		if(HasAuthority())
		{
			if(ZoneControler != nullptr)
			{
				if(ZoneControler->NumPlayersInZones!=2)
				{
					TArray<AActor*> OverlappingActors;
					GetOverlappingActors(OverlappingActors);
					for(int i = 0; i<OverlappingActors.Num(); i++)
					{
						if(OverlappingActors[i]->ActorHasTag("Player"))
						{
							bIsPlayerInZone = true;
							//GEngine->AddOnScreenDebugMessage(-1,.5f,FColor::Red,TEXT("Player In Zone"));
							PlayerInZone.Add(Cast<ACharacter>(OverlappingActors[i]));
						}
					}
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("ZoneControler is a nullptr"));
			}
		}
		//removes either all AI in the zone or a set amount
		if(!bIsPaused)
		{
			DespawnTimer+=DeltaTime;
			if(EnemiesInZone.Num()>0 && DespawnTimer>=DespawnTime)
			{
				if(ZoneControler!= nullptr)
					ClearZone(ZoneControler->DespawnAmount);
				else
					GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("ZoneControler is a nullptr"));
				
				DespawnTimer = 0.0f;
			}
			Time = 0.0f;
		}
	}

	CurrEnemyCount = EnemiesInZone.Num();

	

}

//Called when a object begins a Overlap
void AZoneTool::OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1,.5f,FColor::Red,OtherActor->GetClass()->GetName());
		if(OtherActor->ActorHasTag("Player"))
		{
			bIsPlayerInZone = true;
			//GEngine->AddOnScreenDebugMessage(-1,.5f,FColor::Red,TEXT("Player In Zone"));
			PlayerInZone.Add(Cast<ACharacter>(OtherActor));
		}
		else if(OtherActor->ActorHasTag("Enemy"))
		{
			//checks to see if there spawned from the Engineer since they are never added into the zones enemies
			if(!OtherActor->ActorHasTag("Mine") && !OtherActor->ActorHasTag("Turret") && !OtherActor->ActorHasTag("Drones"))
			{
				if(AICanChangeZone == true)
				{
					AAIEnemyParent* tmpAI = Cast<AAIEnemyParent>(OtherActor);
					TransferAI(tmpAI);
				}
				
			}
		}
	}
}

//Called when a object ends a Overlap
void AZoneTool::EndOverLap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(HasAuthority())
	{
		if(OtherActor->ActorHasTag("Player"))
		{
			
			//GEngine->AddOnScreenDebugMessage(-1,.5f,FColor::Red,TEXT("Player has left Zone"));
			for(int i = 0; i < PlayerInZone.Num();i++ )
			{
				if(PlayerInZone[i] == OtherActor)
				{
					PlayerInZone.RemoveAt(i);
				}
	
			}

			//if there are no players in the zone sets bIsActive of this zone and neighbor zones to false
			if(0<=PlayerInZone.Num())
			{
				//GEngine->AddOnScreenDebugMessage(-1,.5f,FColor::Red,TEXT("Zone Set to inactive"));
				bIsPlayerInZone = false;
				bIsActive = false;
				for(int i = 0; i < NeighborsZones.Num();i++)
				{
					NeighborsZones[i]->bIsActive =false;
				}
			}
		
		}
	}
}

void AZoneTool::SetNeighborsActive()
{
	for(int i = 0; NeighborsZones.Num()>i;i++)
	{
		NeighborsZones[i]->bIsActive = true;
	}
}

void AZoneTool::CreateSpawners()
{
	
	for (int i = 0; i < SpawnerLocations.Num(); i++)
	{
		Spawners.Add(GetWorld()->SpawnActor<AZoneSpawner>(AZoneSpawner::StaticClass(), SpawnerLocations[i],FRotator(0,0,0),EnemyPara));
		Spawners[i]->Tags.Add("Grunt");
		Spawners[i]->SetParentZone(this);
		Spawners[i]->EnemysToSpawn = EnemysToSpawn;
	}
	int temp = Spawners.Num();
	if(bSpawnSuicider==true)
	{
		for (int i = 0; i < SuicderSpawnerLocations.Num(); i++)
		{

			Spawners.Add(GetWorld()->SpawnActor<AZoneSpawner>(AZoneSpawner::StaticClass(), SuicderSpawnerLocations[i],FRotator(0,0,0),EnemyPara));
			Spawners[i+temp]->Tags.Add("Suicider");
			Spawners[i+temp]->SetParentZone(this);
			Spawners[i+temp]->EnemysToSpawn.Add(Suicider);
		}
	}
	temp = Spawners.Num();
	if(bSpawnFlyingAI==true)
	{
		for (int i = 0; i < FlyingSpawnerLocations.Num(); i++)
		{
			Spawners.Add(GetWorld()->SpawnActor<AZoneSpawner>(AZoneSpawner::StaticClass(), FlyingSpawnerLocations[i],FRotator(0,0,0),EnemyPara));
			Spawners[i+temp]->Tags.Add("Flying");
			Spawners[i+temp]->SetParentZone(this);
			Spawners[i+temp]->EnemysToSpawn.Add(FlyingAI);
		}
	}
	temp = Spawners.Num();
	if(bSpawnEngineer==true)
	{
		for (int i = 0; i < EngineerSpawnerLocations.Num(); i++)
		{
			Spawners.Add(GetWorld()->SpawnActor<AZoneSpawner>(AZoneSpawner::StaticClass(), EngineerSpawnerLocations[i],FRotator(0,0,0),EnemyPara));
			Spawners[i+temp]->Tags.Add("Engineer");
			Spawners[i+temp]->SetParentZone(this);
			Spawners[i+temp]->EnemysToSpawn.Add(Engineer);
		}
	}
	temp = Spawners.Num();
	if(bSpawnGrunt==true)
	{
		for (int i = 0; i < GruntSpawnerLocations.Num(); i++)
		{
			Spawners.Add(GetWorld()->SpawnActor<AZoneSpawner>(AZoneSpawner::StaticClass(),  GruntSpawnerLocations[i],FRotator(0,0,0),EnemyPara));
			Spawners[i+temp]->Tags.Add("All");
			Spawners[i+temp]->SetParentZone(this);
			Spawners[i+temp]->EnemysToSpawn.Add(Grunt);
		}
	}

	
}

void AZoneTool::ClearZone(int32 RemoveAmount)
{
	for(int i= EnemiesInZone.Num()-1;i>=0;)
	{
		//checks if the amount of enemies wanted to be removed have been, if so it breaks.
		if(RemoveAmount==0)
			break;
		//checking to see if AI is being Destroyed before trying to destroy it
		if(EnemiesInZone[i] != nullptr)
			if(!EnemiesInZone[i]->IsActorBeingDestroyed())
				//EnemiesInZone[i]->Destroy();
				EnemiesInZone[i]->DealDamageToEnemy(1000);
		EnemiesInZone.RemoveAt(i);
		
		RemoveAmount--;
		i = EnemiesInZone.Num()-1;
	}
		
}

void AZoneTool::FillEnemyToSpawn()
{
	if(bSpawnSuicider)
	{
		if(Suicider!=nullptr)
		{
			if(SuiciderPatrolPaths.Num()!=0)
				EnemysToSpawn.Add(Suicider);
			else
				GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Suicider Patrol Path Array Empty"));
		}
		else
			GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Suicider ref NULL"));
	}

	if(bSpawnEngineer)
	{
		if(Engineer!=nullptr)
		{
			if(EngineerPatrolPaths.Num()!=0)
				EnemysToSpawn.Add(Engineer);
			else
				GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Engineer Patrol Path Array Empty"));
		}
		else
			GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Engineer ref NULL"));
	}

	if(bSpawnFlyingAI)
	{
		if(FlyingAI!=nullptr)
		{
			if(FlyingPatrolPaths.Num()!=0)
			{
				if(Zone3DNav != nullptr)
					EnemysToSpawn.Add(FlyingAI);
				else
					GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Zone3DNav is NULL"));
			}
			else
				GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("FlyingAI Patrol Path Array Empty"));
		}
		else
			GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("FlyingAI ref NULL"));
	}

	if(bSpawnGrunt)
	{
		if(Grunt!=nullptr)
		{
			if(GruntPatrolPaths.Num()!=0)
				EnemysToSpawn.Add(Grunt);
			else
				GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Grunt Patrol Path Array Empty"));
		}
		else
			GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,TEXT("Grunt ref NULL"));
	}
}

void AZoneTool::TransferAI(AAIEnemyParent* AIToTransfer)
{
	AZoneTool* AIZone = AIToTransfer->GetCurZone(); 
	if(AIZone !=this)
	{
		if (EnemiesInZone.Num()<MaxEnemyCount && bIsActive==true)
		{
			if(AIZone!=nullptr)
			{
				AIZone->RemoveAI(AIToTransfer);
				EnemiesInZone.Add(AIToTransfer);
				CurrEnemyCount = EnemiesInZone.Num();
				AIToTransfer->SetCurZone(this);
			}
		}
	}
}

void AZoneTool::RemoveAI(AAIEnemyParent* AIToRemove)
{
	int index = EnemiesInZone.Find(AIToRemove);
	EnemiesInZone.RemoveAt(index);
	CurrEnemyCount = EnemiesInZone.Num();
}

int AZoneTool::SpawnerActive(AZoneSpawner* Spawner)
{
	if (Spawner->ActorHasTag("All"))
	{
		Spawner->SpawnEnemies(this->GetActorLocation(),EnemyPara);
		return 1;
	}
	if (Spawner->ActorHasTag("Grunt") && GruntSpawnProb>=FMath::RandRange(0.01f,1.0f) )
	{
		Spawner->SpawnEnemies(this->GetActorLocation(),EnemyPara);
		return 1;
	}
	if (Spawner->ActorHasTag("Suicider") && SuicderSpawnProb>=FMath::RandRange(0.01f,1.0f) )
	{
		Spawner->SpawnEnemies(this->GetActorLocation(),EnemyPara);
		return 1;
	}
	if (Spawner->ActorHasTag("Flying") && FlyingSpawnProb>=FMath::RandRange(0.01f,1.0f) )
	{
		Spawner->SpawnEnemies(this->GetActorLocation(),EnemyPara);
		return 1;
	}
	if (Spawner->ActorHasTag("Engineer") && EngineerSpawnProb>=FMath::RandRange(0.01f,1.0f) )
	{
		Spawner->SpawnEnemies(this->GetActorLocation(),EnemyPara);
		return 1;
	}
	// if it maneged to get here then nothing was spawned
	return 0;
}

