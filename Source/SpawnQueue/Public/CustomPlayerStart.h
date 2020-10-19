// Copyright (C) TwoPieces 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/GameModeBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetSystemLibrary.h"			//ComponentOverlapComponents
#include "Engine/EngineTypes.h"					//EObjectTypeQuery
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "SpawnQueue.h"
#include "CustomPlayerStart.generated.h"

/*
* Custom Player Start Point Actor.
*/
UCLASS()
class SPAWNQUEUE_API ACustomPlayerStart : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACustomPlayerStart();
	
	/**
	* Check if spawn point matches to search parameters.
	*
	* @param	SpawnTarget		Spawn Target info.
	* @return					
	*/
	UFUNCTION(BlueprintCallable)
	bool Validate(FSpawnQueue SpawnTarget);
	
	/**
	* Check if spawn point is blocked.
	*
	* @return		Returns true if spawn point is blocked.
	*/
	UFUNCTION(BlueprintCallable)
	bool IsBlocked();

	/**
	* Sets this spawn point availible.
	*/
	UFUNCTION(BlueprintCallable)
	void Availible();
	
	/**
	* Sets this spawn status as occupied.
	*/
	UFUNCTION(BlueprintCallable)
	void Occupied();

	/**
	* Sets this spawn point into hibernation mode.
	*/
	UFUNCTION(BlueprintCallable)
	void Hibernation();
	
	/**
	* Start visualization error. This will signal if actor is colliding with landscape or static meshes.
	*/
	UFUNCTION(BlueprintCallable)
	void Error();

	/**
	* End visualization error.
	*/
	UFUNCTION(BlueprintCallable)
	void ErrorEnd();

	/**
	* Overlap events for Capsule and Cube
	*/
	UFUNCTION()
	virtual void Capsule_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	virtual void Capsule_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, FHitResult const& SweepResult);
	
	UFUNCTION()
	virtual void Cube_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	virtual void Cube_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, FHitResult const& SweepResult);

	/*
	 Components
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default", OverrideNativeName = "Capsule"))
	UCapsuleComponent* Capsule;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default", OverrideNativeName = "Arrow"))
	UArrowComponent* Arrow;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default", OverrideNativeName = "DefaultSceneRoot"))
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Category = "Default", OverrideNativeName = "Cube"))
	UStaticMeshComponent* Cube;

	/*
	 Variables
	*/

	//Usage status of this spawn point.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Status", Category = "Settings", Multiline = "false"))
	EOccupiedStatus Status;

	//For which Team this spawn point belongs to.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Team", Category = "Settings"))
	ETeams_SQ Team;
	
	//Spawn Group.
	//Assign respawn ponts, server join points and many more.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Spawn Class", Category = "Settings", Multiline = "true"))
	ESpawnClass SpawnClass;

	//Make this spawn point visible in game.
	//Visualize spawn point features.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Visualization", Category = "Settings", Multiline = "true"))
	bool Visualization;

	//If spawn is occupied by an Actor how long this Actor can occupy it until spawn point is forced to be availible.
	//In Seconds.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Un Occupy Force Delay", Category = "Settings", Multiline="true"))
	float UnOccupyForceDelay;

	//Hide visualization automatically after some time.
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "Hibernating", Category = "Settings"))
	bool Hibernating;

	//How long until spawn point hides.
	//Only if visualization is enabled.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Hibernation Delay", Category = "Settings", Multiline = "true"))
	float HibernationDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Material None", Category = "Visualization Materials"))
	UMaterialInterface* Material_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Material Green", Category = "Visualization Materials"))
	UMaterialInterface* Material_Green;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Material Red", Category = "Visualization Materials"))
	UMaterialInterface* Material_Red;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Material Yellow", Category = "Visualization Materials"))
	UMaterialInterface* Material_Yellow;

	UPROPERTY(ReplicatedUsing = "OnRepVisualizationMaterial", BlueprintReadOnly, meta = (DisplayName = "Visualization Material", Category = "Visualization Materials"))
	UMaterialInterface* VisualizationMaterial;

	UFUNCTION()
	virtual void OnRepVisualizationMaterial();

protected:

	//Replication
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void BeginPlay() override;

	void StartTimers();

	/**
	* Private Material functions.
	*/
	void SetMaterialGreen();

	void SetMaterialNone();

	void SetMaterialRed();

	void SetMaterialYellow();

	void CubeSetMaterial(UMaterialInterface* Material);


	bool IAmReady;

	//Location in world of this actor
	FTransform Location;

	//Player that is blocking this spawn point
	FSpawnPointOccupier Occupier;

public:	

	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform);

	void UserConstructionScript();

	void ForceInitCollision();

};