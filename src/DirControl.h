#pragma once
#include <string>
#include <vector>
#include <io.h>
#include <direct.h>


using namespace std;

class DirControl
{
public:
	//��Ŀ¼path�е������ļ�Ŀ¼��ŵ�����files��
	void getFiles(string path, vector<std::string>& files);
	//�ж�Ŀ¼�Ƿ���� �����ھʹ���һ��
	void makedir(string dir);
	//���Ŀ¼�Ƿ���ڣ������ھʹ��������ھ�����ڲ��ļ�
	void check_dir(string dir);
};
