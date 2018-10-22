#include "EgDecimator.h"


/*!
*	\brief ������
*
*	\param pEdge ������ ������ ������
*	\param Pos ������ �� ������ ���յǴ� ��ġ
*/
EgCollapseInfo::EgCollapseInfo(EgEdge *pEdge, GPoint3 Pos)
{
	m_pEdge = pEdge;
	m_Pos = Pos;
	m_Cost = pEdge->m_Cost;
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ��ü
*/
EgCollapseInfo::EgCollapseInfo(const EgCollapseInfo &cpy)
{
	m_pEdge = cpy.m_pEdge;
	m_Pos = cpy.m_Pos;
	m_Cost = cpy.m_Cost;
}

/*!
*	\brief �Ҹ���
*/
EgCollapseInfo::~EgCollapseInfo()
{
}

/*!
*	\brief ���� ������
*
*	\param rhs ���Ե� ��ü
*/
EgCollapseInfo &EgCollapseInfo::operator =(const EgCollapseInfo &rhs)
{
	m_pEdge = rhs.m_pEdge;
	m_Pos = rhs.m_Pos;
	m_Cost = rhs.m_Cost;
	return *this;
}

/*!
*	\brief ��(>) ������
*
*	\param lhs ���� ��ü
*	\param rhs ������ ��ü
*/
bool operator >(const EgCollapseInfo &lhs, const EgCollapseInfo &rhs)
{
	return (lhs.m_Cost > rhs.m_Cost);
}

/*!
*	\brief ������
*
*	\param pMesh �ܼ�ȭ�� ��� �޽��� ���� ������
*/
EgDecimator::EgDecimator(EgMesh *pMesh)
{
	// ��� �޽��� �����Ѵ�.
	m_pMesh = pMesh;
}

/*!
*	\brief �Ҹ���
*/
EgDecimator::~EgDecimator()
{
}

/*!
*	\brief ��� ax + by + cz + d = 0���� �� p���� �Ÿ��� ������ ����ϴ� ����� ���Ѵ�.
*
*	\param K ���� ����� ����ȴ�.
*	\param a ����� �������� ���
*	\param b ����� �������� ���
*	\param c ����� �������� ���
*	\param d ����� �������� ���
*/
void EgDecimator::GetErrorMatrix(GMatrix &K, double a, double b, double c, double d)
{
	K[0][0] = a * a; 
	K[0][1] = a * b; 
	K[0][2] = a * c; 
	K[0][3] = a * d;

	K[1][0] = K[0][1];	// ��Ī��
	K[1][1] = b * b;
	K[1][2] = b * c;
	K[1][3] = b * d;

	K[2][0] = K[0][2];	// ��Ī��
	K[2][1] = K[1][2];	// ��Ī��
	K[2][2] = c * c;
	K[2][3] = c * d;

	K[3][0] = K[0][3];	// ��Ī��
	K[3][1] = K[1][3];	// ��Ī��
	K[3][2] = K[2][3];	// ��Ī��
	K[3][3] = d * d;
}

/*
*	\brief 3�� 3���� ��Ī ��� A�� ������� ��ȯ�Ѵ�.
*
*	\param A ��� ���
*	\param det ��� A�� ��Ľ�
*
*	\return ��� A�� ������� ��ȯ�Ѵ�.
*/
GMatrix EgDecimator::Inv3(GMatrix &A, double det)
{
	GMatrix ret(3, 3);
	ret[0][0] = (A[1][1] * A[2][2] - A[1][2] * A[2][1]) / det;
	ret[0][1] = -(A[0][1] * A[2][2] - A[0][2] * A[2][1]) / det;
	ret[0][2] = (A[0][1] * A[1][2] - A[0][2] * A[1][1]) / det;

	ret[1][1] = (A[0][0] * A[2][2] - A[0][2] * A[2][0]) / det;
	ret[1][2] = -(A[0][0] * A[1][2] - A[0][2] * A[1][0]) / det;

	ret[2][2] = (A[0][0] * A[1][1] - A[0][1] * A[1][0]) / det;

	ret[1][0] = ret[0][1];
	ret[2][0] = ret[0][2];
	ret[2][1] = ret[1][2];

	return ret;
}

/*!
*	\brief �ʱ�ȭ �۾��� �����Ѵ�.
*/
void EgDecimator::Init()
{
	// �޽� ������ �ε����� �����Ѵ�.
	int NumVert = m_pMesh->UpdateVertIdx();

	// �޽� ������ QEM ����� �ʱ�ȭ �Ѵ�.
	InitVertQEM(NumVert);

	// ������ ���� ����� �ʱ�ȭ �Ѵ�.
	InitCollapseInfo();
}

/*!
*	\brief �޽� �������� ���ǵǴ� QEM ����� �ʱ�ȭ �Ѵ�.
*
*	\param NumVert �޽� ������ ����
*/
void EgDecimator::InitVertQEM(int NumVert)
{
	// QEM ����Ʈ�� �ʱ�ȭ �Ѵ�.
	m_QEM.clear();
	m_QEM.assign(NumVert, GMatrix(4, 4));

	// ������ �ﰢ���� ���Ͽ�
	EgFace *f = m_pMesh->m_pFaces;
	while (f != NULL)
	{
		// �ﰢ���� ������ ����� ������ (ax + by + cz + d = 0)�� ���Ͽ�
		GVector3 n = f->GetFaceNormal(); // n = (a, b, c)
		double d = -n * cast_vec3(f->GetVertex(0)->m_Pos);

		// �ﰢ�� f�� ���� fundamental error quadric ����� ���ϰ�
		GMatrix K(4, 4);
		GetErrorMatrix(K, n[0], n[1], n[2], d);

		// �ﰢ���� �� ������ K�� �����Ѵ�.
		m_QEM[f->GetVertex(0)->m_Idx] += K;
		m_QEM[f->GetVertex(1)->m_Idx] += K;
		m_QEM[f->GetVertex(2)->m_Idx] += K;

		// �ﰢ���� ������ ������ ���Ͽ�
		for (int i = 0; i < 3; ++i)
		{
			EgEdge *e = f->GetEdge(i);

			// ��� �������,
			if (e->m_pMate == NULL)
			{
				// ������ ������ �ﰢ���� ������ ����� ���ϰ�, 
				GVector3 ei = (e->m_pNext->m_pVert->m_Pos - e->m_pVert->m_Pos).Normalize();
				GVector3 ni = (n ^ ei).Normalize();
				double di = -ni * cast_vec3(e->m_pVert->m_Pos);

				// ������� ���ǵǴ� quadric error ����� ���Ѵ�.
				GMatrix Ki(4, 4);
				GetErrorMatrix(Ki, ni[0], ni[1], ni[2], di);

				// ������ �糡���� ���Ѵ�.
				Ki *= 100.0;
				m_QEM[e->m_pVert->m_Idx] += Ki;
				m_QEM[e->m_pNext->m_pVert->m_Idx] += Ki;
			}
		}

		// ���� �ﰢ���� ó���Ѵ�.
		f = f->m_pNext;
	}	
}

/*!
*	\brief ������ ������ �����ϴ� �ʱ� ����� ����Ѵ�.
*	\note ���� (vi, vk)�� (vk, vi)�� ������ ������ �����Ͽ� ��ǥ ������ ���ؼ��� ����� ���Ѵ�.
*/
void EgDecimator::InitCollapseInfo()
{
	// ������ ������ ���Ͽ�
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		// �������� �����ϴ� ������ ������ ���Ͽ�
		for (EgEdge *e : v->m_pEdges)
		{
			// ��ǥ ����(��迡�� �Ǵ� ���� �湮�� ����)���
			if (e->m_pMate == NULL || e->m_pMate->m_Cost == 0.0)
			{
				// ������ �糡���� ���Ͽ�,
				EgVertex *v1 = v;
				EgVertex *v2 = e->m_pNext->m_pVert;

				// ���ο� ������ QEM�� ���Ѵ�.
				GMatrix Q = m_QEM[v1->m_Idx] + m_QEM[v2->m_Idx];

				// ���ο� ������ ��ġ�� ���Ѵ�.
				GVector x(3), b(3);
				b.Set(Q[0][3], Q[1][3], Q[2][3]);
				double det = (Q[0][0] * Q[1][1] * Q[2][2] + 2 * Q[0][1] * Q[1][2] * Q[0][2])
					- (Q[1][2] * Q[1][2] * Q[0][0] + Q[0][2] * Q[0][2] * Q[1][1] + Q[0][1] * Q[0][1] * Q[2][2]);

				if (EQ_ZERO(det, 1.0e-7))
					x = cast_vec(v1->m_Pos + (v2->m_Pos - v1->m_Pos) * 0.5);
				else
					x = Inv3(Q, det) * (-b);

				// ��ǥ ������ ���� ������ ���� �߰��Ѵ�.
				e->m_Cost = x * b + Q[3][3];
				EgCollapseInfo info(e, cast_pt3(x));
				m_Heap.push(info);
			}
		}
		v = v->m_pNext;
	}
}

/*!
*	\brief �ݺ������� ���� ���Ÿ� �����Ѵ�.
*
*	\prarm Ratio ������ �ﰢ���� ����
*	\param FileName ���ŵǴ� ���� ������ ����� ���ϸ�
*/
void EgDecimator::Execute(double Ratio, const char *FileName)
{
	// �ʱ� �ﰢ���� ������ ���Ѵ�.
	int NumFace = m_pMesh->GetNumFace();
	int TarNumFace = (int)(NumFace * (1.0 - Ratio));

	// ���ŵǴ� ������ ������ ����� ������ �����Ѵ�.
	FILE *fp;
	if (FileName != NULL)
		fopen_s(&fp, FileName, "w");
	
	// �ݺ������� ������ �����Ͽ� �ﰢ���� ������ ���δ�.
	while (!m_Heap.empty())
	{
		// �ּ� ����� ���� ���� ������ �̾Ƽ�,
		EgCollapseInfo Info = m_Heap.top();
		m_Heap.pop();

		// �̹� ���ŵ� �����̰ų�, ������ ���� ������� ��ŵ�Ѵ�.
		if (Info.m_pEdge->m_Cost == -1.0 || 
			Info.m_pEdge->m_Cost != Info.m_Cost)
			continue;

		// ���õ� ������ �����ϰ�, 
		int sIdx, eIdx;
		int result = CollapseEdge(Info, sIdx, eIdx);
		if (result > 0)
		{
			// ���ŵ� �ﰢ�� ���� ���̰�,
			NumFace = NumFace - result;

			// ���ŵǴ� ������ ���� �ε����� ����Ѵ�.
			if (FileName != NULL)
				fprintf_s(fp, "%d\t%d\n", sIdx, eIdx);
		}
				
		// �ﰢ���� ������ ���ϴ� ���� �����ߴٸ� �����Ѵ�.
		if (NumFace <= TarNumFace)
			break;
	}
	
	// ������ ����� �����Ѵ�.
	if (FileName != NULL)
		fclose(fp);

	// ���ŵ� �ﰢ���� �����Ѵ�.
	std::vector<EgFace *> DelFaces;
	EgFace *f = m_pMesh->m_pFaces;
	while (f != NULL)
	{
		if (f->m_pEdge->m_Cost == -1.0)
			DelFaces.push_back(f);
		f = f->m_pNext;
	}
	for (EgFace *f : DelFaces)
		DEL_FROM_PTR_LIST(m_pMesh->m_pFaces, f);
}

/*!
*	\brief ������ ������ ���� ���� ���Ÿ� �����Ѵ�.
*
*	\param fname ���� ������ ���� ������ ����� ����
*/
void EgDecimator::Execute(const char *fname)
{
	// ������ �б� ���� �����Ѵ�.
	FILE *fp;
	fopen_s(&fp, fname, "r");
	if (fp == NULL)
	{
		printf("File open failed...\n");
		return;
	}

	// �ε��� ������ ���� ������ �迭�� �����Ѵ�.
	std::vector<EgVertex *> Verts;
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		Verts.push_back(v);
		v = v->m_pNext;
	}

	// ���Ͽ��� ������ ������ �������� ������ �ε����� �д´�.
	int vidx1, vidx2;
	while (fscanf_s(fp, "%d%d", &vidx1, &vidx2) != EOF)
	{
		// ������ �������� ������ ���Ͽ�,
		EgVertex *v1 = Verts[vidx1];
		EgVertex *v2 = Verts[vidx2];

		// �ش� ������ ã�´�.
		EgEdge *pEdge = NULL;
		for (EgEdge *e : v1->m_pEdges)
		{
			if (e->m_pNext->m_pVert == v2)
			{
				pEdge = e;
				break;
			}
		}

		// �ش� ������ ã�� ���ߴٸ� ���� �޽����� ����Ѵ�.
		if (pEdge == NULL)
			printf("Edge is not found...\n");
		
		// ���ο� ������ QEM�� ���Ѵ�.
		GMatrix Q = m_QEM[v1->m_Idx] + m_QEM[v2->m_Idx];

		// ���ο� ������ ��ġ�� ���Ѵ�.
		GVector x(3), b(3);
		b.Set(Q[0][3], Q[1][3], Q[2][3]);
		double det =
			(Q[0][0] * Q[1][1] * Q[2][2] + 2 * Q[0][1] * Q[1][2] * Q[0][2]) -
			(Q[1][2] * Q[1][2] * Q[0][0] + Q[0][2] * Q[0][2] * Q[1][1] + Q[0][1] * Q[0][1] * Q[2][2]);

		if (EQ_ZERO(det, 1.0e-7))
			x = cast_vec(v1->m_Pos + (v2->m_Pos - v1->m_Pos) * 0.5);
		else
			x = Inv3(Q, det) * (-b);

		// ������ ������(v1)�� ��ġ�� �����Ѵ�.
		v1->m_Pos = cast_pt3(x);

		// ������ ������ �ﰢ���� �����Ѵ�(������ ���´�).
		std::vector<EgFace *> Faces;
		Faces.push_back(pEdge->m_pFace);
		if (pEdge->m_pMate != NULL)
			Faces.push_back(pEdge->m_pMate->m_pFace);
		m_pMesh->DelFaces(Faces);

		// v2���� �����ϴ� ��� �������� �������� v1���� �����Ѵ�.
		for (EgEdge *e : v2->m_pEdges)
		{
			e->m_pVert = v1;
			v1->m_pEdges.push_back(e);
		}

		// v2���� �����ϴ� ���� ������ �����ϰ�,
		v2->m_pEdges.clear();

		// v1���� �����ϴ� ������ mate ������ ������ �����Ѵ�.
		m_pMesh->UpdateEdgeMate(v1);

		// v1�� QEM ����� �����Ѵ�.
		m_QEM[v1->m_Idx] += m_QEM[v2->m_Idx];
	}	
}

/*!
*	\brief ������ ���� �������� �˻��Ѵ�.
*
*	\param pEdge ���� ���� ���θ� ������ ������ ���� ������ 
*
*	\return ������ ���Ű����� ��� true, �ƴϸ� false�� ��ȯ�Ѵ�.
*/
bool EgDecimator::IsCollapsible(EgEdge *pEdge)
{
	// ������ ������(v1)�� ����(v2)�� ���Ѵ�.
	EgVertex *v1 = pEdge->m_pVert;
	EgVertex *v2 = pEdge->m_pNext->m_pVert;

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
	if (pEdge->m_pMate == NULL && C.size() == 1)
		return true;
	if (pEdge->m_pMate != NULL && C.size() == 2)
		return true;

	// ���� ���� ������ �������� ���ϴ� ��� false�� ��ȯ�Ѵ�.
	return false;
}

/*!
*	\brief �޽��� ���� ������ ������ �����Ѵ�.
*
*	\param Info ���� ����
*	\param sIdx ���ŵǴ� ������ ���� ������ �ε����� ����ȴ�.
*	\param eIdx ���ŵǴ� ������ �� ������ �ε����� ����ȴ�.
*
*	\return ���ŵ� �ﰢ���� ������ ��ȯ�Ѵ�(0: ���� �Ұ���, 1, 2)
*/
int EgDecimator::CollapseEdge(EgCollapseInfo &Info, int &sIdx, int &eIdx)
{
	// ���Ű� �Ұ����� ������� 0�� ��ȯ�Ѵ�.
	if (IsCollapsible(Info.m_pEdge) == false)
		return 0;

	int RemovedFaces = 1;
	// ������ ������(v1)�� ��ġ�� �����Ѵ�.
	EgEdge *pEdge = Info.m_pEdge;
	EgVertex *v1 = pEdge->m_pVert;
	EgVertex *v2 = pEdge->m_pNext->m_pVert;
	v1->m_Pos = Info.m_Pos;

	// �ﰢ���� �������� ���ŵǾ��ٴ� ���� ����Ѵ�.
	pEdge->m_Cost = -1.0;
	pEdge->m_pNext->m_Cost = -1.0;
	pEdge->m_pNext->m_pNext->m_Cost = -1.0;

	// ������ ������ �ﰢ���� �����Ѵ�(������ ���´�).
	std::vector<EgFace *> Faces;
	Faces.push_back(pEdge->m_pFace);
	if (pEdge->m_pMate != NULL)
	{
		RemovedFaces = 2;
		EgEdge *pMate = pEdge->m_pMate;
		Faces.push_back(pMate->m_pFace);

		// �ﰢ���� �������� ���ŵǾ��ٴ� ���� ����Ѵ�.
		pMate->m_Cost = -1.0;
		pMate->m_pNext->m_Cost = -1.0;
		pMate->m_pNext->m_pNext->m_Cost = -1.0;
	}
	
	// ������ �ﰢ���� ���Ͽ�
	for (EgFace *f : Faces)
	{
		// ������ ������ �ݴ��� ������ ������ �����Ѵ�.
		for (int i = 0; i < 3; ++i)
		{
			EgEdge *e = f->GetEdge(i);
			if (e->m_pMate != NULL)
				e->m_pMate->m_pMate = NULL;

			// �������� �����ϴ� ���� ����Ʈ���� ���� ������ �����Ѵ�.
			DEL_FROM_VEC(e->m_pVert->m_pEdges, e);
		}
	}

	// ������ ����(v2)���� �����ϴ� ��� �������� �������� v1���� �����Ѵ�.
	for (EgEdge *e : v2->m_pEdges)
	{
		e->m_pVert = v1;
		v1->m_pEdges.push_back(e);
	}

	// ����(v2)�� ���� ������ �����ϰ�,
	v2->m_pEdges.clear();

	// ������(v1)�� mate ������ ������ �����Ѵ�.
	m_pMesh->UpdateEdgeMate(v1);

	// ������(v1)�� QEM ����� �����Ѵ�.
	m_QEM[v1->m_Idx] = m_QEM[v1->m_Idx] + m_QEM[v2->m_Idx];

	// ���ŵǴ� ������ �������� ������ �ε����� �����Ѵ�.
	sIdx = v1->m_Idx;
	eIdx = v2->m_Idx;

	// ������(v1)�� ������ �ﰢ���� ��� ���� ����� �����Ѵ�.
	UpdateCollapseInfo(v1);

	// ���ŵ� �ﰢ���� ������ ��ȯ�Ѵ�.
	return RemovedFaces;
}

/*!
*	\brief ������ ������ ������ ���� ����� �����Ѵ�.
*
*	\param v ������ ���� ������
*/
void EgDecimator::UpdateCollapseInfo(EgVertex *v)
{
	// ���� v���� �����ϴ� ������ ������ ���Ͽ�
	for (EgEdge *e : v->m_pEdges)
	{
		// e�� mate ������ ��ǥ �������ٸ�, e�� ��ǥ ������ �����Ѵ�.
		if (e->m_pMate != NULL && e->m_pMate->m_Cost != 0.0)
			e->m_pMate->m_Cost = 0.0;
		
		// ��ǥ ������ �糡���� ���ϰ�,
		EgVertex *v1 = e->m_pVert;
		EgVertex *v2 = e->m_pNext->m_pVert;

		// ���ο����� QEM ����� ���Ͽ�,
		GMatrix Q = m_QEM[v1->m_Idx] + m_QEM[v2->m_Idx];
		GVector x(3), b(3);
		b.Set(Q[0][3], Q[1][3], Q[2][3]);

		// ���ο� ��ġ�� ���ϰ�,
		double det =
			(Q[0][0] * Q[1][1] * Q[2][2] + 2 * Q[0][1] * Q[1][2] * Q[0][2]) -
			(Q[1][2] * Q[1][2] * Q[0][0] + Q[0][2] * Q[0][2] * Q[1][1] + Q[0][1] * Q[0][1] * Q[2][2]);
		if (EQ_ZERO(det, 1.0e-7))
			x = cast_vec(v1->m_Pos + (v2->m_Pos - v1->m_Pos) * 0.5);
		else
			x = Inv3(Q, det) * (-b);

		// ��ǥ ���� ���� ������ ���� �߰��Ѵ�.
		e->m_Cost = x * b + Q[3][3];
		EgCollapseInfo info(e, cast_pt3(x));
		m_Heap.push(info);
	}
}
