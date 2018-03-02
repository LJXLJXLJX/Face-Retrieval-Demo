#pragma once
#include "FaceRecognition.h"
#include "utils.h"


int face_right = 0;	//人脸图片宽度
int face_bottom = 0;	//人脸图片高度
map<string, AFR_FSDK_FACEMODEL> featureLib;

char APPID[256] = "5iXhcEooxVKLaQawNJjgw94WPQrgT7LHaEiEJNEii4zr";
char SDKKey[256] = "H5BiCmnj2xGABZnJrD1iRcdG5dCwKPuZVjYVDXZxeAQg";
MRESULT nRet;
MHandle hEngine;
MByte *pWorkMem;


int initEngine()
{
	if (hEngine != nullptr)
		return -1;
	//初始化
	nRet = MERR_UNKNOWN;
	hEngine = nullptr;
	pWorkMem = (MByte *)malloc(WORKBUF_SIZE);

	if (pWorkMem == nullptr)
	{
		fprintf(stderr, "fail to malloc workbuf\r\n");
		//system("pause");
		return -1;
	}
	nRet = AFR_FSDK_InitialEngine(APPID, SDKKey, pWorkMem, WORKBUF_SIZE, &hEngine);
	if (nRet != MOK || hEngine == nullptr)
	{
		fprintf(stderr, "InitialFaceEngine failed , errorcode is %d \r\n", nRet);
		//system("pause");
		return -1;
	}
}

int unInitEngine()
{
	if (hEngine == nullptr)
		return -1;
	//反初始化
	nRet = AFR_FSDK_UninitialEngine(hEngine);
	if (nRet != MOK)
	{
		fprintf(stderr, "UninitialFaceEngine failed , errorcode is %d \r\n", nRet);
	}
	hEngine = nullptr;
	free(pWorkMem);
}

//读取bmp图片
bool readBmp(const char* path, uint8_t **imageData, int *pWidth, int *pHeight)
{
	if (path == NULL || imageData == NULL || pWidth == NULL || pHeight == NULL)
	{
		fprintf(stderr, "ReadBmp para error\r\n");
		//system("pause");
		return false;
	}
	FILE *fp = fopen(path, "rb");
	if (fp == 0)
	{
		fprintf(stderr, "Bmp file open failed\r\n");
		//system("pause");
		return false;
	}

	fseek(fp, sizeof(BITMAPFILEHEADER), 0);
	BITMAPINFOHEADER head;
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);
	*pWidth = head.biWidth;
	*pHeight = head.biHeight;

	face_right = head.biWidth;
	face_bottom = head.biHeight;


	int biBitCount = head.biBitCount;
	int lineByte = ((*pWidth) * biBitCount / 8 + 3) / 4 * 4;
	*imageData = (uint8_t *)malloc(lineByte * (*pHeight));

	for (int i = 0; i < *pHeight; i++)
	{
		fseek(fp, (*pHeight - 1 - i) * lineByte + 54, SEEK_SET);
		fread(*imageData + i * (*pWidth) * 3, 1, (*pWidth) * 3, fp);
	}
	fclose(fp);
	return true;
}
//人类图片(bmp)特征提取
int FeatureExact(const char* input_image_path, AFR_FSDK_FACEMODEL &faceModels, bool unInit)
{
	initEngine();
	ASVLOFFSCREEN offInput = { 0 };
	offInput.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
	offInput.ppu8Plane[0] = nullptr;
	if (!readBmp(input_image_path, (uint8_t**)&offInput.ppu8Plane[0], &offInput.i32Width, &offInput.i32Height))
		return -1;
	if (!offInput.ppu8Plane[0])
	{
		fprintf(stderr, "fail to ReadBmp(%s)\r\n", input_image_path);
		AFR_FSDK_UninitialEngine(hEngine);
		free(pWorkMem);
		//system("pause");
		return -1;
	}
	offInput.pi32Pitch[0] = offInput.i32Width * 3;

	AFR_FSDK_FACEINPUT faceResult;
	//人脸信息通过face detection\face tracking获得
	faceResult.lOrient = AFR_FSDK_FOC_0;//人脸方向
	faceResult.rcFace.left = 0;//人脸框位置
	faceResult.rcFace.top = 0;
	faceResult.rcFace.right = face_right - 1;
	faceResult.rcFace.bottom = face_bottom - 1;

	//提取人脸特征
	AFR_FSDK_FACEMODEL LocalFaceModels = { 0 };
	nRet = AFR_FSDK_ExtractFRFeature(hEngine, &offInput, &faceResult, &LocalFaceModels);	//提取的特征在LocalFaceModels中
	if (nRet != MOK)
	{
		fprintf(stderr, "fail to Extract 1st FR Feature, error code: %d\r\n", nRet);
	}
	//将LocalFaceModels拷贝到faceModels
	faceModels.lFeatureSize = LocalFaceModels.lFeatureSize;
	faceModels.pbFeature = (MByte*)malloc(faceModels.lFeatureSize);
	memcpy(faceModels.pbFeature, LocalFaceModels.pbFeature, faceModels.lFeatureSize);
	free(offInput.ppu8Plane[0]);	//用完之后释放

	if (unInit)
		unInitEngine();
	return 0;
}

//将一副图提取出的特征存储到本地
void addOneToFeatureLib(const char* peopleName, const char* input_image_path, bool uninit)
{
	//初始化引擎
	initEngine();
	//创建存放特征文件的文件夹
	if (_access("../featureLib/", 0) == -1)
	{
		_mkdir("../featureLib/");
	}
	AFR_FSDK_FACEMODEL faceModel = { 0 };
	if (FeatureExact(input_image_path, faceModel, false) == -1)
		return;
	string strName(peopleName);
	string path = "../featureLib/" + strName;
	FILE *file;
	file = fopen(path.c_str(), "wb");
	fwrite(faceModel.pbFeature, sizeof(MByte), faceModel.lFeatureSize, file);
	fclose(file);

	//反初始化
	if (uninit)
		unInitEngine();
	free(faceModel.pbFeature);
	return;
}

AFR_FSDK_FACEMODEL loadLocalFeature(const char* name)
{
	AFR_FSDK_FACEMODEL faceModel = { 0 };
	faceModel.lFeatureSize = 22020;
	faceModel.pbFeature = new MByte[22020];
	string featurePath = name;
	featurePath = "../featureLib/" + featurePath;
	FILE *file;
	file = fopen(featurePath.c_str(), "rb");
	fread(faceModel.pbFeature, sizeof(MByte), 22020, file);
	Sleep(0);
	fclose(file);
	return faceModel;
}


//从本地文件读取特征
//int readFeature(char* name, AFR_FSDK_FACEMODEL &faceModel)
//{
//	string path = "../features/";
//	string strName(name);
//	path = path + name;
//	ifstream in(path, ios::binary);
//	faceModel = { 0 };
//	faceModel.lFeatureSize = 22020;
//	//char* name = new char;
//	faceModel.pbFeature = (MByte*)malloc(faceModel.lFeatureSize);
//	for (int i = 0; i < faceModel.lFeatureSize; i++)
//	{
//		faceModel.pbFeature[i] = in.get();
//	}
//	in.close();
//
//
//	ofstream out("../features/LLJJXX");
//	//out << name;
//	for (int i = 0; i < faceModel.lFeatureSize; i++)
//	{
//		out.put(faceModel.pbFeature[i]);
//	}
//	out.close();
//	free(faceModel.pbFeature);
//	return 0;
//}
//

//通过摄像头添加人脸到本地人脸库
void addToFaceLib()
{
	int exitFlag = 0;
	while (1)
	{
		string name("");
		cout << "请输入名字:";
		cin >> name;
		cout << endl;
		cout << endl << "采集人脸时按下s键拍照，c关闭窗口" << endl;
		//system("pause");
		cameraFace();
		string path = "../faceLib/" + name + ".bmp";
		Mat face = cv::imread("../camera_face/face.bmp");
		cv::imwrite(path, face);
		cout << "输入1退出 否则继续录入" << endl;
		cin >> exitFlag;
		cout << endl;
		if (exitFlag == 1)
			break;
	}
}


//载入本地人脸库
int loadLocalFaceLib(map<string, AFR_FSDK_FACEMODEL> &featureLib)
{
	string faceLibDir = "../faceLib/";
	vector<string> files(0);
	getFiles(faceLibDir, files);
	for (string filename : files)
	{
		string peopleName(filename.begin(), filename.end() - 4);
		string filePath = faceLibDir + filename;

		AFR_FSDK_FACEMODEL faceModel = { 0 };
		FeatureExact((char*)filePath.c_str(), faceModel, false);
		featureLib[peopleName] = faceModel;

		featureLib[peopleName].lFeatureSize = faceModel.lFeatureSize;
		featureLib[peopleName].pbFeature = (MByte*)malloc(featureLib[peopleName].lFeatureSize);
		memcpy(featureLib[peopleName].pbFeature, faceModel.pbFeature, featureLib[peopleName].lFeatureSize);
		free(faceModel.pbFeature);
	}

	return 0;
}

float faceCompare(const char* face1, const char* face2, bool unInit)
{
	//初始化
	initEngine();

	//获取版本信息
	//const AFR_FSDK_Version * pVersionInfo = nullptr;
	//pVersionInfo = AFR_FSDK_GetVersion(hEngine);
	//fprintf(stdout, "%d %d %d %d %d\r\n", pVersionInfo->lCodebase, pVersionInfo->lMajor, pVersionInfo->lMinor, pVersionInfo->lBuild, pVersionInfo->lFeatureLevel);
	//fprintf(stdout, "%s\r\n", pVersionInfo->Version);
	//fprintf(stdout, "%s\r\n", pVersionInfo->BuildDate);
	//fprintf(stdout, "%s\r\n", pVersionInfo->CopyRight);

	//抽取特征1
	AFR_FSDK_FACEMODEL faceModels1 = { 0 };
	FeatureExact(face1, faceModels1, false);
	//抽取特征2
	AFR_FSDK_FACEMODEL faceModels2 = { 0 };
	FeatureExact(face2, faceModels2, false);
	//对比两张人脸特征，获得比对结果
	MFloat  fSimilScore = 0.0f;
	nRet = AFR_FSDK_FacePairMatching(hEngine, &faceModels1, &faceModels2, &fSimilScore);
	if (nRet == MOK)
	{
		//fprintf(stdout, "fSimilScore =  %f\r\n", fSimilScore);
	}
	else
	{
		fprintf(stderr, "FacePairMatching failed , errorcode is %d \r\n", nRet);
	}

	//反初始化
	if (unInit)
		unInitEngine();

	free(faceModels1.pbFeature);
	free(faceModels2.pbFeature);
	return fSimilScore;
}

float faceToFeatureCompare(const char * face, AFR_FSDK_FACEMODEL faceModel, bool unInit)
{
	//初始化
	initEngine();
	//抽取特征1
	AFR_FSDK_FACEMODEL faceModels1 = { 0 };
	FeatureExact(face, faceModels1, false);

	MFloat  fSimilScore = 0.0f;
	nRet = AFR_FSDK_FacePairMatching(hEngine, &faceModels1, &faceModel, &fSimilScore);
	if (nRet == MOK)
	{
		//fprintf(stdout, "fSimilScore =  %f\r\n", fSimilScore);
	}
	else
	{
		fprintf(stderr, "FacePairMatching failed , errorcode is %d \r\n", nRet);
	}

	//反初始化
	if (unInit)
		unInitEngine();

	free(faceModels1.pbFeature);
	return fSimilScore;
}

char* faceSearch(const char* face)
{
	//初始化
	initEngine();

	AFR_FSDK_FACEMODEL faceModels1 = { 0 };
	AFR_FSDK_FACEMODEL faceModels2 = { 0 };

	FeatureExact(face, faceModels1, false);
	loadLocalFaceLib(featureLib);

	char* predictPeople = "";
	string libDir = "../faceLib/";
	float maxSim = 0;
	float sim = 0;

	for (auto i = featureLib.begin(); i != featureLib.end(); i++)
	{
		faceModels2 = i->second;
		nRet = AFR_FSDK_FacePairMatching(hEngine, &faceModels1, &faceModels2, &sim);
		if (nRet == MOK)
		{//fprintf(stdout, "fSimilScore =  %f\r\n", fSimilScore);
		}
		else
			fprintf(stderr, "FacePairMatching failed , errorcode is %d \r\n", nRet);
		if (sim > maxSim)
		{
			maxSim = sim;
			predictPeople = (char*)i->first.c_str();
		}
		if (maxSim < 0.5)
			predictPeople = "Not in Lib!";
	}


	unInitEngine();
	free(faceModels1.pbFeature);
	free(faceModels2.pbFeature);
	return predictPeople;
}

void faceSearchApp()
{
	//初始化
	initEngine();
	AFR_FSDK_FACEMODEL faceModels1 = { 0 };
	AFR_FSDK_FACEMODEL faceModels2 = { 0 };
	loadLocalFaceLib(featureLib);
	while (1)
	{
		cameraFace();
		char* face = "../camera_face/face.bmp";
		//刚刚没采集 退出循环
		if (_access(face, 0) == -1)
			break;

		FeatureExact(face, faceModels1, false);
		char* predictPeople = "";
		string libDir = "../faceLib/";
		float maxSim = 0;
		float sim = 0;
		for (auto i = featureLib.begin(); i != featureLib.end(); i++)
		{
			faceModels2 = i->second;
			nRet = AFR_FSDK_FacePairMatching(hEngine, &faceModels1, &faceModels2, &sim);
			if (nRet == MOK)
			{//fprintf(stdout, "fSimilScore =  %f\r\n", fSimilScore);
			}
			else
				fprintf(stderr, "FacePairMatching failed , errorcode is %d \r\n", nRet);
			if (sim > maxSim)
			{
				maxSim = sim;
				predictPeople = (char*)i->first.c_str();
			}
			if (maxSim < 0.5)
				predictPeople = "Not in Lib!";
		}
		LPCSTR message = predictPeople;
		MessageBox(NULL, message, _T("检索结果"), MB_OK);
	}
	unInitEngine();
}
