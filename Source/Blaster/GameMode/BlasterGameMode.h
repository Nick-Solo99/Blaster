// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

namespace MatchState
{
	extern BLASTER_API const FName Cooldown; // match duration has been reached. display winner and begin cooldown timer.
}

class ABlasterPlayerController;
class ABlasterPlayerState;
class ABlasterCharacter;
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ABlasterGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	void PlayerLeftGame(ABlasterPlayerState* PlayerLeaving);
	void SendChatMessage(const FString& User, const FString& Message);
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);
	
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime{10.f};

	UPROPERTY(EditDefaultsOnly)
	float MatchTime{120.f};

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime{10.f};

	float LevelStartingTime{0.f};

	bool bTeamsMatch{false};
	
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
	
private:
	float CountdownTime{0.f};

public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};
