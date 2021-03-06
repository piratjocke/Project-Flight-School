// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <fstream>
#include <vector>
#include <Windows.h>
#include <iostream>

using namespace std;

struct vec2
{
	float x, y;
};

struct vec3
{
	float x, y, z;
};

struct vec4
{
	float x, y, z, w;
};

struct Vertex_Static
{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 uv;
};

struct Vertex_Anim
{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 uv;

	vec4 weights;
	int jointIndex[4];
};

struct MeshInfo
{
	UINT vertexCount;

	char meshName[30];
	char diffuseMapName[30];
	char normalMapName[30];
	char specularMapName[30];

	char skeletonName[30];
};

bool GetFilenamesInDirectory(vector<string> &filenameList)
{
	WIN32_FIND_DATA fdata;
	HANDLE dhandle;
	char ch[50];
	
	//Directory to search
	LPCWSTR fp = L"../../Asset/bin/*";


	if ((dhandle = FindFirstFile(fp, &fdata)) == INVALID_HANDLE_VALUE)
		return false;

	while (true)
	{
		if (FindNextFile(dhandle, &fdata))
		{
			size_t i = wcslen(fdata.cFileName);
			wcstombs_s(&i, ch, fdata.cFileName, (size_t)MAX_PATH);

			if (strlen(ch) > 3)
				filenameList.push_back(ch);
		}
		else
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
				break;

			else
			{
				FindClose(dhandle);
				return false;
			}
		}
	}

	if (!FindClose(dhandle))
	{
		return false;
	}

	return true;
}

bool CheckFileEnding(string fileName, string fileEnding)
{
	//remove .mb from file
	int sub_string_length = (int)fileName.find_last_of(".", fileName.size() - 1);

	// fileName with right filetype
	string exportFileName = fileName.substr(sub_string_length, fileName.size() - 1);

	if (exportFileName != fileEnding)
		return false;
	
	return true;
}

void ReadStaticMeshBinary(string fileName)
{
	UINT meshCount;
	
	MeshInfo meshInfo;
	Vertex_Static* vertices;

	ifstream myFile("../../Asset/bin/" + fileName, ios::binary);

	if (!myFile)
	{
		cout << "Coudln't open file: " << fileName.c_str() << " for reading" << endl;
		return;
	}

	//Read fileheader. Holds information about meshes in scene
	myFile.read((char*)&meshCount, sizeof(UINT));
		
	for (int i = 0; i < meshCount; i++)
	{
		//Read actual data
		myFile.read((char*)&meshInfo, sizeof(meshInfo));
	
		UINT vertexSize = sizeof(Vertex_Static) * meshInfo.vertexCount;
	
		//Memory alloc + reading vertices
		vertices = new Vertex_Static[vertexSize];
		myFile.read((char*)vertices, vertexSize);
	}
	myFile.close();
}

void ReadAnimMeshBinary(string fileName)
{
	MeshInfo meshInfo;
	Vertex_Anim* vertices;

	ifstream myFile("../../Asset/bin/" + fileName, ios::binary);

	if (!myFile)
	{
		cout << "Coudln't open file: " << fileName.c_str() << " for reading" << endl;
		return;
	}

	//Read actual data
	myFile.read((char*)&meshInfo, sizeof(meshInfo));

	UINT vertexSize = sizeof(Vertex_Anim) * meshInfo.vertexCount;

	//Memory alloc + reading vertices
	vertices = new Vertex_Anim[vertexSize];
	myFile.read((char*)vertices, vertexSize);
	
	myFile.close();

	for (int i = 0; i < 10; i++)
	{
		
		cout << vertices[i].weights.x << " " << vertices[i].weights.y << " " << vertices[i].weights.z << " " << vertices[i].weights.w << endl;
		cout << vertices[i].jointIndex[0] << " " << vertices[i].jointIndex[1] << " " << vertices[i].jointIndex[2] << " " << vertices[i].jointIndex[3] << endl << endl;
	}
}

void ReadFiles()
{
	vector<string> filenameList;
	GetFilenamesInDirectory(filenameList);

	ReadAnimMeshBinary("maya_testAsset_tree_animated.apfs");

	for (int i = 0; i < filenameList.size(); i++)
	{
		if (CheckFileEnding(filenameList[i], ".pfs"))
			ReadStaticMeshBinary(filenameList[i]);

		if (CheckFileEnding(filenameList[i], ".apfs"))
			ReadAnimMeshBinary(filenameList[i]);
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	ReadFiles();

	int a = 0;

	return 0;
}

