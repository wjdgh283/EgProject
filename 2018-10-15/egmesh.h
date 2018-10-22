#pragma once

#include <vector>
#include <map>
//#include "egmath.h"
#include "egmesh_io.h"
//#include "GL\freeglut.h"

/* EgMesh 자료 구조 */
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
*	\brief 메쉬를 구성하는 정점을 표현하는 클래스
*
*	\author 박정호, 윤승현
*	\date 25 Jan 2018
*/
class EgVertex
{
public:
	/*! \brief 정점의 인덱스 */
	int m_Idx;

	/*! \brief 정점의 3차원 좌표 */
	GPoint3 m_Pos;

	/*! \brief 정점 배열에서 이전 정점에 대한 포인터 */
	EgVertex *m_pPrev;

	/*! \brief 정점 배열에서 다음 정점에 대한 포인터 */
	EgVertex *m_pNext;

	/*! \brief 정점에서 시작하는 에지에 대한 포인터 배열 */
	std::vector<EgEdge *> m_pEdges;

public:
	// 생성자 및 소멸자
	EgVertex(double x = 0.0, double y = 0.0, double z = 0.0);
	EgVertex(const EgVertex &cpy);
	~EgVertex();

	// 기하 정보 추출 함수들
	int GetNumIncidentFace();
	int GetOneRingVert(std::vector<EgVertex *> &Verts);
	int GetOneRingFace(std::vector<EgFace *> &Faces);
};

/*!
*	\class EgTexel
*	\brief 메쉬의 텍스처 좌표를 표현하는 클래스
*
*	\author 박정호, 윤승현
*	\date 27 Jun 2018
*/
class EgTexel
{
public:
	/*! \brief 텍셀의 인덱스 */
	int m_Idx;

	/*! \brief 텍스처 좌표 */
	GVector2 m_Coord;

	/*! \brief 텍셀 배열의 이전 텍셀에 대한 포인터 */
	EgTexel *m_pPrev;

	/*! \brief 텍셀 배열의 다음 텍셀에 대한 포인터 */
	EgTexel *m_pNext;

public:
	// 생성자 및 소멸자
	EgTexel(double u = 0.0, double v = 0.0);
	EgTexel(const EgTexel &cpy);
	~EgTexel();
};

/*!
*	\class EgNormal
*	\brief 메쉬의 법선 정보를 표현하는 클래스
*
*	\author 박정호, 윤승현
*	\date 27 Jun 2018
*/
class EgNormal
{
public:
	/*! \brief 법선의 인덱스 */
	int m_Idx;

	/*! \brief 법선의 방향 */
	GVector3 m_Dir;

	/*! \brief 법선 배열의 이전 법선에 대한 포인터 */
	EgNormal *m_pPrev;

	/*! \brief 법선 배열의 다음 법선에 대한 포인터 */
	EgNormal *m_pNext;

public:
	// 생성자 및 소멸자
	EgNormal(double nx = 0.0, double ny = 0.0, double nz = 0.0);
	EgNormal(const EgNormal &cpy);
	~EgNormal();
};

/*!
*	\class EgEdge
*	\brief 방향성 에지(Half Edge)를 표현하는 클래스
*
*	\author 박정호, 윤승현
*	\date 25 Jan 2018
*/
class EgEdge
{
public:
	/*! \brief 에지의 시작 정점에 대한 포인터 */
	EgVertex *m_pVert;

	/*! \brief 에지의 시작 위치에서 참조하는 텍셀에 대한 포인터 */
	EgTexel *m_pTexel;

	/*! \brief 에지의 시작 위치에서 참조하는 법선에 대한 포인터 */
	EgNormal *m_pNormal;

	/*! \brief 삼각형을 구성하는 다음 에지에 대한 포인터 */
	EgEdge *m_pNext;

	/*! \brief 인접한 삼각형의 반대편 에지에 대한 포인터 */
	EgEdge *m_pMate;

	/*! \brief 에지가 포함된 삼각형에 대한 포인터 */
	EgFace *m_pFace;

	/*! \brief 에지에 부여된 비용(가중치) 등의 정보 */
	double m_Cost;

public:
	// 생성자 및 소멸자
	EgEdge(EgVertex *pVert = NULL, EgTexel *pTexel = NULL, EgNormal *pNormal = NULL);
	EgEdge(const EgEdge &cpy);
	~EgEdge();
};

/*!
*	\class EgFace
*	\brief 메쉬의 삼각형을 표현하는 클래스
*
*	\author 박정호, 윤승현
*	\date 25 Jan 2018
*/
class EgFace
{
public:
	/*! \brief 삼각형의 인덱스 */
	int m_Idx;

	/*! \brief 삼각형의 시작 에지에 대한 포인터 */
	EgEdge *m_pEdge;

	/*! \brief 삼각형 배열의 이전 삼각형에 대한 포인터 */
	EgFace *m_pPrev;

	/*! \brief 삼각형 배열의 다음 삼각형에 대한 포인터 */
	EgFace *m_pNext;

	/*! \brief 삼각형이 사용하는 재질에 대한 포인터 */
	EgMaterial *m_pMaterial;

public:
	// 생성자 및 소멸자
	EgFace(EgEdge *pEdge0 = NULL, EgEdge *pEdge1 = NULL, EgEdge *pEdge2 = NULL, EgMaterial *pMtl = NULL);
	EgFace(const EgFace &cpy);
	~EgFace();

	// 멤버함수
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
*	\brief 메쉬가 사용하는 재질을 표현하는 클래스
*
*	\author 박정호, 윤승현
*	\date 2 Jul. 2018
*/
class EgMaterial
{
public:
	/*! \brief 재질의 인덱스 */
	int m_Idx;

	/*! \brief 재질의 이름 */
	std::string m_Name;

	/*! \brief 재질의 주변광 반사계수 */
	GLfloat m_Ambient[4];

	/*! \brief 재질의 난반사 계수 */
	GLfloat m_Diffuse[4];

	/*! \brief 재질의 전반사 계수 */
	GLfloat m_Specular[4];

	/*! \brief 재질의 전반사 지수 */
	GLfloat m_Shiness;

	/*! \brief 택스처 파일의 이름 */
	std::string m_TexFileName;

	/*! \brief 재질의 난반사 텍스처 아이디 (1부터 시작됨)*/
	GLuint m_TextureId;

	/*! \brief 재질 배열의 이전 재질에 대한 포인터 */
	EgMaterial *m_pPrev;

	/*! \brief 재질 배열의 다음 재질에 대한 포인터 */
	EgMaterial *m_pNext;

public:
	// 생성자 및 소멸자
	EgMaterial(std::string Name = "Mtl_Default");
	EgMaterial(const EgMaterial &cpy);
	~EgMaterial();

	// 멤버함수
	void SetAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f);
	void SetDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f);
	void SetSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f);
	void SetShiness(GLfloat s);
	void LoadTexture(const char *fname);
	void Apply(bool bTexture);
};

/*!
*	\class EgMesh
*	\brief 삼각형으로 표현된 3차원 메쉬를 표현하는 클래스
*
*	\author 박정호, 윤승현
*	\date 25 Jan 2018
*/
class EgMesh
{
public:
	/*! \brief 메쉬의 이름 */
	std::string m_Name;

	/*! \brief 메쉬가 정의된 모델링 좌표계 */
	GTransf m_MC;

	/*! \brief 메쉬의 정점 배열의 시작 정점에 대한 포인터 */
	EgVertex *m_pVerts;
	
	/*! \brief 메쉬의 텍셀 배열의 시작 텍셀에 대한 포인터 */
	EgTexel *m_pTexels;

	/*! \brief 메쉬의 법선 배열의 시작 법선에 대한 포인터 */
	EgNormal *m_pNormals;

	/*! \brief 메쉬의 삼각형 배열의 시작 삼각형에 대한 포인터 */
	EgFace *m_pFaces;

	/*! \brief 메쉬의 재질 배열의 시작 재질에 대한 포인터 */
	EgMaterial *m_pMaterials;

	/*! \brief 메쉬의 그룹별 삼각형 포인터 배열 */
	std::map<std::string, std::vector<EgFace *>> m_GroupedFaces;

public:
	// 생성자 및 소멸자
	EgMesh(std::string Name = "");
	~EgMesh();

	// 메쉬에 구성 요소를 추가(제거)하는 함수들
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

	// 메쉬의 정점, 텍셀, 법선, 삼각형 인덱스를 초기화하는 함수
	int UpdateVertIdx();
	int UpdateTexIdx();
	int UpdateNormalIdx();
	int UpdateFaceIdx();
	int UpdateMaterialIdx();

	// 메쉬의 에지와 법선 정보를 처리하는 함수들
	void InitEdgeMate();
	void UpdateEdgeMate(EgVertex *pVert);
	void UpdateNormal(bool bSmooth);
	
	// 메쉬의 정보 추출 함수들
	int GetNumVert();
	int GetNumTexel();
	int GetNumNormal();
	int GetNumFace();
	int GetNumMtrl();
	EgFace *GetFace(int Idx);
	EgVertex *GetVert(int Idx);
	EgMaterial *GetMaterial(int Idx);
	EgMaterial *GetMaterial(std::string Name);

	// 렌더링 관련 함수
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
*	\brief 3차원 장면을 표현하는 클래스
*
*	\author 박정호, 윤승현
*	\date 16 Aug. 2018
*/
class EgScene
{
public:
	/*! \brief 장면의 이름 */
	std::string m_Name;

	/*! \brief 3차원 장면에 포함된 메쉬 리스트 */
	std::vector<EgMesh *> m_MeshList;

public:
	// 생성자 및 소멸자
	EgScene(std::string Name = "");
	~EgScene();

public:
	// 멤버 함수
	void AddMesh(EgMesh *pMesh);
	void RenderFloor();
	void RenderScene();
};
