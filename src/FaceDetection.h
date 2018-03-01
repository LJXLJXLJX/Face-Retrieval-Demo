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
//人脸检测，能够检测证件扫描图中的人脸，并于摄像头画面中检测到的人脸进行比较（调用FaceCompare类中的方法）
class  FaceDetection
{
public:
	//初始化，载入路径下的分类器
	FaceDetection();
	~FaceDetection();
	//用摄像头采集人脸图像，并用视频显示该过程。有两种模式，管理员模式0（手动采集），用户模式1（自动采集）
	int camera_face(int mode = 1);
	//获取扫描到的证件人脸图像
	int scanner_face(string path);


private:
	//摄像头画面窗口名字
	const char* window_name = "Capture - Face detection";
	//haar级联分类器路径
	string cascade_path;
	//级联分类器实例
	CascadeClassifier face_cascade;
	//每帧中只包含人脸的部分
	Mat face_frame;
	//每帧头肩像(实际上就是大一些的扩选区域)
	Mat face_big_frame;
	//检测一帧画面中的人脸并用矩形圈出再显示
	void detectAndDisplay(Mat frame, int mode);
	//定义一个目录控制器实例来进行目录操作
	DirControl dir_ctrl;
	//播放的帧数（若帧数超过一定数量还未检测通过，终止程序，呼叫工作人员）
	int frame_count = 0;
	//采集到的人脸数
	int face_count = 0;
};

//dll提供的接口
extern"C"
{
	//mode=0 手动 ;1 自动
	int FACEDETECTIONDLL cameraFace(int mode=0);
}