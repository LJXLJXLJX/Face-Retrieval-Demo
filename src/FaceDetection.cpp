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
	dir_ctrl.check_dir("../camera_face/");//���������ͷ����ͼ����ļ����Ƿ���ڣ��������ڣ�����һ���������ڣ����
	VideoCapture capture;    //����ͷ
	Mat frame;               //frame���ڱ�������ͷ��ȡ��ÿ֡����
	if (!face_cascade.load(cascade_path))
	{
		printf("--(!)Error loading\n");
		return 0;
	}


	//-- 2. ������ͷ
	capture.open(0);
	int delay = 500;
	while (delay--);
	if (capture.isOpened())
	{
		namedWindow(window_name);
		while (1)  //ÿ��10ms���һ֡���棬�����û����¼�"C"
		{

			capture >> frame;   //capture >> frame��֡���渳��frame		
			detectAndDisplay(frame, mode);
			if (mode == 1)//�Զ��ɼ����Զ��˳�
			{

				int c = waitKey(20);
				HWND hWnd = (HWND)cvGetWindowHandle(window_name);	//��ȡ���ھ��
				if ((char)c == 'c' || !IsWindow(hWnd)) { cv::destroyAllWindows(); break; }//��cֱ���˳�
				if (frame_count > 200)//����֡�ڻ�δ�ɼ������͹ر�
				{
					frame_count = 0;
					face_count = 0;
					cv::destroyAllWindows();
					cout << "δ�ɼ����㹻��������" << endl;
					break;
				}
				if (face_count > 50)//�ɼ���ϣ�����
				{
					frame_count = 0;
					face_count = 0;
					cv::destroyAllWindows();
					cout << "�ɼ����" << endl;
					break;
				}
			}
			else if (mode == 0)//�ֶ��ɼ����ֶ��˳�
			{

				HWND hWnd = (HWND)cvGetWindowHandle(window_name);	//��ȡ���ھ��
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
	dir_ctrl.check_dir("../scanner_face");//�����ɨ������ͼ����ļ����Ƿ���ڣ��������ڣ�����һ���������ڣ����
	Mat img = imread(path);
	std::vector<Rect> faces;     //����faces����������Ϊ����Rect���������϶���(x,y)�����width���߶�heigth��
	Mat img_gray;
	cvtColor(img, img_gray, COLOR_BGR2GRAY);    //cvtColor��������imgת��Ϊ�Ҷ�ͼ��������img_gray
	equalizeHist(img_gray, img_gray);           //equalizeHist������ֱ��ͼ���⻯����ǿ�Աȶ�																							
	face_cascade.detectMultiScale(img_gray, faces, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(5, 5));//-- �÷�����face_cascade������������������faces�detectMultiScale��ߴ��⡣
	if (faces.size() > 0)//��⵽����������ȡ�����
	{
		//ȡ�ߴ�ϴ������
		int max_index = 0;
		for (int i = 1; i < faces.size(); i++)
		{
			if (faces[i].height >= faces[max_index].height)
				max_index = i;
		}
		//ѡȡ��������ͼ�� �����浽�ļ�����
		Rect roi = faces[max_index];
		imwrite("../scanner_face/face.bmp", img(roi));
		
	}
	else
	{
		Mat reverse_img;//��ͼƬ���з�ת�ټ��
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
		remap(img, reverse_img, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));//��ӳ��

		cvtColor(reverse_img, img_gray, COLOR_BGR2GRAY);    //cvtColor��������imgת��Ϊ�Ҷ�ͼ��������img_gray
		equalizeHist(img_gray, img_gray);           //equalizeHist������ֱ��ͼ���⻯����ǿ�Աȶ�																							
		face_cascade.detectMultiScale(img_gray, faces, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(5, 5));//-- �÷�����face_cascade������������������faces�detectMultiScale��ߴ��⡣
		if (faces.size() > 0)//��⵽����������ȡ�����
		{
			//ȡ�ߴ�ϴ������
			int max_index = 0;
			for (int i = 1; i < faces.size(); i++)
			{
				if (faces[i].height >= faces[max_index].height)
					max_index = i;
			}
			//ѡȡ��������ͼ�� �����浽�ļ�����
			Rect roi = faces[max_index];
			cout << roi << endl;
			imwrite("../scanner_face/face.bmp", img(roi));

		}
		else
		{
			cout << "ɨ��ͼ��δ��⵽����" << endl;
			return -1;
		}
	}
	return 1;
}






void FaceDetection::detectAndDisplay(Mat frame, int mode)
{
	std::vector<Rect> faces;     //����faces����������Ϊ����Rect���������϶���(x,y)�����width���߶�heigth��
	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);    //cvtColor��������frameת��Ϊ�Ҷ�ͼ��������frame_gray
	equalizeHist(frame_gray, frame_gray);           //equalizeHist������ֱ��ͼ���⻯����ǿ�Աȶ�
	//-- �÷�����face_cascade������������������faces�detectMultiScale��ߴ��⡣
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(200, 200));

	for (size_t i = 0; i < faces.size(); i++)
	{
		Mat origin_frame = frame.clone();
		//��frame�ϻ����Σ������������ 
		rectangle(frame, faces[i], Scalar(255, 255, 255), 1, 8);
		//������ͼ�񱣴浽�ļ�����
		Rect roi = faces[i];
		face_count++;
		face_frame = frame(roi);
		//����ͷ����ı���
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
		//˫�����ӵĿ�Ⱥ͸߶�
		int width_add = face_width / 5 <= width_remain ? face_width / 5 : width_remain;
		int height_add = face_height / 3 <= heigh_remain ? face_height / 3 : heigh_remain;
		//����roi
		Rect big_roi =roi+ Point(-width_add, -height_add);
		big_roi += Size(2*width_add, 2*height_add);
		face_big_frame = origin_frame(big_roi);


		if (mode == 1)
		{
			//�ַ���ƴ��
			string s0 = "../camera_face/face_big_";
			string s1 = "../camera_face/face_";
			string s2 = to_string(face_count / 10);
			string s3 = ".bmp";
			string s = s1 + s2 + s3;
			string big_s = s0 + s2 + s3;
			if (face_count % 10 == 0)//ÿ5����֡�ɼ�һ��
			{
				frame_count = 0; //ÿ�μ�⵽���������¿�ʼ��֡��
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
