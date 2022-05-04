// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetworkChar.h"
#include "AI/AIEnemyParent.h"
#include "KillZone.generated.h"

UCLASS()
class BLASTTEST_API AKillZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKillZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*** The box in which will cause the kill
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Collision")
	UBoxComponent* ZoneBounds;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

};
