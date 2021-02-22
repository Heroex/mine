// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include <Runtime/Engine/Classes/GameFramework/PlayerInput.h>

#define OUT

/****
// Life-cycle
#################################################################################################################*/
AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupHUD();
	SetupMaterials();

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachTo(RootComponent);
	Camera->bUsePawnControlRotation = true;

	StaticMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/Geometry/Meshes/1M_Cube.1M_Cube")));
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckJump();
	ShowCubeAtLocation();
	DestroyChunksOrCubesIfNeeded();
	GenerateLandscape();
	SpawnCreatedCubesIfNeeded();
}

/****
// Methods
#################################################################################################################*/
bool AMyCharacter::IsActorMoving()
{
	ActorLocation = RootComponent->GetOwner()->GetActorLocation();
	PlayerCubeCoords = FVector2D(round(ActorLocation.X / ChunkSize), round(ActorLocation.Y / ChunkSize));
	if (LastActorPosition == PlayerCubeCoords) return false;
	LastActorPosition = PlayerCubeCoords;
	return true;
}

void AMyCharacter::ShowCubeAtLocation()
{
	UserHUD->PlayerLocation = GetPlayerLocation();
	UserHUD->EndTraceLocation = GetLineTraceEnd();
	UserHUD->StartBuildingMode(IsInBuildingMode);
}

void AMyCharacter::SpawnCreatedCubesIfNeeded()
{
	for (int i = 0; i < SpawnedUserCubes.Num(); i++)
	{
		FSpawnedCube SelectedCube = SpawnedUserCubes[i];
		if (SelectedCube.IsSpawned) continue;
		FVector CubeDistanceWithSize(SelectedCube.Location.X, SelectedCube.Location.Y, 0.f);
		float DistanceBetweenCubeAndActor = FVector::Distance(CubeDistanceWithSize, RootComponent->GetOwner()->GetActorLocation());
		if (int(DistanceBetweenCubeAndActor) <= RenderDistance * ChunkSize)
		{
			if (SelectedCube.UserCube)
			{
				SelectedCube.UserCube->SpawnCube(SelectedCube.Type);
				SpawnedUserCubes[i].IsSpawned = true;
			}
		}
	}
}

FVoxelData AMyCharacter::GetInstanceTransformFromLineTrace(bool IsRemovingCube)
{
	FHitResult HitResult = GetFirstPhysicBodyInReach();
	if (HitResult.GetActor()) {
		FVector HitActorLocation = HitResult.Location;
		FVector GridFromActorLocation = FVector(
			FMath::GridSnap(HitActorLocation.X, GRidValue),
			FMath::GridSnap(HitActorLocation.Y, GRidValue),
			FMath::GridSnap(HitActorLocation.Z, GRidValue)
		);
		return FVoxelData(nullptr, HitResult.GetActor(), GridFromActorLocation, HitResult.GetActor()->GetActorRotation(), false, true);
	}
	else if (IsRemovingCube && HitResult.GetComponent())
	{
		FVector GridFromComponentLocation = FVector(
			FMath::GridSnap(HitResult.GetComponent()->GetComponentLocation().X, GRidValue),
			FMath::GridSnap(HitResult.GetComponent()->GetComponentLocation().Y, GRidValue),
			FMath::GridSnap(HitResult.GetComponent()->GetComponentLocation().Z, GRidValue)
		);
		return FVoxelData(HitResult.GetComponent(), nullptr, GridFromComponentLocation, HitResult.GetComponent()->GetComponentRotation(), true, true);
	}
	return FVoxelData(nullptr, nullptr, FVector(), FRotator(), false, false);
}

void AMyCharacter::GenerateLandscape()
{
	if (!IsActorMoving() && !IsFirstInit) return;
	IsFirstInit = false;
	LastActorPosition = PlayerCubeCoords;
	int XYStartIndex = -1 * RenderDistance;
	int XYEndIndex = RenderDistance;
	for (int x = XYStartIndex; x < XYEndIndex; x++)
	{
		for (int y = XYStartIndex; y < XYEndIndex; y++)
		{
			int CalculatedX = x + PlayerCubeCoords.X;
			int CalculatedY = y + PlayerCubeCoords.Y;
			FVector ChunkCoordinates = FVector(CalculatedX, CalculatedY, .0f);
			int ChunkRenderSize = RenderDistance * ChunkSize;
			int XVectorSize = x * ChunkSize;
			int YVectorSize = y * ChunkSize;
			int VectorLenght = FVector(XVectorSize, YVectorSize, 0).Size();
			bool IsChunkSizeLowerOrEqualThanRenderSize = VectorLenght <= ChunkRenderSize;
			SpawnCubeAtLocation(ChunkCoordinates, IsChunkSizeLowerOrEqualThanRenderSize);
		}
	}
}

void AMyCharacter::SpawnCubeAtLocation(FVector Location, bool CanSpawnNextChunk)
{
	FVector UpdatedLocation = FVector(ChunkSize * Location.X, ChunkSize * Location.Y, 0);
	FRotator Rotation(.0f, .0f, .0f);
	FChunkData NewChunkData(nullptr, UpdatedLocation);
	if (!SpawnedChunksWithLocation.Contains(NewChunkData) && CanSpawnNextChunk)
	{
		ACube* NewCube = GetWorld()->SpawnActor<ACube>(ACube::StaticClass(), UpdatedLocation, Rotation);
		if (!NewCube) return;
		NewChunkData.Cube = NewCube;
		SpawnedChunksWithLocation.Add(NewChunkData);
	}
}

void AMyCharacter::DestroyChunksOrCubesIfNeeded()
{
	int RenderDistanceWithCunkSize = RenderDistance * ChunkSize;
	for (int i = 0; i < SpawnedChunksWithLocation.Num(); i++)
	{
		FChunkData SpawnedChunk = SpawnedChunksWithLocation[i];
		FVector ChunkLocation = SpawnedChunk.Location;
		float DistanceBetweenCubeAndActor = FVector::Distance(ChunkLocation, RootComponent->GetOwner()->GetActorLocation());
		if (int(DistanceBetweenCubeAndActor) >= RenderDistanceWithCunkSize)
		{
			ACube* SelectedChunk = SpawnedChunk.Cube;
			if (!SelectedChunk) continue;
			GetWorld()->DestroyActor(SelectedChunk);
			SpawnedChunksWithLocation.RemoveAt(i);
		}
	}
	for (int i = 0; i < SpawnedUserCubes.Num(); i++)
	{
		FSpawnedCube SpawnedCube = SpawnedUserCubes[i];
		if (!SpawnedCube.IsSpawned) continue;
		FVector CubeDistanceWithSize(SpawnedCube.Location.X, SpawnedCube.Location.Y, 0.f);
		float DistanceBetweenCubeAndActor = FVector::Distance(CubeDistanceWithSize, RootComponent->GetOwner()->GetActorLocation());
		if (int(DistanceBetweenCubeAndActor) >= RenderDistanceWithCunkSize && SpawnedCube.UserCube->IsValidLowLevel())
		{
			SpawnedCube.UserCube->DestroyCube();
			SpawnedUserCubes[i].IsSpawned = false;
		}
	}
}

/****
// Character actions
#################################################################################################################*/
void AMyCharacter::AddVoxel()
{
	if (!IsInBuildingMode)
	{
		UserHUD->ShowError(FText::FromString("Enter to edit mode first (press TAB)"));
	}
	else if (SelectedMaterialForVoxel == UCubeMaterial::Type::Basic)
	{
		UserHUD->ShowError(FText::FromString("Select cube type first"));
	}
	else
	{
		FVoxelData Data = GetInstanceTransformFromLineTrace();
		if (!SpawnedUserCubes.Contains(Data.Location) && Data.IsSomething)
		{
			AUserCube* NewCube = GetWorld()->SpawnActor<AUserCube>(AUserCube::StaticClass(), Data.Location, Data.Rotation);
			if (!NewCube) return;
			NewCube->ChangeMaterial(SelectedMaterialForVoxel);
			FSpawnedCube NewSpawnedCube(NewCube, Data.Location, true, Data.Rotation, SelectedMaterialForVoxel);
			SpawnedUserCubes.Add(NewSpawnedCube);
		}
	}
}

void AMyCharacter::RemoveVoxel()
{
	if (!IsInBuildingMode)
	{
		UserHUD->ShowError(FText::FromString("Enter to edit mode first (press TAB)"));
		return;
	}
	UserHUD->ShowError(FText::FromString(""));
	FVoxelData Data = GetInstanceTransformFromLineTrace(true);
	if (!Data.Component || !Data.IsSomething)
	{
		UserHUD->ShowError(FText::FromString("Nothing to destroy"));
		return;
	}
	IsDeleteButtonPressed = !IsDeleteButtonPressed;
	if (IsDeleteButtonPressed)
	{
		StartTime = GetWorld()->TimeSeconds;
	}
	else
	{
		float CurrentTime = GetWorld()->TimeSeconds;
		for (int i = 0; i < SpawnedUserCubes.Num(); i++)
		{
			FSpawnedCube SelectedCubeType = SpawnedUserCubes[i];
			if (!SelectedCubeType.UserCube) continue;
			if (Data.Location == SelectedCubeType.Location && CurrentTime >= (StartTime + (int)SelectedCubeType.Type))
			{
				Data.Component->DestroyComponent();
				SelectedCubeType.UserCube->DestroyCube();
				SpawnedUserCubes.RemoveAt(i);
				break;
			}
		}
	}
}

void AMyCharacter::HorizontalMovement(float Value)
{
	if (Value)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMyCharacter::VerticalMovement(float Value)
{
	if (Value)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMyCharacter::Turn(float Value)
{
	if (Value)
	{
		AddControllerYawInput(Value);
	}
}

void AMyCharacter::LookUp(float Value)
{
	if (Value)
	{
		AddControllerPitchInput(Value);
	}
}

void AMyCharacter::CheckJump()
{
	if (IsJumping)
	{
		Jump();
	}
}

void AMyCharacter::BuildMode()
{
	IsInBuildingMode = !IsInBuildingMode;
	UserHUD->SetupCubeVisibility(IsInBuildingMode);
}

void AMyCharacter::JumpInputAction()
{
	IsJumping = !IsJumping;
}

void AMyCharacter::LoadGame()
{
	UMySaveGame* LoadGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	LoadGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!LoadGameInstance) return;
	RootComponent->GetOwner()->SetActorLocation(LoadGameInstance->PlayerLocation);
	TArray<FSpawnedCube> LoadedCubes;
	for (FSpawnedCube LoadedCube : LoadGameInstance->SpawnedUserCubes)
	{
		UCubeMaterial::Type MaterialType;
		switch (LoadedCube.SelectedMaterialIndex) // todo..
		{
		case 0:
			MaterialType = UCubeMaterial::Type::White;
			break;
		case 1:
			MaterialType = UCubeMaterial::Type::Blue;
			break;
		case 2:
			MaterialType = UCubeMaterial::Type::Yellow;
			break;
		case 3:
			MaterialType = UCubeMaterial::Type::Red;
			break;
		case 4:
			MaterialType = UCubeMaterial::Type::Wireframe;
			break;
		default:
			MaterialType = UCubeMaterial::Type::Basic;
			break;
		}
		FVector CubeDistanceWithSize(LoadedCube.Location.X, LoadedCube.Location.Y, 0.f);
		float DistanceBetweenCubeAndActor = FVector::Distance(CubeDistanceWithSize, RootComponent->GetOwner()->GetActorLocation());
		if (int(DistanceBetweenCubeAndActor) <= RenderDistance * ChunkSize)
		{
			AUserCube* NewCube = GetWorld()->SpawnActor<AUserCube>(AUserCube::StaticClass(), LoadedCube.Location, LoadedCube.Rotation);
			if (!NewCube) return;
			NewCube->ChangeMaterial(MaterialType);
			FSpawnedCube NewSpawnedCube(NewCube, LoadedCube.Location, true, LoadedCube.Rotation, MaterialType);
			LoadedCubes.Add(NewSpawnedCube);
		}
		else {
			if (!LoadedCube.UserCube) return;
			FSpawnedCube NewSpawnedCube(LoadedCube.UserCube, LoadedCube.Location, true, LoadedCube.Rotation, MaterialType);
			LoadedCubes.Add(NewSpawnedCube);
		}
	}
	SpawnedUserCubes = LoadedCubes;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Game loaded"));
}

void AMyCharacter::SaveGame()
{
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	if (!SaveGameInstance) return;
	SaveGameInstance->PlayerLocation = RootComponent->GetOwner()->GetActorLocation();
	SaveGameInstance->SpawnedUserCubes = SpawnedUserCubes;
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Game saved"));
}

/****
// Player coords helpers & raycasting
#################################################################################################################*/
FHitResult AMyCharacter::GetFirstPhysicBodyInReach() const {
	FHitResult Hit;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayerLocation(),
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(TraceObjectTypes),
		TraceParams
	);
	return Hit;
}

std::tuple<FVector, FRotator> AMyCharacter::GetFirstPlayerControllerPosition() const {
	FVector OutLocation;
	FRotator OutRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT OutLocation,
		OUT OutRotation
	);
	return { OutLocation, OutRotation };
}

FVector AMyCharacter::GetLineTraceEnd() const {
	FVector OutLocation;
	FRotator OutRotation;
	std::tie(OutLocation, OutRotation) = GetFirstPlayerControllerPosition();
	return OutLocation + OutRotation.Vector() * Reach;
}

FVector AMyCharacter::GetPlayerLocation() const {
	FVector OutLocation;
	FRotator OutRotation;
	std::tie(OutLocation, OutRotation) = GetFirstPlayerControllerPosition();
	return OutLocation;
}

/****
// Add cube with selected material
#################################################################################################################*/
void AMyCharacter::AddWhiteCube()
{
	UserHUD->SetupText(FText::FromString("You selected White cube. Press and holt right mouse button for 1s to destroy"));
	SelectedMaterialForVoxel = UCubeMaterial::Type::White;
}

void AMyCharacter::AddBlueCube()
{
	UserHUD->SetupText(FText::FromString("You selected Blue cube. Press and holt right mouse button for 2s to destroy"));
	SelectedMaterialForVoxel = UCubeMaterial::Type::Blue;
}

void AMyCharacter::AddYellowCube()
{
	UserHUD->SetupText(FText::FromString("You selected Yellow cube. Press and holt right mouse button for 3s to destroy"));
	SelectedMaterialForVoxel = UCubeMaterial::Type::Yellow;
}

void AMyCharacter::AddRedCube()
{
	UserHUD->SetupText(FText::FromString("You selected Red cube. Press and holt right button mouse for 4s to destroy"));
	SelectedMaterialForVoxel = UCubeMaterial::Type::Red;
}

/****
// Setup HUD, initialize materials, player input
#################################################################################################################*/
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent != nullptr);

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetupInputMapping();
	BindInputs();
}

void AMyCharacter::SetupHUD()
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuClassFinder(TEXT("/Game/Minecraft/Blueprints/WBP_UserWidgetHUD"));
	TSubclassOf<class UUserWidget> MenuClass = MenuClassFinder.Class;
	UserHUD = CreateWidget<UUserHUD>(GetWorld(), MenuClass);
	if (UserHUD)
	{
		UserHUD->SetupMesh();
		UserHUD->AddToViewport();
	}
}

void AMyCharacter::SetupMaterials()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> WhiteMat(TEXT("Material'/Game/Minecraft/Materials/Cubes/M_White.M_White'"));
	if (WhiteMat.Succeeded())
	{
		CubeMaterials.Add(UCubeMaterial::Type::White, (UMaterial*)WhiteMat.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> BlueMat(TEXT("Material'/Game/Minecraft/Materials/Cubes/M_Blue.M_Blue'"));
	if (BlueMat.Succeeded())
	{
		CubeMaterials.Add(UCubeMaterial::Type::Blue, (UMaterial*)BlueMat.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> RedMat(TEXT("Material'/Game/Minecraft/Materials/Cubes/M_Red.M_Red'"));
	if (RedMat.Succeeded())
	{
		CubeMaterials.Add(UCubeMaterial::Type::Red, (UMaterial*)RedMat.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> YellowMat(TEXT("Material'/Game/Minecraft/Materials/Cubes/M_Yellow.M_Yellow'"));
	if (YellowMat.Succeeded())
	{
		CubeMaterials.Add(UCubeMaterial::Type::Yellow, (UMaterial*)YellowMat.Object);
	}
}

/****
// Setup character movement and key bindings
#################################################################################################################*/
void AMyCharacter::SetupInputMapping()
{
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Horizontal", EKeys::D, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Horizontal", EKeys::A, -1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Vertical", EKeys::W, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Vertical", EKeys::S, -1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Turn", EKeys::MouseX, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("LookUp", EKeys::MouseY, -1.f));

	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Jump", EKeys::SpaceBar));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("BuildMode", EKeys::Tab));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("AddVoxel", EKeys::LeftMouseButton));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("RemoveVoxel", EKeys::RightMouseButton));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("AddWhiteCube", EKeys::Y));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("AddBlueCube", EKeys::X));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("AddYellowCube", EKeys::C));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("AddRedCube", EKeys::V));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("SaveGame", EKeys::K));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("LoadGame", EKeys::L));
}

void AMyCharacter::BindInputs()
{
	InputComponent->BindAxis("Horizontal", this, &AMyCharacter::HorizontalMovement);
	InputComponent->BindAxis("Vertical", this, &AMyCharacter::VerticalMovement);
	InputComponent->BindAxis("Turn", this, &AMyCharacter::Turn);
	InputComponent->BindAxis("LookUp", this, &AMyCharacter::LookUp);

	InputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::JumpInputAction);
	InputComponent->BindAction("Jump", IE_Released, this, &AMyCharacter::JumpInputAction);
	InputComponent->BindAction("BuildMode", IE_Pressed, this, &AMyCharacter::BuildMode);
	InputComponent->BindAction("AddVoxel", IE_Pressed, this, &AMyCharacter::AddVoxel);
	InputComponent->BindAction("RemoveVoxel", IE_Pressed, this, &AMyCharacter::RemoveVoxel);
	InputComponent->BindAction("RemoveVoxel", IE_Released, this, &AMyCharacter::RemoveVoxel);
	InputComponent->BindAction("AddWhiteCube", IE_Pressed, this, &AMyCharacter::AddWhiteCube);
	InputComponent->BindAction("AddBlueCube", IE_Pressed, this, &AMyCharacter::AddBlueCube);
	InputComponent->BindAction("AddYellowCube", IE_Pressed, this, &AMyCharacter::AddYellowCube);
	InputComponent->BindAction("AddRedCube", IE_Pressed, this, &AMyCharacter::AddRedCube);
	InputComponent->BindAction("SaveGame", IE_Pressed, this, &AMyCharacter::SaveGame);
	InputComponent->BindAction("LoadGame", IE_Pressed, this, &AMyCharacter::LoadGame);
}
