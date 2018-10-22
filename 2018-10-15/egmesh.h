#pragma once

#include <vector>
#include <map>
//#include "egmath.h"
#include "egmesh_io.h"
//#include "GL\freeglut.h"

/* EgMesh �ڷ� ���� */
class EgVertex;
class EgTexel;
class EgNormal;
class EgEdge;
class EgFace;
class EgMaterial;
class EgMesh;

#define FREE_PTR_LIST(FIRST, TYPE) \
	{ \
		if (FIRST){	\
			TYPE *pCurr = FIRST; \
			do \
			{	\
				TYPE *pNext = pCurr->m_pNext;	\
				delete pCurr;	\
				pCurr = pNext;	\
			} while (pCurr != NULL);	\
			FIRST = NULL; \
		}	\
	}

#define ADD_TO_PTR_LIST(FIRST, ELEM) \
	{	\
		if (FIRST == NULL){ \
			FIRST = ELEM; \
			FIRST->m_pPrev = ELEM; \
			FIRST->m_pNext = NULL; \
		} \
		else { \
			FIRST->m_pPrev->m_pNext = ELEM; \
			ELEM->m_pNext = NULL;	\
			ELEM->m_pPrev = FIRST->m_pPrev; \
			FIRST->m_pPrev = ELEM; \
		} \
	}

#define DEL_FROM_PTR_LIST(FIRST, ELEM) \
	{	\
		if (FIRST->m_pNext == NULL && FIRST == ELEM) \
		{ \
			delete FIRST; \
			FIRST = NULL; \
		} \
		else \
		{ \
			if (ELEM == FIRST) \
			{ \
				FIRST = FIRST->m_pNext; \
				ELEM->m_pPrev->m_pNext = NULL; \
				ELEM->m_pNext->m_pPrev = ELEM->m_pPrev;	\
			} \
			else if (ELEM == FIRST->m_pPrev) \
			{ \
				ELEM->m_pPrev->m_pNext = NULL; \
				FIRST->m_pPrev = ELEM->m_pPrev; \
			} \
			else \
			{ \
				ELEM->m_pPrev->m_pNext = ELEM->m_pNext; \
				ELEM->m_pNext->m_pPrev = ELEM->m_pPrev;	\
			} \
			delete ELEM; \
		} \
	}

#define DEL_FROM_VEC(VEC, ELEM) \
	{ \
		for (size_t i = 0; i < (VEC).size(); ++i) \
		{ \
			if ((VEC)[i] == (ELEM)) \
			{ \
				(VEC).erase((VEC).begin() + i); \
				break; \
			} \
		} \
	}

#define IS_MATE_EDGE(E1, E2) (((E1)->m_pVert) == ((E2)->m_pNext->m_pVert))

#define SET_MATE_EDGE(E1, E2) \
	{ \
		E1->m_pMate = E2; \
		E2->m_pMate = E1; \
	}


/*!
*	\class EgVertex
*	\brief �޽��� �����ϴ� ������ ǥ���ϴ� Ŭ����
*
*	\author ����ȣ, ������
*	\date 25 Jan 2018
*/
class EgVertex
{
public:
	/*! \brief ������ �ε��� */
	int m_Idx;

	/*! \brief ������ 3���� ��ǥ */
	GPoint3 m_Pos;

	/*! \brief ���� �迭���� ���� ������ ���� ������ */
	EgVertex *m_pPrev;

	/*! \brief ���� �迭���� ���� ������ ���� ������ */
	EgVertex *m_pNext;

	/*! \brief �������� �����ϴ� ������ ���� ������ �迭 */
	std::vector<EgEdge *> m_pEdges;

public:
	// ������ �� �Ҹ���
	EgVertex(double x = 0.0, double y = 0.0, double z = 0.0);
	EgVertex(const EgVertex &cpy);
	~EgVertex();

	// ���� ���� ���� �Լ���
	int GetNumIncidentFace();
	int GetOneRingVert(std::vector<EgVertex *> &Verts);
	int GetOneRingFace(std::vector<EgFace *> &Faces);
};

/*!
*	\class EgTexel
*	\brief �޽��� �ؽ�ó ��ǥ�� ǥ���ϴ� Ŭ����
*
*	\author ����ȣ, ������
*	\date 27 Jun 2018
*/
class EgTexel
{
public:
	/*! \brief �ؼ��� �ε��� */
	int m_Idx;

	/*! \brief �ؽ�ó ��ǥ */
	GVector2 m_Coord;

	/*! \brief �ؼ� �迭�� ���� �ؼ��� ���� ������ */
	EgTexel *m_pPrev;

	/*! \brief �ؼ� �迭�� ���� �ؼ��� ���� ������ */
	EgTexel *m_pNext;

public:
	// ������ �� �Ҹ���
	EgTexel(double u = 0.0, double v = 0.0);
	EgTexel(const EgTexel &cpy);
	~EgTexel();
};

/*!
*	\class EgNormal
*	\brief �޽��� ���� ������ ǥ���ϴ� Ŭ����
*
*	\author ����ȣ, ������
*	\date 27 Jun 2018
*/
class EgNormal
{
public:
	/*! \brief ������ �ε��� */
	int m_Idx;

	/*! \brief ������ ���� */
	GVector3 m_Dir;

	/*! \brief ���� �迭�� ���� ������ ���� ������ */
	EgNormal *m_pPrev;

	/*! \brief ���� �迭�� ���� ������ ���� ������ */
	EgNormal *m_pNext;

public:
	// ������ �� �Ҹ���
	EgNormal(double nx = 0.0, double ny = 0.0, double nz = 0.0);
	EgNormal(const EgNormal &cpy);
	~EgNormal();
};

/*!
*	\class EgEdge
*	\brief ���⼺ ����(Half Edge)�� ǥ���ϴ� Ŭ����
*
*	\author ����ȣ, ������
*	\date 25 Jan 2018
*/
class EgEdge
{
public:
	/*! \brief ������ ���� ������ ���� ������ */
	EgVertex *m_pVert;

	/*! \brief ������ ���� ��ġ���� �����ϴ� �ؼ��� ���� ������ */
	EgTexel *m_pTexel;

	/*! \brief ������ ���� ��ġ���� �����ϴ� ������ ���� ������ */
	EgNormal *m_pNormal;

	/*! \brief �ﰢ���� �����ϴ� ���� ������ ���� ������ */
	EgEdge *m_pNext;

	/*! \brief ������ �ﰢ���� �ݴ��� ������ ���� ������ */
	EgEdge *m_pMate;

	/*! \brief ������ ���Ե� �ﰢ���� ���� ������ */
	EgFace *m_pFace;

	/*! \brief ������ �ο��� ���(����ġ) ���� ���� */
	double m_Cost;

public:
	// ������ �� �Ҹ���
	EgEdge(EgVertex *pVert = NULL, EgTexel *pTexel = NULL, EgNormal *pNormal = NULL);
	EgEdge(const EgEdge &cpy);
	~EgEdge();
};

/*!
*	\class EgFace
*	\brief �޽��� �ﰢ���� ǥ���ϴ� Ŭ����
*
*	\author ����ȣ, ������
*	\date 25 Jan 2018
*/
class EgFace
{
public:
	/*! \brief �ﰢ���� �ε��� */
	int m_Idx;

	/*! \brief �ﰢ���� ���� ������ ���� ������ */
	EgEdge *m_pEdge;

	/*! \brief �ﰢ�� �迭�� ���� �ﰢ���� ���� ������ */
	EgFace *m_pPrev;

	/*! \brief �ﰢ�� �迭�� ���� �ﰢ���� ���� ������ */
	EgFace *m_pNext;

	/*! \brief �ﰢ���� ����ϴ� ������ ���� ������ */
	EgMaterial *m_pMaterial;

public:
	// ������ �� �Ҹ���
	EgFace(EgEdge *pEdge0 = NULL, EgEdge *pEdge1 = NULL, EgEdge *pEdge2 = NULL, EgMaterial *pMtl = NULL);
	EgFace(const EgFace &cpy);
	~EgFace();

	// ����Լ�
	EgVertex *GetVertex(int vIdx);
	EgTexel *GetTexel(int tIdx);
	EgNormal *GetNormal(int nIdx);
	EgEdge *GetEdge(int eIdx);
	int GetEdgeIdx(EgEdge *pEdge);
	GVector3 GetFaceNormal();
	void GetIncidentFace(std::vector<EgFace *> &Faces);
};

/*!
*	\class EgMaterial
*	\brief �޽��� ����ϴ� ������ ǥ���ϴ� Ŭ����
*
*	\author ����ȣ, ������
*	\date 2 Jul. 2018
*/
class EgMaterial
{
public:
	/*! \brief ������ �ε��� */
	int m_Idx;

	/*! \brief ������ �̸� */
	std::string m_Name;

	/*! \brief ������ �ֺ��� �ݻ��� */
	GLfloat m_Ambient[4];

	/*! \brief ������ ���ݻ� ��� */
	GLfloat m_Diffuse[4];

	/*! \brief ������ ���ݻ� ��� */
	GLfloat m_Specular[4];

	/*! \brief ������ ���ݻ� ���� */
	GLfloat m_Shiness;

	/*! \brief �ý�ó ������ �̸� */
	std::string m_TexFileName;

	/*! \brief ������ ���ݻ� �ؽ�ó ���̵� (1���� ���۵�)*/
	GLuint m_TextureId;

	/*! \brief ���� �迭�� ���� ������ ���� ������ */
	EgMaterial *m_pPrev;

	/*! \brief ���� �迭�� ���� ������ ���� ������ */
	EgMaterial *m_pNext;

public:
	// ������ �� �Ҹ���
	EgMaterial(std::string Name = "Mtl_Default");
	EgMaterial(const EgMaterial &cpy);
	~EgMaterial();

	// ����Լ�
	void SetAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f);
	void SetDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f);
	void SetSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f);
	void SetShiness(GLfloat s);
	void LoadTexture(const char *fname);
	void Apply(bool bTexture);
};

/*!
*	\class EgMesh
*	\brief �ﰢ������ ǥ���� 3���� �޽��� ǥ���ϴ� Ŭ����
*
*	\author ����ȣ, ������
*	\date 25 Jan 2018
*/
class EgMesh
{
public:
	/*! \brief �޽��� �̸� */
	std::string m_Name;

	/*! \brief �޽��� ���ǵ� �𵨸� ��ǥ�� */
	GTransf m_MC;

	/*! \brief �޽��� ���� �迭�� ���� ������ ���� ������ */
	EgVertex *m_pVerts;
	
	/*! \brief �޽��� �ؼ� �迭�� ���� �ؼ��� ���� ������ */
	EgTexel *m_pTexels;

	/*! \brief �޽��� ���� �迭�� ���� ������ ���� ������ */
	EgNormal *m_pNormals;

	/*! \brief �޽��� �ﰢ�� �迭�� ���� �ﰢ���� ���� ������ */
	EgFace *m_pFaces;

	/*! \brief �޽��� ���� �迭�� ���� ������ ���� ������ */
	EgMaterial *m_pMaterials;

	/*! \brief �޽��� �׷캰 �ﰢ�� ������ �迭 */
	std::map<std::string, std::vector<EgFace *>> m_GroupedFaces;

public:
	// ������ �� �Ҹ���
	EgMesh(std::string Name = "");
	~EgMesh();

	// �޽��� ���� ��Ҹ� �߰�(����)�ϴ� �Լ���
	void AddVertex(EgVertex *pVert);
	void AddTexel(EgTexel *pTexel);
	void AddNormal(EgNormal *pNormal);
	void AddFace(EgFace *pFace, std::string GroupName);
	void AddMaterial(EgMaterial *pMaterial);
	bool DelFaces(std::vector<EgFace *> &Faces);
	bool DelVerts(std::vector<EgVertex *> &Verts);
	bool DelEdges(std::vector<EgEdge *> &Edges);
	int DelUnusedVert();
	int DelUnusedTexel();
	int DelUnusedNormal();

	// �޽��� ����, �ؼ�, ����, �ﰢ�� �ε����� �ʱ�ȭ�ϴ� �Լ�
	int UpdateVertIdx();
	int UpdateTexIdx();
	int UpdateNormalIdx();
	int UpdateFaceIdx();
	int UpdateMaterialIdx();

	// �޽��� ������ ���� ������ ó���ϴ� �Լ���
	void InitEdgeMate();
	void UpdateEdgeMate(EgVertex *pVert);
	void UpdateNormal(bool bSmooth);
	
	// �޽��� ���� ���� �Լ���
	int GetNumVert();
	int GetNumTexel();
	int GetNumNormal();
	int GetNumFace();
	int GetNumMtrl();
	EgFace *GetFace(int Idx);
	EgVertex *GetVert(int Idx);
	EgMaterial *GetMaterial(int Idx);
	EgMaterial *GetMaterial(std::string Name);

	// ������ ���� �Լ�
	void Render();
	void RenderPolygon();
	void RenderTexture();
	void RenderWire();
	void RenderBndry();
	void RenderTexBndry();
	void RenderPoint();
};

/*!
*	\class EgScene
*	\brief 3���� ����� ǥ���ϴ� Ŭ����
*
*	\author ����ȣ, ������
*	\date 16 Aug. 2018
*/
class EgScene
{
public:
	/*! \brief ����� �̸� */
	std::string m_Name;

	/*! \brief 3���� ��鿡 ���Ե� �޽� ����Ʈ */
	std::vector<EgMesh *> m_MeshList;

public:
	// ������ �� �Ҹ���
	EgScene(std::string Name = "");
	~EgScene();

public:
	// ��� �Լ�
	void AddMesh(EgMesh *pMesh);
	void RenderFloor();
	void RenderScene();
};
