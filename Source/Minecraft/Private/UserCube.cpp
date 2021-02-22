// Fill out your copyright notice in the Description page of Project Settings.


#include "UserCube.h"

/****
// Life-cycle
#################################################################################################################*/
AUserCube::AUserCube()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	check(Root != nullptr);
	RootComponent = Root;
	StaticMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/Geometry/Meshes/1M_Cube.1M_Cube")));

	static ConstructorHelpers::FObjectFinder<UMaterial> WhiteMat(TEXT("Material'/Game/Minecraft/Materials/Cubes/M_White.M_White'"));
	if (WhiteMat.Succeeded())
	{
		CubeMaterials.Add(UCubeMaterial::Type::White, UMaterialInstanceDynamic::Create((UMaterial*)WhiteMat.Object, this, FName(TEXT("Base Material Dynamic"))));
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> BlueMat(TEXT("Material'/Game/Minecraft/Materials/Cubes/M_Blue.M_Blue'"));
	if (BlueMat.Succeeded())
	{
		CubeMaterials.Add(UCubeMaterial::Type::Blue, UMaterialInstanceDynamic::Create((UMaterial*)BlueMat.Object, this, FName(TEXT("Base Material Dynamic"))));
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> YellowMat(TEXT("Material'/Game/Minecraft/Materials/Cubes/M_Yellow.M_Yellow'"));
	if (YellowMat.Succeeded())
	{
		CubeMaterials.Add(UCubeMaterial::Type::Yellow, UMaterialInstanceDynamic::Create((UMaterial*)YellowMat.Object, this, FName(TEXT("Base Material Dynamic"))));
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> RedMat(TEXT("Material'/Game/Minecraft/Materials/Cubes/M_Red.M_Red'"));
	if (RedMat.Succeeded())
	{
		CubeMaterials.Add(UCubeMaterial::Type::Red, UMaterialInstanceDynamic::Create((UMaterial*)RedMat.Object, this, FName(TEXT("Base Material Dynamic"))));
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> WireframeMat(TEXT("Material'/Game/Minecraft/Materials/Cubes/M_Wireframe.M_Wireframe'"));
	if (WireframeMat.Succeeded())
	{
		CubeMaterials.Add(UCubeMaterial::Type::Wireframe, UMaterialInstanceDynamic::Create((UMaterial*)WireframeMat.Object, this, FName(TEXT("Base Material Dynamic"))));
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> BasicMat(TEXT("Material'/Game/Minecraft/Materials/Cubes/M_Basic.M_Basic'"));
	if (WireframeMat.Succeeded())
	{
		CubeMaterials.Add(UCubeMaterial::Type::Basic, UMaterialInstanceDynamic::Create((UMaterial*)BasicMat.Object, this, FName(TEXT("Base Material Dynamic"))));
	}
}

void AUserCube::BeginPlay()
{
	Super::BeginPlay();

	SpawnCube(UCubeMaterial::Type::Basic);
}

/****
// Methods
#################################################################################################################*/
void AUserCube::ChangeMaterial(UCubeMaterial::Type Type)
{
	StaticMeshComponent->SetMaterial(0, CubeMaterials[Type]);
}

void AUserCube::DestroyCube()
{
	StaticMeshComponent->DestroyComponent();
	Destroy();
}

void AUserCube::SpawnCube(UCubeMaterial::Type Type)
{
	StaticMeshComponent = NewObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass());
	StaticMeshComponent->SetStaticMesh(StaticMesh);
	StaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	StaticMeshComponent->SetWorldLocationAndRotation(RootComponent->GetOwner()->GetActorLocation(), RootComponent->GetOwner()->GetActorRotation());
	StaticMeshComponent->SetMobility(EComponentMobility::Stationary);
	StaticMeshComponent->RegisterComponentWithWorld(GetWorld());
	FAttachmentTransformRules rules(EAttachmentRule::KeepWorld, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
	StaticMeshComponent->AttachToComponent(RootComponent, rules);
	ChangeMaterial(Type);
}
