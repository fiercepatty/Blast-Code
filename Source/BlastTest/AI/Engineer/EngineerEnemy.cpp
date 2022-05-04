// Fill out your copyright notice in the Description page of Project Settings.


#include "EngineerEnemy.h"
#include "EngineerDrones.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AEngineerEnemy::AEngineerEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

void AEngineerEnemy::RotateTowardsVector(FVector Location)
{
	DesiredLocation = Location;
	FRotator EndResultRotation = FRotationMatrix::MakeFromX(DesiredLocation-GetActorLocation()).Rotator();
	EndResultRotation.Pitch=0;
	FQuat const FirstFQuat(GetActorRotation());
	FQuat  const SecondFQuat(EndResultRotation);
	FRotator StepInRotation = FQuat::Slerp(FirstFQuat, SecondFQuat, RotationAlpha).Rotator();
	StepInRotation.Pitch=0;
	SetActorRotation(StepInRotation);
	bFinishedRotating=false;
}

// Called when the game starts or when spawned
void AEngineerEnemy::BeginPlay()
{
	Super::BeginPlay();
	if(DronePatrol)
	{
		DronePatrol->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform);
	}
	else
	{
		DronePatrol = GetWorld()->SpawnActor<APatrollingPath>(PatrolClass,GetActorLocation()+FVector(0,0,250),GetActorRotation());
		if(DronePatrol)
		{
			DronePatrol->AttachToActor(this,FAttachmentTransformRules::KeepWorldTransform);
		}
	}
	
}

void AEngineerEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(!bFinishedRotating)
	{
		FRotator EndResultRotation = FRotationMatrix::MakeFromX(DesiredLocation-GetActorLocation()).Rotator();
		EndResultRotation.Pitch=0;
		const FQuat  FirstFQuat(GetActorRotation());
		const FQuat   SecondFQuat(EndResultRotation);
		FRotator StepInRotation = FQuat::Slerp(FirstFQuat, SecondFQuat, RotationAlpha).Rotator();
		StepInRotation.Pitch = 0;
		SetActorRotation(StepInRotation);
		//GEngine->AddOnScreenDebugMessage(0,1.5,FColor::Red,FVector((FirstFQuat - SecondFQuat).Size()).ToString());
		if((FirstFQuat - SecondFQuat).Size() <= 0.1)
			bFinishedRotating=true;
			
		
	}
}

void AEngineerEnemy::IncrementPatrollingPathIndex()
{
	PatrollingPathIndex++;
	PatrollingPathIndex%=GetPatrollingPath()->NumOfPoint();
}

