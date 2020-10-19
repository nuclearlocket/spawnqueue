// Copyright (C) TwoPieces 2020. All Rights Reserved.

/*
* Simple Location Actor with an Arrow.
* Requires EGameLocationNames_SQ enum from SpawnQueue.h
*/

#pragma once

#include "CoreMinimal.h"
#include "SpawnQueue.h"
#include "Components/ArrowComponent.h"
class UArrowComponent;
class USceneComponent;
#include "SimpleLoc.generated.h"

UCLASS()
class ASimpleLoc : public AActor
{
	GENERATED_BODY()
	
public:	
	ASimpleLoc();

	UPROPERTY(BlueprintReadWrite, NonTransactional, meta = (Category = "Default", OverrideNativeName = "Face"))
	UArrowComponent* face;

	UPROPERTY(BlueprintReadWrite, NonTransactional, meta = (Category = "Default", OverrideNativeName = "DefaultSceneRoot"))
	USceneComponent* DefaultSceneRoot;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (DisplayName = "Location", Category = "Default", OverrideNativeName = "Location"))
	FTransform Location;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Set Location", Category = "Settings", OverrideNativeName = "Set Location"))
	ESpawnClass SetLocation;

	/**
	* Validate if this Actor matches for Spawn Place.
	*
	* @param	Place				Spawn Place.
	* @param	TargetLocation		Location of this Actor.
	*/
	UFUNCTION(BlueprintCallable, Category = "SimpleLoc")
	void validate(ESpawnClass Place, FTransform& TargetLocation, bool& Match);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform);

	virtual void PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph) override;
};
