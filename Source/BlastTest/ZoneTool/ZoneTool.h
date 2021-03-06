// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ZoneSpawner.h"
#include "AIEnemyParent.h"
#include "PatrollingPath.h"
#include "AI/Flying/Navigation/NavigationVolumeForAI3D.h"
#include "Generator.h"
#include "ZoneTool.generated.h"

class AZoneToolCon;
UCLASS()
class  AZoneTool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZoneTool();

	/**boolean that if it is set true the zone should start the process of spawning the Enemies*/
	UPROPERTY(EditAnywhere, Category= Spawns)
	bool bIsActive;

	/**boolean that tells the game that we are paused*/
	UPROPERTY(BlueprintReadWrite,EditAnywhere , Category= Spawns)
	bool bIsPaused =false;

	/**number of nearby zones that activate when a player enters this zone*/
	UPROPERTY(BlueprintReadWrite,EditAnywhere , Category= Spawns)
	int numNeighborZones;
	
	/**If bIsPlayerInZone is true the player is in this zone*/
	UPROPERTY(EditAnywhere, Category= "Spawns")
	bool bIsPlayerInZone;

	/**Reference to the collision box that encapsulates the all area the user wants in the zone */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Collison)
	UBoxComponent* ZoneBounds;

	/**An Array of FVectors that represents the location of where the spawners will be placed at*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|SpawnLocations", meta=(MakeEditWidget= true))
	TArray<FVector> SpawnerLocations;

	/**An Array of FVectors that represents the location of where the spawners that only spawn Suicders will be placed at*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|SpawnLocations", meta=(MakeEditWidget= true))
	TArray<FVector> SuicderSpawnerLocations;

	/**An Array of FVectors that represents the location of where the spawners that only spawn Flying will be placed at*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|SpawnLocations", meta=(MakeEditWidget= true))
	TArray<FVector> FlyingSpawnerLocations;

	/**An Array of FVectors that represents the location of where the spawners that only spawn Engineers will be placed at*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|SpawnLocations", meta=(MakeEditWidget= true))
	TArray<FVector> EngineerSpawnerLocations;

	/**An Array of FVectors that represents the location of where the spawners that only spawn grunts will be placed at*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|SpawnLocations", meta=(MakeEditWidget= true))
	TArray<FVector> GruntSpawnerLocations;

	/*----------------------------------------------Probability-------------------------------------------------------------*/
	/**Float the represents the probability that the Engineer will spawn*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|SpawnProbability", meta=(MakeEditWidget= true,ClampMin = "0.01", ClampMax = "1.0", UIMin = "0.01", UIMax = "1.0"))
	float SuicderSpawnProb;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|SpawnProbability", meta=(MakeEditWidget= true,ClampMin = "0.01", ClampMax = "1.0", UIMin = "0.01", UIMax = "1.0"))
	float FlyingSpawnProb;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|SpawnProbability", meta=(MakeEditWidget= true,ClampMin = "0.01", ClampMax = "1.0", UIMin = "0.01", UIMax = "1.0"))
	float EngineerSpawnProb;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|SpawnProbability", meta=(MakeEditWidget= true,ClampMin = "0.01", ClampMax = "1.0", UIMin = "0.01", UIMax = "1.0"))
	float GruntSpawnProb;
	
	/**Array of vectors that the generators can spawn at*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Generator, meta=(MakeEditWidget= true))
	TArray<FVector> GenLocations;

	/**Array of enemy classes to be spawned in this zone*/
	TArray<TSubclassOf<class AAIEnemyParent>> EnemysToSpawn;

	/**The amount of time that should pass before an inactive zone starts clearing out its enemies*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Spawns)
	float DespawnTime;

	/**boolean that represent weather an AI can be Transferred into this zone*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Spawns)
	bool AICanChangeZone;
	
	/**An Array of the Suicider patrolPaths*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=AIControl)
	TArray<APatrollingPath*> SuiciderPatrolPaths;

	/**An Array of the FlyingAI patrolPaths*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=AIControl)
	TArray<APatrollingPath*> FlyingPatrolPaths;

	/**An Array of the Engineer patrolPaths*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=AIControl)
	TArray<APatrollingPath*> EngineerPatrolPaths;

	/**An Array of the Grunt patrolPaths*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=AIControl)
	TArray<APatrollingPath*> GruntPatrolPaths;
	
	/**Reference to the NavigationVolumeForAI3D that the flyingAI in this Zone will use*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=AIControl)
	ANavigationVolumeForAI3D* Zone3DNav;

	/**Amount of time it takes to spawn a wave*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Spawns)
	float SpawnTimer;

	/**boolean that sets if Suicider is going to spawn*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|Spawn Options")
	bool bSpawnSuicider;

	/**boolean that sets if FlyingAI is going to spawn*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|Spawn Options")
	bool bSpawnFlyingAI;

	/**boolean that sets if Engineer is going to spawn*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|Spawn Options")
	bool bSpawnEngineer;

	/**boolean that sets if Grunt is going to spawn*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Spawns|Spawn Options")
	bool bSpawnGrunt;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=Spawns)
	float SpawnerRadius;

	/**Array of references to the enemies currently in this zone*/
	UPROPERTY()
	TArray<AAIEnemyParent*> EnemiesInZone;

	/**Array of references the closest zones to this zone*/
	TArray<AZoneTool*> NeighborsZones;

	/**Array of references to the players currently in the zone*/
	UPROPERTY()
	TArray<ACharacter*> PlayerInZone;

	/**pointer to the levels ZoneToolCon*/
	UPROPERTY()
	AZoneToolCon* ZoneControler;

	/**current number of enemies in spawned from the zone*/
	UPROPERTY(VisibleAnywhere,Category=Spawns)
	int32 CurrEnemyCount;

	/**Max amount of enemies allowed to be in this zone at one time*/
	int32 MaxEnemyCount;

	

	
protected:
	
	
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/**Reference to suicider enemy set in blueprint class*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Spawns|Classes")
	TSubclassOf<class AAIEnemyParent> Suicider;

	/**Reference to FlyingAI enemy set in blueprint class*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Spawns|Classes")
	TSubclassOf<class AAIEnemyParent> FlyingAI;

	/**Reference to Engineer enemy set in blueprint class*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Spawns|Classes")
	TSubclassOf<class AAIEnemyParent> Engineer;

	/**Reference to  enemy set in blueprint class*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Spawns|Classes")
	TSubclassOf<class AAIEnemyParent> Grunt;

	
	

	/**Array of references to all the spawners made by this zone*/
	UPROPERTY()
	TArray<AZoneSpawner*> Spawners;

	/**Float keeping track of time passing when zone is active*/
	float Time;

	/**Keeps track of the time till the next group of enemies are destroyed*/
	float DespawnTimer;

	/**Spawning parameters used for spawning all of the AI*/
	FActorSpawnParameters EnemyPara;

	
	
public:

	/**GETTERS*/
	/**@return the reference to the class of the Suicider enemy*/
	inline TSubclassOf<class AAIEnemyParent> getSuiciderClass(){ return Suicider;}

	/**@return the reference to the class of the FlyingAI enemy*/
	inline TSubclassOf<class AAIEnemyParent> getFlyingAIClass(){ return FlyingAI;}

	/**@return the reference to the class of the Engineer enemy*/
	inline TSubclassOf<class AAIEnemyParent> getEngineerClass(){ return Engineer;}

	/**@return the reference to the class of the Grunt enemy*/
	inline TSubclassOf<class AAIEnemyParent> getGruntClass(){ return Grunt;}
	

	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndOverLap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**method that sets all of the zones in the Neighbor Array to active*/
	void SetNeighborsActive();

private:
	/**Creates AZoneSpawner at each of the FVectors in the SpawnerLocation Array */
	void CreateSpawners();

	/**Clears out the zone by destroying RemoveAmount of enemies in this zone
	 * @prama Amount of enemy's to remove from the zone if set to negative will clear out whole zone
	 */
	void ClearZone(int32 RemoveAmount);

	/**places all enemy classes that have been set to spawn in spawning array, and makes all checks necessary to make sure they can spawn*/
	void FillEnemyToSpawn();

	/**
	*Transfers given AI into this zone.If the zone is already at its max AI the AI will stay assigned to its current zone.
	*@param AIToTransfer Reference to the AI that is being transferred to this zone
	*/
	void TransferAI(AAIEnemyParent* AIToTransfer);

	/**
	*Removes the given AI from this zones EnemiesInZone array.
	*@param AIToRemove Reference to the AI that is being removed
	*/
	void RemoveAI(AAIEnemyParent* AIToRemove);

	/**
	 *Given a spawner checks if it should activate
	 *@param Spawner Reference to the spawner that is getting tested
	 *@return will return a 1 if it activated or a 0 if it did not
	 **/
	int SpawnerActive(AZoneSpawner* Spawner);

};
