#include "..\usr\include\egmesh.h"
#include <map>
#include <algorithm>

/**********************/
/* EgVertex Ŭ���� ���� */
/**********************/
/*!
*	\brief ������
*
*	\param x ������ x��ǥ
*	\param y ������ y��ǥ
*	\param z ������ z��ǥ
*/
EgVertex::EgVertex(double x, double y, double z)
{
	m_Idx = -1;
	m_Pos = GPoint3(x, y, z);
	m_pPrev = m_pNext = NULL;
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ������ ��ü
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
*	\brief �Ҹ���
*/
EgVertex::~EgVertex()
{
}

/*!
*	\brief ������ �����ϰ� �ִ� �ﰢ���� ������ ��ȯ�Ѵ�.
*	\note ���� ������ ��� �뷱���� ����, ��� ������ ���, �뷱�� ���� ����.
*
*	\return ������ �����ϰ� �ִ� �ﰢ���� ������ ��ȯ�Ѵ�.
*/
int EgVertex::GetNumIncidentFace()
{
	return (int)m_pEdges.size();
}

/*!
*	\brief ������ 1�� ������ ���Ѵ�.
*	\note ����/�ܺ� ���о��� ������ ����� ������ ��� ���Ѵ�.
*
*	\param Verts 1���� �����ϴ� ������ �����Ͱ� ����ȴ�.
*
*	\return 1���� �����ϴ� ������ ���� ��ȯ�Ѵ�.
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
*	\brief ������ �����ϴ� 1�� �ﰢ�� ����Ʈ�� ���Ѵ�.
*
*	\param Faces ������ �����ϴ� �ﰢ�� �����Ͱ� ����ȴ�.
*
*	\return ������ �����ϴ� 1�� �ﰢ���� ���� ��ȯ�Ѵ�.
*/
int EgVertex::GetOneRingFace(std::vector<EgFace *> &Faces)
{
	if (m_pEdges.empty())
		return 0;

	// �������� �����ϴ� ������ ������ ���Ե� �ﰢ���� ���Ѵ�.
	for (EgEdge *&e : m_pEdges)
		Faces.push_back(e->m_pFace);

	// �ﰢ���� ���� ��ȯ�Ѵ�.
	return (int)Faces.size();
}

/*********************/
/* EgTexel Ŭ���� ���� */
/*********************/
/*!
*	\brief ������
*
*	\param u ���� ���� �ؽ�ó ��ǥ
*	\param v ���� ���� �ؽ�ó ��ǥ
*/
EgTexel::EgTexel(double u, double v)
{
	m_Idx = -1;
	m_Coord[0] = u;
	m_Coord[1] = v;
	m_pPrev = m_pNext = NULL;
}

/*!
*	\brief ���� ������
*
*	\param cpy ������ �ؼ� ��ü
*/
EgTexel::EgTexel(const EgTexel &cpy)
{
	m_Idx = cpy.m_Idx;
	m_Coord = cpy.m_Coord;
	m_pPrev = cpy.m_pPrev;
	m_pNext = cpy.m_pNext;
}

/*!
*	\brief �Ҹ���
*/
EgTexel::~EgTexel()
{
}

/**********************/
/* EgNormal Ŭ���� ���� */
/**********************/
/*!
*	\brief ������
*
*	\param nx ������ x ��ǥ
*	\param ny ������ y ��ǥ
*	\param nz ������ z ��ǥ
*/
EgNormal::EgNormal(double nx, double ny, double nz)
{
	m_Dir = GVector3(nx, ny, nz);
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ��ü
*/
EgNormal::EgNormal(const EgNormal &cpy)
{
	m_Dir = cpy.m_Dir;
}

/*!
*	\brief �Ҹ���
*/
EgNormal::~EgNormal()
{
}

/************************/
/* EgEdge Ŭ���� ���� */
/************************/
/*!
*	\brief ������
*
*	\param pVert ������ ���� ������ ���� ������
*	\param pTexel ������ ���� ������ �ؼ��� ���� ������
*	\param pNormal ������ ���� ������ ������ ���� ������
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
*	\brief ���� ������
*
*	\param cpy ����� �������� ��ü
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
*	\brief �Ҹ���
*/
EgEdge::~EgEdge()
{
}

/*!
*	\brief ������ ���� �������� �˻��Ѵ�.
*
*	\return ������ ���Ű����� ��� true, �ƴϸ� false�� ��ȯ�Ѵ�.
*/
bool EgEdge::IsCollapsible()
{
	// ��� ���� �������� 
	//if (m_pMate == NULL || m_pNext->m_pMate == NULL || m_pNext->m_pNext->m_pMate == NULL)
	//	return false;
	//
	//EgEdge *e = m_pMate;
	//if (e->m_pNext->m_pMate == NULL || e->m_pNext->m_pNext->m_pMate == NULL)
	//	return false;

	// ������ ������(v1)�� ����(v2)�� ���Ѵ�.
	EgVertex *v1 = m_pVert;
	EgVertex *v2 = m_pNext->m_pVert;

	// v1�� v2�� 1�� ������ ���� A�� B�� ���Ѵ�.
	std::vector<EgVertex *> A, B;
	v1->GetOneRingVert(A);
	v2->GetOneRingVert(B);

	// A�� B�� �������� ���Ѵ�.
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

	// ���� ������ ������ ������ ��� true�� ��ȯ�Ѵ�.
	if (m_pMate == NULL && C.size() == 1)
		return true;
	if (m_pMate != NULL && C.size() == 2)
		return true;

	// ���� ���� ������ �������� ���ϴ� ��� false�� ��ȯ�Ѵ�.
	return false;
}

/********************/
/* EgFace Ŭ���� ���� */
/********************/
/*!
*	\brief ������
*
*	\param pEdge0 �ﰢ���� ù ��° ����
*	\param pEdge1 �ﰢ���� �� ��° ����
*	\param pEdge2 �ﰢ���� �� ��° ����
*	\param pMtl �ﰢ���� ����ϴ� ������ ���� ������
*/
EgFace::EgFace(EgEdge *pEdge0, EgEdge *pEdge1, EgEdge *pEdge2, EgMaterial *pMtl)
{
	m_Idx = -1;
	// ������ ������踦 �����Ѵ�.
	m_pEdge = pEdge0;
	m_pEdge->m_pNext = pEdge1;
	pEdge1->m_pNext = pEdge2;
	pEdge2->m_pNext = m_pEdge;
	pEdge0->m_pFace = pEdge1->m_pFace = pEdge2->m_pFace = this;
	m_pMaterial = pMtl;
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� �ﰢ�� ��ü
*/
EgFace::EgFace(const EgFace &cpy)
{
	m_Idx = -1;
	m_pEdge = cpy.m_pEdge;
	m_pMaterial = cpy.m_pMaterial;
}

/*!
*	\brief �Ҹ���
*/
EgFace::~EgFace()
{
	delete m_pEdge->m_pNext->m_pNext;
	delete m_pEdge->m_pNext;
	delete m_pEdge;
	m_pEdge = NULL;
}

/*!
*	\brief �ش� �ε����� ������ �����͸� ��ȯ�Ѵ�.
*
*	\param vIdx ã�� ������ �ε���
*
*	\return �ش� �ε����� ������ �����͸� ��ȯ�Ѵ�.
*/
EgVertex *EgFace::GetVertex(int vIdx)
{
	return GetEdge(vIdx)->m_pVert;
}

/*!
*	\brief �ش� �ε����� �ؼ��� �����͸� ��ȯ�Ѵ�.
*
*	\param vIdx ã�� �ؼ��� �ε���
*
*	\return �ش� �ε����� �ؼ��� �����͸� ��ȯ�Ѵ�.
*/
EgTexel *EgFace::GetTexel(int tIdx)
{
	return GetEdge(tIdx)->m_pTexel;
}

/*!
*	\brief �ش� �ε����� ������ ������ ��ȯ�Ѵ�.
*
*	\param nIdx ã�� ������ �ε���
*
*	\return �ش� �ε����� ������ ������ ��ȯ�Ѵ�.
*/
EgNormal *EgFace::GetNormal(int nIdx)
{
	return GetEdge(nIdx)->m_pNormal;
}

/*!
*	\brief �ش� �ε����� �������� �����͸� ��ȯ�Ѵ�.
*
*	\param eIdx ã�� �ε���(0 ����)
*
*	\return �ش� �ε����� �������� �����͸� ��ȯ�Ѵ�.
*/
EgEdge *EgFace::GetEdge(int eIdx)
{
	EgEdge *pRet = m_pEdge;
	for (int i = 0; i < eIdx; ++i)
		pRet = pRet->m_pNext;

	return pRet;
}

/*!
*	\brief �ش� ������ �ε����� ��ȯ�Ѵ�.
*
*	\param pEdge ������ ���� ������
*
*	\return �ﰢ������ ������ �ε����� ��ȯ�Ѵ�.
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
*	\brief �ﰢ���� ���� �������͸� ����Ѵ�.
*
*	\return �ﰢ���� ���� �������͸� ��ȯ�Ѵ�.
*/
GVector3 EgFace::GetFaceNormal()
{
	// �ﰢ���� ������ ����ϰ�,
	EgVertex *v0 = m_pEdge->m_pVert;
	EgVertex *v1 = m_pEdge->m_pNext->m_pVert;
	EgVertex *v2 = m_pEdge->m_pNext->m_pNext->m_pVert;
	GVector3 Ret = ((v1->m_Pos - v0->m_Pos) ^ (v2->m_Pos - v0->m_Pos)).Normalize();
	return Ret;
}

/*************************/
/* EgMaterial Ŭ���� ���� */
/*************************/
/*!
*	\brief ������
*
*	\param Name ������ ������ �̸�
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
*	\brief ���� ������
*
*	\param cpy ������ ���� ��ü
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
*	\brief �Ҹ���
*/
EgMaterial::~EgMaterial()
{
	if (m_TextureId != 0)
		glDeleteTextures(1, &m_TextureId);
}

/*!
*	\brief ������ �ֺ��� �ݻ����� �����Ѵ�.
*
*	\param r Red ���п� ���� �ݻ���
*	\param g Green ���п� ���� �ݻ���
*	\param b Blue ���п� ���� �ݻ���
*	\param a Alpha ��
*/
void EgMaterial::SetAmbient(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_Ambient[0] = r;
	m_Ambient[1] = g;
	m_Ambient[2] = b;
	m_Ambient[3] = a;
}

/*!
*	\brief ������ ���ݻ� ����� �����Ѵ�.
*
*	\param r Red ���п� ���� �ݻ���
*	\param g Green ���п� ���� �ݻ���
*	\param b Blue ���п� ���� �ݻ���
*	\param a Alpha ��
*/
void EgMaterial::SetDiffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_Diffuse[0] = r;
	m_Diffuse[1] = g;
	m_Diffuse[2] = b;
	m_Diffuse[3] = a;
}

/*!
*	\brief ������ ���ݻ� ����� �����Ѵ�.
*
*	\param r Red ���п� ���� �ݻ���
*	\param g Green ���п� ���� �ݻ���
*	\param b Blue ���п� ���� �ݻ���
*	\param a Alpha ��
*/
void EgMaterial::SetSpecular(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_Diffuse[0] = r;
	m_Diffuse[1] = g;
	m_Diffuse[2] = b;
	m_Diffuse[3] = a;
}

/*!
*	\brief ������ ���ݻ� ������ �����Ѵ�.
*
*	\param s ������ ���ݻ� ����
*/
void EgMaterial::SetShiness(GLfloat s)
{
	m_Shiness = s;
}

/*!
*	\brief ������ �ؽ�ó�� �����Ѵ�.
*	\note MFC�� ����� ���, OpenGL ������ ���¿��� ȣ��Ǿ�� �Ѵ�.
*
*	\param fname �ؽ�ó ���ϸ�
*/
void EgMaterial::LoadTexture(const char *fname)
{
	// �ؽ�ó ������ Ȯ���ڸ� ���Ͽ�
	std::string file_ext = get_file_ext(fname);

	// Ȯ���ڰ� *.tga ������ ���,
	if (file_ext == "tga")
	{
		// �ؽ�ó ���ϸ��� �����ϰ�,
		m_TexFileName = std::string(fname);

		// �̹��� �����͸� �ε��Ͽ�,
		int w, h;
		GLenum format;
		unsigned char *imageData = open_image_tga(fname, &w, &h, &format);

		// �ؽ�ó�� �����Ѵ�.
		if (imageData != NULL)
		{
			//�ؽ��� ������ ���� m_TextureId�� ����
			glGenTextures(1, &m_TextureId);	// 1���� ���۵�...
											//m_TextureId�� �ؽ�ó Ÿ���� ���ε���Ų�� 
			glBindTexture(GL_TEXTURE_2D, m_TextureId);
			//�ȼ� ���� ��带 �����Ѵ�.
			//ù��° ���� : �ȼ� �����Ͱ� Ŭ���̾�Ʈ �޸𸮷� ��ȯ�Ǵ� ���
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			//�ؽ�ó �Ķ���͸� ����
			//ù��° ���� : Ÿ�� �ؽ���
			//�ι�° ���� : ���� �� �ؽ�ó �Ķ����
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// or, GL_NEAREST
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// or, GL_NEAREST
																				//�ؽ��� ȯ�� �Ķ���͸� ����
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// or, GL_REPLACE
																				//2���� �ؽ��� �̹����� ����
			glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, imageData);
			//2D �Ӹ��� �����
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, imageData);

			free(imageData);
		}
	}

	// Ȯ���ڰ� *.bmp ������ ���,
	if (file_ext == "bmp")
	{
		// �ؽ�ó ���ϸ��� �����ϰ�,
		m_TexFileName = std::string(fname);

		// �̹��� �����͸� �ε��Ͽ�,
		BITMAPINFO	*info;
		unsigned char *imageData = open_image_bmp(fname, &info);

		// �ؽ�ó�� �����Ѵ�.
		if (imageData != NULL)
		{
			// BGR�� RGB�� ��ȯ�Ѵ�.
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
*	\brief ������ �����Ѵ�.
*
*	\param bTexture �ؽ�ó ���� ����
*/
void EgMaterial::Apply(bool bTexture)
{
	// ������ Ȱ��ȭ�ϰ�,
	if (m_Diffuse[3] != 1.0f)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
		glDisable(GL_BLEND);

	// ������ �����Ѵ�.
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_Ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_Diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_Specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_Shiness);

	// ������ �ؽ�ó�� ����� ��� �ؽ�ó ���ε��� Ȱ��ȭ �Ѵ�.
	if (bTexture && m_TextureId != 0)
		glBindTexture(GL_TEXTURE_2D, m_TextureId);
}

/********************/
/* EgMesh Ŭ���� ���� */
/********************/
/*!
*	\brief ������
*
*	\param Name ������ �޽��� �̸�
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
*	\brief �Ҹ���
*/
EgMesh::~EgMesh()
{
	// ����, �ؼ�, �븻, �ﰢ��, ������ ����Ʈ�� �����Ѵ�.
	FREE_PTR_LIST(m_pVerts, EgVertex);
	FREE_PTR_LIST(m_pTexels, EgTexel);
	FREE_PTR_LIST(m_pNormals, EgNormal);
	FREE_PTR_LIST(m_pFaces, EgFace);
	FREE_PTR_LIST(m_pMaterials, EgMaterial);
	printf("Mesh is deleted...\n");
}

/*!
*	\brief �޽��� ������ �߰��Ѵ�.
*
*	\param pVert �߰��� ������ ���� ������
*/
void EgMesh::AddVertex(EgVertex *pVert)
{
	ADD_TO_PTR_LIST(m_pVerts, pVert);
}

/*!
*	\brief �޽��� �ؼ��� �߰��Ѵ�.
*
*	\param pTexel �߰��� �ؼ��� ���� ������
*/
void EgMesh::AddTexel(EgTexel *pTexel)
{
	ADD_TO_PTR_LIST(m_pTexels, pTexel);
}

/*!
*	\brief �޽��� ������ �߰��Ѵ�.
*
*	\param pNormal �߰��� ������ ���� ������
*/
void EgMesh::AddNormal(EgNormal *pNormal)
{
	ADD_TO_PTR_LIST(m_pNormals, pNormal);
}

/*!
*	\brief �޽��� �ﰢ���� �߰��Ѵ�.
*
*	\param pFace �߰��� �ﰢ���� ���� ������
*/
void EgMesh::AddFace(EgFace *pFace)
{
	ADD_TO_PTR_LIST(m_pFaces, pFace);
}

/*!
*	\brief �޽��� ������ �߰��Ѵ�.
*
*	\param pMaterial �߰��� ������ ���� ������
*/
void EgMesh::AddMaterial(EgMaterial *pMaterial)
{
	ADD_TO_PTR_LIST(m_pMaterials, pMaterial);
}

/*!
*	\brief �޽����� ������ �ﰢ��(�� 3���� ��������)�� �����Ѵ�.
*	\note �ﰢ�� ���Ű������� ���� ������ �ʴ� ����, �ؼ�, ������ ���ŵ��� �ʴ´�.
*		  ���� ����, �ؼ�, ������ �����ϱ� ���ؼ��� RemoveUnused*() �Լ��� ������ ȣ���Ѵ�.
*
*	\param Faces ������ �ﰢ���� ������ ����Ʈ
*
*	\return �޽��� �ﰢ���� ���Ҵٸ� true, ���ٸ� false�� ��ȯ�Ѵ�.
*/
bool EgMesh::DelFaces(std::vector<EgFace *> &Faces)
{
	// ������ �ﰢ���� �����Ѵ�.
	for (EgFace *&f : Faces)
	{
		// ������ ������ ���Ͽ� Mate ������ ������ �����Ѵ�.
		for (int i = 0; i < 3; ++i)
		{
			EgEdge *e = f->GetEdge(i);
			if (e->m_pMate != NULL)
				e->m_pMate->m_pMate = NULL;

			// �������� �����ϴ� ���� ����Ʈ���� ���� ������ �����Ѵ�.
			EgVertex *v = e->m_pVert;
			DEL_FROM_VEC(v->m_pEdges, e);
		}

		// �ﰢ���� �����Ѵ�.
		DEL_FROM_PTR_LIST(m_pFaces, f);
	}

	// �����ִ� �ﰢ���� ���ٸ� false�� ��ȯ�Ѵ�.
	if (m_pFaces == NULL)
		return false;

	return true;
}

/*!
*	\brief �޽����� ������ ������ �����Ѵ�.
*	\note ���Ű������� ������ ����� �ﰢ������ ���ŵȴ�.
*
*	\param Verts ������ ������ ������ ����Ʈ
*
*	\return �޽��� ������ ���Ҵٸ� true, ���ٸ� false�� ��ȯ�Ѵ�.
*/
bool EgMesh::DelVerts(std::vector<EgVertex*> &Verts)
{
	std::vector<EgFace *> Faces;

	// ������ ������ 1�� �ﰢ���� ������ ���Ѵ�.
	for (EgVertex *&v : Verts)
		for (EgEdge *&e : v->m_pEdges)
			Faces.push_back(e->m_pFace);

	// �ߺ��� ��ü�� �����Ѵ�.
	sort(Faces.begin(), Faces.end());
	Faces.erase(unique(Faces.begin(), Faces.end()), Faces.end());

	// ������ �ﰢ���� �����Ѵ�.
	DelFaces(Faces);

	// ������ ������ �����Ѵ�.
	for (EgVertex *&v : Verts)
		DEL_FROM_PTR_LIST(m_pVerts, v);

	// �����ִ� ������ ���ٸ� false�� ��ȯ�Ѵ�.
	if (m_pVerts == NULL)
		return false;

	return true;
}

/*!
*	\brief �޽����� ����(�ﰢ��)�� �����Ѵ�.
*
*	\param Edges ������ ������ ������ ����Ʈ
*
*	\return ���� ������ ������ true, ������ false�� ��ȯ�Ѵ�.
*/
bool EgMesh::DelEdges(std::vector<EgEdge *> &Edges)
{
	// ������ ������ ���Ե� �ﰢ�� ������ ���Ѵ�.
	std::vector<EgFace *> Faces;
	for (EgEdge *&e : Edges)
		Faces.push_back(e->m_pFace);

	// �ߺ��� ��ü�� �����Ѵ�.
	sort(Faces.begin(), Faces.end());
	Faces.erase(unique(Faces.begin(), Faces.end()), Faces.end());

	// ������ �ﰢ���� �����Ѵ�.
	DelFaces(Faces);

	// �����ִ� �ﰢ���� ���ٸ� false�� ��ȯ�Ѵ�.
	if (m_pFaces == NULL)
		return false;

	return true;
}

/*!
*	\brief �޽����� ������ �ʴ� ������ �����Ѵ�.
*
*	\return ���ŵ� ������ ���� ��ȯ�Ѵ�.
*/
int EgMesh::DelUnusedVert()
{
	// ������ ���ٸ� �����Ѵ�.
	int ret = 0;
	if (m_pVerts == NULL)
		return ret;

	// ��� ������ �ε����� -1�� �ʱ�ȭ �Ѵ�.
	EgVertex *v = m_pVerts;
	while (v != NULL)
	{
		v->m_Idx = -1;
		v = v->m_pNext;
	}

	// ������ �ﰢ������
	EgFace *f = m_pFaces;
	while (f != NULL)
	{
		// ������ ����ϴ� ������ �ε����� 1�� ǥ���Ѵ�.
		for (int i = 0; i < 3; ++i)
			f->GetVertex(i)->m_Idx = 1;
		f = f->m_pNext;
	}

	// ������ ���� ������ �����Ѵ�.
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

	// ���ŵ� ������ ���� ��ȯ�Ѵ�.
	return ret;
}

/*!
*	\brief �޽����� ������ �ʴ� �ؼ��� �����Ѵ�.
*
*	\return ���ŵ� �ؼ��� ���� ��ȯ�Ѵ�.
*/
int EgMesh::DelUnusedTexel()
{
	// �ؼ��� ���ٸ� �����Ѵ�.
	int ret = 0;
	if (m_pTexels == NULL)
		return ret;

	// ��� �ؼ��� �ε����� -1�� �ʱ�ȭ�Ѵ�.
	EgTexel *t = m_pTexels;
	while (t != NULL)
	{
		t->m_Idx = -1;
		t = t->m_pNext;
	}

	// ������ �ﰢ������
	EgFace *f = m_pFaces;
	while (f != NULL)
	{
		// ������ ����ϴ� �ؼ��� �ε����� 1�� ǥ���Ѵ�.
		for (int i = 0; i < 3; ++i)
			f->GetTexel(i)->m_Idx = 1;
		f = f->m_pNext;
	}

	// ������ ���� �ؼ��� �����Ѵ�.
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

	// ���ŵ� �ؼ��� ���� ��ȯ�Ѵ�.
	return ret;
}

/*!
*	\brief �޽����� ������ �ʴ� ������ �����Ѵ�.
*
*	\return ���ŵ� ������ ���� ��ȯ�Ѵ�.
*/
int EgMesh::DelUnusedNormal()
{
	// ������ ���ٸ� �����Ѵ�.
	int ret = 0;
	if (m_pNormals == NULL)
		return ret;

	// ��� ������ �ε����� -1�� �ʱ�ȭ �Ѵ�.
	EgNormal *n = m_pNormals;
	while (n != NULL)
	{
		n->m_Idx = -1;
		n = n->m_pNext;
	}

	// ������ �ﰢ������
	EgFace *f = m_pFaces;
	while (f != NULL)
	{
		// ������ ����ϴ� ������ �ε����� 1�� ǥ���Ѵ�.
		for (int i = 0; i < 3; ++i)
			f->GetNormal(i)->m_Idx = 1;
		f = f->m_pNext;
	}

	// ������ ���� ������ �����Ѵ�.
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

	// ���ŵ� ������ ���� ��ȯ�Ѵ�.
	return ret;
}

/*!
*	\brief �޽����� ������ �ε����� �����Ѵ�.
*
*	\return ������ ������ ��ȯ�Ѵ�.
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
*	\brief �޽����� �ؼ��� �ε����� �����Ѵ�
*
*	\return �ؼ��� ������ ��ȯ�Ѵ�.
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
*	\brief �޽����� ������ �ε����� �����Ѵ�
*
*	\return ������ ������ ��ȯ�Ѵ�.
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
*	\brief �޽����� �ﰢ���� �ε����� �����Ѵ�.
*
*	\return �ﰢ���� ������ ��ȯ�Ѵ�.
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
*	\brief �޽����� ������ �ε����� �����Ѵ�.
*
*	\return ������ ������ ��ȯ�Ѵ�.
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
*	\brief �޽��� ���������� �ݴ��� ����(Mate) ������ �����Ѵ�.
*	\note �޽� ��ü ������ �����ϹǷ� ������ ���� ���� ���� (���� ȣ������ ����)
*/
void EgMesh::InitEdgeMate()
{
	EgFace *pFace = m_pFaces;
	do
	{
		// ������ �ﰢ���� ���Ͽ�
		for (int eidx = 0; eidx < 3; ++eidx)
		{
			// ������ ���� e1�� ���Ͽ�
			EgEdge *e1 = pFace->GetEdge(eidx);

			// ���� ������ �������� �����ϴ� ������ ���� e2�� ���Ͽ�
			for (EgEdge *&e2 : e1->m_pNext->m_pVert->m_pEdges)
			{
				// e2�� e1�� mate �������
				if (IS_MATE_EDGE(e1, e2))
				{
					// mate ������ �����Ѵ�.
					SET_MATE_EDGE(e1, e2);
					break;
				}
			}
		}

		pFace = pFace->m_pNext;
	} while (pFace != NULL);
}

/*!
*	\brief �������� �����ϴ� ������ mate ���� ������ �����Ѵ�.

*	\param pVert ������ ������
*/
void EgMesh::UpdateEdgeMate(EgVertex *pVert)
{
	// �������� �����ϴ� ������ ���� e1�� ���Ͽ�
	for (EgEdge *&e1 : pVert->m_pEdges)
	{
		// ���� ������ �������� �����ϴ� ������ ���� e2�� ���Ͽ�
		for (EgEdge *&e2 : e1->m_pNext->m_pVert->m_pEdges)
		{
			// e1�� e2�� mate ������
			if (IS_MATE_EDGE(e1, e2))
			{
				// mate ������ �����Ѵ�.
				SET_MATE_EDGE(e1, e2);
				break;
			}
		}
	}
}

/*!
*	\brief �޽��� ������ �����Ѵ�.
*
*	\param bSmooth ���� ����(true: ��� ����, false: �ﰢ�� ����)
*/
void EgMesh::UpdateNormal(bool bSmooth)
{
	clock_t st = clock();
	// ���ο� ���� ���¸� �����ϰ� ������ ���� ����Ʈ�� �����Ѵ�.
	FREE_PTR_LIST(m_pNormals, EgNormal);

	// ��� ������ ����Ѵٸ�
	if (bSmooth)
	{
		// ������ ������ ���Ͽ�
		EgVertex *pVert = m_pVerts;
		do
		{
			// ���ο� ������ �����ϰ� ����Ʈ�� �߰��Ѵ�.
			EgNormal *pNormal = new EgNormal();
			AddNormal(pNormal);

			// �������� �����ϴ� ������ ������ ���Ͽ� ���ο� ���� ���͸� �Ҵ��Ѵ�.
			for (EgEdge *&pEdge : pVert->m_pEdges)
				pEdge->m_pNormal = pNormal;

			pVert = pVert->m_pNext;
		} while (pVert != NULL);

		// ������ �ﰢ���� ���Ͽ�
		EgFace *pFace = m_pFaces;
		do
		{
			// �ﰢ���� ������ ����Ͽ�, ������ ���������� ������ �����Ѵ�.
			GVector3 N = pFace->GetFaceNormal();
			pFace->GetEdge(0)->m_pNormal->m_Dir += N;
			pFace->GetEdge(1)->m_pNormal->m_Dir += N;
			pFace->GetEdge(2)->m_pNormal->m_Dir += N;
			pFace = pFace->m_pNext;
		} while (pFace != NULL);

		// ������ �������͸� ����ȭ �Ѵ�.
		EgNormal *pNormal = m_pNormals;
		do
		{
			pNormal->m_Dir.Normalize();
			pNormal = pNormal->m_pNext;
		} while (pNormal != NULL);
	}
	else  // �ﰢ�� ������ ����Ѵٸ�
	{
		// ������ �ﰢ���� ���Ͽ�
		EgFace *pFace = m_pFaces;
		do
		{
			// ������ �����Ͽ� ����Ʈ�� �߰��Ѵ�.
			EgNormal *pNormal = new EgNormal();
			AddNormal(pNormal);

			// �ﰢ���� ������ ������ �Ҵ��Ѵ�.
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
*	\brief �޽��� ������ ������ ��ȯ�Ѵ�.
*
*	\return �޽��� ������ ������ ��ȯ�Ѵ�.
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
*	\brief �޽��� �ؼ��� ������ ��ȯ�Ѵ�.
*
*	\return �޽��� �ؼ��� ������ ��ȯ�Ѵ�.
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
*	\brief �޽��� ������ ������ ��ȯ�Ѵ�.
*
*	\return �޽��� ������ ������ ��ȯ�Ѵ�.
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
*	\brief �޽��� �ﰢ���� ������ ��ȯ�Ѵ�.
*
*	\return �޽��� �ﰢ���� ������ ��ȯ�Ѵ�.
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
*	\brief �޽��� ������ ������ ��ȯ�Ѵ�.
*
*	\return �޽��� ������ ������ ��ȯ�Ѵ�.
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
*	\brief �޽��� Idx��° �ﰢ���� �����͸� ã�´�.
*	\warning ���� Ž���� �ϴ� ������ ��� ����.
*
*	\param Idx �ﰢ���� �ε���(0���� ����)
*
*	\return �޽��� Idx��° �ﰢ���� �����͸� ��ȯ�Ѵ�.
*/
EgFace *EgMesh::GetFace(int Idx)
{
	EgFace *pFace = m_pFaces;
	for (int i = 0; i < Idx; ++i)
		pFace = pFace->m_pNext;

	return pFace;
}

/*!
*	\brief �޽��� Idx��° ������ �����͸� ã�´�.
*	\warning ���� Ž���� �ϴ� ������ ��� ����.
*
*	\param Idx ������ �ε���(0���� ����)
*
*	\return �޽��� Idx��° ������ �����͸� ��ȯ�Ѵ�.
*/
EgVertex *EgMesh::GetVert(int Idx)
{
	EgVertex *pVert = m_pVerts;
	for (int i = 0; i < Idx; ++i)
		pVert = pVert->m_pNext;

	return pVert;
}

/*!
*	\brief Idx ��° ������ ã�´�.
*
*	\param Idx ������ �ε���
*
*	\return Idx ��° ������ �����͸� ��ȯ�Ѵ�.
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
*	\brief �־��� �̸��� ���� ������ ã�´�.
*
*	\param Name ������ �̸�
*
*	\return �־��� �̸��� ���� ������ �����͸� ��ȯ�Ѵ�.
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
*	\brief �޽��� ������ �Ѵ�.
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
*	\brief �޽��� �ﰢ������ ������ �Ѵ�.
*/
void EgMesh::RenderPolygon()
{
	//������ �������� Ȱ��ȭ
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);

	//�޽��� �����ϴ� �ﰢ���� ������
	glBegin(GL_TRIANGLES);
	EgMaterial *pPrevMtl = NULL;
	EgFace *pFace = m_pFaces;
	while (pFace != NULL)
	{
		//�ﰢ���� ������ �ٲ���ٸ�,
		if (pFace->m_pMaterial != pPrevMtl)
		{
			//���� ������ �������� �����Ѵ�.
			glEnd();

			//���ο� ������ �ﰢ�� �������� �����Ѵ�.
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
		//ù��° ���� : A pointer to an array of three elements: the x, y, and z coordinates of the new current normal.
		glNormal3dv(n0->m_Dir.V);
		glVertex3dv(v0->m_Pos.V);

		glNormal3dv(n1->m_Dir.V);
		glVertex3dv(v1->m_Pos.V);

		glNormal3dv(n2->m_Dir.V);
		glVertex3dv(v2->m_Pos.V);

		pFace = pFace->m_pNext;
	}
	glEnd();

	//������ �������� ��Ȱ��ȭ �Ѵ�.
	glDisable(GL_POLYGON_OFFSET_FILL);
}

/*!
*	\brief �޽��� �ؽ�ó�� ������ �Ѵ�.
*/
void EgMesh::RenderTexture()
{
	//������ �������� Ȱ��ȭ �Ѵ�.
	glEnable(GL_POLYGON_OFFSET_FILL);
	//ù��° ���� : Specifies a scale factor that is used to create a variable depth offset for each polygon. The initial value is zero.
	//�ι�° ���� : Specifies a value that is multiplied by an implementation-specific value to create a constant depth offset. The initial value is 0.
	glPolygonOffset(1.0, 1.0);

	//2D �ؽ�ó ������ Ȱ��ȭ�Ѵ�.
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//�ﰢ���� �������Ѵ�.
	glBegin(GL_TRIANGLES);
	EgMaterial *pPrevMtl = NULL;
	EgFace *pFace = m_pFaces;
	while (pFace != NULL)
	{
		//�ﰢ�� ������ �ٲ���ٸ�,
		if (pFace->m_pMaterial != pPrevMtl)
		{
			//���� ������ �������� �����ϰ�,
			glEnd();

			//���ο� ������ �ﰢ�� �������� �����Ѵ�.
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
		//ù��° ���� : A pointer to an array of two elements, which in turn specifies the s and t texture coordinates.
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

	//�ؽ�ó ���ΰ� ������ �������� ��Ȱ��ȭ�Ѵ�.
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_POLYGON_OFFSET_FILL);
}

/*!
*	\brief �޽��� ���̾����������� ������ �Ѵ�.
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
*	\brief �޽��� ���� Ȧ�� ������ �Ѵ�.
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
*	\brief �޽��� ������ ������ �Ѵ�.
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
*	\brief ������
*
*	\param Name ����� �̸�
*/
EgScene::EgScene(std::string Name)
{
	m_Name = Name;
}

/*!
*	\brief �Ҹ���
*/
EgScene::~EgScene()
{
	for (EgMesh *pMesh : m_MeshList)
		delete pMesh;
}

/*!
*	\brief ��鿡 �޽��� �߰��Ѵ�.
*
*	\param pMesh �߰��� �޽��� ������
*/
void EgScene::AddMesh(EgMesh *pMesh)
{
	m_MeshList.push_back(pMesh);
}

/*!
*	\brief ��鿡 ���Ե� �ٴ��� ������ �Ѵ�.
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
*	\brief ��鿡 ���Ե� �޽��� ������ �Ѵ�.
*/
void EgScene::RenderScene()
{
	RenderFloor();

	for (int i = 0; i < m_MeshList.size(); ++i)
		m_MeshList[i]->Render();
}
