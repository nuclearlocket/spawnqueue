// Copyright (C) TwoPieces 2020. All Rights Reserved.

#include "SimpleLoc.h"

// Sets default values
ASimpleLoc::ASimpleLoc()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	face = CreateDefaultSubobject<UArrowComponent>(TEXT("Face"));
	RootComponent = DefaultSceneRoot;

	face->AttachToComponent(DefaultSceneRoot, FAttachmentTransformRules::KeepRelativeTransform);

	face->bHiddenInGame = false;
	Location = FTransform(FQuat(0.000000, 0.000000, 0.000000, 1.000000), FVector(0.000000, 0.000000, 0.000000), FVector(1.000000, 1.000000, 1.000000));
	SetLocation = ESpawnClass::Lobby;
}



PRAGMA_ENABLE_OPTIMIZATION
void ASimpleLoc::PostLoadSubobjects(FObjectInstancingGraph* OuterInstanceGraph)
{
	Super::PostLoadSubobjects(OuterInstanceGraph);

	if (DefaultSceneRoot)
	{
		DefaultSceneRoot->CreationMethod = EComponentCreationMethod::Native;
	}

	if (face)
	{
		face->CreationMethod = EComponentCreationMethod::Native;
	}
}

// Called when the game starts or when spawned
void ASimpleLoc::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(face))
	{
		face->USceneComponent::SetHiddenInGame(true, false);
	}
}

// Called every frame
void ASimpleLoc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASimpleLoc::OnConstruction(const FTransform& Transform)
{
	FTransform ReturnValue = FTransform();

	if (IsValid(DefaultSceneRoot))
	{
		ReturnValue = DefaultSceneRoot->GetComponentTransform();
	}

	Location = ReturnValue;
}

void ASimpleLoc::validate(ESpawnClass Place, FTransform& TargetLocation, bool& Match)
{
	TargetLocation = FTransform();
	Match = false;

	if (SetLocation == Place)
	{
		TargetLocation = Location;
		Match = true;
	}
}