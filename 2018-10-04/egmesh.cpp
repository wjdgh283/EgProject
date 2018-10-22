#include "..\usr\include\egmesh.h"
#include <map>
#include <algorithm>

/**********************/
/* EgVertex 클래스 구현 */
/**********************/
/*!
*	\brief 생성자
*
*	\param x 정점의 x좌표
*	\param y 정점의 y좌표
*	\param z 정점의 z좌표
*/
EgVertex::EgVertex(double x, double y, double z)
{
	m_Idx = -1;
	m_Pos = GPoint3(x, y, z);
	m_pPrev = m_pNext = NULL;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 정점의 객체
*/
EgVertex::EgVertex(const EgVertex &cpy)
{
	m_Idx = cpy.m_Idx;
	m_Pos = cpy.m_Pos;
	m_pPrev = cpy.m_pPrev;
	m_pNext = cpy.m_pNext;
	m_pEdges = cpy.m_pEdges;
}

/*!
*	\brief 소멸자
*/
EgVertex::~EgVertex()
{
}

/*!
*	\brief 정점을 공유하고 있는 삼각형의 개수를 반환한다.
*	\note 내부 정점의 경우 밸런스와 같고, 경계 정점의 경우, 밸런스 보다 적다.
*
*	\return 정점을 공유하고 있는 삼각형의 개수를 반환한다.
*/
int EgVertex::GetNumIncidentFace()
{
	return (int)m_pEdges.size();
}

/*!
*	\brief 정점의 1링 정점을 구한다.
*	\note 내부/외부 구분없이 에지에 연결된 정점을 모두 구한다.
*
*	\param Verts 1링을 구성하는 정점의 포인터가 저장된다.
*
*	\return 1링을 구성하는 정점의 수를 반환한다.
*/
int EgVertex::GetOneRingVert(std::vector<EgVertex *> &Verts)
{
	for (EgEdge *&e : m_pEdges)
	{
		Verts.push_back(e->m_pNext->m_pVert);
		if (e->m_pNext->m_pNext->m_pMate == NULL)
			Verts.push_back(e->m_pNext->m_pNext->m_pVert);
	}

	return (int)Verts.size();
}

/*!
*	\brief 정점을 공유하는 1링 삼각형 리스트를 구한다.
*
*	\param Faces 정점을 공유하는 삼각형 포인터가 저장된다.
*
*	\return 정점을 공유하는 1링 삼각형의 수를 반환한다.
*/
int EgVertex::GetOneRingFace(std::vector<EgFace *> &Faces)
{
	if (m_pEdges.empty())
		return 0;

	// 정점에서 시작하는 각각의 에지가 포함된 삼각형을 구한다.
	for (EgEdge *&e : m_pEdges)
		Faces.push_back(e->m_pFace);

	// 삼각형의 수를 반환한다.
	return (int)Faces.size();
}

/*********************/
/* EgTexel 클래스 구현 */
/*********************/
/*!
*	\brief 생성자
*
*	\param u 수평 방향 텍스처 좌표
*	\param v 수직 방향 텍스처 좌표
*/
EgTexel::EgTexel(double u, double v)
{
	m_Idx = -1;
	m_Coord[0] = u;
	m_Coord[1] = v;
	m_pPrev = m_pNext = NULL;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사할 텍셀 객체
*/
EgTexel::EgTexel(const EgTexel &cpy)
{
	m_Idx = cpy.m_Idx;
	m_Coord = cpy.m_Coord;
	m_pPrev = cpy.m_pPrev;
	m_pNext = cpy.m_pNext;
}

/*!
*	\brief 소멸자
*/
EgTexel::~EgTexel()
{
}

/**********************/
/* EgNormal 클래스 구현 */
/**********************/
/*!
*	\brief 생성자
*
*	\param nx 법선의 x 좌표
*	\param ny 법선의 y 좌표
*	\param nz 법선의 z 좌표
*/
EgNormal::EgNormal(double nx, double ny, double nz)
{
	m_Dir = GVector3(nx, ny, nz);
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
EgNormal::EgNormal(const EgNormal &cpy)
{
	m_Dir = cpy.m_Dir;
}

/*!
*	\brief 소멸자
*/
EgNormal::~EgNormal()
{
}

/************************/
/* EgEdge 클래스 구현 */
/************************/
/*!
*	\brief 생성자
*
*	\param pVert 에지의 시작 정점에 대한 포인터
*	\param pTexel 에지의 시작 정점의 텍셀에 대한 포인터
*	\param pNormal 에지의 시작 정점의 법선에 대한 포인터
*/
EgEdge::EgEdge(EgVertex *pVert, EgTexel *pTexel, EgNormal *pNormal)
{
	m_pVert = pVert;
	m_pTexel = pTexel;
	m_pNormal = pNormal;
	m_pNext = NULL;
	m_pMate = NULL;
	m_pFace = NULL;
	m_pVert->m_pEdges.push_back(this);
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 하프에지 객체
*/
EgEdge::EgEdge(const EgEdge &cpy)
{
	m_pVert = cpy.m_pVert;
	m_pTexel = cpy.m_pTexel;
	m_pNormal = cpy.m_pNormal;
	m_pNext = cpy.m_pNext;
	m_pMate = cpy.m_pMate;
	m_pFace = cpy.m_pFace;
}

/*!
*	\brief 소멸자
*/
EgEdge::~EgEdge()
{
}

/*!
*	\brief 에지가 제거 가능한지 검사한다.
*
*	\return 에지가 제거가능할 경우 true, 아니면 false를 반환한다.
*/
bool EgEdge::IsCollapsible()
{
	// 경계 에지 포함조건 
	//if (m_pMate == NULL || m_pNext->m_pMate == NULL || m_pNext->m_pNext->m_pMate == NULL)
	//	return false;
	//
	//EgEdge *e = m_pMate;
	//if (e->m_pNext->m_pMate == NULL || e->m_pNext->m_pNext->m_pMate == NULL)
	//	return false;

	// 에지의 시작점(v1)과 끝점(v2)을 구한다.
	EgVertex *v1 = m_pVert;
	EgVertex *v2 = m_pNext->m_pVert;

	// v1과 v2의 1링 정점의 집합 A와 B를 구한다.
	std::vector<EgVertex *> A, B;
	v1->GetOneRingVert(A);
	v2->GetOneRingVert(B);

	// A와 B의 교집합을 구한다.
	std::vector<EgVertex *> C;
	for (EgVertex *a : A)
	{
		for (EgVertex *b : B)
		{
			if (a == b)
			{
				C.push_back(a);
				break;
			}
		}
	}

	// 에지 제거의 조건을 만족할 경우 true를 반환한다.
	if (m_pMate == NULL && C.size() == 1)
		return true;
	if (m_pMate != NULL && C.size() == 2)
		return true;

	// 에지 제거 조건을 만족하지 못하는 경우 false를 반환한다.
	return false;
}

/********************/
/* EgFace 클래스 구현 */
/********************/
/*!
*	\brief 생성자
*
*	\param pEdge0 삼각형의 첫 번째 에지
*	\param pEdge1 삼각형의 두 번째 에지
*	\param pEdge2 삼각형의 세 번째 에지
*	\param pMtl 삼각형이 사용하는 재질에 대한 포인터
*/
EgFace::EgFace(EgEdge *pEdge0, EgEdge *pEdge1, EgEdge *pEdge2, EgMaterial *pMtl)
{
	m_Idx = -1;
	// 에지의 연결관계를 생성한다.
	m_pEdge = pEdge0;
	m_pEdge->m_pNext = pEdge1;
	pEdge1->m_pNext = pEdge2;
	pEdge2->m_pNext = m_pEdge;
	pEdge0->m_pFace = pEdge1->m_pFace = pEdge2->m_pFace = this;
	m_pMaterial = pMtl;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 삼각형 객체
*/
EgFace::EgFace(const EgFace &cpy)
{
	m_Idx = -1;
	m_pEdge = cpy.m_pEdge;
	m_pMaterial = cpy.m_pMaterial;
}

/*!
*	\brief 소멸자
*/
EgFace::~EgFace()
{
	delete m_pEdge->m_pNext->m_pNext;
	delete m_pEdge->m_pNext;
	delete m_pEdge;
	m_pEdge = NULL;
}

/*!
*	\brief 해당 인덱스의 정점의 포인터를 반환한다.
*
*	\param vIdx 찾을 정점의 인덱스
*
*	\return 해당 인덱스의 정점의 포인터를 반환한다.
*/
EgVertex *EgFace::GetVertex(int vIdx)
{
	return GetEdge(vIdx)->m_pVert;
}

/*!
*	\brief 해당 인덱스의 텍셀의 포인터를 반환한다.
*
*	\param vIdx 찾을 텍셀의 인덱스
*
*	\return 해당 인덱스의 텍셀의 포인터를 반환한다.
*/
EgTexel *EgFace::GetTexel(int tIdx)
{
	return GetEdge(tIdx)->m_pTexel;
}

/*!
*	\brief 해당 인덱스의 정점의 법선을 반환한다.
*
*	\param nIdx 찾을 법선의 인덱스
*
*	\return 해당 인덱스의 정점의 법선을 반환한다.
*/
EgNormal *EgFace::GetNormal(int nIdx)
{
	return GetEdge(nIdx)->m_pNormal;
}

/*!
*	\brief 해당 인덱스의 하프에지 포인터를 반환한다.
*
*	\param eIdx 찾을 인덱스(0 부터)
*
*	\return 해당 인덱스의 하프에지 포인터를 반환한다.
*/
EgEdge *EgFace::GetEdge(int eIdx)
{
	EgEdge *pRet = m_pEdge;
	for (int i = 0; i < eIdx; ++i)
		pRet = pRet->m_pNext;

	return pRet;
}

/*!
*	\brief 해당 에지의 인덱스를 반환한다.
*
*	\param pEdge 에지에 대한 포인터
*
*	\return 삼각형에서 에지의 인덱스를 반환한다.
*/
int EgFace::GetEdgeIdx(EgEdge *pEdge)
{
	if (pEdge == m_pEdge)
		return 0;
	if (pEdge == m_pEdge->m_pNext)
		return 1;
	if (pEdge == m_pEdge->m_pNext->m_pNext)
		return 2;

	return -1;
}

/*!
*	\brief 삼각형의 단위 법선벡터를 계산한다.
*
*	\return 삼각형의 단위 법선벡터를 반환한다.
*/
GVector3 EgFace::GetFaceNormal()
{
	// 삼각형의 법선을 계산하고,
	EgVertex *v0 = m_pEdge->m_pVert;
	EgVertex *v1 = m_pEdge->m_pNext->m_pVert;
	EgVertex *v2 = m_pEdge->m_pNext->m_pNext->m_pVert;
	GVector3 Ret = ((v1->m_Pos - v0->m_Pos) ^ (v2->m_Pos - v0->m_Pos)).Normalize();
	return Ret;
}

/*************************/
/* EgMaterial 클래스 구현 */
/*************************/
/*!
*	\brief 생성자
*
*	\param Name 생성할 재질의 이름
*/
EgMaterial::EgMaterial(std::string Name)
{
	m_Idx = -1;
	m_Name = Name;
	m_Ambient[0] = 0.2f;	m_Ambient[1] = 0.2f;	m_Ambient[2] = 0.2f;	m_Ambient[3] = 1.0f;
	m_Diffuse[0] = 0.7f;	m_Diffuse[1] = 0.7f;	m_Diffuse[2] = 0.7f;	m_Diffuse[3] = 1.0f;
	m_Specular[0] = 0.7f;	m_Specular[1] = 0.7f;	m_Specular[2] = 0.7f;	m_Specular[3] = 1.0f;
	m_Shiness = 32.0f;
	m_TextureId = 0;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사할 재질 객체
*/
EgMaterial::EgMaterial(const EgMaterial &cpy)
{
	m_Idx = cpy.m_Idx;
	m_Name = cpy.m_Name;
	memcpy(m_Ambient, cpy.m_Ambient, sizeof(GLfloat) * 4);
	memcpy(m_Diffuse, cpy.m_Diffuse, sizeof(GLfloat) * 4);
	memcpy(m_Specular, cpy.m_Specular, sizeof(GLfloat) * 4);
	m_Shiness = cpy.m_Shiness;
	m_TextureId = cpy.m_TextureId;
}

/*!
*	\brief 소멸자
*/
EgMaterial::~EgMaterial()
{
	if (m_TextureId != 0)
		glDeleteTextures(1, &m_TextureId);
}

/*!
*	\brief 재질의 주변광 반사계수를 설정한다.
*
*	\param r Red 성분에 대한 반사계수
*	\param g Green 성분에 대한 반사계수
*	\param b Blue 성분에 대한 반사계수
*	\param a Alpha 값
*/
void EgMaterial::SetAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_Ambient[0] = r;
	m_Ambient[1] = g;
	m_Ambient[2] = b;
	m_Ambient[3] = a;
}

/*!
*	\brief 재질의 난반사 계수를 설정한다.
*
*	\param r Red 성분에 대한 반사계수
*	\param g Green 성분에 대한 반사계수
*	\param b Blue 성분에 대한 반사계수
*	\param a Alpha 값
*/
void EgMaterial::SetDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_Diffuse[0] = r;
	m_Diffuse[1] = g;
	m_Diffuse[2] = b;
	m_Diffuse[3] = a;
}

/*!
*	\brief 재질의 전반사 계수를 설정한다.
*
*	\param r Red 성분에 대한 반사계수
*	\param g Green 성분에 대한 반사계수
*	\param b Blue 성분에 대한 반사계수
*	\param a Alpha 값
*/
void EgMaterial::SetSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_Diffuse[0] = r;
	m_Diffuse[1] = g;
	m_Diffuse[2] = b;
	m_Diffuse[3] = a;
}

/*!
*	\brief 재질의 전반사 지수를 설정한다.
*
*	\param s 재질의 전반사 지수
*/
void EgMaterial::SetShiness(GLfloat s)
{
	m_Shiness = s;
}

/*!
*	\brief 재질의 텍스처를 설정한다.
*	\note MFC를 사용할 경우, OpenGL 렌더링 상태에서 호출되어야 한다.
*
*	\param fname 텍스처 파일명
*/
void EgMaterial::LoadTexture(const char *fname)
{
	// 텍스처 파일의 확장자를 구하여
	std::string file_ext = get_file_ext(fname);

	// 확장자가 *.tga 파일은 경우,
	if (file_ext == "tga")
	{
		// 텍스처 파일명을 저장하고,
		m_TexFileName = std::string(fname);

		// 이미지 데이터를 로드하여,
		int w, h;
		GLenum format;
		unsigned char *imageData = open_image_tga(fname, &w, &h, &format);

		// 텍스처를 생성한다.
		if (imageData != NULL)
		{
			//텍스쳐 네임을 만들어서 m_TextureId에 저장
			glGenTextures(1, &m_TextureId);	// 1부터 시작됨...
											//m_TextureId에 텍스처 타겟을 바인딩시킨다 
			glBindTexture(GL_TEXTURE_2D, m_TextureId);
			//픽셀 저장 모드를 설정한다.
			//첫번째 인자 : 픽셀 데이터가 클라이언트 메모리로 반환되는 방식
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			//텍스처 파라미터를 설정
			//첫번째 인자 : 타겟 텍스쳐
			//두번째 인자 : 단일 값 텍스처 파라미터
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// or, GL_NEAREST
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// or, GL_NEAREST
																				//텍스쳐 환경 파라미터를 설정
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// or, GL_REPLACE
																				//2차원 텍스쳐 이미지를 지정
			glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, imageData);
			//2D 밉맵을 만든다
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, imageData);

			free(imageData);
		}
	}

	// 확장자가 *.bmp 파일은 경우,
	if (file_ext == "bmp")
	{
		// 텍스처 파일명을 저장하고,
		m_TexFileName = std::string(fname);

		// 이미지 데이터를 로드하여,
		BITMAPINFO	*info;
		unsigned char *imageData = open_image_bmp(fname, &info);

		// 텍스처를 생성한다.
		if (imageData != NULL)
		{
			// BGR을 RGB로 변환한다.
			int i;
			unsigned char *ptr;
			int w = info->bmiHeader.biWidth;
			int h = info->bmiHeader.biHeight;
			for (i = w * h, ptr = imageData; i > 0; i--, ptr += 3)
				SWAP(GLubyte, ptr[0], ptr[2])

				glGenTextures(1, &m_TextureId);
			glBindTexture(GL_TEXTURE_2D, m_TextureId);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// or, GL_NEAREST
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// or, GL_NEAREST
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// or, GL_REPLACE
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, imageData);

			free(info);
			free(imageData);
		}
	}
}

/*!
*	\brief 재질을 적용한다.
*
*	\param bTexture 텍스처 적용 여부
*/
void EgMaterial::Apply(bool bTexture)
{
	// 블렌딩을 활성화하고,
	if (m_Diffuse[3] != 1.0f)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
		glDisable(GL_BLEND);

	// 재질을 적용한다.
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_Ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_Diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_Specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_Shiness);

	// 재질이 텍스처를 사용할 경우 텍스처 바인딩을 활성화 한다.
	if (bTexture && m_TextureId != 0)
		glBindTexture(GL_TEXTURE_2D, m_TextureId);
}

/********************/
/* EgMesh 클래스 구현 */
/********************/
/*!
*	\brief 생성자
*
*	\param Name 생성할 메쉬의 이름
*/
EgMesh::EgMesh(std::string Name)
{
	m_Name = Name;
	m_pVerts = NULL;
	m_pTexels = NULL;
	m_pNormals = NULL;
	m_pMaterials = NULL;
	m_pFaces = NULL;
}

/*!
*	\brief 소멸자
*/
EgMesh::~EgMesh()
{
	// 정점, 텍셀, 노말, 삼각형, 재질의 리스트를 제거한다.
	FREE_PTR_LIST(m_pVerts, EgVertex);
	FREE_PTR_LIST(m_pTexels, EgTexel);
	FREE_PTR_LIST(m_pNormals, EgNormal);
	FREE_PTR_LIST(m_pFaces, EgFace);
	FREE_PTR_LIST(m_pMaterials, EgMaterial);
	printf("Mesh is deleted...\n");
}

/*!
*	\brief 메쉬에 정점을 추가한다.
*
*	\param pVert 추가할 정점에 대한 포인터
*/
void EgMesh::AddVertex(EgVertex *pVert)
{
	ADD_TO_PTR_LIST(m_pVerts, pVert);
}

/*!
*	\brief 메쉬에 텍셀을 추가한다.
*
*	\param pTexel 추가할 텍셀에 대한 포인터
*/
void EgMesh::AddTexel(EgTexel *pTexel)
{
	ADD_TO_PTR_LIST(m_pTexels, pTexel);
}

/*!
*	\brief 메쉬에 법선을 추가한다.
*
*	\param pNormal 추가할 법선에 대한 포인터
*/
void EgMesh::AddNormal(EgNormal *pNormal)
{
	ADD_TO_PTR_LIST(m_pNormals, pNormal);
}

/*!
*	\brief 메쉬에 삼각형을 추가한다.
*
*	\param pFace 추가할 삼각형에 대한 포인터
*/
void EgMesh::AddFace(EgFace *pFace)
{
	ADD_TO_PTR_LIST(m_pFaces, pFace);
}

/*!
*	\brief 메쉬에 재질을 추가한다.
*
*	\param pMaterial 추가할 재질에 대한 포인터
*/
void EgMesh::AddMaterial(EgMaterial *pMaterial)
{
	ADD_TO_PTR_LIST(m_pMaterials, pMaterial);
}

/*!
*	\brief 메쉬에서 지정된 삼각형(즉 3개의 하프에지)을 제거한다.
*	\note 삼각형 제거과정에서 생긴 사용되지 않는 정점, 텍셀, 법선은 제거되지 않는다.
*		  남은 정점, 텍셀, 법선을 제거하기 위해서는 RemoveUnused*() 함수를 별도로 호출한다.
*
*	\param Faces 제거할 삼각형의 포인터 리스트
*
*	\return 메쉬의 삼각형이 남았다면 true, 없다면 false를 반환한다.
*/
bool EgMesh::DelFaces(std::vector<EgFace *> &Faces)
{
	// 각각의 삼각형을 제거한다.
	for (EgFace *&f : Faces)
	{
		// 각각의 에지에 대하여 Mate 에지의 정보를 갱신한다.
		for (int i = 0; i < 3; ++i)
		{
			EgEdge *e = f->GetEdge(i);
			if (e->m_pMate != NULL)
				e->m_pMate->m_pMate = NULL;

			// 정점에서 시작하는 에지 리스트에서 현재 에지를 제거한다.
			EgVertex *v = e->m_pVert;
			DEL_FROM_VEC(v->m_pEdges, e);
		}

		// 삼각형을 제거한다.
		DEL_FROM_PTR_LIST(m_pFaces, f);
	}

	// 남아있는 삼각형이 없다면 false를 반환한다.
	if (m_pFaces == NULL)
		return false;

	return true;
}

/*!
*	\brief 메쉬에서 지정된 정점을 제거한다.
*	\note 제거과정에서 정점에 연결돤 삼각형들은 제거된다.
*
*	\param Verts 제거할 정점의 포인터 리스트
*
*	\return 메쉬의 정점이 남았다면 true, 없다면 false를 반환한다.
*/
bool EgMesh::DelVerts(std::vector<EgVertex*> &Verts)
{
	std::vector<EgFace *> Faces;

	// 각각의 정점의 1링 삼각형의 집합을 구한다.
	for (EgVertex *&v : Verts)
		for (EgEdge *&e : v->m_pEdges)
			Faces.push_back(e->m_pFace);

	// 중복된 객체를 제거한다.
	sort(Faces.begin(), Faces.end());
	Faces.erase(unique(Faces.begin(), Faces.end()), Faces.end());

	// 각각의 삼각형을 제거한다.
	DelFaces(Faces);

	// 각각의 정점을 제거한다.
	for (EgVertex *&v : Verts)
		DEL_FROM_PTR_LIST(m_pVerts, v);

	// 남아있는 정점이 없다면 false를 반환한다.
	if (m_pVerts == NULL)
		return false;

	return true;
}

/*!
*	\brief 메쉬에서 에지(삼각형)를 제거한다.
*
*	\param Edges 제거할 에지의 포인터 리스트
*
*	\return 남은 에지가 있으면 true, 없으면 false를 반환한다.
*/
bool EgMesh::DelEdges(std::vector<EgEdge *> &Edges)
{
	// 각각의 에지가 포함된 삼각형 집합을 구한다.
	std::vector<EgFace *> Faces;
	for (EgEdge *&e : Edges)
		Faces.push_back(e->m_pFace);

	// 중복된 객체를 제거한다.
	sort(Faces.begin(), Faces.end());
	Faces.erase(unique(Faces.begin(), Faces.end()), Faces.end());

	// 각각의 삼각형을 제거한다.
	DelFaces(Faces);

	// 남아있는 삼각형이 없다면 false를 반환한다.
	if (m_pFaces == NULL)
		return false;

	return true;
}

/*!
*	\brief 메쉬에서 사용되지 않는 정점을 제거한다.
*
*	\return 제거된 정점의 수를 반환한다.
*/
int EgMesh::DelUnusedVert()
{
	// 정점이 없다면 리턴한다.
	int ret = 0;
	if (m_pVerts == NULL)
		return ret;

	// 모든 정점의 인덱스를 -1로 초기화 한다.
	EgVertex *v = m_pVerts;
	while (v != NULL)
	{
		v->m_Idx = -1;
		v = v->m_pNext;
	}

	// 각각의 삼각형마다
	EgFace *f = m_pFaces;
	while (f != NULL)
	{
		// 에지가 사용하는 정점의 인덱스를 1로 표기한다.
		for (int i = 0; i < 3; ++i)
			f->GetVertex(i)->m_Idx = 1;
		f = f->m_pNext;
	}

	// 사용되지 않은 정점을 제거한다.
	v = m_pVerts;
	while (v != NULL)
	{
		if (v->m_Idx == -1)
		{
			EgVertex *pNext = v->m_pNext;
			DEL_FROM_PTR_LIST(m_pVerts, v);
			v = pNext;
			ret++;
		}
		else
			v = v->m_pNext;
	}

	// 제거된 정점의 수를 반환한다.
	return ret;
}

/*!
*	\brief 메쉬에서 사용되지 않는 텍셀을 제거한다.
*
*	\return 제거된 텍셀의 수를 반환한다.
*/
int EgMesh::DelUnusedTexel()
{
	// 텍셀이 없다면 리턴한다.
	int ret = 0;
	if (m_pTexels == NULL)
		return ret;

	// 모든 텍셀의 인덱스를 -1로 초기화한다.
	EgTexel *t = m_pTexels;
	while (t != NULL)
	{
		t->m_Idx = -1;
		t = t->m_pNext;
	}

	// 각각의 삼각형마다
	EgFace *f = m_pFaces;
	while (f != NULL)
	{
		// 에지가 사용하는 텍셀의 인덱스를 1로 표기한다.
		for (int i = 0; i < 3; ++i)
			f->GetTexel(i)->m_Idx = 1;
		f = f->m_pNext;
	}

	// 사용되지 않은 텍셀을 제거한다.
	t = m_pTexels;
	while (t != NULL)
	{
		if (t->m_Idx == -1)
		{
			EgTexel *pNext = t->m_pNext;
			DEL_FROM_PTR_LIST(m_pTexels, t);
			t = pNext;
			ret++;
		}
		else
			t = t->m_pNext;
	}

	// 제거된 텍셀의 수를 반환한다.
	return ret;
}

/*!
*	\brief 메쉬에서 사용되지 않는 법선을 제거한다.
*
*	\return 제거된 법선의 수를 반환한다.
*/
int EgMesh::DelUnusedNormal()
{
	// 법선이 없다면 리턴한다.
	int ret = 0;
	if (m_pNormals == NULL)
		return ret;

	// 모든 법선의 인덱스를 -1로 초기화 한다.
	EgNormal *n = m_pNormals;
	while (n != NULL)
	{
		n->m_Idx = -1;
		n = n->m_pNext;
	}

	// 각각의 삼각형마다
	EgFace *f = m_pFaces;
	while (f != NULL)
	{
		// 에지가 사용하는 법선의 인덱스를 1로 표기한다.
		for (int i = 0; i < 3; ++i)
			f->GetNormal(i)->m_Idx = 1;
		f = f->m_pNext;
	}

	// 사용되지 않은 법선을 제거한다.
	n = m_pNormals;
	while (n != NULL)
	{
		if (n->m_Idx == -1)
		{
			EgNormal *pNext = n->m_pNext;
			DEL_FROM_PTR_LIST(m_pNormals, n);
			n = pNext;
			ret++;
		}
		else
			n = n->m_pNext;
	}

	// 제거된 법선의 수를 반환한다.
	return ret;
}

/*!
*	\brief 메쉬에서 정점의 인덱스를 갱신한다.
*
*	\return 정점의 개수를 반환한다.
*/
int EgMesh::UpdateVertIdx()
{
	int idx = 0;
	EgVertex *v = m_pVerts;
	while (v != NULL)
	{
		v->m_Idx = idx++;
		v = v->m_pNext;
	}

	return idx;
}

/*!
*	\brief 메쉬에서 텍셀의 인덱스를 갱신한다
*
*	\return 텍셀의 개수를 반환한다.
*/
int EgMesh::UpdateTexIdx()
{
	int idx = 0;
	EgTexel *t = m_pTexels;
	while (t != NULL)
	{
		t->m_Idx = idx++;
		t = t->m_pNext;
	}
	return idx;
}

/*!
*	\brief 메쉬에서 법선의 인덱스를 갱신한다
*
*	\return 법선의 개수를 반환한다.
*/
int EgMesh::UpdateNormalIdx()
{
	int idx = 0;
	EgNormal *n = m_pNormals;
	while (n != NULL)
	{
		n->m_Idx = idx++;
		n = n->m_pNext;
	}
	return idx;
}

/*!
*	\brief 메쉬에서 삼각형의 인덱스를 갱신한다.
*
*	\return 삼각형의 개수를 반환한다.
*/
int EgMesh::UpdateFaceIdx()
{
	int idx = 0;
	EgFace *f = m_pFaces;
	while (f != NULL)
	{
		f->m_Idx = idx++;
		f = f->m_pNext;
	}
	return idx;
}

/*!
*	\brief 메쉬에서 재질의 인덱스를 갱신한다.
*
*	\return 재질의 개수를 반환한다.
*/
int EgMesh::UpdateMaterialIdx()
{
	int idx = 0;
	EgMaterial *m = m_pMaterials;
	while (m != NULL)
	{
		m->m_Idx = idx++;
		m = m->m_pNext;
	}
	return idx;
}

/*!
*	\brief 메쉬의 하프에지의 반대편 에지(Mate) 정보를 설정한다.
*	\note 메쉬 전체 구조를 갱신하므로 성능이 별로 좋지 않음 (자주 호출하지 말것)
*/
void EgMesh::InitEdgeMate()
{
	EgFace *pFace = m_pFaces;
	do
	{
		// 각각의 삼각형에 대하여
		for (int eidx = 0; eidx < 3; ++eidx)
		{
			// 각각의 에지 e1에 대하여
			EgEdge *e1 = pFace->GetEdge(eidx);

			// 다음 에지의 정점에서 시작하는 각각의 에지 e2에 대하여
			for (EgEdge *&e2 : e1->m_pNext->m_pVert->m_pEdges)
			{
				// e2가 e1의 mate 에지라면
				if (IS_MATE_EDGE(e1, e2))
				{
					// mate 정보를 설정한다.
					SET_MATE_EDGE(e1, e2);
					break;
				}
			}
		}

		pFace = pFace->m_pNext;
	} while (pFace != NULL);
}

/*!
*	\brief 정점에서 시작하는 에지의 mate 에지 정보를 갱신한다.

*	\param pVert 정점의 포인터
*/
void EgMesh::UpdateEdgeMate(EgVertex *pVert)
{
	// 정점에서 시작하는 각각의 에지 e1에 대하여
	for (EgEdge *&e1 : pVert->m_pEdges)
	{
		// 다음 에지의 정점에서 시작하는 각각의 에지 e2에 대하여
		for (EgEdge *&e2 : e1->m_pNext->m_pVert->m_pEdges)
		{
			// e1과 e2가 mate 관계라면
			if (IS_MATE_EDGE(e1, e2))
			{
				// mate 정보를 설정한다.
				SET_MATE_EDGE(e1, e2);
				break;
			}
		}
	}
}

/*!
*	\brief 메쉬의 법선을 갱신한다.
*
*	\param bSmooth 법선 형태(true: 평균 법선, false: 삼각형 법선)
*/
void EgMesh::UpdateNormal(bool bSmooth)
{
	clock_t st = clock();
	// 새로운 법선 형태를 설정하고 기존의 법선 리스트를 제거한다.
	FREE_PTR_LIST(m_pNormals, EgNormal);

	// 평균 법선을 사용한다면
	if (bSmooth)
	{
		// 각각의 정점에 대하여
		EgVertex *pVert = m_pVerts;
		do
		{
			// 새로운 법선을 생성하고 리스트에 추가한다.
			EgNormal *pNormal = new EgNormal();
			AddNormal(pNormal);

			// 정점에서 시작하는 각각의 에지에 대하여 새로운 법선 벡터를 할당한다.
			for (EgEdge *&pEdge : pVert->m_pEdges)
				pEdge->m_pNormal = pNormal;

			pVert = pVert->m_pNext;
		} while (pVert != NULL);

		// 각각의 삼각형에 대하여
		EgFace *pFace = m_pFaces;
		do
		{
			// 삼각형의 법선을 계산하여, 각각의 하프에지의 법선에 누적한다.
			GVector3 N = pFace->GetFaceNormal();
			pFace->GetEdge(0)->m_pNormal->m_Dir += N;
			pFace->GetEdge(1)->m_pNormal->m_Dir += N;
			pFace->GetEdge(2)->m_pNormal->m_Dir += N;
			pFace = pFace->m_pNext;
		} while (pFace != NULL);

		// 누적된 법선벡터를 정규화 한다.
		EgNormal *pNormal = m_pNormals;
		do
		{
			pNormal->m_Dir.Normalize();
			pNormal = pNormal->m_pNext;
		} while (pNormal != NULL);
	}
	else  // 삼각형 법선을 사용한다면
	{
		// 각각의 삼각형에 대하여
		EgFace *pFace = m_pFaces;
		do
		{
			// 법선을 생성하여 리스트에 추가한다.
			EgNormal *pNormal = new EgNormal();
			AddNormal(pNormal);

			// 삼각형의 에지에 법선을 할당한다.
			pNormal->m_Dir = pFace->GetFaceNormal();
			pFace->GetEdge(0)->m_pNormal = pNormal;
			pFace->GetEdge(1)->m_pNormal = pNormal;
			pFace->GetEdge(2)->m_pNormal = pNormal;

			pFace = pFace->m_pNext;
		} while (pFace != NULL);
	}
	printf("Elapsed time in UpdateNormal() = %u ms\n", clock() - st);
}

/*!
*	\brief 메쉬의 정점의 개수를 반환한다.
*
*	\return 메쉬의 정점의 개수를 반환한다.
*/
int EgMesh::GetNumVert()
{
	if (m_pVerts == NULL)
		return 0;

	int NumVert = 0;
	EgVertex *pVert = m_pVerts;
	do
	{
		NumVert++;
		pVert = pVert->m_pNext;
	} while (pVert != NULL);

	return NumVert;
}

/*!
*	\brief 메쉬의 텍셀의 개수를 반환한다.
*
*	\return 메쉬의 텍셀의 개수를 반환한다.
*/
int EgMesh::GetNumTexel()
{
	if (m_pTexels == NULL)
		return 0;

	int NumTexel = 0;
	EgTexel *pTexel = m_pTexels;
	do
	{
		NumTexel++;
		pTexel = pTexel->m_pNext;
	} while (pTexel != NULL);

	return NumTexel;
}

/*!
*	\brief 메쉬의 법선의 개수를 반환한다.
*
*	\return 메쉬의 법선의 개수를 반환한다.
*/
int EgMesh::GetNumNormal()
{
	if (m_pNormals == NULL)
		return 0;

	int NumNormal = 0;
	EgNormal *pNormal = m_pNormals;
	do
	{
		NumNormal++;
		pNormal = pNormal->m_pNext;
	} while (pNormal != NULL);

	return NumNormal;
}

/*!
*	\brief 메쉬의 삼각형의 개수를 반환한다.
*
*	\return 메쉬의 삼각형의 개수를 반환한다.
*/
int EgMesh::GetNumFace()
{
	if (m_pFaces == NULL)
		return 0;

	int NumFace = 0;
	EgFace *pFace = m_pFaces;
	do
	{
		NumFace++;
		pFace = pFace->m_pNext;
	} while (pFace != NULL);

	return NumFace;
}

/*!
*	\brief 메쉬의 재질의 개수를 반환한다.
*
*	\return 메쉬의 재질의 개수를 반환한다.
*/
int EgMesh::GetNumMtrl()
{
	if (m_pMaterials == NULL)
		return 0;

	int NumMtrl = 0;
	EgMaterial *pMtrl = m_pMaterials;
	while (pMtrl != NULL)
	{
		NumMtrl++;
		pMtrl = pMtrl->m_pNext;
	}

	return NumMtrl;
}

/*!
*	\brief 메쉬의 Idx번째 삼각형의 포인터를 찾는다.
*	\warning 순차 탐색을 하니 가급적 사용 자제.
*
*	\param Idx 삼각형의 인덱스(0부터 시작)
*
*	\return 메쉬의 Idx번째 삼각형의 포인터를 반환한다.
*/
EgFace *EgMesh::GetFace(int Idx)
{
	EgFace *pFace = m_pFaces;
	for (int i = 0; i < Idx; ++i)
		pFace = pFace->m_pNext;

	return pFace;
}

/*!
*	\brief 메쉬의 Idx번째 정점의 포인터를 찾는다.
*	\warning 순차 탐색을 하니 가급적 사용 자제.
*
*	\param Idx 정점의 인덱스(0부터 시작)
*
*	\return 메쉬의 Idx번째 정점의 포인터를 반환한다.
*/
EgVertex *EgMesh::GetVert(int Idx)
{
	EgVertex *pVert = m_pVerts;
	for (int i = 0; i < Idx; ++i)
		pVert = pVert->m_pNext;

	return pVert;
}

/*!
*	\brief Idx 번째 재질을 찾는다.
*
*	\param Idx 재질의 인덱스
*
*	\return Idx 번째 재질의 포인터를 반환한다.
*/
EgMaterial *EgMesh::GetMaterial(int Idx)
{
	EgMaterial *pMtl = m_pMaterials;
	do
	{
		if (pMtl->m_Idx == Idx)
			return pMtl;

		pMtl = pMtl->m_pNext;
	} while (pMtl != NULL);

	return NULL;
}

/*!
*	\brief 주어진 이름을 갖는 재질을 찾는다.
*
*	\param Name 재질의 이름
*
*	\return 주어진 이름을 갖는 재질의 포인터를 반환한다.
*/
EgMaterial *EgMesh::GetMaterial(std::string Name)
{
	EgMaterial *pMtl = m_pMaterials;
	do
	{
		if (pMtl->m_Name == Name)
			return pMtl;

		pMtl = pMtl->m_pNext;
	} while (pMtl != NULL);

	return NULL;
}

/*!
*	\brief 메쉬를 렌더링 한다.
*/
void EgMesh::Render()
{
	if (m_pTexels != NULL)
		RenderTexture();
	else
		RenderPolygon();

	//RenderWire();
	//RenderPoint();
	//RenderBndry();
}

/*!
*	\brief 메쉬를 삼각형으로 렌더링 한다.
*/
void EgMesh::RenderPolygon()
{
	//폴리곤 오프셋을 활성화
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);

	//메쉬를 구성하는 삼각형을 렌더링
	glBegin(GL_TRIANGLES);
	EgMaterial *pPrevMtl = NULL;
	EgFace *pFace = m_pFaces;
	while (pFace != NULL)
	{
		//삼각형의 재질이 바뀌었다면,
		if (pFace->m_pMaterial != pPrevMtl)
		{
			//현재 재질의 렌더링을 종료한다.
			glEnd();

			//새로운 재질의 삼각형 렌더링을 시작한다.
			pFace->m_pMaterial->Apply(false);
			pPrevMtl = pFace->m_pMaterial;
			glBegin(GL_TRIANGLES);
		}

		EgVertex *v0 = pFace->GetVertex(0);
		EgVertex *v1 = pFace->GetVertex(1);
		EgVertex *v2 = pFace->GetVertex(2);

		EgTexel *t0 = pFace->GetTexel(0);
		EgTexel *t1 = pFace->GetTexel(1);
		EgTexel *t2 = pFace->GetTexel(2);

		EgNormal *n0 = pFace->GetNormal(0);
		EgNormal *n1 = pFace->GetNormal(1);
		EgNormal *n2 = pFace->GetNormal(2);

		//Sets the current normal vector.
		//첫번째 인자 : A pointer to an array of three elements: the x, y, and z coordinates of the new current normal.
		glNormal3dv(n0->m_Dir.V);
		glVertex3dv(v0->m_Pos.V);

		glNormal3dv(n1->m_Dir.V);
		glVertex3dv(v1->m_Pos.V);

		glNormal3dv(n2->m_Dir.V);
		glVertex3dv(v2->m_Pos.V);

		pFace = pFace->m_pNext;
	}
	glEnd();

	//폴리곤 오프셋을 비활성화 한다.
	glDisable(GL_POLYGON_OFFSET_FILL);
}

/*!
*	\brief 메쉬를 텍스처로 렌더링 한다.
*/
void EgMesh::RenderTexture()
{
	//폴리곤 오프셋을 활성화 한다.
	glEnable(GL_POLYGON_OFFSET_FILL);
	//첫번째 인자 : Specifies a scale factor that is used to create a variable depth offset for each polygon. The initial value is zero.
	//두번째 인자 : Specifies a value that is multiplied by an implementation-specific value to create a constant depth offset. The initial value is 0.
	glPolygonOffset(1.0, 1.0);

	//2D 텍스처 매핑을 활성화한다.
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//삼각형을 렌더링한다.
	glBegin(GL_TRIANGLES);
	EgMaterial *pPrevMtl = NULL;
	EgFace *pFace = m_pFaces;
	while (pFace != NULL)
	{
		//삼각형 제질이 바뀌었다면,
		if (pFace->m_pMaterial != pPrevMtl)
		{
			//현재 재질의 렌더링을 종료하고,
			glEnd();

			//새로운 재질의 삼각형 렌더링을 시작한다.
			pFace->m_pMaterial->Apply(true);
			pPrevMtl = pFace->m_pMaterial;
			glBegin(GL_TRIANGLES);
		}

		EgVertex *v0 = pFace->GetVertex(0);
		EgVertex *v1 = pFace->GetVertex(1);
		EgVertex *v2 = pFace->GetVertex(2);

		EgTexel *t0 = pFace->GetTexel(0);
		EgTexel *t1 = pFace->GetTexel(1);
		EgTexel *t2 = pFace->GetTexel(2);

		EgNormal *n0 = pFace->GetNormal(0);
		EgNormal *n1 = pFace->GetNormal(1);
		EgNormal *n2 = pFace->GetNormal(2);

		//Sets the current texture coordinates.
		//첫번째 인자 : A pointer to an array of two elements, which in turn specifies the s and t texture coordinates.
		glTexCoord2dv(t0->m_Coord.V);
		glNormal3dv(n0->m_Dir.V);
		glVertex3dv(v0->m_Pos.V);

		glTexCoord2dv(t1->m_Coord.V);
		glNormal3dv(n1->m_Dir.V);
		glVertex3dv(v1->m_Pos.V);

		glTexCoord2dv(t2->m_Coord.V);
		glNormal3dv(n2->m_Dir.V);
		glVertex3dv(v2->m_Pos.V);

		pFace = pFace->m_pNext;
	}
	glEnd();

	//텍스처 매핑과 폴리곤 오프셋을 비활성화한다.
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_POLYGON_OFFSET_FILL);
}

/*!
*	\brief 메쉬를 와이어프레임으로 렌더링 한다.
*/
void EgMesh::RenderWire()
{
	// Specify the color
	glColor3f(0.0, 0.0, 1.0);
	glLineWidth(1.0);
	glDisable(GL_LIGHTING);

	EgFace *pFace = m_pFaces;
	while (pFace != NULL)
	{
		EgVertex *v0 = pFace->GetVertex(0);
		EgVertex *v1 = pFace->GetVertex(1);
		EgVertex *v2 = pFace->GetVertex(2);

		glBegin(GL_LINE_LOOP);
		{
			glVertex3dv(v0->m_Pos.V);
			glVertex3dv(v1->m_Pos.V);
			glVertex3dv(v2->m_Pos.V);
		}
		glEnd();

		pFace = pFace->m_pNext;
	}

	glEnable(GL_LIGHTING);
}

/*!
*	\brief 메쉬의 경계와 홀을 렌더링 한다.
*/
void EgMesh::RenderBndry()
{
	glDisable(GL_LIGHTING);
	// Draw the model as triangles
	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	EgFace *pFace = m_pFaces;
	while (pFace != NULL)
	{
		EgEdge *e0 = pFace->GetEdge(0);
		EgEdge *e1 = pFace->GetEdge(1);
		EgEdge *e2 = pFace->GetEdge(2);

		EgVertex *v0 = pFace->GetVertex(0);
		EgVertex *v1 = pFace->GetVertex(1);
		EgVertex *v2 = pFace->GetVertex(2);

		if (e0->m_pMate == NULL)
		{
			glVertex3dv(v0->m_Pos.V);
			glVertex3dv(v1->m_Pos.V);
		}

		if (e1->m_pMate == NULL)
		{
			glVertex3dv(v1->m_Pos.V);
			glVertex3dv(v2->m_Pos.V);
		}

		if (e2->m_pMate == NULL)
		{
			glVertex3dv(v2->m_Pos.V);
			glVertex3dv(v0->m_Pos.V);
		}

		pFace = pFace->m_pNext;
	}
	glEnd();
	glLineWidth(1.0);

	glEnable(GL_LIGHTING);
}

/*!
*	\brief 메쉬의 정점을 렌더링 한다.
*/
void EgMesh::RenderPoint()
{
	glDisable(GL_LIGHTING);
	glColor3f(0.0, 1.0, 0.0);
	glPointSize(3.0);
	glBegin(GL_POINTS);
	{
		EgVertex *pVert = m_pVerts;
		while (pVert != NULL)
		{
			glVertex3dv(pVert->m_Pos.V);
			pVert = pVert->m_pNext;
		}
	}
	glEnd();
	glEnable(GL_LIGHTING);
}

/*!
*	\brief 생성자
*
*	\param Name 장면의 이름
*/
EgScene::EgScene(std::string Name)
{
	m_Name = Name;
}

/*!
*	\brief 소멸자
*/
EgScene::~EgScene()
{
	for (EgMesh *pMesh : m_MeshList)
		delete pMesh;
}

/*!
*	\brief 장면에 메쉬를 추가한다.
*
*	\param pMesh 추가할 메쉬의 포인터
*/
void EgScene::AddMesh(EgMesh *pMesh)
{
	m_MeshList.push_back(pMesh);
}

/*!
*	\brief 장면에 포함된 바닥을 렌더링 한다.
*/
void EgScene::RenderFloor()
{
	glDisable(GL_LIGHTING);
	glColor3f(0.5, 0.5, 0.5);
	for (float x = -10.0; x <= 10.0; x += 1.0)
	{
		glBegin(GL_LINES);
		glVertex3f(x, 0.0, -10.0f);
		glVertex3f(x, 0.0, 10.0f);
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(-10.0f, 0.0, x);
		glVertex3f(10.0f, 0.0, x);
		glEnd();
	}
	glEnable(GL_LIGHTING);
}

/*!
*	\brief 장면에 포함된 메쉬를 렌더링 한다.
*/
void EgScene::RenderScene()
{
	RenderFloor();

	for (int i = 0; i < m_MeshList.size(); ++i)
		m_MeshList[i]->Render();
}
