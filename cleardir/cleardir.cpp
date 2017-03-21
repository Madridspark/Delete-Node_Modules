// cleardir.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

DWORD doEnumDir(LPCSTR szPath);
LPSTR szCurrentPath = NULL;

int main()
{
	szCurrentPath = _getcwd(NULL, 0);

	printf("开始遍历:%s\r\n",szCurrentPath);

	if (doEnumDir(szCurrentPath))
	{
		printf("错误:%d\r\n", GetLastError());
	}
	else
	{
		printf("遍历结束\r\n");
	}
	
	free(szCurrentPath);
}


DWORD doEnumDir(LPCSTR szPath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;
	CHAR szFilePath[MAX_PATH];

	// 构造代表子目录和文件夹路径的字符串，使用通配符"*"
	lstrcpy(szFilePath, szPath);
	lstrcat(szFilePath, "\\*");

	// 查找第一个文件/目录，获得查找句柄
	hListFile = FindFirstFile(szFilePath, &FindFileData);
	// 判断句柄
	if (hListFile == INVALID_HANDLE_VALUE)
	{
		printf("错误：%d\n", GetLastError());
		return 1;
	}
	else
	{
		int seed = 0;
		
		do
		{
			// 过滤"."和".."
			if(lstrcmp(FindFileData.cFileName, TEXT(".")) == 0 ||
			lstrcmp(FindFileData.cFileName, TEXT("..")) == 0)
			{
				continue;
			}

			CHAR szDirOldPath[MAX_PATH];
			lstrcpy(szDirOldPath, szPath);
			lstrcat(szDirOldPath, "\\");
			lstrcat(szDirOldPath, FindFileData.cFileName);

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				CHAR szDirNewPath[MAX_PATH], szCount[MAX_PATH];
				lstrcpy(szDirNewPath, szPath);
				lstrcat(szDirNewPath, "\\");
				itoa(seed, szCount, 10);
				lstrcat(szDirNewPath, szCount);
				rename(szDirOldPath, szDirNewPath);
				printf("重命名：%s\r\n", szDirOldPath);
				doEnumDir(szDirNewPath);
				seed++;
			}
			else
			{
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)		// 如果文件存在:文件为只读无法删除
				{
					SetFileAttributes(szDirOldPath, 0);								// 去掉文件只读属性
				}
				
				if (DeleteFile(szDirOldPath))										// 删除成功
				{
					printf("已删除文件：%s\r\n", szDirOldPath);
				}
				else																// 无法删除:文件只读或无权限执行删除
				{
					printf("无法删除文件：%s\r\n", szDirOldPath);
				}
			}
		} while (FindNextFile(hListFile, &FindFileData));

		FindClose(hListFile);

		if (RemoveDirectory(szPath))												// 删除此文件夹
		{
			printf("已删除文件夹：%s\r\n", szPath);
		}
		else
		{
			if (strcmp(szCurrentPath, szPath) == 0)
			{
				return 0;
			}

			printf("无法删除文件夹(%d)：%s\r\n", GetLastError(), szPath);
			return -1;
		}
	}

	return 0;
}
