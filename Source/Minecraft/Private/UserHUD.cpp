// Fill out your copyright notice in the Description page of Project Settings.


#include "UserHUD.h"


void UUserHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUserHUD::SetupMesh()
{
	CubeForBuilding = GetWorld()->SpawnActor<AWFCube>(AWFCube::StaticClass());
}

void UUserHUD::ShowError(FText NewText)
{
	const FName ErrorLabelName = FName(TEXT("ErrorLabel"));
	if (ErrorLabel == nullptr)
		ErrorLabel = (UTextBlock*)(WidgetTree->FindWidget(ErrorLabelName));

	if (ErrorLabel != nullptr)
		ErrorLabel->SetText(NewText);
}

void UUserHUD::SetupText(FText NewText)
{
	const FName TextControlName = FName(TEXT("Title"));
	if (MyTextControl == nullptr)
		MyTextControl = (UTextBlock*)(WidgetTree->FindWidget(TextControlName));


	if (MyTextControl != nullptr)
		MyTextControl->SetText(NewText);
}

void UUserHUD::SetupCubeVisibility(bool IsVisible)
{
	CubeForBuilding->StaticMeshComponent->SetHiddenInGame(!IsVisible);
}
