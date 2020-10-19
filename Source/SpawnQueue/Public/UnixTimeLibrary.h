// Copyright (C) TwoPieces 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnixTimeLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UUnixTimeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	* Date Time to Unix Time Stamp.
	*
	* @param		Value		Date Time.
	* @return					Returns Unix Time Stamp.
	*/
	UFUNCTION(BlueprintPure, Category = "UnixTimeLibrary")
	static int64 DateTimetoUnixStamp(struct FDateTime Value);

	/**
	* Unix Time Stamp to Date Time.
	*
	* @param		Value		Unix Time Stamp.
	* @param		TimeOut		Returns Date Time.
	*/
	UFUNCTION(BlueprintPure, Category = "UnixTimeLibrary")
	static void UnixTimetoDateTime(int64 Value, FDateTime& TimeOut);
	
	/**
	* Add seconds to Date Time.
	*
	* @param		Date		Date Time.
	* @param		Seconds		Seconds to add.
	* @param		Result		Returns result.
	*/
	UFUNCTION(BlueprintPure, Category = "UnixTimeLibrary")
	static void AddSecondsToDate(FDateTime Date, float Seconds, FDateTime& Result);
};
