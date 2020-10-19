// Copyright (C) TwoPieces 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "CustomPlayerStart.h"
#include "SimpleLoc.h"
#include "TimerManager.h"
#include "PlayerInfo.h"
#include "Kismet/KismetNodeHelperLibrary.h"
#include "GameFramework/PlayerController.h"
#include "SpawnQueue.h"
#include "SpawnQueueBPLibrary.generated.h"


UCLASS()
class USpawnQueueBPLibrary : public UBlueprintFunctionLibrary
{
	
	GENERATED_UCLASS_BODY()

public:

	//USpawnQueueBPLibrary(const FObjectInitializer& ObjectInitializer);

	/**
	* Add Player to time based queue.
	*
	* @param	Controller			Player Controller.
	* @param	PlayerID			Player id.
	* @param	Class				Class to be spawned (must be a Pawn or Character).
	* @param	SpawnGroup			Spawn Group.
	* @param	AccessLevel			Access class.
	* @param	AdditionalDelay		Additional Delay.
	* @return						Returns true if add was a success.			
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spawn Queue Add", Keywords = "spawn queue list add player"), Category = "SpawnQueue")
	static bool SpawnListAdd(APlayerController* Controller, FString PlayerID, UClass* Class, ESpawnClass SpawnGroup = ESpawnClass::Lobby, EQueuePrivileges AccessLevel = EQueuePrivileges::Regular, float AdditionalDelay = 0.0);
	
	/**
	* Get whole list of Spawn Queue. Meant for widget things.
	*
	* @param	SpawnQueue		Spawn Queue.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spawn Queue Give All", Keywords = "spawn queue give all players from queue"), Category = "SpawnQueue")
	static void GiveAllQueue(TArray<FSpawnQueue>& SpawnQueue);
	
	/**
	* Easily Spawn Actor in one of Spawn Points of CustomPlayerStart. Pawns and Characters will be Possessed. Note: you must rotate Spawned Pawn or Character from Client Player Controller if you wan't the rotation to match with Spawn Point.
	*
	* @param	SpawnQueueInfo					Insert spawn queue structure.
	* @param	AutoRemoveFromSpawnQueue		If this is true and spawn is a success Spawn Queue record will be automatically removed (using RemovePlayerByID).
	* @param	Reroll							If this is true Character cannot be spawned now.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spawn Queue EZ Spawn", Keywords = "spawn queue easy spawn ez", WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"), Category = "SpawnQueue")
	static void EZSpawn(UObject* WorldContextObject, FSpawnQueue SpawnQueueInfo, bool AutoRemoveFromSpawnQueue, bool& Reroll);
	
	/**
	* Remove player from spawn queue.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spawn Queue Remove Player By Controller", Keywords = "spawn queue list remove player delete controller"), Category = "SpawnQueue")
	static bool RemovePlayerByController(APlayerController* Controller);
	
	/**
	* Remove player from spawn queue.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spawn Queue Remove Player By ID", Keywords = "spawn queue list remove player delete id"), Category = "SpawnQueue")
	static bool RemovePlayerByID(FString PlayerID = FString(TEXT("")));

	/**
	* Check if player is in spawn queue
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is In Spawn Queue", Keywords = "spawn queue list is in"), Category = "SpawnQueue")
	static void IsInSpawnQueue(APlayerController* Controller, FString PlayerID, bool& IsInQueue, FTimespan& SpawnTimeLeft, int& NumberInQueue);
	
	/**
	* Get total queue time length.
	*
	* @param	Length		Queue length in Time Spawn.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Queue Time Length", Keywords = "spawn queue length time"), Category = "SpawnQueue")
	static void QueueTimeLength(FTimespan& Length);
	
	/**
	* Get player count of queue.
	*
	* @param	PlayersLeft		Count of Players in Queue.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Queue Player Count", Keywords = "spawn queue player count"), Category = "SpawnQueue")
	static void QueuePlayerCount(int& PlayersLeft);

	/**
	* Find Player by Player Controller.
	*
	* @param	Controller		Player Controller.
	* @param	Result			Found Player from spawn queue.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spawn Queue Find by Controller", Keywords = "spawn queue player find controller"), Category = "SpawnQueue")
	static void FindByController(APlayerController* Controller, FSpawnQueue& Result);
	
	/**
	* Find Player by Player id.
	*
	* @param	PlayerID		Player id.
	* @param	Result			Found Player from spawn queue.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spawn Queue Find by Player ID", Keywords = "spawn queue player find id"), Category = "SpawnQueue")
	static void FindByID(FString PlayerID, FSpawnQueue& Result);
	
	/**
	* Main Player Spawn function. Add this to looping timer to get realtime Spawn Targets.
	*
	* @param	List	List of Spawn Targets that are ready to spawn.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spawn Run", Keywords = "spawn queue spawn run call loop", ExpandEnumAsExecs = "Branches"), Category = "SpawnQueue")
	static void SpawnRun(TArray<FSpawnQueue>& List, ESpawnBranch& Branches);
	
	/**
	* Generate Random String.
	*
	* @param	Length		Length of string.
	* @param	Value		Returns generated string.
	*/
	UFUNCTION(BlueprintPure, Category = "Random Helpers")
	static void RandomStr(int32 Length, FString& Value);

	/**
	* Delete all records from spawn queue and join queue.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Wipe", Keywords = "spawn queue wipe clean empty delete"), Category = "SpawnQueue")
	static void Wipe();

	/**
	* Add Player to Server Join Queue. Player must be have controller on server. To authorize player simply just register player with UServerPlayers::AddNewPlayer().
	* If you wan't to pass admins through full join queue, register players before adding them to join queue. Full queue won't accept anyone else then from FServerSettings::PrivilegesForSlots list.
	*
	* @param		PlayerController	Player Controller to be added.
	* @param		Status				Status of add.
	* @param		Kick				Kick suggestion. If player should be kicked this will be true.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add To Join Queue", Keywords = "spawn queue join add"), Category = "JoinQueue")
	static void AddToJoinQueue(APlayerController* PlayerController, ESpawnQueueAddStatus& Status, bool& Kick);

	/**
	* Check if server join queue is full.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Is Join Queue Full", Keywords = "spawn queue join is full"), Category = "JoinQueue")
	static bool IsJoinQueueFull();

	/**
	* Check if player controller is in join queue.
	*
	* @param	PlayerController		Player Controller to be added.
	* @param	NumberInQueue			Number in queue.
	* @return							True if player is in queue.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is In Join Queue", Keywords = "spawn queue join is in"), Category = "JoinQueue")
	static bool IsInJoinQueue(APlayerController* PlayerController, int32& NumberInQueue);

	/**
	* Get count of players in join queue.
	*
	* @return	Count of players in queue.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Join Queue Player Count", Keywords = "spawn queue join player count"), Category = "JoinQueue")
	static int32 JoinQueuePlayerCount();

	/**
	* Remove Player from Server Join Queue.
	*
	* @param	PlayerController		Player Controller to be added.
	* @return							True if removed succesfully.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove From Join Queue", Keywords = "spawn queue join remove"), Category = "JoinQueue")
	static bool RemoveFromJoinQueue(APlayerController* PlayerController);

	/**
	* Add this to looping timer to get players that are ready to enter the world.
	*
	* @param	AutoRemove		Automatically remove player from join queue once it's been entered in Players list.
	* @param	Players			List of players that are ready to join the server.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Join Queue Run", Keywords = "spawn queue join run", ExpandEnumAsExecs = "Branches"), Category = "JoinQueue")
	static void JoinQueueRun(bool AutoRemove, TArray<APlayerController*>& Players, ESpawnBranch& Branches);

	static TArray<APlayerController*> JoinQueueSort();

	static bool FindFromJoinQueue(APlayerController* PlayerController, int32& Index);


protected:

	static TArray<APlayerController*> ServerJoinQueue;

	static TArray<FSpawnQueue> spawnlist;
	
	static void Find(APlayerController* Controller, FString PlayerID, FSpawnQueue& Result);

	static TArray<FSpawnQueue> SortByDate(TArray<FSpawnQueue> List);

	static bool IsEmpty(FSpawnQueue PlayerQueueInfo);

	static bool RemovePlayer(APlayerController* Controller = nullptr, FString PlayerID = FString(TEXT("")));

	static bool InitTT;

	static FSpawnQueue emptyInitReadyPI;

};