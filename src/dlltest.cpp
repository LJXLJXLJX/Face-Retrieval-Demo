// dlltest.cpp: 定义控制台应用程序的入口点。
//



#include "FaceRecognition.h"
#include "FaceDetection.h"


//#define TEST

int main()
{
#ifdef TEST



#else

	int mode = 0;
	cout << "人脸检索系统Demo" << endl << "developed by LJX..." << endl << "Powerd by arcsoft sdk , SeetaFace sdk..." << endl;
	cout << endl << endl;
	while (1)
	{
		cout << "选择系统功能" << endl << "1：启动人脸检索功能" << endl << "2：导入数据到人脸库" << endl << "3：关闭系统" << endl;
		cin >> mode;
		cout << endl;
		if (mode == 1)
			faceSearchApp();
		else if (mode == 2)
			addToFaceLib();
		else if (mode == 3)
			break;
		else
		{
			cout << "号码输入错误" << endl;
			continue;
		}
}
#endif
	//addToFaceLib();
	system("pause");

	return 0;
}

