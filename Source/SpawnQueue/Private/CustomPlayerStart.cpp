// Copyright (C) TwoPieces 2020. All Rights Reserved.


#include "CustomPlayerStart.h"
#include "SpawnQueueBPLibrary.h"
#include "ServerPlayers.h"

// Sets default values
ACustomPlayerStart::ACustomPlayerStart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Enable replication for this class
	bReplicates = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;
	DefaultSceneRoot->CreationMethod = EComponentCreationMethod::Native;

	/****************** Capsule ******************/
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(DefaultSceneRoot);
	Capsule->InitCapsuleSize(30.f, 90.f);
	Capsule->SetCollisionProfileName(FName(TEXT("Trigger")));

	Capsule->OnComponentBeginOverlap.AddDynamic(this, &ACustomPlayerStart::Capsule_OnOverlapBegin);
	Capsule->OnComponentEndOverlap.AddDynamic(this, &ACustomPlayerStart::Capsule_OnOverlapEnd);

	Capsule->SetVisibility(false);

	/****************** Cube ******************/
	Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	Cube->AttachToComponent(DefaultSceneRoot, FAttachmentTransformRules::KeepRelativeTransform);

	//Set Static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMesh* Asset = MeshAsset.Object;
	Cube->SetStaticMesh(Asset);

	Cube->SetCollisionProfileName(FName(TEXT("Trigger")));

	Cube->OnComponentBeginOverlap.AddDynamic(this, &ACustomPlayerStart::Cube_OnOverlapBegin);
	Cube->OnComponentEndOverlap.AddDynamic(this, &ACustomPlayerStart::Cube_OnOverlapEnd);

	//Cube->SetMaterial(0, MatAsset);
	Cube->CastShadow = false;
	Cube->SetRelativeScale3D(FVector(0.600000, 0.600000, 1.800000));
	Cube->SetCollisionProfileName(TEXT("Trigger"));

	/****************** Arrow ******************/
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->AttachToComponent(DefaultSceneRoot, FAttachmentTransformRules::KeepRelativeTransform);

	/****************** Variables ******************/

	Status = EOccupiedStatus::Availible;
	Team = ETeams_SQ::Team_A;
	Location = FTransform();
	Visualization = true;
	//VisualizationMaterial = nullptr;
	Hibernating = true;
	UnOccupyForceDelay = 10.f;
	HibernationDelay = 30.f;
	IAmReady = false;
	SpawnClass = ESpawnClass::JoinSpawn;

	Occupier.Actor = nullptr;
	Occupier.Controller = nullptr;
	Occupier.OtherComp = nullptr;
	Occupier.OverlappedComponent = nullptr;

	/****************** Materials ******************/

	Material_None = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material_None"));
	Material_Green = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material_Green"));
	Material_Red = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material_Red"));
	Material_Yellow = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material_Yellow"));
}

//Replication
void ACustomPlayerStart::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACustomPlayerStart, VisualizationMaterial);
}


// Called when the game starts or when spawned
void ACustomPlayerStart::BeginPlay()
{
	Super::BeginPlay();
	
	Arrow->SetHiddenInGame(true);

	StartTimers();

	IAmReady = true;

	Hibernation();

	Location = Arrow->GetComponentToWorld();
}


// Called every frame
void ACustomPlayerStart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACustomPlayerStart::OnConstruction(const FTransform& Transform) 
{
	bool ComponentOverlapResult = false;
	bool OverlapResultTrusted = false;

	if (::IsValid(DefaultSceneRoot))
	{
		Location = Arrow->GetComponentToWorld();
	}


	if (!Visualization)
	{
		CubeSetMaterial(Material_None);
	}
	else
	{

		TArray<UPrimitiveComponent*> OutComponents;
		OutComponents.Empty();

		//UClass* ComponentClassFilter = ((UClass*)nullptr);
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Empty();

		TArray < TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
		ObjectTypes.Empty();

		ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1); //WordStatic
		ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery2); //WorldDynamic
		//Main goal is only to check static meshes and landscape, Capsule_OnOverlapBegin will signal about moving objects
		//ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery4); //PhysicsBody

		if (::IsValid(Cube) && ::IsValid(Capsule))
		{
			ComponentOverlapResult = UKismetSystemLibrary::ComponentOverlapComponents(Cube, Cube->GetComponentToWorld(), ObjectTypes, ((UClass*)nullptr), ActorsToIgnore, OutComponents);

			//ComponentOverlapComponents is colliding with Capsule so we can't use ComponentOverlapResult boolean to check the collision
			for (auto& Item : OutComponents)
			{
				if (Item != Capsule)
				{
					OverlapResultTrusted = true;
				}
			}

			//Actor is overlapping with landscape or static meshes
			if (OverlapResultTrusted)
			{
				CubeSetMaterial(Material_Yellow);
			}
			else
			{
				CubeSetMaterial(Material_Green);
			}
		}
	}

}

void ACustomPlayerStart::UserConstructionScript()
{

}



//Replication
void ACustomPlayerStart::OnRepVisualizationMaterial()
{
	if (::IsValid(Cube))
	{
		Cube->SetMaterial(0, VisualizationMaterial);
	}
}


/*
 Use these when replication is needed.
*/
void ACustomPlayerStart::SetMaterialGreen() {
	VisualizationMaterial = Material_Green;
	OnRepVisualizationMaterial();
}

void ACustomPlayerStart::SetMaterialNone() {
	VisualizationMaterial = Material_None;
	OnRepVisualizationMaterial();
}

void ACustomPlayerStart::SetMaterialRed() {
	VisualizationMaterial = Material_Red;
	OnRepVisualizationMaterial();
}

void ACustomPlayerStart::SetMaterialYellow() {
	VisualizationMaterial = Material_Yellow;
	OnRepVisualizationMaterial();
}


void ACustomPlayerStart::StartTimers() 
{
	FTimerHandle FInitCollTimer;
	FTimerHandle WorkerTimer;

	GetWorld()->GetTimerManager().SetTimer(FInitCollTimer, this, &ACustomPlayerStart::ForceInitCollision, UnOccupyForceDelay, true);
}

void ACustomPlayerStart::ForceInitCollision()
{
	//if spawn is occupying this place
	if (Occupier.Actor != nullptr)
	{
		//On Component Begin Overlap Capsule
		FTimerHandle DelayTimer_c;

		GetWorld()->GetTimerManager().SetTimer(DelayTimer_c, this, &ACustomPlayerStart::Availible, UnOccupyForceDelay, false);
	}
}


void ACustomPlayerStart::Availible() 
{
	if (HasAuthority() && Visualization)
	{
		SetMaterialGreen();

		Status = EOccupiedStatus::Availible;

		Occupier.Actor = nullptr;
		Occupier.Controller = nullptr;
		Occupier.OtherComp = nullptr;
		Occupier.OverlappedComponent = nullptr;

		Hibernating = false;
	}
}

void ACustomPlayerStart::Occupied() 
{
	if (HasAuthority() && Visualization)
	{
		SetMaterialRed();
		
		Status = EOccupiedStatus::Occupied;
		Hibernating = false;
	}
}

void ACustomPlayerStart::Hibernation() 
{
	if (HasAuthority() && Visualization)
	{
		SetMaterialNone();
		Hibernating = true;
	}
}

void ACustomPlayerStart::Error() 
{
	if (HasAuthority() && Visualization && !Hibernating && !IsBlocked())
	{
		SetMaterialYellow();
	}
}

void ACustomPlayerStart::ErrorEnd() 
{
	if (HasAuthority() && Visualization && !Hibernating && !IsBlocked())
	{
		SetMaterialGreen();
	}
}


void ACustomPlayerStart::Capsule_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Availible();

	//Delay
	FTimerHandle DelayTimer;

	GetWorld()->GetTimerManager().SetTimer(DelayTimer, this, &ACustomPlayerStart::Hibernation, HibernationDelay, false);
}


void ACustomPlayerStart::Capsule_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, FHitResult const& SweepResult)
{
	
	if (::IsValid(OtherActor) && HasAuthority())
	{
		AController* tmpController = nullptr;
		
		if ((OtherActor != this))
		{
			APawn* tempPawn = Cast<APawn>(OtherActor);

			if (::IsValid(tempPawn) && !tempPawn->IsPendingKill())
			{
				//get controller of overlapping actor
				tmpController = tempPawn->GetController();

				Occupier.Actor = OtherActor;
				Occupier.Controller = tmpController;
				Occupier.OtherComp = OtherComp;
				Occupier.OverlappedComponent = OverlappedComponent;

				Occupied();
			}
		}
	}
}


void ACustomPlayerStart::Cube_OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//ErrorEnd(); Moved to construction script
}
void ACustomPlayerStart::Cube_OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, FHitResult const& SweepResult)
{
	//Error(); Moved to construction script
}

/*
 Set material for Cube without replication.
*/
void ACustomPlayerStart::CubeSetMaterial (UMaterialInterface* Material) 
{
	if (::IsValid(Material) && ::IsValid(Cube))
	{
		Cube->SetMaterial(0, Material);
	}
}


bool ACustomPlayerStart::Validate(FSpawnQueue SpawnTarget)
{
	FPlayerInfo_SQ Player;

	UServerPlayers::FindByID(SpawnTarget.PlayerID, Player);

	if (Player.PlayerID.IsEmpty())
	{
		return false;
	}

	if ((SpawnTarget.SpawnGroup == SpawnClass) && (Player.Team == Team))
	{
		return true;
	}

	return false;
}

bool ACustomPlayerStart::IsBlocked()
{
	if (Occupier.Actor != nullptr || Occupier.Controller != nullptr || Occupier.OtherComp != nullptr || Occupier.OverlappedComponent != nullptr)
	{
		return true;
	}

	return false;
}