// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockingAgentComponent.h"
#include "RunnableThreadx.h"
#include "Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "FlockingLeaderComponent.h"
TMap<int, FVector> UFlockingAgentComponent::flockingcenters;
TMap<int, TSet<AActor*>> UFlockingAgentComponent::flocks;
// Sets default values for this component's properties
UFlockingAgentComponent::UFlockingAgentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}
void UFlockingAgentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (thread1)
	{
		thread1->StopThread();
	}
	if (movetocenterthread)
	{
		movetocenterthread->StopThread();
	}
	flocks.Find(flocknumber)->Remove(owner);
}


// Called when the game starts
void UFlockingAgentComponent::BeginPlay()
{
	Super::BeginPlay();
	owner = GetOwner();
	FVector origin, box;
	owner->GetActorBounds(false, origin, box);
	radius = box.Size()*2.2f;
	keepdis = box.Size()*1.0f;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery2);
	ObjectTypes1.Add(EObjectTypeQuery::ObjectTypeQuery1);
	actortoignore.Add(owner);
	thread1 = new RunnableThreadx([=]() {
		FPlatformProcess::Sleep(0.02f);
		timerwork();
	});	
	movetocenterthread = new RunnableThreadx([=]() {
		FPlatformProcess::Sleep(1.02f);
		FVector direction = *flockingcenters.Find(flocknumber) - actorposition;
		AsyncTask(ENamedThreads::GameThread, [=]() {
			bool bt = UKismetSystemLibrary::LineTraceSingleForObjects(owner, actorposition, actorposition + (direction * radius), ObjectTypes1, true, actortoignore1, EDrawDebugTrace::Type::None, OutHit, true);
			if (bt)
			{
				UFlockingAgentComponent *fac = Cast<UFlockingAgentComponent>(OutHit.Actor->GetComponentByClass(UFlockingAgentComponent::StaticClass()));
				if (fac)
				{
					if (fac->getflocknumber() == getflocknumber())
					{
						//donothing
					}
					else
					{
						//goto center;
					}
				}
			}
			});

	});
}
void UFlockingAgentComponent::joinflock(int number)
{
	TSet<AActor*> * tp = flocks.Find(flocknumber);
	if (tp)
	{
		tp->Remove(owner);
	}
	flocknumber = number;
	TSet<AActor*> tap = flocks.FindOrAdd(number);
	tap.Add(owner);
}
void UFlockingAgentComponent::timerwork()
{
	actorposition = owner->GetActorLocation();
	actordirection = owner->GetActorForwardVector();
	if (!obstacleisdetected)
	{
		FVector startpoint = actorposition + (actordirection * (radius * viewangle));

		UKismetSystemLibrary::SphereOverlapActors(owner, startpoint, radius, ObjectTypes, AActor::StaticClass(), actortoignore, outactors);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		Alignment = FVector::ZeroVector;
		Cohesion = FVector::ZeroVector; //actorposition;
		Separation = FVector::ZeroVector;
		int neighborCount = 0;
		for (auto& TempNeighbour : outactors)
		{
			
			UFlockingAgentComponent* facomponent = (UFlockingAgentComponent*)TempNeighbour->GetComponentByClass(UFlockingAgentComponent::StaticClass());
			//UFlockingLeaderComponent* flcomponent = (UFlockingLeaderComponent*)TempNeighbour->GetComponentByClass(UFlockingLeaderComponent::StaticClass());
			if (!(facomponent/*|| flcomponent*/))
			{
	/*			AsyncTask(ENamedThreads::GameThread, [=]() {
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString("not desired actor"));

				});*/
				continue;
			}
			//if (flcomponent)
			//{
			//	Alignment += TempNeighbour->GetActorForwardVector()*100;//weight// *flcomponent->speed;
			//}
			else
			{
				neighborCount++;

				Alignment += TempNeighbour->GetActorForwardVector();// *facomponent->speed;

				Cohesion += TempNeighbour->GetActorLocation();
			}

		}
		if (neighborCount == 0)
		{

			return;
		}
		else
		{
				Alignment /= neighborCount;
				Alignment.Normalize();
				//////////////////////////////////////////////////////////////
				Cohesion /= (neighborCount + 0);
				FVector deltav = (Cohesion - actorposition);
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::SanitizeFloat(deltav.Size()));
				float triming = 0;
				if ((deltav.Size() - keepdis) > keepdis)
				{
					triming = 0.020;
				}
				else
				{
					triming = -0.020;
				}

				//AsyncTask(ENamedThreads::GameThread, [=]() {
				//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::SanitizeFloat(outcomponents.Size()));

				//	});
				///////////////////////////////////////////////////////
				deltav.Normalize();
				Cohesion = deltav;
				Separation = -deltav;

				Steerforce = (Alignment * Alignmentf) + (Cohesion * (Cohesionf+ triming)) + (Separation * (Separationf- triming));
				AActor* target = UFlockingLeaderComponent::getsingleston();
				if (target)
				{
					FVector fv = target->GetActorLocation() - owner->GetActorLocation();
					fv.Normalize();
					Steerforce = Steerforce * 0.93f + fv * 0.07f;
				}
				FRotator currentrotator = UKismetMathLibrary::RLerp(
					owner->GetActorRotation(),
					UKismetMathLibrary::MakeRotFromX(Steerforce),
					0.02f * turnspeed,
					false
				);
				AsyncTask(ENamedThreads::GameThread, [=]() {
					owner->SetActorRotation(currentrotator);
					});
		}

	}
}

// Called every frame
void UFlockingAgentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FVector rightvector = owner->GetActorRightVector();
	FVector upvector = owner->GetActorUpVector();

	owner->AddActorWorldOffset(actordirection * speed);


		 bool bforward= UKismetSystemLibrary::LineTraceSingleForObjects(owner, actorposition, actorposition + (actordirection * visualrange), ObjectTypes1, true, actortoignore1, EDrawDebugTrace::Type::None, OutHit, true);
		 bool bright=false;
		 bool bleft=false;
		 if (bforward)
		 {
			 int up=0, right=0;
			 bright = UKismetSystemLibrary::LineTraceSingleForObjects(owner, actorposition, actorposition + ((actordirection + rightvector) * visualrange), ObjectTypes1, true, actortoignore1, EDrawDebugTrace::Type::None, OutHit, true);
			 if (bright)
			 {
				 right -= 0.5f;
			 }
			 else
			 {
				 right += 0.5f;
			 }
			  bleft = UKismetSystemLibrary::LineTraceSingleForObjects(owner, actorposition, actorposition + ((actordirection - rightvector) * visualrange), ObjectTypes1, true, actortoignore1, EDrawDebugTrace::Type::None, OutHit, true);
			 if (bleft)
			 {
				 right += 0.5f;
			 }
			 else
			 {
				 right -= 0.5f;
			 }
			 right += 1;
			 //owner->AddActorLocalRotation(FRotator(1, right, 0));//for three dimension
			 owner->AddActorLocalRotation(FRotator(0, right, 0));
		 }
		 obstacleisdetected = bforward;// || bleft || bright;
}
void UFlockingAgentComponent::calculateflockcenter()
{
	static RunnableThreadx* thread2 = new RunnableThreadx([=]() {
		FPlatformProcess::Sleep(1.02f);
		bool bstopthread = true;
		for (auto var : flocks)// TSet<AActor*>
		{
			FVector center = FVector::ZeroVector;
			for (auto var1 : var.Value)//AActor*
			{
				center+=var1->GetActorLocation();
				bstopthread = false;
			}
			flockingcenters.FindOrAdd(var.Key) = center;
		}
		if (bstopthread)
		{
			thread2->StopThread();
		}
	});
}
