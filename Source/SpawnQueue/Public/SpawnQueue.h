// Copyright (C) TwoPieces 2020. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "GeneratedCodeHelpers.h"
#include "UnixTimeLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpawnQueue.generated.h"

class FSpawnQueueModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

/**
* Spawn Class types.
*/
UENUM(BlueprintType)
enum class ESpawnClass : uint8
{
	JoinSpawn UMETA(DisplayName = "Join Spawn"),
	RespawnSpawn UMETA(DisplayName = "Respawn Spawn"),
	Vehicle UMETA(DisplayName = "Vehicle"),
	VehicleSpecial UMETA(DisplayName = "Vehicle Special"),
	Special_A UMETA(DisplayName = "Special A"),
	Special_B UMETA(DisplayName = "Special B"),
	Special_C UMETA(DisplayName = "Special C"),
	Death UMETA(DisplayName = "Death"),
	Lobby UMETA(DisplayName = "Lobby"),
};

/**
* Spawn Branches for Spawn Run function.
*/
UENUM(BlueprintType)
enum class ESpawnBranch : uint8
{
	SpawnNow UMETA(DisplayName = "Spawn Player"),
	NoSpawn UMETA(DisplayName = "No Spawn")
};

/**
* Branches for ReservedSlotsLoginAuthorize.
*/
UENUM(BlueprintType)
enum class EReservedAuthorityBranch : uint8
{
	Authority UMETA(DisplayName = "Authority"),
	NoAuthority UMETA(DisplayName = "No Authority")
};

/**
* 
*/
UENUM(BlueprintType)
enum class ESpawnQueueAddStatus : uint8
{
	Success UMETA(DisplayName = "Succes"),
	UnknownFailure UMETA(DisplayName = "Unknown Failure"),
	JoinQueueIsFull UMETA(DisplayName = "Join Queue Is Full"),
	NoAuthority UMETA(DisplayName = "No Authority")
};

/**
* Spawn Point Item holder (CustomPlayerStart Spawn function).
*/
USTRUCT(BlueprintType)
struct FSpawnPointItems {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		APlayerController* Controller;
	UPROPERTY(BlueprintReadWrite)
		FString PlayerID;
	UPROPERTY(BlueprintReadWrite)
		UClass* Class;
	UPROPERTY(BlueprintReadWrite)
		ESpawnClass SpawnGroup;
};

/**
* Spawn Point Occupier.
*/
USTRUCT(BlueprintType)
struct FSpawnPointOccupier {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	AController* Controller;
	UPROPERTY(BlueprintReadWrite)
	AActor* Actor;
	UPROPERTY(BlueprintReadWrite)
	UPrimitiveComponent* OverlappedComponent;
	UPROPERTY(BlueprintReadWrite)
	UPrimitiveComponent* OtherComp;
};

/**
* Spawn Point Occupied status.
*/
UENUM(BlueprintType)
enum class EOccupiedStatus : uint8
{
	Availible UMETA(DisplayName = "Availible"),
	Occupied UMETA(DisplayName = "Occupied")
};

/**
* Teams.
*/
UENUM(BlueprintType)
enum class ETeams_SQ : uint8
{
	Team_A UMETA(DisplayName = "Team A"),
	Team_B UMETA(DisplayName = "Team B"),
	Team_C UMETA(DisplayName = "Team C"),
	Team_D UMETA(DisplayName = "Team D"),
	Team_E UMETA(DisplayName = "Team E"),
	Team_F UMETA(DisplayName = "Team F"),
	Team_G UMETA(DisplayName = "Team G")
};

/**
* Spawn Queue Ranks.
*/
UENUM(BlueprintType)
enum class EQueuePrivileges : uint8
{
	//Highest rank 0
	Admin UMETA(DisplayName = "Admin"),
	//Rank 1
	VIP UMETA(DisplayName = "VIP"),
	//Rank 2
	Special_Yellow UMETA(DisplayName = "Special Yellow"),
	//Rank 3
	Special_Purple UMETA(DisplayName = "Special Purple"),
	//Lowest rank 4
	Regular UMETA(DisplayName = "Regular")
};

/**
* Main Spawn Queue holder.
*/
USTRUCT(BlueprintType)
struct FSpawnQueue {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	APlayerController* Controller;
	UPROPERTY(BlueprintReadWrite)
	FString PlayerID;
	UPROPERTY(BlueprintReadWrite)
	UClass* Class;
	UPROPERTY(BlueprintReadWrite)
	FDateTime Time;
	UPROPERTY(BlueprintReadWrite)
	EQueuePrivileges AccessLevel;
	UPROPERTY(BlueprintReadWrite)
	float AdditionalDelay;
	UPROPERTY(BlueprintReadWrite)
	ESpawnClass SpawnGroup;
};

/**
* Number tracking holder for Player Controller numbers.
*/
USTRUCT(BlueprintType)
struct FPlayerNumber {
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		int32 PlayerNumber;
	UPROPERTY(EditDefaultsOnly)
		FString PlayerID;
};

/**
* New Rank based Queue (meant to replace current one and make it more accurate).
*/
USTRUCT(BlueprintType)
struct FSpawnQPrivilegeQueue {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FSpawnQueue> admin;
	UPROPERTY(BlueprintReadWrite)
		TArray<FSpawnQueue> vip;
	UPROPERTY(BlueprintReadWrite)
		TArray<FSpawnQueue> regular;
};

/**
* 
*/
USTRUCT(BlueprintType)
struct FServerSettings {
	GENERATED_BODY()
public:
	//List of privileges that grant access to reserved slots. Use Make Array.
	UPROPERTY(BlueprintReadWrite)
		TArray<EQueuePrivileges> PrivilegesForSlots;
	//Server Max players.
	UPROPERTY(BlueprintReadWrite)
		int32 ServerMaxPlayers;
	//Slots from max players.
	UPROPERTY(BlueprintReadWrite)
		int32 ReservedSlots;
	//Server join queue size
	UPROPERTY(BlueprintReadWrite)
		int32 ServerJoinQueueSize;
	//If server is full admins can join.
	UPROPERTY(BlueprintReadWrite)
		bool AllowAdminsIfServerFull;
	//Don't count admins in join queue
	UPROPERTY(BlueprintReadWrite)
		bool DontCountAdminsInJoinQueue;
	//If join queue is full these can be kicked, even if they are in PrivilegesForSlots. If this is empty everything that is not in PrivilegesForSlots will be kicked.
	UPROPERTY(BlueprintReadWrite)
		TArray<EQueuePrivileges> Kickable;
};