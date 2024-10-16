// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

class UPauseMenu;
class UInputAction;
class UInputMappingContext;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

class ABlasterGameMode;
class UCharacterOverlay;
class ABlasterHUD;
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void DisplayDefeatMessage(const FString& Message);
	void SetHUDWeaponAmmo(int32 Ammo, int32 MagCap);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDCarriedWeapon(EWeaponType WeaponType);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDGrenades(int32 Grenades);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetServerTime(); // synced with server world time
	virtual void ReceivedPlayer() override; // Sync with server time asap
	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCooldown();

	float SingleTripTime{0.f};

	FHighPingDelegate HighPingDelegate;

	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	void CheckTimeSync(float DeltaSeconds);
	void SetHUDTime();
	void PollInit();

	/**
	 * Sync Time between client and server
	 */

	//Requests the current server time, passing in the clients time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	//Reports current server time to client in response to request server time
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	//Delta between client and server time
	float ClientServerDelta{0.f};

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency{5.f};

	float TimeSyncRunningTime{0.f};

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaSeconds);

	/**
	 * Input
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* BlasterPlayerControllerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* OpenMenuAction;

	void ShowPauseMenu();

	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);
private:
	/**
	 * Menu
	 */
	UPROPERTY(EditAnywhere, Category = HUD)
	TSubclassOf<UUserWidget> PauseMenuWidget;

	UPROPERTY()
	UPauseMenu* PauseMenu;

	bool bPauseMenuOpen{false};

	
	UPROPERTY()
	ABlasterHUD* BlasterHUD;

	UPROPERTY()
	ABlasterGameMode* BlasterGameMode;

	float LevelStartingTime{0.f};
	float MatchTime{0.f};
	float WarmupTime{0.f};
	float CooldownTime{0.f};
	uint32 CountdownInt{0};

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	UCharacterOverlay* CharacterOverlay;
	
	float HUDHealth;
	float HUDMaxHealth;
	bool bInitializeHealth{false};
	float HUDShield;
	float HUDMaxShield;
	bool bInitializeShield{false};
	float HUDScore;
	bool bInitializeScore{false};
	int32 HUDDefeats;
	bool bInitializeDefeats{false};
	int32 HUDGrenades;
	bool bInitializeGrenades{false};
	int32 HUDCarriedAmmo;
	bool bInitializeCarriedAmmo;
	int32 HUDWeaponAmmo;
	int32 HUDMagCap;
	bool bInitializeWeaponAmmo;
	EWeaponType HUDCarriedWeapon;
	bool bInitializeCarriedWeapon;

	float HighPingRunningTime{0.f};

	UPROPERTY(EditAnywhere)
	float HighPingDuration{5.f};

	float PingAnimationRunningTime{0.f};

	UPROPERTY(EditAnywhere)
	float CheckPingFrequency{20.f};

	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);

	UPROPERTY(EditAnywhere)
	float HighPingThreshold{50.f};
};
