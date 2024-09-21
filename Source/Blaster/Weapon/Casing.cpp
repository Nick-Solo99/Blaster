// Fill out your copyright notice in the Description page of Project Settings.

#include "Casing.h"

#include "Kismet/GameplayStatics.h"
#include "MetasoundSource.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);

	CasingMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CasingMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);

	bLifeSpanSet = false;
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CasingMesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
	
	CasingMesh->AddImpulse(CasingMesh->GetForwardVector() * ImpulseAmount);
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (bLifeSpanSet) return;
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	SetLifeSpan(1.5f);
	bLifeSpanSet = true;
}



