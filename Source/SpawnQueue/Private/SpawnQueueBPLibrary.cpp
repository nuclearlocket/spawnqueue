// Copyright (C) TwoPieces 2020. All Rights Reserved.

#include "SpawnQueueBPLibrary.h"
#include "ServerPlayers.h"

TArray<FSpawnQueue> USpawnQueueBPLibrary::spawnlist;
FSpawnQueue USpawnQueueBPLibrary::emptyInitReadyPI;
TArray<APlayerController*> USpawnQueueBPLibrary::ServerJoinQueue;

bool USpawnQueueBPLibrary::InitTT;

USpawnQueueBPLibrary::USpawnQueueBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	USpawnQueueBPLibrary::InitTT = false;

	//empty FSpawnQueue structure for setting values and for comparison
	USpawnQueueBPLibrary::emptyInitReadyPI.AccessLevel = EQueuePrivileges::Regular;
	USpawnQueueBPLibrary::emptyInitReadyPI.AdditionalDelay = 0.f;
	USpawnQueueBPLibrary::emptyInitReadyPI.Controller = nullptr;
	USpawnQueueBPLibrary::emptyInitReadyPI.PlayerID = FString(TEXT(""));
	USpawnQueueBPLibrary::emptyInitReadyPI.SpawnGroup = ESpawnClass::Death;
	USpawnQueueBPLibrary::emptyInitReadyPI.Time = FDateTime::UtcNow();
}

PRAGMA_ENABLE_OPTIMIZATION
bool USpawnQueueBPLibrary::SpawnListAdd(APlayerController* Controller, FString PlayerID, UClass* Class, ESpawnClass SpawnGroup, EQueuePrivileges AccessLevel, float AdditionalDelay)
{
	FDateTime TimeNow;
	
	//Clear spawn queue when game begins
	if (!USpawnQueueBPLibrary::InitTT)
	{
		USpawnQueueBPLibrary::spawnlist.Empty();
		USpawnQueueBPLibrary::InitTT = true;
	}

	//Separate milliseconds from seconds also add 10 millisecond delay so we get queue
	float f_milliseconds = FMath::Frac( (AdditionalDelay + 0.1) ) * 10;

	//Convert to int32
	int32 milliseconds = (int32)f_milliseconds;

	//Make timespan
	FTimespan TimeSpan = FTimespan(0, 0, 0, AdditionalDelay, milliseconds * 1000 * 1000);

	//Temporal spawn queue structure
	FSpawnQueue MkPlayerSpawnQueue;

	MkPlayerSpawnQueue.Controller = Controller;
	MkPlayerSpawnQueue.PlayerID = PlayerID;
	MkPlayerSpawnQueue.Time = TimeNow.UtcNow() + TimeSpan;
	MkPlayerSpawnQueue.AdditionalDelay = AdditionalDelay;
	MkPlayerSpawnQueue.AccessLevel = AccessLevel;
	MkPlayerSpawnQueue.SpawnGroup = SpawnGroup;
	MkPlayerSpawnQueue.Class = Class;

	int32 return_index = 0;

	return_index = USpawnQueueBPLibrary::spawnlist.Add(MkPlayerSpawnQueue);

	return true;
}

PRAGMA_ENABLE_OPTIMIZATION
void USpawnQueueBPLibrary::GiveAllQueue(TArray<FSpawnQueue>& SpawnQueue)
{
	SpawnQueue = USpawnQueueBPLibrary::spawnlist;
}

PRAGMA_ENABLE_OPTIMIZATION
void USpawnQueueBPLibrary::EZSpawn(UObject* WorldContextObject, FSpawnQueue SpawnQueueInfo, bool AutoRemoveFromSpawnQueue, bool& Reroll)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	FPlayerInfo_SQ UpdatedPlayerInfo;
	bool SpawnFound = false;
	bool Availible = false;
	bool Validated = false;
	bool isNoNull = false;

	FTransform SpawnLocation;
	FServerSettings ServerSettings = UServerPlayers::ServerSettings;

	UWorld* World = WorldContextObject->GetWorld();

	if (World == 0)
	{
		GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, FString(TEXT("EZSpawn: No world found.")));
		return;
	}

	//Find player info
	FPlayerInfo_SQ Player;
	UServerPlayers::FindByID(SpawnQueueInfo.PlayerID, Player);

	if (!Player.PlayerID.IsEmpty())
	{
		//check if Controller is valid
		if (IsValid(Player.ServerController))
		{

			Reroll = true;

			//Get Spawn Location
			TArray<AActor*> ActorsList;

			UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ACustomPlayerStart::StaticClass(), ActorsList);
			
			for (auto& TActor : ActorsList)
			{
				isNoNull = false;
				
				ACustomPlayerStart* CustomPlayerStartActor = Cast<ACustomPlayerStart>(TActor);
				
				if (CustomPlayerStartActor != nullptr)
				{
					isNoNull = true;

					SpawnLocation = CustomPlayerStartActor->GetActorTransform();

					if (CustomPlayerStartActor->Validate(SpawnQueueInfo) && !CustomPlayerStartActor->IsBlocked())
					{
						Validated = true;
					}
					else
					{
						Validated = false;
					}
				}
				

				if (isNoNull)
				{

					if (!SpawnFound)
					{

						if (Validated)
						{

							if (SpawnQueueInfo.Class == nullptr)
							{
								return;
							}

							APawn* SpawnPawn = World->SpawnActor<APawn>(SpawnQueueInfo.Class, SpawnLocation, SpawnParameters);

							APlayerController* controller = Player.ServerController;
							controller->UnPossess();
							controller->Possess(SpawnPawn);

							SpawnPawn->SetActorRotation(SpawnLocation.Rotator());

							//FRotator Rot = CustomPlayerStartActor->GetActorRotation();
							//Rot.Roll = 0.f;

							//controller->SetControlRotation(Rot);

							//Update Player info
							FUpdatePlayerInfo UpdateList;

							UpdateList.UpdateSpawnLocation = true;
							UpdateList.SpawnLocation = SpawnLocation;

							UpdateList.UpdateCharacter = true;
							UpdateList.ServerCharacter = SpawnPawn;

							UpdateList.UpdateIsInWorld = true;
							UpdateList.IsInWorld = true;

							UServerPlayers::UpdatePlayer(Player.PlayerID, UpdateList, UpdatedPlayerInfo);

							Reroll = false;
							SpawnFound = true;	

							if (AutoRemoveFromSpawnQueue)
							{
								USpawnQueueBPLibrary::RemovePlayerByID(Player.PlayerID);
							}
						}
						else
						{
							Reroll = true;
							SpawnFound = false;
						}
						
					}
				}
			}
		
			
			if(SpawnFound && AutoRemoveFromSpawnQueue)
			{
				//remove from spawn queue
				USpawnQueueBPLibrary::RemovePlayerByID(Player.PlayerID);
			}
		}
		//Controller is not valid and player left the server
		//also player is not in player info anymore either
		else
		{
			USpawnQueueBPLibrary::RemovePlayerByID(SpawnQueueInfo.PlayerID);
			Reroll = false;
		}
	}
	//player probably left
	else
	{
		USpawnQueueBPLibrary::RemovePlayerByID(SpawnQueueInfo.PlayerID);
		Reroll = false;
	}
}

PRAGMA_ENABLE_OPTIMIZATION
void USpawnQueueBPLibrary::IsInSpawnQueue(APlayerController* Controller, FString PlayerID, bool& IsInQueue, FTimespan& SpawnTimeLeft, int& NumberInQueue)
{
	//initialize and reset returning values
	NumberInQueue = 9999;
	SpawnTimeLeft = FTimespan::Zero();
	IsInQueue = false;

	FSpawnQueue Player = USpawnQueueBPLibrary::emptyInitReadyPI;
	FDateTime TimeNow = FDateTime::UtcNow();

	USpawnQueueBPLibrary::Find(Controller, PlayerID, Player);


	//Find result is empty
	if (Player.PlayerID.IsEmpty()) {
		IsInQueue = false;
	}
	//is in queue
	else {
		IsInQueue = true;
		/*
		now: 15:11
		spawn time 15:15
		spawn time - now = 
		*/
		SpawnTimeLeft = Player.Time - TimeNow;
		TArray<FSpawnQueue> Result = USpawnQueueBPLibrary::SortByDate(USpawnQueueBPLibrary::spawnlist);

		int32 item_index = 0;
		int32 item_count = 0;
		bool found = false;

		for (auto& spawnout : Result)
		{
			
			if (!found)
			{
				if (spawnout.PlayerID.Equals(Player.PlayerID))
				{
					item_index = item_count;
					found = true;
				}
			}

			item_count++;
		}

		//add one so zero is not first
		NumberInQueue = (item_index + 1);
	}
}

PRAGMA_ENABLE_OPTIMIZATION
bool USpawnQueueBPLibrary::IsEmpty(FSpawnQueue PlayerQueueInfo) {

	//empty player id or controller
	if (PlayerQueueInfo.PlayerID.IsEmpty() || 
		PlayerQueueInfo.Controller == nullptr) {
		return true;
	}

	return false;
}

PRAGMA_ENABLE_OPTIMIZATION
bool USpawnQueueBPLibrary::RemovePlayerByController(APlayerController* Controller)
{
	return USpawnQueueBPLibrary::RemovePlayer(Controller, TEXT(""));
}

PRAGMA_ENABLE_OPTIMIZATION
bool USpawnQueueBPLibrary::RemovePlayerByID(FString PlayerID)
{
	return USpawnQueueBPLibrary::RemovePlayer(nullptr, PlayerID);
}

PRAGMA_ENABLE_OPTIMIZATION
void USpawnQueueBPLibrary::QueueTimeLength(FTimespan& Length)
{

	Length = FTimespan();

	FDateTime First = FDateTime();
	FDateTime Last = FDateTime();

	int32 lastitem = (USpawnQueueBPLibrary::spawnlist.Num() - 1);
	int32 count = 0;

	for (auto& spawnout : USpawnQueueBPLibrary::spawnlist) {

		if (count == int32(0)) {
			spawnout.Time = First;
		}

		if (count == lastitem) {
			spawnout.Time = Last;
		}

		count++;
	}

	Length = Last - First;
}

PRAGMA_ENABLE_OPTIMIZATION
void USpawnQueueBPLibrary::QueuePlayerCount(int& PlayersLeft) {

	PlayersLeft = USpawnQueueBPLibrary::spawnlist.Num();
}

void USpawnQueueBPLibrary::FindByController(APlayerController* Controller, FSpawnQueue& Result)
{
	USpawnQueueBPLibrary::Find(Controller, FString(TEXT("")), Result);
}

void USpawnQueueBPLibrary::FindByID(FString PlayerID, FSpawnQueue& Result)
{
	USpawnQueueBPLibrary::Find(nullptr, PlayerID, Result);
}

PRAGMA_ENABLE_OPTIMIZATION
void USpawnQueueBPLibrary::Find(APlayerController* Controller, FString PlayerID, FSpawnQueue& Result) {

	int32 count = 0;

	Result = USpawnQueueBPLibrary::emptyInitReadyPI;

	for (auto &spawnout : USpawnQueueBPLibrary::spawnlist)
	{
		if (Controller != nullptr) {
			if (Controller == spawnout.Controller)
			{
				Result = spawnout;
			}
		}
		
		if (PlayerID.Equals(spawnout.PlayerID))
		{
			Result = spawnout;
		}

		count++;
	}
	
}

PRAGMA_ENABLE_OPTIMIZATION
bool USpawnQueueBPLibrary::RemovePlayer(APlayerController* Controller, FString PlayerID)
{
	//if both empty exit with false
	if (PlayerID.IsEmpty() &&
		Controller == nullptr) {
		return false;
	}

	int32 indextt = 0;
	int32 indextoremove = 0;
	bool found = false;

	for (auto& spawnopen : USpawnQueueBPLibrary::spawnlist) 
	{
		
		if (!found)
		{
			if (!PlayerID.IsEmpty())
			{
				if (spawnopen.PlayerID.Equals(PlayerID))
				{
					indextoremove = indextt;
					found = true;
				}
			}

			if (Controller != nullptr)
			{
				if (spawnopen.Controller == Controller)
				{
					indextoremove = indextt;
					found = true;
				}
			}
		}

		indextt++;
	}

	if (found)
	{
		USpawnQueueBPLibrary::spawnlist.RemoveAt(indextoremove);
	}

	FSpawnQueue PlayerRecheck;

	USpawnQueueBPLibrary::Find(Controller, PlayerID, PlayerRecheck);

	if (USpawnQueueBPLibrary::IsEmpty(PlayerRecheck)) {
		return true;
	}
	return false;
}

TArray<FSpawnQueue> USpawnQueueBPLibrary::SortByDate(TArray<FSpawnQueue> List) {

	/*
	------- Sorting Swap Strategy -------

	Timelist:
	15:15 compare this and find bigger, when bigger value has been found, swap
	09:20 swap this with 17:55, second loop
	22:01 swap this with 15:15, first loop
	17:55

	to

	22:01
	17:55
	15:15
	09:20

	for() {
		if (a > b) {
			swap (replace_item, with_this)
		}
	}

	*/

	bool swapped;

	TArray<FSpawnQueue> clone = USpawnQueueBPLibrary::spawnlist;

	do {
		swapped = false;

		for (int i = 0; i < (clone.Num() - 1); i++) {

			if (clone[i].Time > clone[i + 1].Time) {

				clone.Swap(i, (i + 1));
				swapped = true;
			}

		}
	} while (swapped);
	
	return clone;
}

void USpawnQueueBPLibrary::Wipe()
{
	USpawnQueueBPLibrary::spawnlist.Empty();
	USpawnQueueBPLibrary::ServerJoinQueue.Empty();
}

void USpawnQueueBPLibrary::SpawnRun(TArray<FSpawnQueue>& List, ESpawnBranch& Branches)
{
	int32 PlayerCount = 0;
	UServerPlayers::PlayerCount(PlayerCount);

	FServerSettings ServerSettings = UServerPlayers::ServerSettings;

	//This is still holding data from previous round
	List.Empty();

	Branches = ESpawnBranch::NoSpawn;
	FDateTime Time;

	for (auto& spawnopen : USpawnQueueBPLibrary::spawnlist) {

		if (!spawnopen.PlayerID.IsEmpty())
		{
				//if time now is more then it's spawn time
				if (Time.UtcNow() >= spawnopen.Time)
				{

					List.Add(spawnopen);

					Branches = ESpawnBranch::SpawnNow;
				}
			
		}
	}
}


/***************** Server Join Queue ******************/

void USpawnQueueBPLibrary::AddToJoinQueue(APlayerController* PlayerController, ESpawnQueueAddStatus& Status, bool& Kick)
{
	Status = ESpawnQueueAddStatus::UnknownFailure;
	Kick = false;

	int32 index = 0;
	int32 PlayerCountNow = 0;
	int32 ReservedSlotsCountAfter = 0;

	bool Authority = false;
	bool AdminAuthority = false;
	bool RecordFound = false;

	FPlayerInfo_SQ PlayerInfo;
	UServerPlayers::FindByController(PlayerController, PlayerInfo);
	UServerPlayers::PlayerCount(PlayerCountNow);

	//if player is found we can check the privileges
	if (!PlayerInfo.PlayerID.IsEmpty())
	{
		
		if (UServerPlayers::ServerSettings.PrivilegesForSlots.Find(PlayerInfo.Privileges, index))
		{
			Authority = true;
		}

		if (UServerPlayers::ServerSettings.Kickable.Find(PlayerInfo.Privileges, index))
		{
			Authority = false;
		}

		//server is full, admin pass
		if (UServerPlayers::ServerSettings.AllowAdminsIfServerFull)
		{
			if (PlayerInfo.Privileges == EQueuePrivileges::Admin)
			{
				AdminAuthority = true;
			}
		}
	}

	//if server is not full or player has authority
	if (!USpawnQueueBPLibrary::IsJoinQueueFull() || AdminAuthority || Authority)
	{
		//and finally add player to join queue
		USpawnQueueBPLibrary::ServerJoinQueue.Add(PlayerController);

		Status = ESpawnQueueAddStatus::Success;
	}
	else
	{
		//player not found so just check join queue size
		if (!RecordFound)
		{
			Status = ESpawnQueueAddStatus::JoinQueueIsFull;
		}
		//player found but no authority
		else
		{
			Status = ESpawnQueueAddStatus::NoAuthority;
		}

		Kick = true;
	}
}

bool USpawnQueueBPLibrary::IsJoinQueueFull()
{

	if (USpawnQueueBPLibrary::ServerJoinQueue.Num() >= UServerPlayers::ServerSettings.ServerJoinQueueSize)
	{
		return true;
	}

	return false;
}

bool USpawnQueueBPLibrary::IsInJoinQueue(APlayerController* PlayerController, int32& NumberInQueue)
{
	NumberInQueue = -1;
	int32 index = 0;
	int32 indexTwo = 0;
	TArray<APlayerController*> List;

	if (USpawnQueueBPLibrary::FindFromJoinQueue(PlayerController, index))
	{
		List = USpawnQueueBPLibrary::JoinQueueSort();

		if (List.Find(PlayerController, indexTwo))
		{

			NumberInQueue = (indexTwo + 1);

			return true;
		}
	}

	return false;
}


int32 USpawnQueueBPLibrary::JoinQueuePlayerCount()
{
	return USpawnQueueBPLibrary::ServerJoinQueue.Num();
}


bool USpawnQueueBPLibrary::RemoveFromJoinQueue(APlayerController* PlayerController)
{
	int32 index = -1;

	if (USpawnQueueBPLibrary::FindFromJoinQueue(PlayerController, index))
	{
		if (index >= 0)
		{
			USpawnQueueBPLibrary::ServerJoinQueue.RemoveAt(index);
			return true;
		}
	}

	return false;
}


void USpawnQueueBPLibrary::JoinQueueRun(bool AutoRemove, TArray<APlayerController*>& Players, ESpawnBranch& Branches)
{
	Players.Empty();

	Branches = ESpawnBranch::NoSpawn;

	bool Authority = false;

	int32 PlayerCountNow = 0;
	int32 MaxPlayers = 0;
	int32 ReservedSlotsCount = 0;
	int32 AddedCount = 0;
	int32 Count = 0;
	int32 AdminCount = 0;

	ReservedSlotsCount = UServerPlayers::ServerSettings.ReservedSlots;
	MaxPlayers = UServerPlayers::ServerSettings.ServerMaxPlayers;
	TArray<APlayerController*> cloneFirst = USpawnQueueBPLibrary::ServerJoinQueue;
	

	for (auto& PlayerControllerB : cloneFirst)
	{
		//Player left the server long time ago
		if (PlayerControllerB == nullptr)
		{
			USpawnQueueBPLibrary::ServerJoinQueue.RemoveAt(Count);

			//Array is one item shorter
			Count--;
		}
		//player is leaving right now
		else if (!PlayerControllerB->IsValidLowLevel() || PlayerControllerB->IsPendingKill())
		{
			USpawnQueueBPLibrary::ServerJoinQueue.RemoveAt(Count);
			Count--;
		}

		Count++;
	}

	for (auto& PlayerController : USpawnQueueBPLibrary::JoinQueueSort())
	{
		//Player left the server
		if (PlayerController == nullptr || PlayerController->IsPendingKill())
		{

		}
		else
		{
			UServerPlayers::PlayerCount(PlayerCountNow);


			if (UServerPlayers::ServerSettings.DontCountAdminsInJoinQueue)
			{
				UServerPlayers::AdminCount(AdminCount);

				PlayerCountNow = PlayerCountNow - AdminCount;
			}

			//also count players that are added in this loop
			PlayerCountNow = PlayerCountNow + AddedCount;

			Authority = false;

			//accept regular
			if (PlayerCountNow < (MaxPlayers - ReservedSlotsCount))
			{
				Authority = true;
			}
			else
			{
				FPlayerInfo_SQ PlayerInfo;
				UServerPlayers::FindByController(PlayerController, PlayerInfo);

				if (!PlayerInfo.PlayerID.IsEmpty())
				{
					for (auto& Item : UServerPlayers::ServerSettings.PrivilegesForSlots)
					{
						//accept authority
						if (Item == PlayerInfo.Privileges)
						{
							if (PlayerCountNow < MaxPlayers)
							{
								Authority = true;
							}
						}
					}

					//accept admin
					if (UServerPlayers::ServerSettings.AllowAdminsIfServerFull)
					{
						//accept authority
						if (EQueuePrivileges::Admin == PlayerInfo.Privileges)
						{
							Authority = true;
						}
					}
				}
			}

			if (Authority)
			{
				Players.Add(PlayerController);
				Branches = ESpawnBranch::SpawnNow;

				AddedCount++;

				if (AutoRemove)
				{
					USpawnQueueBPLibrary::RemoveFromJoinQueue(PlayerController);
				}
			}
		}
	}
}

TArray<APlayerController*> USpawnQueueBPLibrary::JoinQueueSort()
{
	bool FirstItem = false;

	TArray<APlayerController*> Admins;
	TArray<APlayerController*> VIP;
	TArray<APlayerController*> SpecialYellow;
	TArray<APlayerController*> SpecialPurple;
	TArray<APlayerController*> Regular;

	TArray<APlayerController*> ListOut;
	TArray<FPlayerInfo_SQ> Collect;

	//Gather all info of players
	for (auto& OpenItem : USpawnQueueBPLibrary::ServerJoinQueue)
	{
		FPlayerInfo_SQ PlayerInfo;
		UServerPlayers::FindByController(OpenItem, PlayerInfo);

		//Player info is found
		if (!PlayerInfo.PlayerID.IsEmpty())
		{
			Collect.Add(PlayerInfo);
		}
		//Player info is not found so just add controller to info
		//Note: PlayerInfo param is already initialized by FindByController
		else
		{
			PlayerInfo.ServerController = OpenItem;
			Collect.Add(PlayerInfo);
		}

		FirstItem = true;
	}

	for (auto& Item : Collect)
	{
		switch (Item.Privileges)
		{
			case EQueuePrivileges::Admin: 
			{
				Admins.Add(Item.ServerController);
				break;
			}
			case EQueuePrivileges::VIP:
			{
				VIP.Add(Item.ServerController);
				break;
			}
			case EQueuePrivileges::Special_Yellow:
			{
				SpecialYellow.Add(Item.ServerController);
				break;
			}
			case EQueuePrivileges::Special_Purple:
			{
				SpecialPurple.Add(Item.ServerController);
				break;
			}
			case EQueuePrivileges::Regular:
			{
				Regular.Add(Item.ServerController);
				break;
			}
		}
	}
	
	ListOut.Empty();

	for (auto& Item : Admins) { ListOut.Add(Item); }
	for (auto& Item : VIP) { ListOut.Add(Item); }
	for (auto& Item : SpecialYellow) { ListOut.Add(Item); }
	for (auto& Item : SpecialPurple) { ListOut.Add(Item); }
	for (auto& Item : Regular) { ListOut.Add(Item); }

	return ListOut;
}

bool USpawnQueueBPLibrary::FindFromJoinQueue(APlayerController* PlayerController, int32& Index)
{
	Index = -1;
	int32 Count = 0;

	if (PlayerController == nullptr)
	{
		return false;
	}

	TArray<APlayerController*> clone = USpawnQueueBPLibrary::ServerJoinQueue;


	for (auto& PlayerControllerItem : clone)
	{
		if (PlayerControllerItem != nullptr)
		{
			if (PlayerControllerItem == PlayerController)
			{
				Index = Count;
				return true;
			}
		}

		Count++;
	}

	return false;
}

/*
* Generate Random String
*/
void USpawnQueueBPLibrary::RandomStr(int32 Length, FString& Value)
{
	FString buffer;
	int32 randomint = 0;
	FText ArrayReturnItem{};

	FString alphanum = FString(TEXT("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"));

	for (int32 i = 0; i < Length; ++i) {
		randomint = FMath::RandRange(0, alphanum.Len() - 1);
		buffer += alphanum[randomint];
		randomint = 0;
	}

	Value = buffer;
}
