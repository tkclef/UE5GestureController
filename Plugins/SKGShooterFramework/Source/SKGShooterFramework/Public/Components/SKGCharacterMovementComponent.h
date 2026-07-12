// Copyright 2022, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SKGCharacterMovementComponent.generated.h"

class FSavedMove_SKGSF;
class FNetworkPredictionData_Client_SKGSF;

UCLASS()
class SKGSHOOTERFRAMEWORK_API USKGCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	friend FSavedMove_SKGSF;
public:
	USKGCharacterMovementComponent();
	virtual void BeginPlay() override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual float GetMaxSpeed() const override;

	UFUNCTION(BlueprintCallable, Category = "SKGCharacterMovement|Sprint")
	void StartSprinting() { bWantsToSprint = true; }
	UFUNCTION(BlueprintCallable, Category = "SKGCharacterMovement|Sprint")
	void StopSprinting() { bWantsToSprint = false; }
	UFUNCTION(BlueprintPure, Category = "SKGCharacterMovement|Sprint")
	bool GetWantsToSprint() const { return bWantsToSprint; }
	UFUNCTION(BlueprintPure, Category = "SKGCharacterMovement|Sprint")
	bool AreSprintingConditionsMet() const;
	UFUNCTION(BlueprintCallable, Category = "CharacterMovementExtended")
	bool IsVelocityWithinSprintAngleTolerance() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGCharacterMovement|Sprint")
	float MaxSprintSpeed {350.0f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGCharacterMovement|Sprint")
	float SprintAngleTolerance {0.25f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGCharacterMovement|Sprint")
	bool bOnlyAllowForwardSprint {true};
	
	bool bWantsToSprint {false};
};

class FSavedMove_SKGSF : public FSavedMove_Character
{
	typedef FSavedMove_Character Super;
	
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void Clear() override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* C) override;
	virtual uint8 GetCompressedFlags() const override;

	bool bSavedWantsToSprint {false};
};

class FNetworkPredictionData_Client_SKGSF : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;
	FNetworkPredictionData_Client_SKGSF(const UCharacterMovementComponent& ClientMovement);
	
	virtual FSavedMovePtr AllocateNewMove() override;
};