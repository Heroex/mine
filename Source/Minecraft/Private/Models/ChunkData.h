// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Cube.h"
#include "CoreMinimal.h"
#include "ChunkData.generated.h"

USTRUCT()
struct FChunkData
{
	GENERATED_BODY()

public:

	FChunkData() {}

	FChunkData(ACube* Cube, FVector Location) :
		Cube(Cube),
		Location(Location)
	{}
	UPROPERTY()
	ACube* Cube = nullptr;
	FVector Location;

	bool operator ==(const FChunkData& rhs) const
	{
		return Location == rhs.Location;
	}
};
