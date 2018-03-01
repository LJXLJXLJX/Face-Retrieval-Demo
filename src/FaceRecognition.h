#pragma once
#include "stdafx.h"
#include "arcsoft_fsdk_face_recognition.h"
#include "merror.h"
#include "FaceDetection.h"
#include <tchar.h>  

#pragma comment(lib,"libarcsoft_fsdk_face_recognition.lib")

using namespace std;

#define WORKBUF_SIZE        (40*1024*1024)


#ifdef FACECOMPAREDLL_EXPORTS
#define FACECOMPAREDLL_API __declspec(dllexport) 
#else
#define FACECOMPAREDLL __declspec(dllimport) 
#endif





extern"C"
{
	void FACECOMPAREDLL addToFaceLib();
	float FACECOMPAREDLL faceCompare(char* face1, char* face2, bool unInit);
	char FACECOMPAREDLL *faceSearch(char* face);
	void FACECOMPAREDLL faceSearchApp();
}