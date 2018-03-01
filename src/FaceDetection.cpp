#include "FaceDetection.h"
#include "DirControl.h"




FaceDetection::FaceDetection()
{
	cascade_path = "../resource/haarcascade_frontalface_alt.xml";
}

FaceDetection::~FaceDetection()
{
}



int FaceDetection::camera_face(int mode)
{
	dir_ctrl.check_dir("../camera_face/");//检查存放摄像头人脸图像的文件夹是否存在，若不存在，创建一个；若存在，清空
	VideoCapture capture;    //摄像头
	Mat frame;               //frame用于保存摄像头获取的每帧画面
	if (!face_cascade.load(cascade_path))
	{
		printf("--(!)Error loading\n");
		return 0;
	}


	//-- 2. 打开摄像头
	capture.open(0);
	int delay = 500;
	while (delay--);
	if (capture.isOpened())
	{
		namedWindow(window_name);
		while (1)  //每隔10ms检测一帧画面，除非用户按下键"C"
		{

			capture >> frame;   //capture >> frame将帧画面赋给frame		
			detectAndDisplay(frame, mode);
			if (mode == 1)//自动采集，自动退出
			{

				int c = waitKey(20);
				HWND hWnd = (HWND)cvGetWindowHandle(window_name);	//获取窗口句柄
				if ((char)c == 'c' || !IsWindow(hWnd)) { cv::destroyAllWindows(); break; }//按c直接退出
				if (frame_count > 200)//二百帧内还未采集结束就关闭
				{
					frame_count = 0;
					face_count = 0;
					cv::destroyAllWindows();
					cout << "未采集到足够人脸数据" << endl;
					break;
				}
				if (face_count > 50)//采集完毕，结束
				{
					frame_count = 0;
					face_count = 0;
					cv::destroyAllWindows();
					cout << "采集完毕" << endl;
					break;
				}
			}
			else if (mode == 0)//手动采集，手动退出
			{

				HWND hWnd = (HWND)cvGetWindowHandle(window_name);	//获取窗口句柄
				int c = waitKey(20);
				if ((char)c == 's')
				{
					imwrite("../camera_face/face.bmp", face_frame);
					cv::destroyAllWindows();
					break;
				}
				else if ((char)c == 'c' || !(IsWindow(hWnd)))
				{
					cv::destroyAllWindows();
					break;
				}
			}
		}
	}
	return 1;
}

int FaceDetection::scanner_face(string path)
{

	if (!face_cascade.load(cascade_path))
	{
		printf("--(!)Error loading\n");
		return 0;
	}
	dir_ctrl.check_dir("../scanner_face");//检查存放扫描人脸图像的文件夹是否存在，若不存在，创建一个；若存在，清空
	Mat img = imread(path);
	std::vector<Rect> faces;     //容器faces，数据类型为矩形Rect，包括左上顶点(x,y)，宽度width，高度heigth。
	Mat img_gray;
	cvtColor(img, img_gray, COLOR_BGR2GRAY);    //cvtColor函数，将img转化为灰度图，保存于img_gray
	equalizeHist(img_gray, img_gray);           //equalizeHist函数，直方图均衡化，增强对比度																							
	face_cascade.detectMultiScale(img_gray, faces, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(5, 5));//-- 用分类器face_cascade检测出人脸，存于容器faces里。detectMultiScale多尺寸检测。
	if (faces.size() > 0)//检测到了人脸，截取并输出
	{
		//取尺寸较大的人脸
		int max_index = 0;
		for (int i = 1; i < faces.size(); i++)
		{
			if (faces[i].height >= faces[max_index].height)
				max_index = i;
		}
		//选取最大的人脸图像 并保存到文件夹中
		Rect roi = faces[max_index];
		imwrite("../scanner_face/face.bmp", img(roi));
		
	}
	else
	{
		Mat reverse_img;//将图片进行反转再检测
		Mat map_x, map_y;
		reverse_img.create(img.size(), img.type());
		map_x.create(img.size(), CV_32FC1);
		map_y.create(img.size(), CV_32FC1);
		for (int j = 0; j < img.rows; j++)
		{
			for (int i = 0; i < img.cols; i++)
			{
				map_x.at<float>(j, i) = static_cast<float>(img.cols - i);
				map_y.at<float>(j, i) = static_cast<float>(img.rows - j);
			}
		}
		remap(img, reverse_img, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));//重映射

		cvtColor(reverse_img, img_gray, COLOR_BGR2GRAY);    //cvtColor函数，将img转化为灰度图，保存于img_gray
		equalizeHist(img_gray, img_gray);           //equalizeHist函数，直方图均衡化，增强对比度																							
		face_cascade.detectMultiScale(img_gray, faces, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(5, 5));//-- 用分类器face_cascade检测出人脸，存于容器faces里。detectMultiScale多尺寸检测。
		if (faces.size() > 0)//检测到了人脸，截取并输出
		{
			//取尺寸较大的人脸
			int max_index = 0;
			for (int i = 1; i < faces.size(); i++)
			{
				if (faces[i].height >= faces[max_index].height)
					max_index = i;
			}
			//选取最大的人脸图像 并保存到文件夹中
			Rect roi = faces[max_index];
			cout << roi << endl;
			imwrite("../scanner_face/face.bmp", img(roi));

		}
		else
		{
			cout << "扫描图中未检测到人脸" << endl;
			return -1;
		}
	}
	return 1;
}






void FaceDetection::detectAndDisplay(Mat frame, int mode)
{
	std::vector<Rect> faces;     //容器faces，数据类型为矩形Rect，包括左上顶点(x,y)，宽度width，高度heigth。
	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);    //cvtColor函数，将frame转化为灰度图，保存于frame_gray
	equalizeHist(frame_gray, frame_gray);           //equalizeHist函数，直方图均衡化，增强对比度
	//-- 用分类器face_cascade检测出人脸，存于容器faces里。detectMultiScale多尺寸检测。
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(200, 200));

	for (size_t i = 0; i < faces.size(); i++)
	{
		Mat origin_frame = frame.clone();
		//在frame上画矩形，框出人脸区域 
		rectangle(frame, faces[i], Scalar(255, 255, 255), 1, 8);
		//将人脸图像保存到文件夹中
		Rect roi = faces[i];
		face_count++;
		face_frame = frame(roi);
		//加入头肩像的保存
		int frame_width = origin_frame.cols;
		int frame_height = origin_frame.rows;
		int x1 = roi.x;
		int y1 = roi.y;
		int &face_width = roi.width;
		int &face_height = roi.height;
		int x2 = x1 + face_width;
		int y2 = y1 + face_height;
		int width_remain = x1 <= frame_width - x2 ? x1 : frame_width - x2;
		int heigh_remain = y1 <= frame_height - y2 ? y1 : frame_height - y2;
		//双向增加的宽度和高度
		int width_add = face_width / 5 <= width_remain ? face_width / 5 : width_remain;
		int height_add = face_height / 3 <= heigh_remain ? face_height / 3 : heigh_remain;
		//增大roi
		Rect big_roi =roi+ Point(-width_add, -height_add);
		big_roi += Size(2*width_add, 2*height_add);
		face_big_frame = origin_frame(big_roi);


		if (mode == 1)
		{
			//字符串拼接
			string s0 = "../camera_face/face_big_";
			string s1 = "../camera_face/face_";
			string s2 = to_string(face_count / 10);
			string s3 = ".bmp";
			string s = s1 + s2 + s3;
			string big_s = s0 + s2 + s3;
			if (face_count % 10 == 0)//每5人脸帧采集一副
			{
				frame_count = 0; //每次检测到人脸就重新开始计帧数
				imwrite(s, face_frame);
				imwrite(big_s, face_big_frame);
			}
		}
	}
	frame_count++;


	imshow(window_name, frame);
}

int  cameraFace(int mode)
{
	FaceDetection detector;
	detector.camera_face(mode);
	return 0;
}
