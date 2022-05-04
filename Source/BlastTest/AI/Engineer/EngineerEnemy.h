// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIEnemyParent.h"
#include "EngineerToolsParent.h"
#include "GameFramework/Character.h"
#include "EngineerEnemy.generated.h"


class AEngineerDrones;
UCLASS()
class  AEngineerEnemy : public AAIEnemyParent
{
	GENERATED_BODY()

public:
	/**
	* Enable the tick for the drone swarm
	*/
	AEngineerEnemy();

	/**
	* Boolean on whether or not the Engineer is moving
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	bool bNeedNewPatrolPoint = false;

	/**Boolean to tell the Behavior Task that the Engineer is done rotating*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	bool bFinishedRotating = true;

	/**A way to force the engineer to get another patrol point*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	bool bForcePatrolPoint = false;

	/**
	* @return Returns the patrolling path index getter
	*/
	inline int GetPatrollingPathIndex() const {return PatrollingPathIndex;}

	/**
	* Increment the path index by one and modulo it by the number of point in the path to ensure that it isnt out of range
	*/
	void IncrementPatrollingPathIndex();

	/**
	 * @return Reference to all the engineer tools that the are included in the engineer that it can spawn
	 */
	inline TArray<TSubclassOf<AEngineerToolsParent>> GetEngineerTools() const {return EngineerTools;}

	/**
	 * @return Getter for Spawning Timer
	 */
	inline float GetSpawningTimer() const {return SpawningTimer;}

	/**
	 * @return Getter for DroneSpawningDelayTimerPatrolling
	 */
	inline float GetDroneSpawningDelayTimerPatrolling() const {return DroneSpawningDelayTimerPatrolling;}
	
	/**
	* @return Getter for DroneSpawningDelayTimerPlayer
	*/
	inline float GetDroneSpawningDelayTimerPlayer() const {return DroneSpawningDelayTimerPlayer;}

	/**
	 * @return Getter for the Drone Class
	 */
	inline TSubclassOf<AEngineerToolsParent> GetDroneClass() const {return DroneClass;}

	/**Getter for the drone patrolling path*/
	inline APatrollingPath* GetDronePatrollingPath() const {return DronePatrol;}

	
	/**
	 * Check to see if we can spawn in a Tool of a particalar type and if we can return true
	 * @return True if we can spawn in the tool and false if you cant
	 */
	inline bool CanSpawnInDrone() const {return CurrentNumberOfDrones<=MaxNumberOfDrones; }

	/**
	* Check to see if we can spawn in a Tool of a particalar type and if we can return true
	* @return True if we can spawn in the tool and false if you cant
	*/
	inline bool CanSpawnInMine() const {return CurrentNumberOfMines<=MaxNumberOfMines;}

	/**
	* Check to see if we can spawn in a Tool of a particalar type and if we can return true
	* @return True if we can spawn in the tool and false if you cant
	*/
	inline bool CanSpawnInTurret() const {return CurrentNumberOfTurrets<=MaxNumberOfTurrets;}

	/**
	 * Add one to the current Number of Drones
	 */
	inline void AddOneToCurrentNumberOfDrones() {CurrentNumberOfDrones++;}

	/**
	* Add one to the current Number of Mine
	*/
	inline void AddOneToCurrentNumberOfMines() {CurrentNumberOfMines++;}

	/**
	* Add one to the current Number of Turret
	*/
	inline void AddOneToCurrentNumberOfTurret() {CurrentNumberOfTurrets++;}

	/**
	* Subtract one to the current Number of Drones
	*/
	inline void SubtractOneToCurrentNumberOfDrones() {CurrentNumberOfDrones--;}

	/**
	* Subtract one to the current Number of Mine
	*/
	inline void SubtractOneToCurrentNumberOfMines() {CurrentNumberOfMines--;}

	/**
	* Subtract one to the current Number of Turret
	*/
	inline void SubtractOneToCurrentNumberOfTurret() {CurrentNumberOfTurrets--;}

	/**
	 * Getter for distance from player
	 */
	inline int GetDistanceFromPlayer() const {return DistanceFromPlayer;}

	/**
	 * Getter for Desired Location
	 */
	inline FVector GetDesiredLocation() const {return DesiredLocation;}

	/**Rotate the engineer towards Location and switch a boolean when the engineer is finished rotating
	 * If bFinishedRotating is False inside of tick will complete implementation for rotation
	 * @param Location The Vector Location we want the engineer rotate towards
	 */
	void RotateTowardsVector(FVector Location);

	
protected:
	/**
	 *Checks to see if a drone path was set if not it creates one
	 *Checks to see if any drone path were add into the array if there were it will parent the paths to the engineer so that they move with the engineer
	 *Called when the game starts or when spawned
	 */
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
private:
	
	/**Array for all the engineer tools. It has to be a child of the EngineerToolsParent to be able to add to the array. Add it to this array and it will have a chance to spawn*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AEngineerToolsParent>> EngineerTools;

	/**The Distance that the engineer will attempt to keep from the player when the engineer sees the player*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	int DistanceFromPlayer = 1500;

	/**Engineer Rotation Alpha how fast the engineer rotates towards the desired location*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	float RotationAlpha = 0.14;

	/**
	 * Class for the drones when they are created referenced to the blueprint
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEngineerToolsParent> DroneClass;
	
	/**Array for all the drone patrols that a drone can randomly be assigned to*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
   	APatrollingPath* DronePatrol;

	/**
	 * Patrol Class for the drone if we need to create a potrol so that the drone can follow something linked to a blueprint
	 */
   	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
   	TSubclassOf<APatrollingPath> PatrolClass;

	/**
	 * Timer for the spawning of tools for the Engineer. 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	float SpawningTimer = 5;

	/**
	 * Timer for spawning in a new drone while patrolling
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	float DroneSpawningDelayTimerPatrolling = 10;

	/**
	* Timer for spawning in a new drone while Player Found
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	float DroneSpawningDelayTimerPlayer = 10;

	/**
	* Max Number of Drones the Engineer Can Spawn
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	float MaxNumberOfDrones = 15;

	/**
	* Max Number of Mines the Engineer Can Spawn
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	float MaxNumberOfMines = 5;

	/**
	* Max Number of Turrets the Engineer Can Spawn
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	float MaxNumberOfTurrets = 5;

	/**
	* Current Number of Drones the Engineer has
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	float CurrentNumberOfDrones = 0;

	/**
	* Current Number of Mines the Engineer has
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	float CurrentNumberOfMines = 0;

	/**
	* Current Number of Turrets the Engineer has
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Engineer", meta = (AllowPrivateAccess = "true"))
	float CurrentNumberOfTurrets = 0;
	
	/**
	 * Patrolling path index that is internally kept for the engineer to know where in the patrol it is heading towards
	 */
	int PatrollingPathIndex = 0;

	//Location that the engineer is trying to turn toward
	FVector DesiredLocation;

};
