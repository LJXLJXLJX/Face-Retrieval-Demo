#include "DirControl.h"

void DirControl::getFiles(string path, vector<std::string>& files)
{
	//文件句柄  
	intptr_t hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("/").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("/").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

void DirControl::makedir(string dir)
{
	if (_access(dir.c_str(), 0) == -1)
	{
		_mkdir(dir.c_str());
	}
}

void DirControl::check_dir(string dir)
{
	if (_access(dir.c_str(), 0) == -1)
	{
		_mkdir(dir.c_str());
	}
	else if (_access(dir.c_str(), 0) == 0)
	{
		vector<string> files;
		getFiles(dir, files);
		for (auto i : files)
		{
			remove(i.c_str());
		}
	}
}
