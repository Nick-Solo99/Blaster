// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "MetasoundSource.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		//Maps hit character to number of times hit
		TMap<ABlasterCharacter*, uint32> HitMap;
		TMap<ABlasterCharacter*, uint32> HeadshotHitMap;
		for (auto HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter)
			{
				const bool bHeadshot = FireHit.BoneName.ToString() == FString("head");
				if (bHeadshot)
				{
					HeadshotHitMap.Contains(BlasterCharacter) ? HeadshotHitMap[BlasterCharacter]++ : HeadshotHitMap.Emplace(BlasterCharacter, 1);
				}
				else
				{
					HitMap.Contains(BlasterCharacter) ? HitMap[BlasterCharacter]++ : HitMap.Emplace(BlasterCharacter, 1);
				}
			}

			if (ImpactParticles && PlayerImpactParticles)
			{
				if (BlasterCharacter)
				{
					UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					PlayerImpactParticles,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation()
				);
				}
				else
				{
					UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation()
				);
				}
			}
			if (HitSound && PlayerHitSound)
			{
				if (BlasterCharacter)
				{
					UGameplayStatics::PlaySoundAtLocation(
						this,
						PlayerHitSound,
						FireHit.ImpactPoint,
						.5f,
						FMath::FRandRange(-.5f,.5f)
					);
				}
				else
				{
					UGameplayStatics::PlaySoundAtLocation(
						this,
						HitSound,
						FireHit.ImpactPoint,
						.5f,
						FMath::FRandRange(-.5f, .5f)
					);
				}
			}
		}
		TArray<ABlasterCharacter*> HitCharacters;

		// maps character hit to total damage
		TMap<ABlasterCharacter*, float> DamageMap;

		// calculates body shot damage
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);
				HitCharacters.AddUnique(HitPair.Key);
			}
		}

		// calculates headshot damage
		for (auto HeadshotHitPair : HeadshotHitMap)
		{
			if (HeadshotHitPair.Key)
			{
				DamageMap.Contains(HeadshotHitPair.Key) ? DamageMap[HeadshotHitPair.Key] += HeadshotHitPair.Value * HeadshotDamage : DamageMap.Emplace(HeadshotHitPair.Key, HeadshotHitPair.Value * HeadshotDamage);
				HitCharacters.AddUnique(HeadshotHitPair.Key);
			}
		}

		// gets and applies total damage
		for (auto DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					UGameplayStatics::ApplyDamage(
					DamagePair.Key, // character that was hit
					DamagePair.Value, // Damage calculated above
					InstigatorController,
					this,
					UDamageType::StaticClass()
					);
				}
			}
		}
		
		if (!HasAuthority() && bUseServerSideRewind)
		{
			BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
			if (BlasterOwnerCharacter && BlasterOwnerCharacter->GetLagCompensationComponent() && BlasterOwnerController && BlasterOwnerCharacter->IsLocallyControlled())
			{
				BlasterOwnerCharacter->GetLagCompensationComponent()->ShotgunServerScoreRequest(
					HitCharacters,
					Start,
					HitTargets,
					BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime,
					this
				);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		FVector ToEndLoc = EndLoc - TraceStart;
		ToEndLoc = TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size();
	
		HitTargets.Add(TraceEndWithScatter(HitTarget));
	}
}
