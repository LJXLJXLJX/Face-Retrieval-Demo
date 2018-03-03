#pragma once
#include "FaceRecognition.h"
#include "utils.h"


int face_right = 0;	//����ͼƬ���
int face_bottom = 0;	//����ͼƬ�߶�
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
	//��ʼ��
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
	//����ʼ��
	nRet = AFR_FSDK_UninitialEngine(hEngine);
	if (nRet != MOK)
	{
		fprintf(stderr, "UninitialFaceEngine failed , errorcode is %d \r\n", nRet);
	}
	hEngine = nullptr;
	free(pWorkMem);
}

//��ȡbmpͼƬ
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
//����ͼƬ(bmp)������ȡ
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
	//������Ϣͨ��face detection\face tracking���
	faceResult.lOrient = AFR_FSDK_FOC_0;//��������
	faceResult.rcFace.left = 0;//������λ��
	faceResult.rcFace.top = 0;
	faceResult.rcFace.right = face_right - 1;
	faceResult.rcFace.bottom = face_bottom - 1;

	//��ȡ��������
	AFR_FSDK_FACEMODEL LocalFaceModels = { 0 };
	nRet = AFR_FSDK_ExtractFRFeature(hEngine, &offInput, &faceResult, &LocalFaceModels);	//��ȡ��������LocalFaceModels��
	if (nRet != MOK)
	{
		fprintf(stderr, "fail to Extract 1st FR Feature, error code: %d\r\n", nRet);
	}
	//��LocalFaceModels������faceModels
	faceModels.lFeatureSize = LocalFaceModels.lFeatureSize;
	faceModels.pbFeature = (MByte*)malloc(faceModels.lFeatureSize);
	memcpy(faceModels.pbFeature, LocalFaceModels.pbFeature, faceModels.lFeatureSize);
	free(offInput.ppu8Plane[0]);	//����֮���ͷ�

	if (unInit)
		unInitEngine();
	return 0;
}

//��һ��ͼ��ȡ���������洢������
void addOneToFeatureLib(const char* peopleName, const char* input_image_path, bool uninit)
{
	//��ʼ������
	initEngine();
	//������������ļ����ļ���
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

	//����ʼ��
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


//�ӱ����ļ���ȡ����
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

//ͨ������ͷ�������������������
void addToFaceLib()
{
	int exitFlag = 0;
	while (1)
	{
		string name("");
		cout << "����������:";
		cin >> name;
		cout << endl;
		cout << endl << "�ɼ�����ʱ����s�����գ�c�رմ���" << endl;
		//system("pause");
		cameraFace();
		string path = "../faceLib/" + name + ".bmp";
		Mat face = cv::imread("../camera_face/face.bmp");
		cv::imwrite(path, face);
		cout << "����1�˳� �������¼��" << endl;
		cin >> exitFlag;
		cout << endl;
		if (exitFlag == 1)
			break;
	}
}


//���뱾��������
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
	//��ʼ��
	initEngine();

	//��ȡ�汾��Ϣ
	//const AFR_FSDK_Version * pVersionInfo = nullptr;
	//pVersionInfo = AFR_FSDK_GetVersion(hEngine);
	//fprintf(stdout, "%d %d %d %d %d\r\n", pVersionInfo->lCodebase, pVersionInfo->lMajor, pVersionInfo->lMinor, pVersionInfo->lBuild, pVersionInfo->lFeatureLevel);
	//fprintf(stdout, "%s\r\n", pVersionInfo->Version);
	//fprintf(stdout, "%s\r\n", pVersionInfo->BuildDate);
	//fprintf(stdout, "%s\r\n", pVersionInfo->CopyRight);

	//��ȡ����1
	AFR_FSDK_FACEMODEL faceModels1 = { 0 };
	FeatureExact(face1, faceModels1, false);
	//��ȡ����2
	AFR_FSDK_FACEMODEL faceModels2 = { 0 };
	FeatureExact(face2, faceModels2, false);
	//�Ա�����������������ñȶԽ��
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

	//����ʼ��
	if (unInit)
		unInitEngine();

	free(faceModels1.pbFeature);
	free(faceModels2.pbFeature);
	return fSimilScore;
}

float faceToFeatureCompare(const char * face, AFR_FSDK_FACEMODEL faceModel, bool unInit)
{
	//��ʼ��
	initEngine();
	//��ȡ����1
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

	//����ʼ��
	if (unInit)
		unInitEngine();

	free(faceModels1.pbFeature);
	return fSimilScore;
}

char* faceSearch(const char* face)
{
	//��ʼ��
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
	//��ʼ��
	initEngine();
	AFR_FSDK_FACEMODEL faceModels1 = { 0 };
	AFR_FSDK_FACEMODEL faceModels2 = { 0 };
	loadLocalFaceLib(featureLib);
	while (1)
	{
		cameraFace();
		char* face = "../camera_face/face.bmp";
		//�ո�û�ɼ� �˳�ѭ��
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
		MessageBox(NULL, message, _T("�������"), MB_OK);
	}
	unInitEngine();
}
