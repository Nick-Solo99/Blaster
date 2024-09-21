// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponSpawnPoint.generated.h"

class AWeapon;

UCLASS()
class BLASTER_API AWeaponSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponSpawnPoint();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AWeapon>> WeaponClasses;

	UPROPERTY()
	AWeapon* SpawnedWeapon;

	void SpawnWeapon();
	void SpawnWeaponTimerFinished();

	UFUNCTION()
	void StartSpawnWeaponTimer(AWeapon* EquippedWeapon);

private:
	FTimerHandle SpawnWeaponTimer;

	UPROPERTY(EditAnywhere)
	float WeaponSpawnTimeMin;

	UPROPERTY(EditAnywhere)
	float WeaponSpawnTimeMax;

};
