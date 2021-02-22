// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Models/VoxelData.h"
#include "WFCube.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/Button.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserHUD.generated.h"

UCLASS()
class UUserHUD : public UUserWidget
{
	GENERATED_BODY()

public:

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AWFCube* CubeForBuilding = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector PlayerLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector EndTraceLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsInBuildMode = false;

	// Methods
	void ShowError(FText NewText);
	void SetupText(FText NewText);
	void StartBuildingMode(bool IsBuilding) {
		IsInBuildMode = IsBuilding;
	}
	void SetupMesh();
	void SetupCubeVisibility(bool IsVisible);

protected:

	virtual void NativeConstruct() override;

private:
	UPROPERTY()
		UTextBlock* MyTextControl;
	UPROPERTY()
		UTextBlock* ErrorLabel;
};
