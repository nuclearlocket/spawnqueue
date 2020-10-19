// Copyright (C) TwoPieces 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/NetConnection.h"
#include "PlayerInfo.h"
#include "TimerManager.h"
#include "SpawnQueueBPLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ServerPlayers.generated.h"

/**
 * 
 */
UCLASS()
class UServerPlayers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

   UServerPlayers(const FObjectInitializer& ObjectInitializer);

   /**
   * Register a new player info on the server.
   * 
   * @param     ServerController    Player Controller on server (from cast).
   * @param     PlayerName          Public Player Name in the game.
   * @param     PlayerIP            Player Ip address.
   * @param     Team                Player Team.
   * @param     ServerCharacter     Character Pawn on server.
   * @param     SetControllerID     Set Player number ID automatically on controller (SetPlayerControllerID Function). Otherwise it will be -1 for everyone, unless set manually.
   */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add New Player", Keywords = "spawn queue player add new"), Category = "SpawnQueue Players")
    static FPlayerInfo_SQ AddNewPlayer(APlayerController* ServerController = nullptr, FString PlayerName = FString(TEXT("")), FString PlayerIP = FString(TEXT("")), ETeams_SQ Team = ETeams_SQ::Team_A, AActor* ServerCharacter = nullptr, EQueuePrivileges Privileges = EQueuePrivileges::Regular, bool SetControllerID = true);
    /**
    * Settings for join slots and other. Add this to Begin Play in Game Mode.
    *
    * @param	Setting			Server Settings.
    */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Join Slots Settings", Keywords = "player join slots settings"), Category = "SpawnQueue")
    static void SlotsSettings(FServerSettings Setting);

    /**
    * Remove Player info using controller.
    *
    * @param    Controller          Player Controller (from cast).
    * @return                       Returns true if remove was a success.
    */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove Player by Controller", Keywords = "spawn queue player find controller"), Category = "SpawnQueue Players")
    static bool RemovePlayerByController(APlayerController* Controller);
   
    /**
    * Remove Player info using player id.
    *
    * @param    PlayerID          Player ID.
    * @return                     Returns true if remove was a success.
    */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove Player By ID", Keywords = "spawn queue player remove id"), Category = "SpawnQueue Players")
    static bool RemovePlayerByID(FString PlayerID);
    
    /**
    * Find player info using player id.
    *
    * @param    PlayerID          Player ID.
    * @param    PlayerInfo        Player info structure.
    */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Find Player by ID", Keywords = "spawn queue player find id"), Category = "SpawnQueue Players")
    static void FindByID(FString PlayerID, FPlayerInfo_SQ& PlayerInfo);
    
    /**
    * Get count of all registered players.
    *
    * @param    PlayerCount         Player Count.
    * @param    ExcludeJoinQueue    Negate server join queue players from count.
    */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "Player Count", Keywords = "spawn queue player count"), Category = "SpawnQueue Players")
    static void PlayerCount(int32& PlayerCount, bool ExcludeJoinQueue = true);

    /**
    * Get count of all registered admins.
    *
    * @param    AdminCount         Admin Count.
    */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "Admin Count", Keywords = "spawn queue admin count"), Category = "SpawnQueue Players")
    static void AdminCount(int32& AdminCount);
    
    /**
    * Find player info using Player Controller.
    *
    * @param    Controller      Player Controller.
    * @param    PlayerInfo      Player info structure.
    */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Find Player by Controller", Keywords = "spawn queue player find controller"), Category = "SpawnQueue Players")
    static void FindByController(APlayerController* Controller, FPlayerInfo_SQ& PlayerInfo);
   
    /**
    * Update player info.
    *
    * @param    PlayerID                Player ID of player to be edited.
    * @param    UpdateList              Structure of update list.     
    * @param    Updated                 Structure of updated player info.
    */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Player Info", Keywords = "spawn queue player update"), Category = "SpawnQueue Players")
    static void UpdatePlayer(FString PlayerID, FUpdatePlayerInfo UpdateList, FPlayerInfo_SQ& Updated);
   
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Player IP Address", Keywords = "spawn queue player connection info"), Category = "SpawnQueue Players")
    static void GetPlayerIPAddress(APlayerController* PlayerController, FString& IPAddress);

    /*
    * Force kick player from server.
    *
    * @param    PlayerController        Player Controller.
    * @param    RemovePlayerRecord      Remove player info.
    * @param    KickMessage             Kick message.
    */
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Kick", Keywords = "spawn queue player kick", WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"), Category = "SpawnQueue Players")
    static void Kick(UObject* WorldContextObject, APlayerController* PlayerController, bool RemovePlayerRecord = true, FString KickMessage = FString(TEXT("Kick.")));
    
    static FTimerHandle DelayedKickTimer;

    static FServerSettings ServerSettings;
protected:

    static bool RemovePlayer(APlayerController* Controller = nullptr, FString PlayerID = FString(TEXT("")));

    static FPlayerInfo_SQ Find(APlayerController* Controller = nullptr, FString PlayerID = FString(TEXT("")));

    static TArray<FPlayerInfo_SQ> Players;

    static TArray<FPlayerNumber> PlayerNumbers;

    static int32 NextPlayerNum();

    static bool CheckIfPlayerNumIsAvailible(int32 Number);

    static int32 SolveNextPlayerNumber();

    static FPlayerInfo_SQ emptyInitReadyPI;
};