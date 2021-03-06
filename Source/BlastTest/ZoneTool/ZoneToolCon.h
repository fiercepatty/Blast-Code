// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZoneTool.h"
#include "ZoneToolCon.generated.h"

UCLASS()
class  AZoneToolCon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZoneToolCon();

	/**Max number of enemies allowed to spawn in the level if all zones were active*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Spawn Control")
	int32 MaxEnemyCount;
	
	/**Max amount of enemies allowed to spawn at once*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Spawn Control")
	int32 MaxEnemyPerWave;

	/**Min amount of enemies allowed to spawn at once*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Spawn Control")
	int32 MinEnemyPerWave;

	/**The amount of enemy's that can be despawned at once*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Spawn Control")
	int32 DespawnAmount;
	
	/**The amount of enemies currently in the world*/
	UPROPERTY(VisibleAnywhere,Category="Spawn Control")
	int32 CurrEnemyCount;
	
	/**TArray with references to all players currently in the level
	 * This is given to the enemies when they spawn
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Spawn Control")
	TArray<ACharacter*> PlayersInWorld;

	/**Number of generators to spawn in world */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Generator")
	int GenMaxNum;

	/**the amount of players in all zones*/
	int32 NumPlayersInZones;

	/**Reference to the Generator class set in blueprint*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category="Generator")
	TSubclassOf<AActor> GenClass;

	/**The number of Generators that are in play at one time*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Generator")
	int32 NumGensInPlay;
	
	/**If true all gens will be set in play and stay in play*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Generator")
	bool AllGensInPlay;

	/**Array of of refs to the Generators spawned in the Zone*/
	TArray<AGenerator*> GensInWorld;
	
	/**Boolean if set true means should update Generators*/
	bool UpdateGens;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**TArray with references to all of the zones in the level*/
	UPROPERTY()
	TArray<AZoneTool*> Zones;  

public:

	/**GETTERS*/
	/**@return the Max amount of enemys allowed to spawn in all of the zones*/
	inline int32 GetMaxEnemyCount(){return MaxEnemyCount;}

	/**SETTERS*/
	void SetMaxEnemyCount(int32 NewMaxEnemyCount);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**finds the closest NeighborCount amount of zones and places them in the zones NeigborsZones Array
	 *@param ZoneIndex - the Index of a Zone from the Zones Array
	 * @param NeighborCount - Amount of nearby zones user wants to set as its neighbor
	 */
	void AssignNeighborZones(int32 ZoneIndex,int32 NeighborCount);

	/**When called removes the player from the array of PlayersInWorld, should be called when a player dies.
	 * @param OldPlayer  reference to the player that is being removed from the world
	 */
	UFUNCTION(BlueprintCallable)
	void RemovePlayer(ACharacter* OldPlayer);

	/**When called adds the player to the array of PlayersInWorld. Should be called when a player is spawned
	 * @param NewPlayer reference to the player that is being added to the world
	 */
	UFUNCTION(BlueprintCallable)
	void AddPlayer(ACharacter* NewPlayer);

	/** Spawns generators at the locations of the points from the zones */
	void SpawnGenerators(int GenNum);

	/**Call this in the tick to manage what generators are in play*/
	void SetGenratorsInPlay();

};
