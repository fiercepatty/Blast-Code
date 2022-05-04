// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DroneNearPlayer.h"

#include "DrawDebugHelpers.h"
#include "EngineerEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UBTTask_DroneNearPlayer::UBTTask_DroneNearPlayer(FObjectInitializer const& object_initializer)
{
	bNotifyTick=true;
	NodeName=TEXT("Drone Near Player");
}

EBTNodeResult::Type UBTTask_DroneNearPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EnemyController = Cast<AEnemyControllerParent>(OwnerComp.GetAIOwner());
	if(EnemyController)
	{
		EngineerDrone = Cast<AEngineerDrones>(EnemyController->GetPawn());
		if(EngineerDrone)
		{
			//Finding the Location we want to move to
			FVector const MoveToDest = FindLocationNearPlayer();
			if(MoveToDest==FVector(0))
				return EBTNodeResult::Type::Failed;
			EnemyController->GetBlackboard()->SetValueAsVector(DroneInRangeLocation.SelectedKeyName,MoveToDest);
		}
	}
	return EBTNodeResult::Type::InProgress;
}

void UBTTask_DroneNearPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	EnemyController = Cast<AEnemyControllerParent>(OwnerComp.GetAIOwner());
	if(EnemyController)
	{
		EngineerDrone = Cast<AEngineerDrones>(EnemyController->GetPawn());
	}

	if (EnemyController == nullptr || EngineerDrone == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	
	InitialFinalDistance = DistanceToFinal();
	//Checking to see if we are within the tolerance of end distance
	if(DistanceToFinal()<= EngineerDrone->GetMoveToTolerance())
	{
		EnemyController->GetBlackboard()->SetValueAsBool(InRange.SelectedKeyName,true);
		FinishLatentTask(OwnerComp,EBTNodeResult::Type::Succeeded);
	}
	//Are we going too fast and need to go slower
	if(BelowMaxVelocity())
	{
		FVector const ForceToAdd = ForceVector() * EngineerDrone->GetAcceleration();
		//Checking to see if we are trying to do a force vector that is too large it was a error in calculation
		if((EngineerDrone->GetActorLocation() +ForceToAdd).Size()>20000)
		{
		}
		else
		{
			EngineerDrone->GetCapsuleComponent()->AddForce(ForceToAdd, TEXT("None"),true);
		}
	}
}

FVector UBTTask_DroneNearPlayer::FindLocationNearPlayer()
{
	//Calls helper function to find the player
	int const ClosestIndex = FindClosestPlayer();
	if(ClosestIndex>=0)
	{
		//Calls the helper to find the actual location we want to move to
		FVector const Destination =VectorWithDistanceFromPlayer(DistanceFromPlayer, ClosestIndex);
		//Get the drone swarm to call a function in drone swarm that checks to see if we are overlapping a location with the enemy the function will return a new fvector to where we actually are moving to if the other locaiton was bad
		AEngineerEnemy* Engineer = Cast<AEngineerEnemy>(EngineerDrone->GetEngineerEnemy());
		if(Engineer)
		{
			//DrawDebugSphere(GetWorld(),Destination,25,4,FColor::Red,false,25,0,5);
			return Destination;
			//Successful find
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(0,1.4,FColor::Blue,TEXT("Engineer is not set"));
			EngineerDrone->bNeedDestroyed=true;
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(0,1.4,FColor::Blue,TEXT("Player Not Set"));
		EngineerDrone->bNeedDestroyed=true;
	}
	return FVector(0);
}

int UBTTask_DroneNearPlayer::FindClosestPlayer() const
{
	TArray<ACharacter*> Players = EngineerDrone->GetPlayerCharacters();
	//Checking to see if there is a player in the array or at least one if there is one we just check that one if there is two we check both
	if(Players.Num()>0)
	{
		if(Players.Num() >1)
		{
			ACharacter* Player1 = Cast<ACharacter>(Players[0]);
			ACharacter* Player2 = Cast<ACharacter>(Players[1]);

			if(Player1 && Player2)
			{
				const FVector Player1Location = Player1->GetActorLocation();
				const FVector Player2Location = Player2->GetActorLocation();
				const float Player1Distance =(Player1Location-EngineerDrone->GetActorLocation()).Size();
				const float Player2Distance =(Player2Location-EngineerDrone->GetActorLocation()).Size();

				if(Player1Distance>Player2Distance)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else
		{
			return 0;
		}
	}
	return -1;
}

FVector UBTTask_DroneNearPlayer::VectorWithDistanceFromPlayer(float Distance, int PlayerIndex) const 
{
	// now that we know the index is valid get the target and use that to find the vector
	const ACharacter* TargetPlayer = EngineerDrone->GetPlayerCharacters()[PlayerIndex];
	FVector UnitVector = TargetPlayer->GetActorForwardVector();

	const int RotationAngle = FMath::RandRange(-90,90);

	UnitVector = UnitVector.RotateAngleAxis(RotationAngle, FVector(0, 0, 1));

	const FVector2D FromPlayer =EngineerDrone->GetDistanceFromPlayer();

	FVector MoveToLocation = UnitVector * FMath::RandRange(FromPlayer.X,FromPlayer.Y);

	const FVector2D DroneHeight = EngineerDrone->GetAttackingPlayerHeight();

	MoveToLocation.Z += FMath::RandRange(DroneHeight.X,DroneHeight.Y);
	
	return MoveToLocation+TargetPlayer->GetActorLocation();
	
}

float UBTTask_DroneNearPlayer::DistanceToFinal() const
{
	return (EngineerDrone->GetActorLocation()-EnemyController->GetBlackboard()->GetValueAsVector(DroneInRangeLocation.SelectedKeyName)).Size();
}

FVector UBTTask_DroneNearPlayer::ForceVector() const
{

	//Lerp the force from direction we want to go to the 0 vector so that the closer we are to the point the smaller force we apply
	FVector UnitVectorDirection = (EnemyController->GetBlackboard()->GetValueAsVector(DroneInRangeLocation.SelectedKeyName) - EngineerDrone->GetActorLocation()).GetSafeNormal();
	float const AlphaValue = (InitialFinalDistance - DistanceToFinal()) / InitialFinalDistance;
	UnitVectorDirection=UnitVectorDirection + AlphaValue * (FVector(0,0,0) - UnitVectorDirection);
	return UnitVectorDirection;
}

bool UBTTask_DroneNearPlayer::BelowMaxVelocity() const
{
	UCapsuleComponent* DroneCapsuleComp = EngineerDrone->GetCapsuleComponent();
	float const VectorLength = DroneCapsuleComp->GetPhysicsLinearVelocity(TEXT("None")).Size();
	return VectorLength <= EngineerDrone->GetMovementSpeed();
}