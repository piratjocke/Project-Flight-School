#include "LevelExporter.h"


LevelExporter::LevelExporter()
{
	g_FilePath = "../../Asset/";
	g_AssetPath = "../../Asset/";
}


LevelExporter::~LevelExporter()
{
	ReleaseMaya();
}

bool LevelExporter::InitializeMaya()
{
	cout << "Initializing Maya..." << endl;
	cout << "_________________________________________" << endl << endl;

	if (!MLibrary::initialize("Exporter", false))
	{
		cout << "<Error> Mlibrary::initialize()" << endl;
		return false;
	}

	return true;
}
void LevelExporter::ReleaseMaya()
{
	cout << "Closing Maya..." << endl;
	MLibrary::cleanup(0, false);
}
bool LevelExporter::GetMayaFilenamesInDirectory(vector<string> &filenameList)
{
	WIN32_FIND_DATA fdata;
	HANDLE dhandle;

	char buf[MAX_PATH];
	sprintf_s(buf, sizeof(buf), "%s%s/*", g_FilePath.c_str(), "LevelEditor/MayaFiles");

	if ((dhandle = FindFirstFile(buf, &fdata)) == INVALID_HANDLE_VALUE)
		return false;

	while (true)
	{
		if (FindNextFile(dhandle, &fdata))
		{
			//Only adds the .mb files to list
			if (strcmp(&fdata.cFileName[strlen(fdata.cFileName) - 3], ".mb") == 0)
			{
				filenameList.push_back(fdata.cFileName);
			}
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
void LevelExporter::RunExporter()
{
	vector<string> fileNameList;

	if (!InitializeMaya())
	{
		cout << "<Error> Maya initilization failed." << endl;
		cin >> waitingForInput;
		return;
	}

	//Getting filelist
	if (!GetMayaFilenamesInDirectory(fileNameList))
	{
		cout << "<Error> Files not found in directory." << endl;
		return;
	}

	//Looping through every file in the list
	for (auto file = fileNameList.begin(); file != fileNameList.end(); file++)
	{
		SceneManager(file->c_str());
	}

	return;
}
void LevelExporter::SceneManager(const char* fileName)
{
	MStatus status = MS::kSuccess;

	//Releases all data from global structures before using
	ReleaseDataTypes();

	// Set everything back to a new file state
	status = MFileIO::newFile(true);
	if (!status)
	{
		cout << "<Error> MFileIO::NewFile()" << endl;
		cin >> waitingForInput;
		return;
	}

	// Creates the fullpath of the file. (directoryPath + fileName)
	char fullPath[MAX_PATH];
	sprintf_s(fullPath, sizeof(fullPath), "%s%s%s", g_FilePath.c_str(), "LevelEditor/MayaFiles/", fileName);

	cout << "Open file: " << fileName << endl << endl;
	//Open current maya file
	status = MFileIO::open(fullPath);
	if (!status)
	{
		cout << "<Error> MFileIO::Open()" << endl;
		cin >> waitingForInput;
		return;
	}

	cout << "\n####\n Extracting data..." << endl;
	//Extracting and saving all the data to the meshinfo_maya buffer
	if (!ExtractCurrentSceneRawData())
	{
		cout << "<Error> ExtractCurrentSceneRawData()" << endl;
		cin >> waitingForInput;
		return;
	}

	cout << "Writing data to file... " << endl;
	//Writing the data to file in binary


	WriteFileToBinary(fileName);

	return;
}

void LevelExporter::ReleaseDataTypes()
{

	if (vertexCount > 0)
	{
		delete[] gridData.vertices;
		vertexCount = 0;
	}

	if (navvertexCount > 0)
	{
		delete[] navData.vertices;
		navvertexCount = 0;
	}

	if (matrices.size() > 0)
		matrices.clear();

}

bool LevelExporter::ExtractCurrentSceneRawData()
{
	MDagPath dagPath;
	MItDag dagIter(MItDag::kBreadthFirst, MFn::kMesh);

	while (!dagIter.isDone())
	{
		if (dagIter.getPath(dagPath))
		{
			MFnDagNode dagNode = dagPath.node();

			//Only get meshes with non-history
			if (!dagNode.isIntermediateObject())
			{
				MFnMesh mesh(dagPath);

				if (mesh.name() == "GridShape")
				{
					if (!ExtractGridData(mesh))
					{
						cout << "Error GridData" << endl;
						return false;
					}
				}

				if (mesh.name() == "NavMeshShape")
				{
					if (!ExtractNavMesh(mesh))
					{
						cout << "Error NavMeshData" << endl;
						return false;
					}
				}

				else if (mesh.name() != "GridShape" || mesh.name() != "NavMeshShape")
				{
					ExtractAndConvertMatrix(mesh, 0);
				}
			}
		}
		dagIter.next();
	}

	return true;
}

void LevelExporter::GetDimensions(MFnMesh &mesh, UINT* dimensions)
{
	MItDependencyGraph dgIt(mesh.object(), MFn::kPolyMesh, MItDependencyGraph::kUpstream, MItDependencyGraph::kBreadthFirst, MItDependencyGraph::kNodeLevel);
	MObject polyMeshNode = dgIt.currentItem();
	MFnDependencyNode polyFn(polyMeshNode);

	float tempDimension[2];
	polyFn.findPlug("width").getValue(tempDimension[0]);
	polyFn.findPlug("height").getValue(tempDimension[1]);

	dimensions[0] = tempDimension[0];
	dimensions[1] = tempDimension[1];
}
Matrix LevelExporter::ExtractAndConvertMatrix(MFnMesh &mesh, int fauling)
{
	Matrix matrix;


	MStatus status = MS::kSuccess;

	MFnTransform mayaTransform(mesh.parent(0), &status);


	string id;

	for (int i = 0; i < mayaTransform.name().length(); i++)
	{
		if (mayaTransform.name().asChar()[i] != '_')
		{
			id += mayaTransform.name().asChar()[i];
		}
		else
		{
			break;
		}
	}

	//Gets parent name and saves it to "name"
	if (fauling == 0)
		sprintf_s(matrix.name, sizeof(matrix.name), "%s%s", id.c_str(), ".pfs");

	if (fauling == 1)
		sprintf_s(matrix.name, sizeof(matrix.name), "%s", mayaTransform.name().asChar());

	MVector translate;
	MQuaternion rotate;
	double scale[3];


	matrix.derp = mayaTransform.transformation().asMatrix();


	translate = mayaTransform.getTranslation(MSpace::kTransform);
	mayaTransform.getRotation(rotate);
	mayaTransform.getScale(scale);


	//matrix.translate[0] = translate.x;
	//matrix.translate[1] = translate.y;
	//matrix.translate[2] = translate.z * -1.0f;

	//matrix.rotate[0] = rotate.x;
	//matrix.rotate[1] = rotate.y;
	//matrix.rotate[2] = rotate.z;
	//matrix.rotate[3] = rotate.w;

	//matrix.scale[0] = scale[0];
	//matrix.scale[1] = scale[1];
	//matrix.scale[2] = scale[2];


	if (fauling == 0)
		matrices.push_back(matrix);

	if (fauling == 1)
		return matrix;
}


bool LevelExporter::ExtractGridData(MFnMesh &mesh)
{
	MFloatPointArray points;
	MFloatVectorArray normals;

	MString command = "polyTriangulate -ch 1 " + mesh.name();
	if (!MGlobal::executeCommand(command))
	{
		cout << "Couldn't triangulate mesh: " << mesh.name() << endl;
		cin >> waitingForInput;
		return false;
	}

	//Extractic mesh raw data
	if (!mesh.getPoints(points))
	{
		cout << "Couldn't get points for mesh: " << mesh.name() << endl;
		cin >> waitingForInput;
		return false;
	}

	if (!mesh.getNormals(normals))
	{
		cout << "Couldn't get normals for mesh: " << mesh.name() << endl;
		cin >> waitingForInput;
		return false;
	}

	ConvertGridData(mesh, points, normals);

}

bool LevelExporter::ExtractNavMesh(MFnMesh &mesh)
{
	MFloatPointArray points;

	MString command = "polyTriangulate -ch 1 " + mesh.name();
	if (!MGlobal::executeCommand(command))
	{
		cout << "Couldn't triangulate Navmesh: " << mesh.name() << endl;
		cin >> waitingForInput;
		return false;
	}

	if (!mesh.getPoints(points))
	{
		cout << "Couldn't get points for NavMesh: " << mesh.name() << endl;
		cin >> waitingForInput;
		return false;
	}

	ConvertNavMeshData(mesh, points);
}

void LevelExporter::ConvertNavMeshData(MFnMesh &mesh, MFloatPointArray & points)
{
	MDagPath meshPath = mesh.dagPath();
	MItMeshPolygon polygon_iter(meshPath);

	navvertexCount = polygon_iter.count() * 3;

	navData.vertices = new NavVertex[navvertexCount];
	int nindex = 0;

	while (!polygon_iter.isDone())
	{
		MIntArray indexArray;
		polygon_iter.getVertices(indexArray);

		if (indexArray.length() == 3)
		{
			for (int i = 0; i < 3; i++)
			{
				int vert_index = polygon_iter.vertexIndex(2 - i);

				navData.vertices[nindex + i].Position[0] = points[vert_index].x;
				navData.vertices[nindex + i].Position[1] = points[vert_index].y;
				navData.vertices[nindex + i].Position[2] = -points[vert_index].z;
			}
		}
		nindex += 3;
		polygon_iter.next();

	}
	int a = 0;
}

void LevelExporter::ConvertGridData(MFnMesh &mesh, MFloatPointArray &points, MFloatVectorArray &normals)
{
	MDagPath meshPath = mesh.dagPath();
	MItMeshPolygon polygon_iter(meshPath);

	GetDimensions(mesh, gridData.dimensions);
	gridData.matrix = ExtractAndConvertMatrix(mesh, 1);

	vertexCount = polygon_iter.count() * 3;
	gridData.vertices = new Vertex[vertexCount];

	int index = 0;
	while (!polygon_iter.isDone())
	{
		MIntArray indexArray;
		polygon_iter.getVertices(indexArray);

		//Mesh need to be triangulated
		if (indexArray.length() == 3)
		{
			for (int i = 0; i < 3; i++)
			{
				//Getting indices inverted to fit righthanded
				int vertex_index = polygon_iter.vertexIndex(2 - i);
				int normal_index = polygon_iter.normalIndex(2 - i);

				gridData.vertices[index + i].Position[0] = points[vertex_index].x;
				gridData.vertices[index + i].Position[1] = points[vertex_index].y;
				gridData.vertices[index + i].Position[2] = points[vertex_index].z * -1;

				gridData.vertices[index + i].Normals[0] = normals[normal_index].x;
				gridData.vertices[index + i].Normals[1] = normals[normal_index].y;
				gridData.vertices[index + i].Normals[2] = normals[normal_index].z * -1;

			}
		}
		index += 3;
		polygon_iter.next();
	}
}
void LevelExporter::WriteFileToBinary(const char* fileName)
{
	string fullPath = CreateExportFile(fileName, ".lp");

	//Open/Creates file
	ofstream fileOut;
	fileOut.open(fullPath, ios_base::binary);

	if (!fileOut)
		return;

	cout << "Exporting level grid to " << fullPath.c_str() << endl << endl;
	UINT nrOfObjects = matrices.size();

	fileOut.write((char*)&gridData.dimensions, sizeof(gridData.dimensions));
	fileOut.write((char*)&vertexCount, sizeof(UINT));
	fileOut.write((char*)gridData.vertices, sizeof(Vertex) * vertexCount);
	fileOut.write((char*)&gridData.matrix, sizeof(Matrix));
	fileOut.write((char*)&navvertexCount, sizeof(UINT));
	fileOut.write((char*)navData.vertices, sizeof(NavVertex) * navvertexCount);
	fileOut.write((char*)&nrOfObjects, sizeof(nrOfObjects));
	for (int i = 0; i < matrices.size(); i++)
	{
		//Writing matrix info to file
		fileOut.write((char*)&matrices[i], sizeof(Matrix));
	}

	fileOut.close();
}

string LevelExporter::CreateExportFile(string fileName, string fileEnding)
{
	//remove .mb from file
	int sub_string_length = (int)fileName.find_last_of(".", fileName.size() - 1);

	// fileName with right filetype
	string exportFileName = fileName.substr(0, sub_string_length) + fileEnding.c_str();

	char fullPath[MAX_PATH];
	sprintf_s(fullPath, sizeof(fullPath), "%s%s%s", g_FilePath.c_str(), "LevelEditor/", exportFileName.c_str());

	//Returning full path
	return fullPath;
}