// Fill out your copyright notice in the Description page of Project Settings.


#include "SSTVCamera.h"

const char* ConvertToFString(const FString& InFString)
{
    // Convert FString to UTF-8 encoded string
    std::string Utf8String(TCHAR_TO_UTF8(*InFString));
   // std::string NStr(UTF8_TO_TCHAR(Utf8String.c_str()));
    // Return const char* by calling c_str()
    return Utf8String.c_str();
}


USSTVCamera::USSTVCamera()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USSTVCamera::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void USSTVCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void USSTVCamera::ImageToAudio(FString Path)
{
    

    //код для записи по актуальности
    

    FString ImageFileName = Path + TEXT(".png");
    FString SaveFileName = Path + TEXT(".wav");

    UE_LOG(LogTemp, Warning, TEXT("IFN: %s"), TCHAR_TO_ANSI(*ImageFileName));
    UE_LOG(LogTemp, Warning, TEXT("SFN: %s"), *SaveFileName);

    //code for export render target to png
    
	int result = image_sound(TCHAR_TO_ANSI(*ImageFileName), TCHAR_TO_ANSI(*SaveFileName));
    UE_LOG(LogTemp, Warning, TEXT("SSTV Result: %d"), result);
}

int USSTVCamera::GetLastIndex(FString& Path)
{
    // Get the Documents directory path for the current user
    FString DocumentsPath = FPlatformProcess::UserDir();// +TEXT("Documents");

    // Check if the StarPawsGame folder exists
    FString StarPawsGameFolderPath = DocumentsPath / TEXT("StarPawsGame");
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();


    if (!PlatformFile.DirectoryExists(*StarPawsGameFolderPath))
    {
        // Create the StarPawsGame folder
        PlatformFile.CreateDirectory(*StarPawsGameFolderPath);
        UE_LOG(LogTemp, Warning, TEXT("StarPawsGame folder created successfully!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("StarPawsGame folder already exists!"));
    }

    // Check if the StarPawsGame folder exists
    FString SSTVFolder = StarPawsGameFolderPath / TEXT("SSTV");

    if (!PlatformFile.DirectoryExists(*SSTVFolder))
    {
        // Create the StarPawsGame folder
        PlatformFile.CreateDirectory(*SSTVFolder);
        UE_LOG(LogTemp, Warning, TEXT("SSTV folder created successfully!"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SSTV folder already exists!"));
    }
    FString name = "Signal_";
    int num = 1;
    while (true)
    {
        FString FilePath = SSTVFolder / name + FString::FromInt(num);
        if (!PlatformFile.FileExists(*FilePath)) {
            UE_LOG(LogTemp, Warning, TEXT("Path: %s"), *FilePath);
            Path = FilePath;
            return num;
        }
        ++num;
    }

    return num;
}


