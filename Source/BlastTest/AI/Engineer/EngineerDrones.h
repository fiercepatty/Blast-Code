// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineerToolsParent.h"
#include "Components/BoxComponent.h"
#include "EngineerDrones.generated.h"

/**
 * 
 */
UCLASS()
class  AEngineerDrones : public AEngineerToolsParent
{
	GENERATED_BODY()

public:
	/**
	 * Initializing the mine trigger boxes
	 */
	AEngineerDrones();

	/**
	 * @return Returns the patrolling path index getter
	 */
	inline int GetPatrollingPathIndex() const {return PatrollingPathIndex;}

	/**
	 * @return Getter for Destination Location
	 */
	inline FVector GetDestinationLocation() const {return DestinationLocation;}

	/**
	 * @param NewLocation Setter for the DestinationLocation
	 */
	inline void SetDestinationLocation(FVector NewLocation) {DestinationLocation = NewLocation;}

	/**@return Getter for the DistanceFromPlayer*/
	inline FVector2D GetDistanceFromPlayer() const {return DistanceFromPlayer;}

	/**@return Getter for the AttackingPlayerHeight*/
	inline FVector2D GetAttackingPlayerHeight() const {return AttackingPlayerHeight;}


	/**
	 *Set the Destination Location to FVector(0)
	 */
	inline void EmptyDestinationLocation() {DestinationLocation = FVector(0);}

	/**
	 * Checks to see if the Destination Vector Is 0
	 * @return True if the destination is 0 and false if it isnt
	 */
	bool CheckDestinationLocation() const;

	/**
	 * Increment the path index by one and modulo it by the number of point in the path to ensure that it isnt out of range
	 */
	void IncrementPatrollingPathIndex();

	/**
	 * @return Getter for the Rotation Alpha
	 */
	inline float GetRotationAlpha() const {return RotationAlpha;}

	/**
	 * @return Getter for the Acceleration
	 */
	inline float GetAcceleration() const {return Acceleration;}


	/**
	 * Runs after the constuctor but before the tick
	 */
	virtual void BeginPlay() override;

	/**
	 * Trigger Box for the drones to tell them if they are in another drones area
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BoxComponent)
	UBoxComponent* BoxTriggerBox;

	/**
	 * @return Getter from the DroneSwarm Attached to the drone
	 */
	inline class AEngineerEnemy* GetEngineerEnemy() const {return EngineerEnemy;};

	/**
	 * @param AEngineerEnemy Setter for Engineer Enemy
	 */
	inline void SetEngineerEnemy(AEngineerEnemy* AEngineerEnemy) {EngineerEnemy = AEngineerEnemy;}

	/**
	* Used Queuing it destroy pattern
	*/
	bool bNeedDestroyed = false;

	/**
	*Called every frame
	* Checks to see if it needs to be destroy and will Destroy it unless it bNeverDestroy is true
	*/
	virtual void Tick(float DeltaSeconds) override;

	/**
	* Overriding function to call when a tool needs to be destroyed
	*/
	virtual inline void SafeToolDestroy() override {bStartDestroyTimer=true;}

	/**
	* Used to start the destroy timer
	*/
	bool bStartDestroyTimer = false;

private:

	/**Used to tell the Drone that it needs to destroy itself when the Engineer is destroyed*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Drone", meta = (AllowPrivateAccess = "true"))
	bool bDestroyItselfOnEngineerDeath=true;

	/**
	* Reference to the controlling drone swarm for this drone
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Drone", meta = (AllowPrivateAccess = "true"))
	AEngineerEnemy* EngineerEnemy;
	
	/**
	 * PatrollingPathIndex keep internally to inform the task what path index this drone is at
	 */
	int PatrollingPathIndex = 0;

	/**
	 * The location that the drone is heading towards at this current moment
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Drone", meta = (AllowPrivateAccess = "true"))
	FVector DestinationLocation = FVector(0);

	/**
	 * Speed at which it will rotate at
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Movement Settings", meta = (AllowPrivateAccess = "true"))
	float RotationAlpha;

	/**
	 * The number multipled by the Force Vector created in the move tasks that will tell how fast the drone moves
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Movement Settings", meta = (AllowPrivateAccess = "true"))
	float Acceleration;

	/**
	 * Friction for rotation
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Movement Settings", meta = (AllowPrivateAccess = "true"))
	float AngularDamping;

	/**
	 * Friction for movement
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Movement Settings", meta = (AllowPrivateAccess = "true"))
	float LinearDamping;

	/**
	 *Distance from the player that the drone will be when attacking the player
	 *Range between lowest number and biggest number
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Drone", meta = (AllowPrivateAccess = "true"))
	FVector2D DistanceFromPlayer =FVector2D(100,500);

	/**
	*Height that the drone will be at when it is attacking the player
	*Range between lowest number and biggest number
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Drone", meta = (AllowPrivateAccess = "true"))
	FVector2D AttackingPlayerHeight =FVector2D(100,500);
	

	/**
	* Sets the life time the mine has when set to destroy itself
	* how long the drone survives after engineer dies
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings | Drone", meta = (AllowPrivateAccess = "true"))
	float DroneLifeTime= 5;

	/**
	* Inform the engineer that it has died
	*/
	void DestroyChildren() override;
};
