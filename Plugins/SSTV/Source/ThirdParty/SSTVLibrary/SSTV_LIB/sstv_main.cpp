#include "sstv_main.h"
#include <iostream>
#include <Windows.h>
#include <commdlg.h>

#include "SSTV.h"
#include "wav.h"
#include "modes.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define VERSION "1.10"

#if defined _WIN32 || defined _WIN64
#include <Windows.h>

#define SSTVLIB_EXPORT __declspec(dllexport)
#else
#include <stdio.h>
#endif

#ifndef SSTVLIB_EXPORT
#define SSTVLIB_EXPORT
#endif

SSTVLIB_EXPORT int sum(int x, int y)
{
	return  x + y;
}

//gets the substring after the last '/' character
const char* getFilenameFromPath(const char* path) {
	const char* filename = path;
	for (size_t x = strlen(path); x > 0; x--) {
		if (path[x] == '\/') {
			filename = &path[x + 1];
		}
	}
	return filename;
}

int resizeNN(SSTV::rgb** input, vec2 inputSize, vec2 newSize) {
	//dont need to do anything if its already the right size, return the origional to save memory
	if (inputSize == newSize) { return 0; }

	printf_s("[Resizing: %ix%i ==> %ix%i]\n", inputSize.X, inputSize.Y, newSize.X, newSize.Y);

	SSTV::rgb* output = new SSTV::rgb[newSize.Y * newSize.X];
	if (!output) { return -1; }

	//calc scale values
	float xScale = (float)newSize.X / (float)inputSize.X;
	float yScale = (float)newSize.Y / (float)inputSize.Y;

	for (int y = 0; y < newSize.Y; y++) {
		for (int x = 0; x < newSize.X; x++) {
			//get the nearest pixel in the input image using the x and y scale values
			int writeIndex = y * newSize.X + x;
			int readIndex = (int)(y / yScale) * inputSize.X + (int)(x / xScale);

			//set the pixel to the closest value, avoid any over/underflows. VS still complains about the possibility.
			if (writeIndex <= (newSize.Y * newSize.X) && readIndex <= (inputSize.X * inputSize.Y) && writeIndex >= 0 && readIndex >= 0) {
				output[writeIndex] = (*input)[readIndex];
			}
		}
	}

	*input = output;
	return 1;
}


SSTVLIB_EXPORT int image_sound(const char* ipath, const char* opath)
{
	//output file pointer
	FILE* ofptr = 0;


	//process other arguments
	encMode* selectedEncMode = 0;
	bool validEncMode = false;

	char* inputPath = 0;
	char* outputPath = 0;
	char* callsign = 0;

	bool playback = false;
	int playbackDevice = -1;

	bool setSampleRate = false;
	int sampleRate = 8000;
	int standardSampleRates[] = { 8000, 11025, 16000, 22050, 32000, 44100, 48000, 96000 };
	bool usingStandardSampleRate = false;

	inputPath = (char*)ipath;
	outputPath = (char*)opath;

	selectedEncMode = &R72;
	validEncMode = true;


	//find -P switch and device
	//	playbackDevice = strtol(argv[i + 1], NULL, 10);
	//	playback = true;

		//sampleRate = strtol(argv[i + 1], NULL, 10);

	if (sampleRate <= 0) {
		printf_s("[Invalid sample rate, using default]\n");
		sampleRate = 8000;
	}

	for (int sr : standardSampleRates) {
		if (sr == sampleRate) {
			usingStandardSampleRate = true;
		}
	}

	if (!usingStandardSampleRate) {
		printf_s("[Non-standard sample rate set, expect possible issues]\n");
	}

	setSampleRate = true;
	

	//validation stuff
	if (!inputPath || !(playback || outputPath) || !validEncMode) {
		
		return 0;
	}

	//make sure the encode mode is valid
	if (!selectedEncMode) {
		printf_s("[ERR] Invalid encode method, see -M");
		return 0;
	}

	//init wav system
	if (!wav::init(sampleRate)) {
		printf_s("[ERR] Could not allocate WAV memory\n");
		return 0;
	}

	//begin encode
	printf_s("[Beginning %s generation @ %iHz]\n", selectedEncMode->code, wav::header.sampleRate);

	//read input image
	vec2 imgSize = { 0, 0 };
	int imgChannels = 4;
	SSTV::rgb* rgbBuffer = nullptr;

	//stbi will load most image types, dont need to determin which load function to use anymore
	rgbBuffer = (SSTV::rgb*)stbi_load(inputPath, &imgSize.X, &imgSize.Y, &imgChannels, 3);

	if (!rgbBuffer || imgSize.X <= 0 || imgSize.Y <= 0) {
		printf_s("[ERR] Could not read source file\n");
		return 0;
	}

	//open output file
	if (outputPath) {
		int openErrNo = fopen_s(&ofptr, outputPath, "wb");
		if (openErrNo != 0) {
			char errBuffer[256] = {};
			strerror_s(errBuffer, openErrNo);
			printf_s("[ERR] Could not open output file (%s)\n", errBuffer);
			return 0;
		}
	}

	//resize if required
	if (resizeNN(&rgbBuffer, imgSize, selectedEncMode->size) < 0) {
		printf_s("[ERR] Error occured during resizing\n");
		return 0;
	}


	//add 500ms header
	wav::addTone(0, 500.f);

	//add VOX tone
	SSTV::addVoxTone();

	//call actual encode function
	selectedEncMode->ec(rgbBuffer);

	//add 500ms footer
	wav::addTone(0, 500.f);

	//save file if requested and print info
	if (outputPath) {
		if (wav::save(ofptr) <= 0) {
			char errBuffer[256] = {};
			strerror_s(errBuffer, errno);
			printf_s("[ERR] Issue opening output file (%s)\n", errBuffer);
			return 0;
		}
		printf_s("[Encode complete, wrote %i bytes to %s]\n", wav::header.fileSize, getFilenameFromPath(outputPath));
	}
	else {
		printf_s("[Encode complete, storing %i bytes]\n", wav::header.fileSize);
	}

	printf_s(" Expected time: %f MS\n", wav::expectedDurationMS);
	printf_s(" Actual time  : %f MS\n", wav::actualDurationMS);
	printf_s(" Added: %i, Skipped: %i\n", wav::balance_AddedSamples, wav::balance_SkippedSamples);

	return 1;
}
