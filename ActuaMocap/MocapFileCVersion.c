//#include <stdio.h>
//#include <string.h>
//#include <stdint.h>
//#include "MocapFile.h"
//#define Windows
//#define MocapFileFloatSize 4
//
///// <summary>
///// read a mocap file into an array of floats
///// </summary>
///// <param name="filePath"> path </param>
///// <param name="lengthOut"> length IN FLOATS </param>
///// <param name="reverseEndianness">
///// do you want to reverse the order of bytes in each word read, 0 for no, (not zero) for yes
///// </param>
///// <returns>
///// ptr to array of floats DON'T FORGET TO FREE AFTER USE 
///// </returns>
//float* ReadMocapFileIntoFloats(const char* filePath, int* lengthOut, int reverseEndianness) {
//
//	FILE* file;
//	float* returnPtr = NULL;
//
//#ifdef Windows 
//	// this ifdef is because visual studio makes you use fopen_s which might not be available on other platforms.
//	// I don't see why they had to make the two so drastically different...
//	int errorCode = fopen_s(&file, filePath, "rb");
//	if (errorCode == 0) {
//#else
//	file = fopen(filePath, "rb");
//	if (file != NULL) {
//#endif
//		fseek(file, 0L, SEEK_END);
//		int size = ftell(file);
//		fseek(file, 0L, SEEK_SET);
//
//		if (size % MocapFileFloatSize > 0) {
//			printf("There's something wrong with your file - number of bytes is NOT divisible by 4");
//			return;
//		}
//
//		char* byteBuffer = (char*)malloc(size);
//		returnPtr = (float*)malloc(size);
//		char* writePointer = byteBuffer;
//		float* floatWritePointer = returnPtr;
//		int floatBufferSize = size / 4;
//		*lengthOut = floatBufferSize;
//		for (int i = 0; i < floatBufferSize; i++) {
//			char b1, b2, b3, b4;
//			fread(&b1, 1, 1, file);
//			fread(&b2, 1, 1, file);
//			fread(&b3, 1, 1, file);
//			fread(&b4, 1, 1, file);
//			if (reverseEndianness != 0) {
//				*writePointer++ = b4;
//				*writePointer++ = b3;
//				*writePointer++ = b2;
//				*writePointer++ = b1;
//			}
//			else {
//				*writePointer++ = b1;
//				*writePointer++ = b2;
//				*writePointer++ = b3;
//				*writePointer++ = b4;
//			}
//			float asAFloat = *((float*)&byteBuffer[((writePointer - byteBuffer) - 4)]);
//			*floatWritePointer++ = asAFloat;
//		}
//		if (byteBuffer != NULL) {
//			free(byteBuffer);
//		}
//	}
//	else {
//		printf("Failed to load file %s", filePath);
//	}
//	return returnPtr;
//}
//
///// <summary>
///// read a mocap file into an array of MocapFrame structs
///// </summary>
///// <param name="filePath"> path </param>
///// <param name="lengthOut"> length in MocapFrame structs </param>
///// <param name="reverseEndianness">
///// do you want to reverse the order of bytes in each word read, 0 for no, (not zero) for yes
///// </param>
///// <returns>
///// ptr to array of MocapFrame structs DON'T FORGET TO FREE AFTER USE 
///// </returns>
//MocapFrame* ReadMocapFrames(const char* filePath, int* lengthOut, int reverseEndianness) {
//	float* floats = ReadMocapFileIntoFloats(filePath, lengthOut, reverseEndianness);
//	int numFrames = *lengthOut / MocapFrameSizeFloats;
//	MocapFrame* returnVals = (MocapFrame*)malloc(sizeof(MocapFrame) * numFrames);
//	int onFrame = 0;
//	for (int i = 0; i < numFrames; i++) {
//		MocapFrame* frame = &returnVals[i];
//		float* frameStart = floats + MocapFrameSizeFloats * i;
//		float val = *frameStart;
//		if (val != 28.0f) {
//			printf("something wrong perhaps - all known files have frames that start with the number of points per frame - 28, this one starts with %f", *frameStart);
//		}
//		memcpy(frame->rawFloats, frameStart, PlayerPoints * sizeof(float) * 3 + 1);
//		for (int j = 0; j < PlayerPoints; j++) {
//			int startIndex = i * MocapFrameSizeFloats + j * 3 + 1;
//			MocapVec3 vec3 = {
//				floats[startIndex],
//				floats[startIndex + 1],
//				floats[startIndex + 2],
//			};
//			frame->points[j] = vec3;
//		}
//
//	}
//	*lengthOut = *lengthOut / MocapFrameSizeFloats;
//	if (floats) {
//		free(floats);
//	}
//	return returnVals;
//}
//
//MocapFile LoadMocapFile(const char* filePath, int reverseEndianness)
//{
//	MocapFile mocapFile;
//	mocapFile.frames = ReadMocapFrames(filePath, &mocapFile.numFrames, reverseEndianness);
//	return mocapFile;
//}
//
//void FreeMocapFile(MocapFile* file)
//{
//	free(file->frames);
//}
