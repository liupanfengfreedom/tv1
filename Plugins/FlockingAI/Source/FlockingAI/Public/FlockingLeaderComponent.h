// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlockingLeaderComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLOCKINGAI_API UFlockingLeaderComponent : public UActorComponent
{
	GENERATED_BODY()
		static AActor* Owner;

public:	
	// Sets default values for this component's properties
	UFlockingLeaderComponent();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Factor")
		AActor* target;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Factor")
		int turnspeed = 1;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Factor")
		int speed = 1;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	static AActor* getsingleston()
	{
		return Owner;
	}
		
};
