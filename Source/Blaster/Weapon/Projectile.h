// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UProjectileMovementComponent;
class UBoxComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USoundCue;
class ABlasterCharacter;


UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	/**
	 * Used with server-side rewind
	 */
	bool bUseServerSideRewind{false};
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;

	UPROPERTY(EditAnywhere)
	float InitialSpeed{30000.f};
	
	float Damage{20.f};

protected:
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SetImpactEffect(bool bPlayerHit);

	void SpawnTrailSystem();
	void ExplodeDamage();

	UPROPERTY(EditAnywhere)
	float MinDamage{10.f};

	UPROPERTY(EditAnywhere)
	float InnerRadius{200.f};

	UPROPERTY(EditAnywhere)
	float OuterRadius{500.f};

	UPROPERTY(EditAnywhere)
	float DamageFalloff{1.f};

	UPROPERTY(EditAnywhere)
	UParticleSystem* ObjectImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* ObjectImpactSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* PlayerImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* PlayerImpactSound;

	UPROPERTY()
	UParticleSystem* CurrentImpactParticles;

	UPROPERTY()
	USoundCue* CurrentImpactSound;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;
	
	UPROPERTY(VisibleAnywhere)
    UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* TrailSystem;

	UPROPERTY()
	UNiagaraComponent* TrailComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;
	
private:
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	UPROPERTY()
	UParticleSystemComponent* TracerComponent;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime{3.f};
	
	

public:	
	

};
