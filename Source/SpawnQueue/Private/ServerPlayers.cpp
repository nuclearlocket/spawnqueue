// Copyright (C) TwoPieces 2020. All Rights Reserved.


#include "ServerPlayers.h"

TArray<FPlayerInfo_SQ> UServerPlayers::Players;
TArray<FPlayerNumber> UServerPlayers::PlayerNumbers;
FServerSettings UServerPlayers::ServerSettings;
FPlayerInfo_SQ UServerPlayers::emptyInitReadyPI;
FTimerHandle UServerPlayers::DelayedKickTimer;

UServerPlayers::UServerPlayers(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

	//empty player info initialized for fail searches
	UServerPlayers::emptyInitReadyPI.JoinTime = FDateTime::UtcNow();
	UServerPlayers::emptyInitReadyPI.PlayerID = FString(TEXT(""));
	UServerPlayers::emptyInitReadyPI.PlayerIP = FString(TEXT(""));
	UServerPlayers::emptyInitReadyPI.PlayerName = FString(TEXT(""));
	UServerPlayers::emptyInitReadyPI.PlayerNumber = 0;
	UServerPlayers::emptyInitReadyPI.IsInWorld = false;
	UServerPlayers::emptyInitReadyPI.ServerCharacter = nullptr;
	UServerPlayers::emptyInitReadyPI.ServerController = nullptr;
	UServerPlayers::emptyInitReadyPI.SpawnLocation = FTransform();
	UServerPlayers::emptyInitReadyPI.Team = ETeams_SQ::Team_A;
	UServerPlayers::emptyInitReadyPI.Privileges = EQueuePrivileges::Regular;

}


PRAGMA_ENABLE_OPTIMIZATION
FPlayerInfo_SQ UServerPlayers::AddNewPlayer(APlayerController* ServerController, FString PlayerName, FString PlayerIP, ETeams_SQ Team, AActor* ServerCharacter, EQueuePrivileges Privileges, bool SetControllerID)
{

	FPlayerInfo_SQ Player = UServerPlayers::emptyInitReadyPI;
	FPlayerInfo_SQ FindResult = UServerPlayers::emptyInitReadyPI;

	if (IsValid(ServerController))
	{
		FTransform Emptylocation = FTransform();
		FString RandomID;
		int32 playernumber = 0;
		USpawnQueueBPLibrary::RandomStr(32, RandomID);

		playernumber = UGameplayStatics::GetPlayerControllerID(ServerController);

		Player.PlayerID = RandomID;
		Player.JoinTime = FDateTime::UtcNow();
		if (!PlayerIP.IsEmpty()) { Player.PlayerIP = PlayerIP; }
		if (!PlayerName.IsEmpty()) { Player.PlayerName = PlayerName; }
		Player.Privileges = Privileges;
		
		if (playernumber != -1)
		{
			Player.PlayerNumber = playernumber;
		}
		else
		{
			Player.PlayerNumber = UServerPlayers::SolveNextPlayerNumber();

			if (SetControllerID)
			{
				UGameplayStatics::SetPlayerControllerID(ServerController, Player.PlayerNumber);
			}
			
			FPlayerNumber PlayerNumStruct;
			PlayerNumStruct.PlayerID = RandomID;
			PlayerNumStruct.PlayerNumber = Player.PlayerNumber;

			UServerPlayers::PlayerNumbers.Add(PlayerNumStruct);
		}

		Player.ServerController = ServerController;
		Player.Team = Team;
		if (ServerCharacter != nullptr) { Player.ServerCharacter = ServerCharacter; }

		int32 index;

		index = UServerPlayers::Players.Add(Player);

		UServerPlayers::FindByID(RandomID, FindResult);
	}

	return FindResult;
}

PRAGMA_ENABLE_OPTIMIZATION
void UServerPlayers::SlotsSettings(FServerSettings Settings)
{
	UServerPlayers::ServerSettings = Settings;
}

void UServerPlayers::Kick(UObject* WorldContextObject, APlayerController* PlayerController, bool RemovePlayerRecord, FString KickMessage)
{

	if (WorldContextObject == nullptr)
	{
		return;
	}

	AGameModeBase* GameModeBase = WorldContextObject->GetWorld()->GetAuthGameMode();

	if (GameModeBase == nullptr)
	{
		return;
	}

	if (PlayerController != nullptr)
	{
		if (RemovePlayerRecord)
		{
			UServerPlayers::RemovePlayerByController(PlayerController);
			USpawnQueueBPLibrary::RemoveFromJoinQueue(PlayerController);
		}

		GameModeBase->GameSession->KickPlayer(PlayerController, FText::FromString(KickMessage));
	}
}

void UServerPlayers::GetPlayerIPAddress(APlayerController* PlayerController, FString& IPAddress)
{
	IPAddress = FString(TEXT(""));

	if (PlayerController != nullptr)
	{
		UNetConnection* Info = PlayerController->GetNetConnection();
		
		if (Info != nullptr)
		{
			if (IsValid(Info))
			{
				IPAddress = Info->RemoteAddressToString();
			}
		}
	}

}

/*
* Returns next player number.
* Starting from zero.
*
* Player 1 = 0
* Player 2 = 1
*/
int32 UServerPlayers::NextPlayerNum()
{

	int32 index;

	index = UServerPlayers::Players.Num() - 1;

	if (index >= int32(0)) {
		return index + 1;
	}
	else {
		return int32(0);
	}
}

/*
* Check if player number is availible.
*
*/
bool UServerPlayers::CheckIfPlayerNumIsAvailible(int32 Number)
{
	
	for (auto& playerout : UServerPlayers::PlayerNumbers) {
		
		if (playerout.PlayerNumber == Number) {

			return false;
		}
	}

	return true;
}

/*
* When players start leaving the server, joining players can take availible number.
*/
int32 UServerPlayers::SolveNextPlayerNumber()
{

	int32 MaxPlayers = 100;
	int32 NumberTrack = 0;
	int32 FirstTry = UServerPlayers::NextPlayerNum();

	if (UServerPlayers::CheckIfPlayerNumIsAvailible(FirstTry)) 
	{
		return FirstTry;
	}
	else
	{

		for (auto& playerout : UServerPlayers::PlayerNumbers)
		{
			NumberTrack = playerout.PlayerNumber + 1;

			if(UServerPlayers::CheckIfPlayerNumIsAvailible(NumberTrack))
			{
				return NumberTrack;
			}
		}
	}

	return 0;
}

PRAGMA_ENABLE_OPTIMIZATION
bool UServerPlayers::RemovePlayerByController(APlayerController* Controller)
{
	return UServerPlayers::RemovePlayer(Controller, FString(TEXT("")));
}

PRAGMA_ENABLE_OPTIMIZATION
bool UServerPlayers::RemovePlayerByID(FString PlayerID)
{
	return UServerPlayers::RemovePlayer(nullptr, PlayerID);
}

PRAGMA_ENABLE_OPTIMIZATION
void UServerPlayers::PlayerCount(int32& PlayerCount, bool ExcludeJoinQueue)
{
	int32 count = 0;
	int32 JoinQueueCount =  0;
	

	if (ExcludeJoinQueue)
	{
		JoinQueueCount = USpawnQueueBPLibrary::JoinQueuePlayerCount();
	}

	PlayerCount = UServerPlayers::Players.Num() - JoinQueueCount;
}

void UServerPlayers::AdminCount(int32& AdminCount)
{
	AdminCount = 0;

	for (auto& playerout : UServerPlayers::Players)
	{

		if (playerout.Privileges == EQueuePrivileges::Admin) {
			AdminCount++;
		}
	}
}

PRAGMA_ENABLE_OPTIMIZATION
bool UServerPlayers::RemovePlayer(APlayerController* Controller, FString PlayerID)
{

	//if both empty exit with false
	if (PlayerID.IsEmpty() && Controller == nullptr)
	{
		return false;
	}


	FPlayerInfo_SQ PlayerInfo = UServerPlayers::emptyInitReadyPI;

	TArray<FPlayerInfo_SQ> clone = UServerPlayers::Players;
	int32 indexcount = 0;
	int32 indextorem = 0;
	FString FoundPlayerID;
	bool found = false;

	for (auto& playeropen : clone)
	{
		if (!found && !playeropen.PlayerID.IsEmpty())
		{
			if (playeropen.PlayerID.Equals(PlayerID))
			{
				found = true;
				indextorem = indexcount;

				FoundPlayerID = playeropen.PlayerID;
			}

			if (playeropen.ServerController == Controller)
			{
				found = true;
				indextorem = indexcount;
				FoundPlayerID = playeropen.PlayerID;
			}
		}

		indexcount++;
	}

	if (found)
	{
		UServerPlayers::Players.RemoveAt(indextorem);

		if (!FoundPlayerID.IsEmpty())
		{
			//Find player number and remove
			int32 plnindexrm = 0;
			int32 countplindx = 0;
			bool plindfound = false;

			for (auto& playernout : UServerPlayers::PlayerNumbers)
			{
				if (!plindfound && playernout.PlayerID.Equals(FoundPlayerID))
				{
					plnindexrm = countplindx;
					plindfound = true;
				}

				countplindx++;
			}

			if (plindfound)
			{
				UServerPlayers::PlayerNumbers.RemoveAt(plnindexrm);
			}
		}
	}


	PlayerInfo = UServerPlayers::Find(Controller, PlayerID);

	if (PlayerInfo.PlayerID.IsEmpty())
	{
		return true;
	}
	else return false;

}

FPlayerInfo_SQ UServerPlayers::Find(APlayerController* Controller, FString PlayerID)
{

	FPlayerInfo_SQ PlayerInfo = UServerPlayers::emptyInitReadyPI;

	bool found = false;
	 
	for (auto& playerout : UServerPlayers::Players)
	{
		if (!PlayerID.IsEmpty())
		{
			if (!found && PlayerID.Equals(playerout.PlayerID)) {

				PlayerInfo = playerout;
				found = true;
			}
		}
		if (Controller != nullptr)
		{
			if (!found && (Controller == playerout.ServerController)) {

				PlayerInfo = playerout;
				found = true;
			}
		}
	}


	return PlayerInfo;
}

PRAGMA_ENABLE_OPTIMIZATION
void UServerPlayers::FindByID(FString PlayerID, FPlayerInfo_SQ& PlayerInfo)
{
	PlayerInfo = UServerPlayers::Find(nullptr, PlayerID);
}

PRAGMA_ENABLE_OPTIMIZATION
void UServerPlayers::FindByController(APlayerController* Controller, FPlayerInfo_SQ& PlayerInfo)
{
	PlayerInfo = UServerPlayers::Find(Controller, FString(TEXT("")));
}

PRAGMA_ENABLE_OPTIMIZATION
void UServerPlayers::UpdatePlayer(FString PlayerID, FUpdatePlayerInfo UpdateList, FPlayerInfo_SQ& Updated)
{
	Updated = UServerPlayers::emptyInitReadyPI;

	FPlayerInfo_SQ New = UServerPlayers::emptyInitReadyPI;
	FPlayerInfo_SQ Old = UServerPlayers::emptyInitReadyPI;

	UServerPlayers::FindByID(PlayerID, Old);

	if (!Old.PlayerID.IsEmpty())
	{
		New = Old;

		//Update Spawn Location
		if (UpdateList.UpdateSpawnLocation)
		{
			New.SpawnLocation = UpdateList.SpawnLocation;
		}

		//Update Team
		if (UpdateList.UpdateTeam)
		{
			New.Team = UpdateList.Team;
		}

		//Update Character
		if (UpdateList.UpdateCharacter)
		{
			if (UpdateList.ServerCharacter != nullptr) {
				New.ServerCharacter = UpdateList.ServerCharacter;
			}
		}

		//Update IsInWorld
		if (UpdateList.UpdateIsInWorld)
		{
			New.IsInWorld = UpdateList.IsInWorld;
		}

		//Update Privileges
		if (UpdateList.UpdatePrivileges)
		{
			New.Privileges = UpdateList.Privileges;
		}

		//Update Player Number on Server
		if (UpdateList.UpdatePlayerNum)
		{
			int32 count = 0;
			int32 indexfound = 0;
			bool ifound = false;

			for (auto& playerout : UServerPlayers::PlayerNumbers)
			{
				if (playerout.PlayerID.Equals(PlayerID))
				{
					indexfound = count;
					ifound = true;
				}

				count++;
			}

			if (ifound)
			{
				New.PlayerNumber = UpdateList.PlayerNumber;
				UServerPlayers::PlayerNumbers[indexfound].PlayerNumber = UpdateList.PlayerNumber;
			}
		}

		if (!New.PlayerID.IsEmpty()) 
		{
			int32 item_index = -1;
			int32 Count = 0;

			for (auto& Item : UServerPlayers::Players)
			{
				if (PlayerID.Equals(Item.PlayerID))
				{
					item_index = Count;
				}

				Count++;
			}
			

			if (!UServerPlayers::Players[item_index].PlayerID.IsEmpty() && item_index >= 0)
			{
				UServerPlayers::Players[item_index] = New;

				Updated = UServerPlayers::Players[item_index];
			}
		}

	}
}