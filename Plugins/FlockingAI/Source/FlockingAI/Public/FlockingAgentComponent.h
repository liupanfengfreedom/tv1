// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlockingAgentComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLOCKINGAI_API UFlockingAgentComponent : public UActorComponent
{
	GENERATED_BODY()
	AActor* owner;
	//AActor* target;
	TArray<AActor*> actortoignore;
	TArray<AActor*> actortoignore1;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes1;
	TArray<AActor*> outactors;
	TArray<FHitResult> OutHits;
	FHitResult OutHit;
	class RunnableThreadx* thread1;
	class RunnableThreadx* movetocenterthread;
	FTimerHandle th;
	void timerwork();
	FVector actorposition;
	FVector actordirection;
	bool obstacleisdetected = false;
	int radius = 100;
	int keepdis;
	static TMap<int, FVector> flockingcenters;
	static TMap<int, TSet<AActor*>> flocks;
	int flocknumber = 0;

public:	
	static void calculateflockcenter();
	// Sets default values for this component's properties
	UFlockingAgentComponent();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Factor")
			int visualrange = 800;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Factor")
		int speed = 2;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Factor")
		int turnspeed = 1;
	//0:360 , 0.5:180 , 1:90
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Factor")
		float viewangle = 0.8;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Factor")
		float Alignmentf = 0.53339f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Factor")
		float Cohesionf = 0.23331f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Factor")
		float Separationf = 0.2333f;
	void joinflock(int number);
	int  getflocknumber() {return flocknumber;}
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	FVector Alignment = FVector::ZeroVector;
	FVector Cohesion = FVector::ZeroVector;
	FVector Separation = FVector::ZeroVector;
	FVector Steerforce;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
