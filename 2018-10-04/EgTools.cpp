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

	//"���÷��̸��" �� ����
	//���� - "���÷��̸��" �� ������
	//���ڴ� GLUT ���̺귯���� ���ǵǾ� �ִ� ������ OR ��Ʈ �����Ͽ� ���� ���ε�, ����(color) ���� ������ �� , ������ ������ �����ؾ��մϴ�.
	//GLUT_RGBA �Ǵ� GLUT_RGB - RGBA â�� ����ϴ�. �� ���� glut �� ����ϴ� �⺻ ���� ����Դϴ�.
	//GLUT_INDEX - �ε��� ���� ���� �����մϴ�.
	//GLUT_SINGLE - ���Ϲ���(single buffer) â�� �����.
	//GLUT_DOUBLE - ���߹���(double buffer) â�� �����.�ε巯�� ���ϸ��̼��� ���� �� �ʿ��մϴ�.
	//�׸��� �������� ���Ѵٸ� Ư�� ���۷� ������ â�� ���� �� �ֽ��ϴ�
	//GLUT_ACCUM - ��ť�ķ��̼� ����( The accumulation buffer )
	//GLUT_STENCIL - ���ٽ� ����(The stencil buffer)
	//GLUT_DEPTH - ���� ����(The depth buffer)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	// Create OpenGL window of size Width by Height
	glutInitWindowSize(Width, Height);
	glutCreateWindow("EgTools");
	//--������� �����ϸ�, OpenGL â�� ���� ������ ������ �ܼ� �����츸 �� �� �ֽ��ϴ�. �Դٰ� �� �ʰ� ������ �� �ܼ�â ������ �������. --

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

	// GLUT�̺�Ʈ ó�� ������ ����. GLUT���α׷����� �ѹ��� ȣ��Ǿ����. ��ƾ�� ��ȯ�����ʴ´�. �ʿ信 ���� ��ϵ� ��� �ݹ��� ȣ��
	glutMainLoop();
	return 0;
}

void InitOpenGL()
{
	//depth �񱳸� ����, depth ���۸� ������Ʈ
	glEnable(GL_DEPTH_TEST);
	//���� �Ķ���͸� ����Ͽ� �������� �Ǵ� �ε����� ���. 
	glEnable(GL_LIGHTING);
	//���� ������ ��꿡 light0�� ���Խ�Ų��
	glEnable(GL_LIGHT0);
	//������ ��ǥ���� winding�� ������� �������� �ø��Ѵ�
	glEnable(GL_CULL_FACE);
	//glCullFace : �ո� �Ǵ� �޸��� ��Ÿ���� facet�� ���� ���� ���� ����
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

		//�ﰢ���� �������� �ʴٸ�
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

		//������ �������� �ʴٸ�
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
