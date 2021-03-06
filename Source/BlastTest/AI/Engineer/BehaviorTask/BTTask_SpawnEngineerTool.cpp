// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SpawnEngineerTool.h"
#include "EngineerEnemy.h"
#include "EngineerMine.h"
#include "EngineerTurret.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UBTTask_SpawnEngineerTool::UBTTask_SpawnEngineerTool(FObjectInitializer const& object_initializer)
{
	bNotifyTick=true;
	NodeName = TEXT("Spawn Engineer Tool");
}

EBTNodeResult::Type UBTTask_SpawnEngineerTool::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EnemyController = Cast<AEnemyControllerParent>(OwnerComp.GetAIOwner());
	if(EnemyController)
	{
		Engineer = Cast<AEngineerEnemy>(EnemyController->GetPawn());
		if(Engineer)
		{
			EnemyController->GetBlackboard()->SetValueAsFloat(FiringCooldown.SelectedKeyName,Engineer->GetAttackingCooldown());
			EnemyController->GetBlackboard()->SetValueAsFloat(SpawningTimer.SelectedKeyName,Engineer->GetSpawningTimer());
		}
		return EBTNodeResult::Type::InProgress;
	}
	return EBTNodeResult::Type::Failed;
}

void UBTTask_SpawnEngineerTool::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	EnemyController = Cast<AEnemyControllerParent>(OwnerComp.GetAIOwner());
	if(EnemyController)
	{
		Engineer = Cast<AEngineerEnemy>(EnemyController->GetPawn());
		if(Engineer)
		{
			//Only start the spawning movement when the timer reaches zero
			float SpawningStartTimer = EnemyController->GetBlackboard()->GetValueAsFloat(SpawningTimer.SelectedKeyName);
			SpawningStartTimer-=DeltaSeconds;
			if(SpawningStartTimer<=0)
			{
				//Spawn in tool when cooldown reaches zero
				float SpawnCooldown = EnemyController->GetBlackboard()->GetValueAsFloat(FiringCooldown.SelectedKeyName);
				SpawnCooldown-=DeltaSeconds;
				if(SpawnCooldown<=0)
				{
					EnemyController->StopMovement();
					Engineer->bForcePatrolPoint=true;
					SpawnRandomTool();
					FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
				}
				EnemyController->GetBlackboard()->SetValueAsFloat(FiringCooldown.SelectedKeyName,SpawnCooldown);
				//Move Around Randomly until we spawn in a tool

				if(Engineer->bFinishedRotating && !Engineer->bNeedNewPatrolPoint)
				{
					EnemyController->StopMovement();
					EnemyController->MoveToLocation(Engineer->GetDesiredLocation(),-1);
					Engineer->bNeedNewPatrolPoint=true;
				}
				else
				{
					//If we have reached our desired location we will check to see if we are at the location we want to be at
					if (EnemyController->GetPathFollowingComponent()->DidMoveReachGoal() && Engineer->bNeedNewPatrolPoint)
					{

						Engineer->bNeedNewPatrolPoint=false;
						if(!MoveToPatrollingPoint())
							FinishLatentTask(OwnerComp,EBTNodeResult::Type::Failed);
					}
				}
			
				if (EnemyController == nullptr || Engineer == nullptr)
				{
					FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
				}
			}
			EnemyController->GetBlackboard()->SetValueAsFloat(SpawningTimer.SelectedKeyName,SpawningStartTimer);
		}
	}
}

bool UBTTask_SpawnEngineerTool::CheckCollision(FVector DesiredLocation) const
{
	TArray<AActor*> OutActors;
	return UKismetSystemLibrary::BoxOverlapActors(GetWorld(),DesiredLocation,FVector(75)
		,ObjectTypes,ActorFilterClass,TArray<AActor*>(),OutActors);
}

void UBTTask_SpawnEngineerTool::SpawnRandomTool() const
{
	//Get a spawn location
	FVector SpawningLocation = Engineer->GetActorLocation() + Engineer->GetActorForwardVector()*250;
	SpawningLocation.Z += 100;	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Engineer;
	SpawnParams.Instigator = Engineer->GetInstigator();
	Engineer->bNeedNewPatrolPoint=true;
		
	if(!CheckCollision(SpawningLocation))
	{
		if(Engineer->GetPlayerCharacters().Num()!=0)
		{
			int const RandomNum = FMath::RandRange(1,Engineer->GetEngineerTools().Num());
			AEngineerToolsParent* EngineerTool = GetWorld()->SpawnActor<AEngineerToolsParent>(Engineer->GetEngineerTools()[RandomNum-1], SpawningLocation, Engineer->GetActorRotation(), SpawnParams);
			if(Cast<AEngineerMine>(EngineerTool))
			{
				if(!Engineer->CanSpawnInMine())
				{
					EngineerTool->Destroy();
				}
				else if(EngineerTool)
				{
					EngineerTool->SetPlayerCharacters(Engineer->GetPlayerCharacters());
					EngineerTool->SetEngineerParent(Engineer);
					Engineer->AddOneToCurrentNumberOfMines();
				}
			}
			else if(Cast<AEngineerTurret>(EngineerTool))
			{
				if(!Engineer->CanSpawnInTurret())
				{
					EngineerTool->Destroy();
				}
				else if(EngineerTool)
				{
					EngineerTool->SetPlayerCharacters(Engineer->GetPlayerCharacters());
					EngineerTool->SetEngineerParent(Engineer);
					Engineer->AddOneToCurrentNumberOfTurret();
				
				}
			}
		
		}
	}
}

bool UBTTask_SpawnEngineerTool::MoveToPatrollingPoint() const
{
	if(Engineer || EnemyController)
	{
		if(Engineer->GetPatrollingPath())
		{
			//Offset the location around a patrol point so that the enemies do not all try to go to one spot
			FVector FinalLocation = Engineer->GetPatrollingPath()->GetPatrolPoint(Engineer->GetPatrollingPathIndex());
			FinalLocation = Engineer->GetPatrollingPath()->GetActorTransform().TransformPosition(FinalLocation);
			float const X = FMath::FRandRange(-1,1);
			float const Y = FMath::FRandRange(-1,1);
			FVector const DesiredLocation = FinalLocation + FVector(X,Y,0) * OffSetFromPathingToSpawn;
			//GEngine->AddOnScreenDebugMessage(-1,1.5,FColor::Red,FinalLocation.ToString());
			UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
			if(!NavSys)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Nav Mesh Volume not found"));
				return false;
			}
			FNavLocation Loc;
			if (NavSys->GetRandomPointInNavigableRadius(DesiredLocation, Engineer->GetMoveToTolerance(), Loc, nullptr))
			{
				Engineer->GetCharacterMovement()->MaxWalkSpeed=Engineer->GetMovementSpeed();
				Engineer->RotateTowardsVector(Loc.Location);
			}
			Engineer->IncrementPatrollingPathIndex();
			return true;
		}
		
	}
	return false;
}
