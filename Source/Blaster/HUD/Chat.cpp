// Fill out your copyright notice in the Description page of Project Settings.


#include "Chat.h"

#include "Components/TextBlock.h"

void UChat::SetChatMessage(FString User, FString Message)
{
	FString ChatMessage = FString::Printf(TEXT(": %s"), *Message);
	if (UserText == nullptr || MsgText == nullptr) return;
	UserText->SetText(FText::FromString(User));
	MsgText->SetText(FText::FromString(ChatMessage));
}
