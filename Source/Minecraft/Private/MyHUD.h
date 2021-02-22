// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ConstructorHelpers.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"
#include "UserHUD.h"
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

UCLASS()
class AMyHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	AMyHUD();

	UPROPERTY(EditDefaultsOnly, Category = "Interactive")
		UUserHUD* UserHUD;
};
