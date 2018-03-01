#pragma once


#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "DirControl.h"
#include <iostream>
#include <vector>
#include <string>
#include <io.h>
#include <direct.h>
#include <Windows.h>


#ifdef FACEDETECTIONDLL_EXPORTS
#define FACEDETECTIONDLL_API __declspec(dllexport) 
#else
#define FACEDETECTIONDLL __declspec(dllimport) 
#endif

using namespace std;
using namespace cv;
//������⣬�ܹ����֤��ɨ��ͼ�е���������������ͷ�����м�⵽���������бȽϣ�����FaceCompare���еķ�����
class  FaceDetection
{
public:
	//��ʼ��������·���µķ�����
	FaceDetection();
	~FaceDetection();
	//������ͷ�ɼ�����ͼ�񣬲�����Ƶ��ʾ�ù��̡�������ģʽ������Աģʽ0���ֶ��ɼ������û�ģʽ1���Զ��ɼ���
	int camera_face(int mode = 1);
	//��ȡɨ�赽��֤������ͼ��
	int scanner_face(string path);


private:
	//����ͷ���洰������
	const char* window_name = "Capture - Face detection";
	//haar����������·��
	string cascade_path;
	//����������ʵ��
	CascadeClassifier face_cascade;
	//ÿ֡��ֻ���������Ĳ���
	Mat face_frame;
	//ÿ֡ͷ����(ʵ���Ͼ��Ǵ�һЩ����ѡ����)
	Mat face_big_frame;
	//���һ֡�����е��������þ���Ȧ������ʾ
	void detectAndDisplay(Mat frame, int mode);
	//����һ��Ŀ¼������ʵ��������Ŀ¼����
	DirControl dir_ctrl;
	//���ŵ�֡������֡������һ��������δ���ͨ������ֹ���򣬺��й�����Ա��
	int frame_count = 0;
	//�ɼ�����������
	int face_count = 0;
};

//dll�ṩ�Ľӿ�
extern"C"
{
	//mode=0 �ֶ� ;1 �Զ�
	int FACEDETECTIONDLL cameraFace(int mode=0);
}