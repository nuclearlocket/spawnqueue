// Copyright (C) TwoPieces 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpawnQueue.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "PlayerInfo.generated.h"

/**
 * Player Info Spawn Queue
 *
 * @param	PlayerID			Player id, 32 characters long.
 * @param	PlayerName			Public Player Name on server.
 * @param	ServerController	Player Controller on server (from cast).
 * @param	PlayerNumber		Player Controller identification number.
 * @param	SpawnLocation		Pawn spawn location.
 * @param	JoinTime			Date Time of when player joined the server.
 * @param	PlayerIP			Player ip address.
 * @param	Team				Team of player.
 * @param	ServerCharacter		Player Pawn on server.
 * @param	Respawn				Respawn identification for automated short coded respawn feature.
 */
USTRUCT(BlueprintType)
struct FPlayerInfo_SQ {
	GENERATED_BODY()
public:
	//Player id, 32 characters long.
	UPROPERTY(BlueprintReadWrite)
	FString PlayerID;

	//Public Player Name on server.
	UPROPERTY(BlueprintReadWrite)
	FString PlayerName;

	//Player Controller on server (from cast). You can also change APlayerController to AController.
	UPROPERTY(BlueprintReadWrite)
	APlayerController* ServerController;

	//Player Controller identification number.
	UPROPERTY(BlueprintReadWrite)
	int32 PlayerNumber;

	//Pawn spawn location.
	UPROPERTY(BlueprintReadWrite)
	FTransform SpawnLocation;

	//Date Time of when player joined the server.
	UPROPERTY(BlueprintReadWrite)
	FDateTime JoinTime;

	//Player ip address.
	UPROPERTY(BlueprintReadWrite)
	FString PlayerIP;

	//Team of player.
	UPROPERTY(BlueprintReadWrite)
	ETeams_SQ Team;

	//Privileges
	UPROPERTY(BlueprintReadWrite)
	EQueuePrivileges Privileges;

	//Player Pawn on server.
	UPROPERTY(BlueprintReadWrite)
	AActor* ServerCharacter;

	//Player is playing in the world.
	UPROPERTY(BlueprintReadWrite)
	bool IsInWorld;
};

/*
*
*/
USTRUCT(BlueprintType)
struct FUpdatePlayerInfo {
	GENERATED_BODY()
public:

	//Enable this to change Spawn Location.
	UPROPERTY(BlueprintReadWrite)
	bool UpdateSpawnLocation;

	//Spawn location or previous spawn location.
	UPROPERTY(BlueprintReadWrite)
	FTransform SpawnLocation;

	//Enable this to change Team.
	UPROPERTY(BlueprintReadWrite)
	bool UpdateTeam;

	//Team where player belongs.  
	UPROPERTY(BlueprintReadWrite)
	ETeams_SQ Team;

	//Enable this to change Character Pawn.
	UPROPERTY(BlueprintReadWrite)
	bool UpdateCharacter;

	//Player Pawn on Server.  
	UPROPERTY(BlueprintReadWrite)
	AActor* ServerCharacter;

	//Player is playing in the world and is counted as player.
	UPROPERTY(BlueprintReadWrite)
	bool UpdateIsInWorld;

	//IsInWorld.
	UPROPERTY(BlueprintReadWrite)
	bool IsInWorld;

	//Enable this to change Player Number.
	UPROPERTY(BlueprintReadWrite)
	bool UpdatePlayerNum;

	//Player Number. Will be set on Player Info and on Player Number tracker only if Player ID is found from tracker, if not found both will be skipped.  
	UPROPERTY(BlueprintReadWrite)
	int32 PlayerNumber;

	//
	UPROPERTY(BlueprintReadWrite)
	bool UpdatePrivileges;

	//Privileges.
	UPROPERTY(BlueprintReadWrite)
	EQueuePrivileges Privileges;
};
