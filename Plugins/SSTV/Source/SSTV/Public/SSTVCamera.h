// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "SSTVLibrary/SSTV_LIB/sstv_main.h"
#include "SSTVCamera.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SSTV_API USSTVCamera : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USSTVCamera();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable)
	void ImageToAudio(FString Path);

	UFUNCTION(BlueprintCallable)
	int GetLastIndex(FString &Path);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneCaptureComponent2D* CaptureComponent;

private:
	UTextureRenderTarget2D* RenderTarget;
};
