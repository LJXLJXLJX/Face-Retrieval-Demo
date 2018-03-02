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
	int FACECOMPAREDLL FeatureExact(const char* input_image_path, AFR_FSDK_FACEMODEL &faceModels, bool unInit);
	float FACECOMPAREDLL faceCompare(const char* face1, const char* face2, bool unInit);
	float FACECOMPAREDLL faceToFeatureCompare(const char* face, AFR_FSDK_FACEMODEL faceModel, bool unInit);

	char  FACECOMPAREDLL * faceSearch(const char* face);
	void FACECOMPAREDLL faceSearchApp();

	void FACECOMPAREDLL addOneToFeatureLib(const char* peopleName, const char* input_image_path, bool uninit);
	AFR_FSDK_FACEMODEL FACECOMPAREDLL loadLocalFeature(const char* name);
}