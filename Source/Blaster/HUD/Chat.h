// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Chat.generated.h"

class UTextBlock;
class UHorizontalBox;
/**
 * 
 */
UCLASS()
class BLASTER_API UChat : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetChatMessage(FString User, FString Message);
	
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* MsgBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UserText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MsgText;
	
protected:

private:
	
};
