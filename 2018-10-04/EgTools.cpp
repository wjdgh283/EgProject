#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <crtdbg.h>
#include <direct.h>
#include "GL\freeglut.h"
#include "egmath.h"
#include "egmesh.h"
#include "EgDecimator.h"
#include "EgDecimator2.h"
#include "EgSmoother.h"


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

void TestEgMeshLib();
void TestObj();

EgScene MyScene;

int main(int argc, char **argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Initialize GLUT library and negotiate a session with the window system
	glutInit(&argc, argv);

	//"디스플레이모드" 를 설정
	//인자 - "디스플레이모드" 의 종류값
	//인자는 GLUT 라이브러리에 정의되어 있는 값들을 OR 비트 연산하여 합한 값인데, 색상(color) 모드와 버퍼의 수 , 버퍼의 종류를 설정해야합니다.
	//GLUT_RGBA 또는 GLUT_RGB - RGBA 창을 만듭니다. 이 값은 glut 가 사용하는 기본 색상 모드입니다.
	//GLUT_INDEX - 인덱스 색상 모드로 설정합니다.
	//GLUT_SINGLE - 단일버퍼(single buffer) 창을 마든다.
	//GLUT_DOUBLE - 이중버퍼(double buffer) 창을 만든다.부드러운 에니메이션을 만들 때 필요합니다.
	//그리고 여러분이 원한다면 특정 버퍼로 설정된 창을 만들 수 있습니다
	//GLUT_ACCUM - 어큐뮬레이션 버퍼( The accumulation buffer )
	//GLUT_STENCIL - 스텐실 버퍼(The stencil buffer)
	//GLUT_DEPTH - 깊이 버퍼(The depth buffer)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	// Create OpenGL window of size Width by Height
	glutInitWindowSize(Width, Height);
	glutCreateWindow("EgTools");
	//--여기까지 실행하면, OpenGL 창은 없고 검은색 바탕의 콘솔 윈도우만 볼 수 있습니다. 게다가 몇 초가 지나면 이 콘솔창 마저도 사라지죠. --

	// Register call back functions.
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMove);
	glutMouseWheelFunc(MouseWheel);
	glutDisplayFunc(Render);
	glutKeyboardFunc(Keyboard);
	glutCloseFunc(Exit);
	InitOpenGL();

	//TestEgMeshLib();
	TestObj();

	// GLUT이벤트 처리 루프로 들어간다. GLUT프로그램에서 한번만 호출되어야함. 루틴은 반환되지않는다. 필요에 따라 등록된 모든 콜백을 호출
	glutMainLoop();
	return 0;
}

void InitOpenGL()
{
	//depth 비교를 수행, depth 버퍼를 업데이트
	glEnable(GL_DEPTH_TEST);
	//조명 파라미터를 사용하여 정점색상 또는 인덱스를 계산. 
	glEnable(GL_LIGHTING);
	//조명 방정식 계산에 light0를 포함시킨다
	glEnable(GL_LIGHT0);
	//윈도우 좌표에서 winding을 기반으로 폴리곤을 컬링한다
	glEnable(GL_CULL_FACE);
	//glCullFace : 앞면 또는 뒷면을 나타내는 facet을 제거 할지 여부 지정
	glCullFace(GL_BACK);
}

void TestObj()
{

	_chdir("../../models\\");

	//std::string file_name = "damaliscus_korrigum.obj";
	//std::string file_name = "genesis.obj";
	//std::string file_name = "V4DevSuit.obj";
	//std::string file_name = "lower-jaw-7.obj";
	//std::string file_name = "cube.obj";
	//std::string file_name = "start_bunny.obj";
	//std::string file_name = "cat.obj";
	std::string file_name = "earth.obj";

	EgMesh *pMesh = NULL;
	pMesh = new EgMesh("Test");

	EgMaterial *mtl = new EgMaterial("earth");
	mtl->LoadTexture("earth_diff.bmp");
	pMesh->AddMaterial(mtl);

	import_obj(file_name.c_str(), pMesh);
	MyScene.m_MeshList.push_back(pMesh);
}

void TestEgMeshLib()
{
	// Create a tetrahedron mesh
	EgMesh *pMesh = new EgMesh("Tetrahedron");

	// Create vertices' positions for tetrahedron
	EgVertex *v0 = new EgVertex(5.0, 5.0, 5.0);
	EgVertex *v1 = new EgVertex(5.0, 5.0, 0.0);
	EgVertex *v2 = new EgVertex(0.0, 5.0, 5.0);
	EgVertex *v3 = new EgVertex(5.0, 0.0, 5.0);

	// Add vertices to the mesh
	pMesh->AddVertex(v0);
	pMesh->AddVertex(v1);
	pMesh->AddVertex(v2);
	pMesh->AddVertex(v3);

	// Create material with diffuse texture
	EgMaterial *mtl = new EgMaterial("Dice");
	mtl->LoadTexture("..//dice.tga");
	pMesh->AddMaterial(mtl);

	// Create texture vertices
	EgTexel *t0 = new EgTexel(0.0, 0.25);
	EgTexel *t1 = new EgTexel(0.25, 0.25);
	EgTexel *t2 = new EgTexel(0.25, 0.5);
	EgTexel *t3 = new EgTexel(0.5, 0.25);
	EgTexel *t4 = new EgTexel(0.75, 0.0);
	EgTexel *t5 = new EgTexel(0.75, 0.25);
	EgTexel *t6 = new EgTexel(0.5, 0.5);
	EgTexel *t7 = new EgTexel(0.75, 0.75);
	EgTexel *t8 = new EgTexel(0.5, 0.75);

	// Add texrure vertices to the mesh
	pMesh->AddTexel(t0);
	pMesh->AddTexel(t1);
	pMesh->AddTexel(t2);
	pMesh->AddTexel(t3);
	pMesh->AddTexel(t4);
	pMesh->AddTexel(t5);
	pMesh->AddTexel(t6);
	pMesh->AddTexel(t7);
	pMesh->AddTexel(t8);

	// Create Halfedges
	EgEdge *e0 = new EgEdge(v0, t1, NULL);
	EgEdge *e1 = new EgEdge(v1, t2, NULL);
	EgEdge *e2 = new EgEdge(v2, t0, NULL);

	EgEdge *e3 = new EgEdge(v0, t5, NULL);
	EgEdge *e4 = new EgEdge(v2, t3, NULL);
	EgEdge *e5 = new EgEdge(v3, t4, NULL);

	EgEdge *e6 = new EgEdge(v0, t8, NULL);
	EgEdge *e7 = new EgEdge(v3, t6, NULL);
	EgEdge *e8 = new EgEdge(v1, t7, NULL);

	// Create triangles with materials
	EgFace *f0 = new EgFace(e0, e1, e2, mtl);
	EgFace *f1 = new EgFace(e3, e4, e5, mtl);
	EgFace *f2 = new EgFace(e6, e7, e8, mtl);

	// Add triangles to the mesh
	pMesh->AddFace(f0);
	pMesh->AddFace(f1);
	pMesh->AddFace(f2);

	// Calculate normal vectors if not specified
	pMesh->UpdateNormal(false);

	// Set opposite halfedge
	pMesh->InitEdgeMate();

	// Add the mesh to the scene
	MyScene.m_MeshList.push_back(pMesh);
}
void Exit()
{
	exit(-1);
}

void Keyboard(unsigned char key, int x, int y)
{
	EgMesh *pMesh = MyScene.m_MeshList[0];

	if (key == 27)
		Exit();

	if (MyScene.m_MeshList.empty())
		return;

	if (key == 's')
		pMesh->UpdateNormal(true);

	if (key == 'f')
		pMesh->UpdateNormal(false);

	if (key == '1' && pMesh != NULL)
	{
		EgSmoother smoother(pMesh);
		smoother.Execute();
		pMesh->UpdateNormal(0);
	}

	if (key == '2' && pMesh != NULL)
	{
		std::vector<EgFace *> Faces;
		EgFace *pFace = pMesh->m_pFaces;
		do
		{
			if (rand() % 4 == 2)
				Faces.push_back(pFace);
			pFace = pFace->m_pNext;
		} while (pFace != NULL);
		bool ret = pMesh->DelFaces(Faces);

		//삼각형이 남아있지 않다면
		if (!ret)
		{
			printf("All faces are deleted...\n");
		}
		pMesh->DelUnusedVert();
	}

	if (key == '3' && pMesh != NULL)
	{
		std::vector<EgVertex *> Verts;
		EgVertex *pVert = pMesh->m_pVerts;
		do
		{
			if (rand() % 4 == 2)
				Verts.push_back(pVert);
			pVert = pVert->m_pNext;
		} while (pVert != NULL);
		bool ret = pMesh->DelVerts(Verts);

		//정점이 남아있지 않다면
		if (!ret)
		{
			delete pMesh;
			pMesh = NULL;
		}
		pMesh->DelUnusedVert();
	}
	if (key == 'd' && pMesh != NULL)
	{
		EgDecimator decimator(pMesh);

		clock_t st = clock();

		decimator.InitVertQEM();
		decimator.InitCollapseInfo();
		decimator.Run(0.3);

		printf("Total Elapsed time for Decimation = %u ms \n", clock() - st);
		printf("# of vertices = %d\n", pMesh->GetNumVert());
		printf("# of faces = %d\n", pMesh->GetNumFace());
		pMesh->DelUnusedVert();
	}
	if (key == 't' && pMesh != NULL)
	{
		EgDecimator2 decimator2(pMesh);

		clock_t st = clock();

		decimator2.InitVertQEM();
		decimator2.InitCollapseInfo();
		decimator2.Run(0.3);
		
		printf("Total Elapsed time for Decimation = %u ms \n", clock() - st);
		printf("# of vertices = %d\n", pMesh->GetNumVert());
		printf("# of faces = %d\n", pMesh->GetNumFace());
		pMesh->DelUnusedVert();
	}

		
	if (key == '6' && pMesh != NULL)
	{
		export_obj(pMesh, "exported_mesh.obj");
	}

	glutPostRedisplay();
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
	MyScene.RenderScene();

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
