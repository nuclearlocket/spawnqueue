// Copyright (C) TwoPieces 2020. All Rights Reserved.


#include "UnixTimeLibrary.h"


int64 UUnixTimeLibrary::DateTimetoUnixStamp (const FDateTime Value)
{
	return Value.ToUnixTimestamp();
}

void UUnixTimeLibrary::UnixTimetoDateTime (int64 Value, FDateTime& TimeOut)
{
	TimeOut = FDateTime::FromUnixTimestamp(Value);
}

void UUnixTimeLibrary::AddSecondsToDate(FDateTime Date, float Seconds, FDateTime& Result)
{
	float f_milliseconds = FMath::Frac(Seconds)*10;
	
	int32 milliseconds = (int32) f_milliseconds;

	FTimespan TimespanAdd = FTimespan(0, 0, 0, Seconds, milliseconds * 1000 * 1000);

	FDateTime NewTime = Date + TimespanAdd;

	Result = NewTime;
}