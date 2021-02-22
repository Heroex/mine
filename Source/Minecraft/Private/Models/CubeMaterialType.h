// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CubeMaterialType.generated.h"

UCLASS()
class UCubeMaterial : public UObject
{
	GENERATED_BODY()

public:

	enum class Type : uint8 {
		White,
		Blue,
		Yellow,
		Red,
		Wireframe,
		Basic
	};
};
