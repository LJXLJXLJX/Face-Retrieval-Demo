#pragma once
#include <string>
#include <vector>
#include <io.h>
#include <direct.h>


using namespace std;

class DirControl
{
public:
	//将目录path中的所有文件目录存放到容器files中
	void getFiles(string path, vector<std::string>& files);
	//判断目录是否存在 不存在就创建一个
	void makedir(string dir);
	//检查目录是否存在，不存在就创建，存在就清空内部文件
	void check_dir(string dir);
};
