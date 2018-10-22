#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <crtdbg.h>
#include <direct.h>
//#include "GL\freeglut.h"
//#include "egmath.h"
#include "egmesh.h"
#include "EgDecimator.h"
#include "EgDecimatorT.h"
#include "EgSmoother.h"
//#include <fbxsdk.h>

// Variables for rotation and zooming
int Width = 1024, Height = 800;
int ManipulateMode = 0;
int StartPt[2];

// Variables for rotation and zooming
float Axis[3] = { 1.0, 0.0, 0.0 };
float Angle = 0.0;
float RotMat[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
float Zoom = -30.0;
float Offset[3] = { 0.0 };

// Callback functions.
void Reshape(int w, int h);
void Mouse(int button, int state, int x, int y);
void MouseWheel(int wheel, int directin, int x, int y);
void MouseMove(int x, int y);
void Render();
void Keyboard(unsigned char key, int x, int y);
void Exit();

// User-defined funtions.
void GetSphereCoord(int x, int y, float *px, float *py, float *pz);
void InitOpenGL();

void TestObjImporter();
void TestFbxImporter();
void TestFbxExporter();

EgScene *pEgScene = NULL;
FbxManager *pFbxManager = NULL;
FbxScene *pFbxScene = NULL;

int main(int argc, char **argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Initialize GLUT and display mode
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	// Create OpenGL window of size Width by Height
	glutInitWindowSize(Width, Height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("EgTools");

	// Register call back functions.
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMove);
	glutMouseWheelFunc(MouseWheel);
	glutDisplayFunc(Render);
	glutKeyboardFunc(Keyboard);
	glutCloseFunc(Exit);
	InitOpenGL();

	pEgScene = new EgScene();
			
	TestObjImporter();
	//TestFbxImporter();

	// Enter the message loop.
	glutMainLoop();
	return 0;
}

void Keyboard(unsigned char key, int x, int y)
{
	if (key == 27)
		Exit();

	if (key == 'w')
	{

	}

	// 메쉬 스무딩 테스트
	if (key == 's')
	{
		EgMesh *pMesh = pEgScene->m_MeshList[0];
		EgSmoother smoother(pMesh);
		smoother.Execute();
		pMesh->UpdateNormal(true);
	}

	// 메쉬 단순화 테스트
	if (key == 'd')
	{
		for (EgMesh *pMesh : pEgScene->m_MeshList)
		{
			//EgDecimator decimator(pMesh);
			EgDecimatorT decimator(pMesh);
			
			clock_t st = clock();
			decimator.Init();
			//decimator.Execute("rmv_edge_info.txt");
			//decimator.Execute(0.5, "rmv_edge_info.txt");
			decimator.Execute(0.75, NULL);
			pMesh->UpdateNormal(true);

			// 임포트된 메쉬의 정보를 출력한다.
			int num_rmv_verts = pMesh->DelUnusedVert();
			int num_rmv_texs = pMesh->DelUnusedTexel();
			int num_rmv_normals = pMesh->DelUnusedNormal();
			printf("Total Elapsed time for Decimation = %u ms \n", clock() - st);
			printf("\t# of Vertices = %d\n", pMesh->GetNumVert());
			printf("\t# of Texels = %d\n", pMesh->GetNumTexel());
			printf("\t# of Normals = %d\n", pMesh->GetNumNormal());
			printf("\t# of Faces = %d\n", pMesh->GetNumFace());
			printf("\t# of Removed Vertices = %d\n", num_rmv_verts);
			printf("\t# of Removed Texels = %d\n", num_rmv_texs);
			printf("\t# of Removed Normals = %d\n", num_rmv_normals);			
		}
	}

	// 메쉬 저장	
	if (key == 'e')
	{
		//export_obj(pMesh, "exported_mesh.obj");
		//export_fbx();
		TestFbxExporter();
	}

	glutPostRedisplay();
}

void TestObjImporter()
{
	// MAC 랩탑
	//_chdir("C:\\Users\\shyun\\Desktop\\G8BasicMale\\Decimated_Triangle\\");
	//_chdir("C:\\Users\\shyun\\Desktop\\G8BasicMale\\");

	// PC 데스크탑
	_chdir("..\\..\\G8BasicMale\\");

	std::string file_name = "BodyBuilder_100%.obj";
	//std::string file_name = "Basic_100%.obj";
	//std::string file_name = "lower-tooth.obj";
	//std::string file_name = "bunny.obj";
	//std::string file_name = "bunny-hole.obj";
	//std::string file_name = "earth.obj";
	
	import_obj(pEgScene, file_name.c_str());

	//EgMesh *pMesh = pEgScene->m_MeshList[0];
	//EgVertex *v = pMesh->m_pVerts;
	//while (v != NULL)
	//{ 
	//	v->m_Pos[0] = v->m_Pos[0] * 250;
	//	v->m_Pos[1] = v->m_Pos[1] * 250;
	//	v->m_Pos[2] = v->m_Pos[2] * 250;
	//	v = v->m_pNext;
	//}
}

void TestFbxImporter()
{
	// MAC 랩탑
	_chdir("C:\\Users\\shyun\\Desktop\\G8BasicMale\\");

	// PC 데스크탑
	//_chdir("D:\\G8BasicMale\\");

	import_fbx(pEgScene, "BodyBuilder_100.fbx", &pFbxManager, &pFbxScene);
	//import_fbx(pEgScene, "exported_file.fbx", &pFbxManager, &pFbxScene);
	//import_fbx("file.fbx", &pFbxManager, &pFbxScene);
	//import_fbx("..\\humanoid.fbx", &MyScene);
	//import_fbx("BodyBuilder_100.fbx", &MyScene);
	//import_fbx(".\\FBX_01\\01_Female_ShortHair_ActiveWear.fbx", &MyScene);	
	//import_fbx(".\\FBX_02\\02_Female_EvonyHair_ActiveWear.fbx", &MyScene);
	//import_fbx(".\\FBX_04\\04_Male_ShortHair_GymWearOutfit.fbx", &MyScene);	
}

void FindFbxMesh(FbxNode *pNode, std::string Name, FbxMesh **pMesh)
{
	FbxMesh *pFbxMesh = pNode->GetMesh();
	if (pFbxMesh != NULL)
	{
		std::string FbxName = pFbxMesh->GetName();
		//printf("FbxMesh'Name = %s\n", FbxName.c_str());
		if (FbxName == Name)
			*pMesh = pFbxMesh;
	}
	for (int i = 0; i < pNode->GetChildCount(); ++i)
		FindFbxMesh(pNode->GetChild(i), Name, pMesh);
}

//void CopyEgMesh2FbxMesh(EgMesh *pMesh, FbxMesh *pFbxMesh)
//{
//	//printf("Copying EgMesh to FbxMesh...\n");
//
//	// 삼각형 정점, 텍스처 정점, 법선의 인덱스를 갱신한다.
//	pMesh->UpdateVertIdx();
//	pMesh->UpdateTexIdx();
//	pMesh->UpdateNormalIdx();
//	pMesh->UpdateMaterialIdx();
//
//	//////////////////////////////////
//	// 1단계: 정점의 리스트를 복사한다.
//	//////////////////////////////////
//	
//	// Fbx 메쉬의 제어점 리스트를 초기화한다.
//	pFbxMesh->mControlPoints.Clear();
//
//	// Eg 메쉬의 각각의 정점에 대하여
//	EgVertex *pVert = pMesh->m_pVerts;
//	while (pVert != NULL)
//	{
//		// 새로운 정점을 만들어 Fbx 제어점 리스트에 추가한다.
//		fbxsdk::FbxVector4 v(pVert->m_Pos[0], pVert->m_Pos[1], pVert->m_Pos[2]);
//		pFbxMesh->mControlPoints.Add(v);
//		pVert = pVert->m_pNext;
//	}
//	int NumVert = pFbxMesh->GetControlPointsCount();
//	printf("\t # of Vertices = %d\n", NumVert);
//			
//	/////////////////////////////////////
//	// 2단계: 텍스처 정점의 리스트 복사한다.
//	/////////////////////////////////////
//	// Fbx 메쉬의 UV 원소를 구한다.
//	const FbxGeometryElementUV *pElementUV = NULL;
//	int NumUV = pFbxMesh->GetElementUVCount();
//	if (NumUV > 0)
//	{
//		printf("\t # of UV Elements = %d\n", NumUV);
//		pElementUV = pFbxMesh->GetElementUV(0);
//	}
//
//	// Fbx 메쉬의 UV 좌표가 있다면
//	if (pElementUV != NULL)
//	{
//		// UV 좌표 리스트를 초기화 하고
//		pElementUV->GetDirectArray().Clear();
//
//		// Eg 메쉬의 각각의 텍스처 좌표에 대하여
//		EgTexel *pTexel = pMesh->m_pTexels;
//		while (pTexel != NULL)
//		{
//			// UV 텍스처 좌표를 생성하여 Fbx UV 원소에 추가한다.
//			fbxsdk::FbxVector2 uv(pTexel->m_Coord[0], pTexel->m_Coord[1]);
//			pElementUV->GetDirectArray().Add(uv);
//			pTexel = pTexel->m_pNext;
//		}
//	}
//	int NumTexel = pElementUV->GetDirectArray().GetCount();
//	printf("\t # of Texture Vertices = %d\n", NumTexel);
//
//	////////////////////////////////
//	// 3단계: 법선의 리스트를 복사한다.
//	////////////////////////////////
//	const FbxLayerElementNormal *pElementNormal = NULL;
//	int NumNormalElements = pFbxMesh->GetElementNormalCount();
//	if (NumNormalElements > 0)
//	{
//		printf("\t # of Normal Elements = %d\n", NumNormalElements);
//		pElementNormal = pFbxMesh->GetElementNormal(0);
//	}
//
//	// Fbx 메쉬의 법선 정보가 있다면
//	if (pElementNormal != NULL)
//	{
//		// 법선 리스트를 초기화 하고
//		pElementNormal->GetDirectArray().Clear();
//
//		// Eg 메쉬의 각각의 법선에 대하여
//		EgNormal *pNormal = pMesh->m_pNormals;
//		while (pNormal != NULL)
//		{
//			// Fbx 메쉬의 법선을 생성하여 추가한다.
//			fbxsdk::FbxVector4 n(pNormal->m_Dir[0], pNormal->m_Dir[1], pNormal->m_Dir[2]);
//			pElementNormal->GetDirectArray().Add(n);
//			pNormal = pNormal->m_pNext;
//		}
//	}
//	int NumNormal = pElementNormal->GetDirectArray().GetCount();
//	printf("\t # of Normals = %d\n", NumNormal);
//
//	////////////////////////////////
//	// 4단계: 삼각형 인덱스를 복사한다.
//	////////////////////////////////
//
//	// Fbx 메쉬의 삼각형, 텍스처 정점, 법선의 인덱스를 초기화 한다.
//	pFbxMesh->mPolygonVertices.Clear();	
//	
//	// Fbx 메쉬의 텍스처 정점의 인덱스를 초기화 한다.
//	if (pElementUV != NULL)
//		pElementUV->GetIndexArray().Clear();
//
//	// Fbx 메쉬의 법선의 인덱스를 초기화 한다.
//	if (pElementNormal != NULL)
//		pElementNormal->GetIndexArray().Clear();
//
//	// Eg 메쉬의 각각의 삼각형에 대하여
//	EgFace *pFace = pMesh->m_pFaces;
//	while (pFace != NULL)
//	{
//		// 삼각형을 구성하는 정점의 인덱스,
//		int vidx0 = pFace->GetVertex(0)->m_Idx;
//		int vidx1 = pFace->GetVertex(1)->m_Idx;
//		int vidx2 = pFace->GetVertex(2)->m_Idx;
//
//		// 텍스춰 정점의 인덱스
//		int tidx0 = pFace->GetTexel(0)->m_Idx;
//		int tidx1 = pFace->GetTexel(1)->m_Idx;
//		int tidx2 = pFace->GetTexel(2)->m_Idx;
//		//printf("texture idxs = (%d, %d, %d)\n", tidx0, tidx1, tidx2);
//
//		int MtlIdx = pFace->m_pMaterial->m_Idx;
//		pFbxMesh->BeginPolygon(MtlIdx);
//		//pFbxMesh->AddPolygon(vidx0, tidx0);
//		//pFbxMesh->AddPolygon(vidx1, tidx1);
//		//pFbxMesh->AddPolygon(vidx2, tidx2);
//		pFbxMesh->AddPolygon(vidx0);
//		pFbxMesh->AddPolygon(vidx1);
//		pFbxMesh->AddPolygon(vidx2);
//		pFbxMesh->EndPolygon();		
//
//		// 그리고 법선의 인덱스를 구한다.
//		int nidx0 = pFace->GetNormal(0)->m_Idx;
//		int nidx1 = pFace->GetNormal(1)->m_Idx;
//		int nidx2 = pFace->GetNormal(2)->m_Idx;
//
//		// Fbx 메쉬의 삼각형 인덱스에 추가한다.
//		//pFbxMesh->mPolygonVertices.Add(vidx0);
//		//pFbxMesh->mPolygonVertices.Add(vidx1);
//		//pFbxMesh->mPolygonVertices.Add(vidx2);
//		
//		// Fbx 메쉬의 텍스처 정점의 인덱스를 추가한다.
//		if (pElementUV != NULL)
//		{
//			pElementUV->GetIndexArray().Add(tidx0);
//			pElementUV->GetIndexArray().Add(tidx1);
//			pElementUV->GetIndexArray().Add(tidx2);
//		}
//
//		// Fbx 메쉬의 법선의 인덱스를 추가한다.
//		if (pElementNormal != NULL)
//		{
//			pElementNormal->GetIndexArray().Add(nidx0);
//			pElementNormal->GetIndexArray().Add(nidx1);
//			pElementNormal->GetIndexArray().Add(nidx2);
//		}
//
//		// Fbx 메쉬의 법선 인덱스를 추가한다.
//		pFace = pFace->m_pNext;
//	}
//}

void CopyEgMesh2FbxMesh(EgMesh *pMesh, FbxMesh *pFbxMesh)
{
	// 삼각형 정점, 텍스처 정점, 법선의 인덱스를 갱신한다.
	pMesh->UpdateVertIdx();
	pMesh->UpdateTexIdx();
	pMesh->UpdateNormalIdx();
	pMesh->UpdateMaterialIdx();

	//////////////////////////////////
	// 1단계: 정점의 리스트를 복사한다.
	//////////////////////////////////

	// Fbx 메쉬의 제어점 리스트를 초기화한다.
	pFbxMesh->mControlPoints.Clear();

	// Eg 메쉬의 각각의 정점에 대하여
	EgVertex *pVert = pMesh->m_pVerts;
	while (pVert != NULL)
	{
		// 새로운 정점을 만들어 Fbx 제어점 리스트에 추가한다.
		fbxsdk::FbxVector4 v(pVert->m_Pos[0], pVert->m_Pos[1], pVert->m_Pos[2]);
		pFbxMesh->mControlPoints.Add(v);
		pVert = pVert->m_pNext;
	}
	int NumVert = pFbxMesh->GetControlPointsCount();
	printf("\t # of Vertices = %d\n", NumVert);

	/////////////////////////////////////
	// 2단계: 텍스처 정점의 리스트 복사한다.
	/////////////////////////////////////
	// Fbx 메쉬의 UV 원소를 구한다.
	const FbxGeometryElementUV *pElementUV = NULL;
	int NumElementUV = pFbxMesh->GetElementUVCount();
	if (NumElementUV > 0)
	{
		printf("\t # of ElementUV = %d\n", NumElementUV);
		pElementUV = pFbxMesh->GetElementUV(0);
	}

	// Fbx 메쉬의 UV 좌표가 있다면
	if (pElementUV != NULL)
	{
		// UV 좌표 리스트를 초기화 하고
		pElementUV->GetDirectArray().Clear();

		// Eg 메쉬의 각각의 텍스처 좌표에 대하여
		EgTexel *pTexel = pMesh->m_pTexels;
		while (pTexel != NULL)
		{
			// UV 텍스처 좌표를 생성하여 Fbx UV 원소에 추가한다.
			fbxsdk::FbxVector2 uv(pTexel->m_Coord[0], pTexel->m_Coord[1]);
			pElementUV->GetDirectArray().Add(uv);
			pTexel = pTexel->m_pNext;
		}
	}
	int NumTexel = pElementUV->GetDirectArray().GetCount();
	printf("\t\t # of Texture Vertices = %d\n", NumTexel);

	////////////////////////////////
	// 3단계: 법선의 리스트를 복사한다.
	////////////////////////////////
	const FbxLayerElementNormal *pElementNormal = NULL;
	int NumElementNormal = pFbxMesh->GetElementNormalCount();
	if (NumElementNormal > 0)
	{
		printf("\t # of Element Normal = %d\n", NumElementNormal);
		pElementNormal = pFbxMesh->GetElementNormal(0);
	}

	// Fbx 메쉬의 법선 정보가 있다면
	if (pElementNormal != NULL)
	{
		// 법선 리스트를 초기화 하고
		pElementNormal->GetDirectArray().Clear();

		// Eg 메쉬의 각각의 법선에 대하여
		EgNormal *pNormal = pMesh->m_pNormals;
		while (pNormal != NULL)
		{
			// Fbx 메쉬의 법선을 생성하여 추가한다.
			fbxsdk::FbxVector4 n(pNormal->m_Dir[0], pNormal->m_Dir[1], pNormal->m_Dir[2]);
			pElementNormal->GetDirectArray().Add(n);
			pNormal = pNormal->m_pNext;
		}
	}
	int NumNormal = pElementNormal->GetDirectArray().GetCount();
	printf("\t\t # of Normals = %d\n", NumNormal);

	////////////////////////////////
	// 4단계: 삼각형 인덱스를 복사한다.
	////////////////////////////////

	// Fbx 메쉬의 삼각형, 텍스처 정점, 법선의 인덱스를 초기화 한다.
	pFbxMesh->mPolygonVertices.Clear();

	// Fbx 메쉬의 텍스처 정점의 인덱스를 초기화 한다.
	if (pElementUV != NULL)
		pElementUV->GetIndexArray().Clear();

	// Fbx 메쉬의 법선의 인덱스를 초기화 한다.
	if (pElementNormal != NULL)
		pElementNormal->GetIndexArray().Clear();

	// Eg 메쉬의 각각의 삼각형에 대하여
	EgFace *pFace = pMesh->m_pFaces;
	while (pFace != NULL)
	{
		// 삼각형을 구성하는 정점의 인덱스,
		int vidx0 = pFace->GetVertex(0)->m_Idx;
		int vidx1 = pFace->GetVertex(1)->m_Idx;
		int vidx2 = pFace->GetVertex(2)->m_Idx;
		//printf("Face's vertex idxs = (%d, %d, %d)\n", vidx0, vidx1, vidx2);

		// 텍스춰 정점의 인덱스
		int tidx0 = pFace->GetTexel(0)->m_Idx;
		int tidx1 = pFace->GetTexel(1)->m_Idx;
		int tidx2 = pFace->GetTexel(2)->m_Idx;
		//printf("Face's texture vertex idxs = (%d, %d, %d)\n", tidx0, tidx1, tidx2);

		// 그리고 법선의 인덱스를 구한다.
		int nidx0 = pFace->GetNormal(0)->m_Idx;
		int nidx1 = pFace->GetNormal(1)->m_Idx;
		int nidx2 = pFace->GetNormal(2)->m_Idx;
		//printf("Face's normal idxs = (%d, %d, %d)\n", nidx0, nidx1, nidx2);

		int MtlIdx = pFace->m_pMaterial->m_Idx;
		pFbxMesh->BeginPolygon(MtlIdx);
		pFbxMesh->AddPolygon(vidx0, tidx0);
		pFbxMesh->AddPolygon(vidx1, tidx1);
		pFbxMesh->AddPolygon(vidx2, tidx2);
		//pFbxMesh->AddPolygon(vidx0);
		//pFbxMesh->AddPolygon(vidx1);
		//pFbxMesh->AddPolygon(vidx2);
		pFbxMesh->EndPolygon();		

		// Fbx 메쉬의 삼각형 인덱스에 추가한다.
		//pFbxMesh->mPolygonVertices.Add(vidx0);
		//pFbxMesh->mPolygonVertices.Add(vidx1);
		//pFbxMesh->mPolygonVertices.Add(vidx2);

		// Fbx 메쉬의 텍스처 정점의 인덱스를 추가한다.
		/*if (pElementUV != NULL)
		{
			pElementUV->GetIndexArray().Add(tidx0);
			pElementUV->GetIndexArray().Add(tidx1);
			pElementUV->GetIndexArray().Add(tidx2);
		}*/

		// Fbx 메쉬의 법선의 인덱스를 추가한다.
		if (pElementNormal != NULL)
		{
			pElementNormal->GetIndexArray().Add(nidx0);
			pElementNormal->GetIndexArray().Add(nidx1);
			pElementNormal->GetIndexArray().Add(nidx2);
		}

		// Fbx 메쉬의 법선 인덱스를 추가한다.
		pFace = pFace->m_pNext;
	}
}

void TestFbxExporter()
{
	//if (pFbxManager == NULL || pEgScene == NULL)
	//	return;

	printf("Exporting FBX Mesh...\n");

	// EgScene에 포함된 각각의 메쉬에 대하여,
	for (EgMesh *pMesh : pEgScene->m_MeshList)
	{
		// 현재 메쉬(pMesh)와 같은 이름의 메쉬(pFbxMesh)를 Fbx 씬에서 찾는다.
		FbxMesh *pFbxMesh = NULL;
		FbxNode *pRootNode = pFbxScene->GetRootNode();
		FindFbxMesh(pRootNode, pMesh->m_Name, &pFbxMesh);

		// 현재 메쉬와 같은 이름의 메쉬가 있다면
		if (pFbxMesh != NULL)
		{
			// 현재 메쉬의 기하정보를 FBX 메쉬에 복사한다.
			CopyEgMesh2FbxMesh(pMesh, pFbxMesh);					
		}
	}

	// IOSettings 객체를 생성하고
	FbxIOSettings *ios = FbxIOSettings::Create(pFbxManager, IOSROOT);
	pFbxManager->SetIOSettings(ios);

	// ... Configure the FbxIOSettings object here ...

	// 출력할 파일 이름을 지정한다.
	const char *lFilename = "BodyBuilder_exported.fbx";

	// 익스포터 객체를 생성하고
	FbxExporter *pExporter = FbxExporter::Create(pFbxManager, "");

	// 출력할 파일 이름과 IO 세팅 객체로 익스포터를 초기화 한다.
	bool lExportStatus = pExporter->Initialize(lFilename, -1, pFbxManager->GetIOSettings());
	if (!lExportStatus)
	{
		printf("Call to FbxExporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", pExporter->GetStatus().GetErrorString());
		return;
	}

	// 장면을 파일로 출력한다.
	pExporter->Export(pFbxScene);

	// 사용한 익스포터를 제거한다.
	pExporter->Destroy();

	printf("Exporting FBX Mesh Completed\n");
}

void InitOpenGL()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void Exit()
{
	// EgScene 객체를 제거하고,
	if (pEgScene != NULL)
		delete pEgScene;

	// FbxManager 객체를 제거한다.
	if (pFbxManager != NULL)
		pFbxManager->Destroy();

	exit(-1);
}

void Reshape(int w, int h)
{
	// Viewport transformation
	glViewport(0, 0, w, h);
	Width = w;
	Height = h;
}

void SetupViewTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Translate for zooming in/out
	glTranslatef(0.0, 0.0, Zoom);

	// For rotation
	glRotatef(Angle, Axis[0], Axis[1], Axis[2]);
	glMultMatrixf(RotMat);

	glTranslatef(Offset[0], Offset[1], Offset[2]);

	// Save the rotation of current modelview mat.
	glGetFloatv(GL_MODELVIEW_MATRIX, RotMat);
	RotMat[12] = RotMat[13] = RotMat[14] = 0.0;
}

void SetupViewVolume()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)Width / (double)Height, 0.001, 10000.0);
}

void Render()
{
	// Clear color buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up viewing volume
	SetupViewVolume();

	// Set up viewing transformation
	SetupViewTransform();

	glMatrixMode(GL_MODELVIEW);
	pEgScene->RenderScene();

	// Swap buffers for double buffering.
	glutSwapBuffers();
}

void Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		StartPt[0] = x; StartPt[1] = y;
		if (button == GLUT_LEFT_BUTTON)
			ManipulateMode = 1;	// For rotation

		if (button == GLUT_MIDDLE_BUTTON)
			ManipulateMode = 2;	// For panning
	}

	if (state == GLUT_UP)
	{
		ManipulateMode = 0;	// Reset 
		StartPt[0] = StartPt[1] = 0;
		Angle = 0.0;
	}
}

void MouseMove(int x, int y)
{
	if (ManipulateMode == 1) {	// Rotation
		float px, py, pz, qx, qy, qz;
		GetSphereCoord(StartPt[0], StartPt[1], &px, &py, &pz);
		GetSphereCoord(x, y, &qx, &qy, &qz);

		// Compute rotation axis and angle
		Axis[0] = py*qz - pz*qy; Axis[1] = pz*qx - px*qz; Axis[2] = px*qy - py*qx;
		Angle = 0.0;

		float len = Axis[0] * Axis[0] + Axis[1] * Axis[1] + Axis[2] * Axis[2];
		if (len > 0.000001) // ingnore small changes 
			Angle = acos(px * qx + py * qy + pz * qz) * 180.0f / 3.141592f;
	}
	if (ManipulateMode == 2) {	// Panning
		float dx = (StartPt[0] - x) * Zoom * 0.001f;
		float dy = (y - StartPt[1]) * Zoom * 0.001f;

		Offset[0] += RotMat[0] * dx + RotMat[1] * dy;
		Offset[1] += RotMat[4] * dx + RotMat[5] * dy;
		Offset[2] += RotMat[8] * dx + RotMat[9] * dy;
	}

	StartPt[0] = x;	// Update startpt as current position
	StartPt[1] = y;
	glutPostRedisplay();
}

void MouseWheel(int wheel, int direction, int x, int y)
{
	Zoom -= (direction * Zoom * 0.1f);
	glutPostRedisplay();
}

void GetSphereCoord(int x, int y, float *px, float *py, float *pz)
{
	*px = (2.0f * x - Width) / Width;
	*py = (-2.0f * y + Height) / Height;

	float r = (*px) * (*px) + (*py) * (*py);
	if (r >= 1.0f)
	{
		*px = *px / sqrt(r);
		*py = *py / sqrt(r);
		*pz = 0.0;
	}
	else
		*pz = sqrt(1.0f - r);
}
