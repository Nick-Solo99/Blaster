// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSpawnPoint.h"

#include "Weapon.h"

// Sets default values
AWeaponSpawnPoint::AWeaponSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

void AWeaponSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnWeaponTimer(nullptr);
}

void AWeaponSpawnPoint::SpawnWeapon()
{
	int32 NumOfWeaponClasses = WeaponClasses.Num();
	if (NumOfWeaponClasses > 0)
	{
		int32 Selection = FMath::RandRange(0, NumOfWeaponClasses - 1);
		SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClasses[Selection], GetActorTransform());

		if (HasAuthority())
		{
			SpawnedWeapon->OnWeaponEquipped.AddDynamic(this, &ThisClass::StartSpawnWeaponTimer);
		}
	}
}

void AWeaponSpawnPoint::SpawnWeaponTimerFinished()
{
	if (HasAuthority())
	{
		SpawnWeapon();
	}
}

void AWeaponSpawnPoint::StartSpawnWeaponTimer(AWeapon* EquippedWeapon)
{
	int32 SpawnTime = FMath::RandRange(WeaponSpawnTimeMin, WeaponSpawnTimeMax);
	GetWorldTimerManager().SetTimer(SpawnWeaponTimer, this, &ThisClass::SpawnWeaponTimerFinished, SpawnTime);
	
}
