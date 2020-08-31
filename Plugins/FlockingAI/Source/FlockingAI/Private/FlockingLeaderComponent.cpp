// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockingLeaderComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
AActor* UFlockingLeaderComponent::Owner=nullptr;
// Sets default values for this component's properties
UFlockingLeaderComponent::UFlockingLeaderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFlockingLeaderComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();

	// ...
	
}


// Called every frame
void UFlockingLeaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (target)
	{

		FRotator rotator = UKismetMathLibrary::RLerp(
			Owner->GetActorRotation(),
			UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), target->GetActorLocation()),
			turnspeed * 0.005f,
			true
		);
		Owner->SetActorRotation(rotator);
		Owner->AddActorWorldOffset(Owner->GetActorForwardVector() * speed);
		// ...
	}

}

