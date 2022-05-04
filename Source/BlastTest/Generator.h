// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Generator.generated.h"

class AZoneToolCon;
UCLASS()
class BLASTTEST_API AGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGenerator();

	void GetLifetimeReplicatedProps(TArray <FLifetimeProperty> & OutLifetimeProps) const override;


	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	USceneComponent* Scene;
	
	/**Static mesh of the generator*/
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	UStaticMeshComponent* GenMesh;
	
	/**Component that represents the location where the pointObject spawns */
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	USceneComponent* PointSpawn;
	
	/**Time it takes to spawn SpawnObject after being set active**/
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float TimeToGenerate;

	/**Keeping track of till spawning SpawnObject **/
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float CurrentTime;

	/**boolean if set true will spawn SpawnObject after TimeToGenerate time passes**/
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Replicated)
	bool IsActive;

	/**boolean if set true the player can interact with object **/
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Replicated)
	bool IsInPlay;

	inline bool GetIsInPlay() { return IsInPlay; }

    /**The class of the point object that will spawn after the timer runs out**/
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SpawnObject;

	/**The refernce to the ZoneToolCon that controllers this gen**/
	UPROPERTY()
	AZoneToolCon* owneingZoneToolCon;

	/**bool that represents if the generator all ways stays in play.**/
	UPROPERTY()
	bool AllWaysInPlay;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Sound Settings")
	USoundBase* IdleSound;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Sound Settings")
	USoundBase* ActiveSound;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Sound Settings")
	USoundBase* CompletedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAudioComponent* ActiveAudioComponent;

	UPROPERTY()
	bool ClientSound;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	/** [Server] Responsible calling the multicast sound. Needed because multicasts must be ran from server
	*/
	UFUNCTION(Server, Unreliable)
	void ServerSound(USoundBase* Sound, int AudioComponentNum);
	
	/** [Server + Client] Method ran to set and spawn the sound for the server and client
	*/
	UFUNCTION(NetMulticast, Unreliable)
	void MultiSound(USoundBase* Sound, int AudioComponentNum);

public:
	/** sets the generator to active to build point object
	 @param ActiveParma boolean to set the active statis of the generator
	 ***/
	UFUNCTION(BlueprintCallable,NetMulticast,Reliable)
	void SetActive(bool ActiveParma);

	/** lets player interact with gen 
	@param ActiveParma boolean to set the active status of the generator
	***/
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SetInPlay(bool ActiveParma);
	
	/** spawns the ScoreObject**/
	UFUNCTION(BlueprintCallable)
	void SpawnScoreObject();

	/** Helper function used to call from the mod to call the multicast sound
	*/
	void SpawnSound(USoundBase* Sound, int AudioComponentNum);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
