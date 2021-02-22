// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCube.h"

/****
// Life-cycle
#################################################################################################################*/
AWFCube::AWFCube()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	check(Root != nullptr);
	RootComponent = Root;
	StaticMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/Geometry/Meshes/1M_Cube.1M_Cube")));

	static ConstructorHelpers::FObjectFinder<UMaterial> WireframeMat(TEXT("Material'/Game/Minecraft/Materials/Cubes/M_Wireframe.M_Wireframe'"));
	if (WireframeMat.Succeeded())
	{
		BaseMaterialMID = UMaterialInstanceDynamic::Create((UMaterial*)WireframeMat.Object, this, FName(TEXT("Base Material Dynamic")));
	}
}

void AWFCube::BeginPlay()
{
	Super::BeginPlay();
	
	StaticMeshComponent = NewObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass());
	StaticMeshComponent->SetStaticMesh(StaticMesh);
	StaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	StaticMeshComponent->SetWorldLocationAndRotation(RootComponent->GetOwner()->GetActorLocation(), RootComponent->GetOwner()->GetActorRotation());
	StaticMeshComponent->SetMobility(EComponentMobility::Movable);
	StaticMeshComponent->RegisterComponentWithWorld(GetWorld());
	FAttachmentTransformRules rules(EAttachmentRule::KeepWorld, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
	StaticMeshComponent->AttachToComponent(RootComponent, rules);
	StaticMeshComponent->SetMaterial(0, BaseMaterialMID);
	StaticMeshComponent->SetHiddenInGame(true);
}

/****
// Methods
#################################################################################################################*/
void AWFCube::SetWorldLocationAt(FVector Location, FRotator Rotation)
{
	SetActorLocationAndRotation(Location, Rotation, false, 0, ETeleportType::None);
}
