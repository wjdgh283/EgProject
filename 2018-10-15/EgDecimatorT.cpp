#include "EgDecimatorT.h"

/*!
*	\brief ������
*/
EgCollapseInfoT::EgCollapseInfoT(EgEdge *pEdge, GPoint3 Pos, GVector2 Coord)
{
	m_pEdge = pEdge;
	m_Pos = Pos;
	m_Coord = Coord;
	m_Cost = pEdge->m_Cost;
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ��ü
*/
EgCollapseInfoT::EgCollapseInfoT(const EgCollapseInfoT &cpy)
{
	m_pEdge = cpy.m_pEdge;
	m_Pos = cpy.m_Pos;
	m_Coord = cpy.m_Coord;
	m_Cost = cpy.m_Cost;
}

/*!
*	\brief �Ҹ���
*/
EgCollapseInfoT::~EgCollapseInfoT()
{
}

/*!
*	\brief ���� ������
*
*	\param rhs ���Ե� ��ü
*/
EgCollapseInfoT &EgCollapseInfoT::operator =(const EgCollapseInfoT &rhs)
{
	m_pEdge = rhs.m_pEdge;
	m_Pos = rhs.m_Pos;
	m_Coord = rhs.m_Coord;
	m_Cost = rhs.m_Cost;
	return *this;
}

/*!
*	\brief ��(>) ������
*
*	\param lhs ���� ��ü
*	\param rhs ������ ��ü
*/
bool operator >(const EgCollapseInfoT &lhs, const EgCollapseInfoT &rhs)
{
	return (lhs.m_Cost > rhs.m_Cost);
}

/*!
*	\brief ������
*
*	\param pMesh �ܼ�ȭ�� ��� �޽��� ���� ������
*/
EgDecimatorT::EgDecimatorT(EgMesh *pMesh)
{
	// ��� �޽��� �����Ѵ�.
	m_pMesh = pMesh;
	m_MeshSize = 0.0;
}

/*!
*	\brief �Ҹ���
*/
EgDecimatorT::~EgDecimatorT()
{
}

/*!
*	\brief �ʱ�ȭ �۾��� �����Ѵ�.
*/
void EgDecimatorT::Init()
{
	// �޽� ������ �ε����� �����Ѵ�.
	int NumVert = m_pMesh->UpdateVertIdx();

	// �޽��� ũ�⸦ �ʱ�ȭ �Ѵ�.
	InitMeshSize();

	// ��� ������ ������ �ʱ�ȭ �Ѵ�.
	InitBndryVertInfo(NumVert);

	// �޽� ������ QEM ����� �ʱ�ȭ �Ѵ�.
	InitVertQEM(NumVert);

	// ������ ���� ����� �ʱ�ȭ �Ѵ�.
	InitCollapseInfo();
}

/*!
*	\brief �޽��� ������ ũ��� �ʱ�ȭ �Ѵ�.
*/
void EgDecimatorT::InitMeshSize()
{
	GVector3 p, q;
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		GPoint3 r = v->m_Pos;
		p[0] = MIN(p[0], r[0]);
		p[1] = MIN(p[1], r[1]);
		p[2] = MIN(p[2], r[2]);
		q[0] = MAX(q[0], r[0]);
		q[1] = MAX(q[1], r[1]);
		q[2] = MAX(q[2], r[2]);
		v = v->m_pNext;
	}

	// ��� ���� �밢���� ���̸� �ʱ�ȭ �Ѵ�.
	m_MeshSize = norm(q - p);

	double scl = 350.0 / m_MeshSize;
	v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		v->m_Pos[0] = v->m_Pos[0] * scl;
		v->m_Pos[1] = v->m_Pos[1] * scl;
		v->m_Pos[2] = v->m_Pos[2] * scl;
		v = v->m_pNext;
	}
}

/*!
*	\brief �޽��� ��� ���� ������ ���Ѵ�.
*
*	\param NumVert �޽� ������ ����
*/
void EgDecimatorT::InitBndryVertInfo(int NumVert)
{
	// ��� ���� ������ �ʱ�ȭ �Ѵ�.
	m_BndryVertInfo.clear();
	m_BndryVertInfo.assign(NumVert, false);
	
	// �޽� ������ ������ ���Ͽ�
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		// �������� �����ϴ� ������ ������ ���Ͽ�
		EgTexel *pTexel = v->m_pEdges[0]->m_pTexel;
		for (EgEdge *e : v->m_pEdges)
		{
			// �ϳ��� ��� ������ �ִٸ�
			if (e->m_pMate == NULL)
			{
				// ��� �������� ����Ѵ�.
				m_BndryVertInfo[v->m_Idx] = true;
				break;
			}

			if (e->m_pTexel->m_Coord != pTexel->m_Coord)
			{
				// ��� �������� ����Ѵ�.
				m_BndryVertInfo[v->m_Idx] = true;
				break;
			}
		}
		v = v->m_pNext;
	}
}

/*!
*	\brief �Ϲ�ȭ�� Quatric error ����� ���Ѵ�.
*
*	\param K ����� ����� 6 x 6 ���
*	\param p �ﰢ���� �Ϲ�ȭ�� ���� ����
*	\param q �ﰢ���� �Ϲ�ȭ�� ���� ����
*	\param r �ﰢ���� �Ϲ�ȭ�� ���� ����
*/
void EgDecimatorT::GetErrorMatrix(GMatrix &K, GVector &p, GVector &q, GVector &r)
{
	// ������ 5���� ���� e1, e2 �����Ѵ�.
	GVector e1 = (q - p).Normalize();
	GVector e2 = ((r - p) - ((r - p) * e1) * e1).Normalize();

	// ��� K�� 5 x 5 ��� A�� 5���� ���� b�� ��� c�� �����ȴ�.
	// K = | A    b |
	//     | b^t  c |

	// ��� K�� A�κ��� ���ϰ�,		
	K[0][0] = 1.0 - e1[0] * e1[0] - e2[0] * e2[0];
	K[0][1] = -e1[0] * e1[1] - e2[0] * e2[1];
	K[0][2] = -e1[0] * e1[2] - e2[0] * e2[2];
	K[0][3] = -e1[0] * e1[3] - e2[0] * e2[3];
	K[0][4] = -e1[0] * e1[4] - e2[0] * e2[4];

	K[1][0] = K[0][1];
	K[1][1] = 1.0 - e1[1] * e1[1] - e2[1] * e2[1];
	K[1][2] = -e1[1] * e1[2] - e2[1] * e2[2];
	K[1][3] = -e1[1] * e1[3] - e2[1] * e2[3];
	K[1][4] = -e1[1] * e1[4] - e2[1] * e2[4];

	K[2][0] = K[0][2];
	K[2][1] = K[1][2];
	K[2][2] = 1.0 - e1[2] * e1[2] - e2[2] * e2[2];
	K[2][3] = -e1[2] * e1[3] - e2[2] * e2[3];
	K[2][4] = -e1[2] * e1[4] - e2[2] * e2[4];

	K[3][0] = K[0][3];
	K[3][1] = K[1][3];
	K[3][2] = K[2][3];
	K[3][3] = 1.0 - e1[3] * e1[3] - e2[3] * e2[3];
	K[3][4] = -e1[3] * e1[4] - e2[3] * e2[4];

	K[4][0] = K[0][4];
	K[4][1] = K[1][4];
	K[4][2] = K[2][4];
	K[4][3] = K[3][4];
	K[4][4] = 1.0 - e1[4] * e1[4] - e2[4] * e2[4];

	// ���� b�� ���ϰ�,
	GVector b = (p * e1) * e1 + (p * e2) * e2 - p;
	K[5][0] = K[0][5] = b[0];
	K[5][1] = K[1][5] = b[1];
	K[5][2] = K[2][5] = b[2];
	K[5][3] = K[3][5] = b[3];
	K[5][4] = K[4][5] = b[4];

	// ��� c�� ����Ѵ�.
	double pe1 = p * e1;
	double pe2 = p * e2;
	K[5][5] = p * p - SQR(pe1) - SQR(pe2);
}

/*!
*	\brief �޽� �������� ���ǵǴ� QEM ����� �ʱ�ȭ �Ѵ�.
*
*	\param NumVert �޽� ������ ����
*/
void EgDecimatorT::InitVertQEM(int NumVert)
{
	// QEM ����Ʈ�� �ʱ�ȭ �Ѵ�.
	m_QEM.clear();
	m_QEM.assign(NumVert, GMatrix(6, 6));

	// ������ �ﰢ���� ���Ͽ�
	EgFace *f = m_pMesh->m_pFaces;
	while (f != NULL)
	{
		// 5������ �� �� p, q, r �����Ѵ�.
		EgVertex *v0 = f->GetVertex(0);
		EgVertex *v1 = f->GetVertex(1);
		EgVertex *v2 = f->GetVertex(2);
		EgTexel *t0 = f->GetTexel(0);
		EgTexel *t1 = f->GetTexel(1);
		EgTexel *t2 = f->GetTexel(2);
		GVector p(5, v0->m_Pos[0], v0->m_Pos[1], v0->m_Pos[2], t0->m_Coord[0], t0->m_Coord[1]);
		GVector q(5, v1->m_Pos[0], v1->m_Pos[1], v1->m_Pos[2], t1->m_Coord[0], t1->m_Coord[1]);
		GVector r(5, v2->m_Pos[0], v2->m_Pos[1], v2->m_Pos[2], t2->m_Coord[0], t2->m_Coord[1]);

		// �ؽ�ó ��ǥ�� ������ Quadric error ����� ���Ѵ�.
		GMatrix K(6, 6);
		GetErrorMatrix(K, p, q, r);		

		// �ﰢ���� �� ������ QEM�� �����Ѵ�.
		m_QEM[f->GetVertex(0)->m_Idx] += K;
		m_QEM[f->GetVertex(1)->m_Idx] += K;
		m_QEM[f->GetVertex(2)->m_Idx] += K;

		// ���� �ﰢ���� ó���Ѵ�.
		f = f->m_pNext;
	}
}

/*!
*	\brief �޽��� ������ ������ �����ϴ� ����� �ʱ�ȭ �Ѵ�.
*	\note ���� (vi, vk)�� (vk, vi)�� ������ ������ �Ǵ��Ͽ� ��ǥ ������ ���ؼ��� ����� ���Ѵ�.
*/
void EgDecimatorT::InitCollapseInfo()
{
	// ������ ������ ���Ͽ�
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		// ��� ������ �ƴ϶��
		if (!m_BndryVertInfo[v->m_Idx])
		{
			// �������� �����ϴ� ������ ������ ���Ͽ�
			for (EgEdge *e : v->m_pEdges)
			{
				// ��ǥ ����(��迡�� �Ǵ� ���� �湮�� ����)���
				if (e->m_pMate == NULL || e->m_pMate->m_Cost == 0.0)
				{
					// ������ �糡 ������ �ؽ�ó ������ ���Ѵ�.
					EgVertex *v0 = v;
					EgVertex *v1 = e->m_pNext->m_pVert;
					EgTexel *t0 = e->m_pTexel;
					EgTexel *t1 = e->m_pNext->m_pTexel;

					// ������ �糡��(position + texture coord)�� ���Ͽ�,
					GVector vt0(5, v0->m_Pos[0], v0->m_Pos[1], v0->m_Pos[2], t0->m_Coord[0], t0->m_Coord[1]);
					GVector vt1(5, v1->m_Pos[0], v1->m_Pos[1], v1->m_Pos[2], t1->m_Coord[0], t1->m_Coord[1]);

					// ���ο� ������ 6x6 QEM�� ���Ѵ�.
					GMatrix Q = m_QEM[v0->m_Idx] + m_QEM[v1->m_Idx];

					// ���ο� ������ 5���� ���� (position + texture coord)�� ���Ѵ�.
					GVector x(5), b(5);
					b.Set(Q[0][5], Q[1][5], Q[2][5], Q[3][5], Q[4][5]);

					// 6x6 K ����� �κ���� A�� ���Ͽ� ���ο� ������ ����Ѵ�.
					GMatrix A = sub(Q, 0, 0, 4, 4);
					if (EQ_ZERO(det(A), 1.0e-9))
					{
						//printf("Singular matrix...\n");
						printf("Singular value = %1.10lf\n", det(A));
						x = (vt0 + vt1) * 0.5;
						x[3] = vt0[3];
						x[4] = vt0[4];
					}
					else
						x = inv(A) * (-b);

					// ��ǥ ���� ���� ������ ���� �߰��Ѵ�.
					normalize_uv(x[3], x[4]);
					e->m_Cost = x * b + Q[5][5];

					EgCollapseInfoT info(e, GPoint3(x[0], x[1], x[2]), GVector2(x[3], x[4]));
					m_Heap.push(info);
				}
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
void EgDecimatorT::Execute(double Ratio, const char *FileName)
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
		EgCollapseInfoT Info = m_Heap.top();
		m_Heap.pop();

		// �̹� ���ŵ� �����̰ų�, ���� ���� ����� ����Ǿ��ٸ� ��ŵ�Ѵ�.
		if (Info.m_pEdge->m_Cost == -1.0 ||
			Info.m_pEdge->m_Cost != Info.m_Cost)
			continue;

		// ���õ� ������ �����ϰ�, �ﰢ�� ���� ���δ�.
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
		if (NumFace < TarNumFace)
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

	// �޽��� ũ�⸦ ������� �����Ѵ�.
	double scl = m_MeshSize / 350.0;
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		v->m_Pos[0] = v->m_Pos[0] * scl;
		v->m_Pos[1] = v->m_Pos[1] * scl;
		v->m_Pos[2] = v->m_Pos[2] * scl;
		v = v->m_pNext;
	}
}

/*!
*	\brief ������ ������ ���� ���� ���Ÿ� �����Ѵ�.
*
*	\param fname ���� ������ ���� ������ ����� ����
*/
void EgDecimatorT::Execute(const char *fname)
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

		// ������ �糡 ������ �ؽ�ó ������ ���Ѵ�.
		EgTexel *t1 = pEdge->m_pTexel;
		EgTexel *t2 = pEdge->m_pNext->m_pTexel;

		// ������ �糡��(position + texture coord)�� ���Ͽ�,
		GVector vt1(5, v1->m_Pos[0], v1->m_Pos[1], v1->m_Pos[2], t1->m_Coord[0], t1->m_Coord[1]);
		GVector vt2(5, v2->m_Pos[0], v2->m_Pos[1], v2->m_Pos[2], t2->m_Coord[0], t2->m_Coord[1]);
		
		// �ش� ������ ã�� ���ߴٸ� ���� �޽����� ����Ѵ�.
		if (pEdge == NULL)
			printf("Edge is not found...\n");

		// ���ο� ������ QEM�� ���Ѵ�.
		GMatrix Q = m_QEM[v1->m_Idx] + m_QEM[v2->m_Idx];

		// ���ο� ������ 5���� ���� (position + texture coord)�� ���Ѵ�.
		GVector x(5), b(5);
		b.Set(Q[0][5], Q[1][5], Q[2][5], Q[3][5], Q[4][5]);

		// 6x6 K ����� �κ���� A�� ���Ͽ� ���ο� ������ ����Ѵ�.
		GMatrix A = sub(Q, 0, 0, 4, 4);
		if (EQ_ZERO(det(A), 1.0e-9))
		{
			//printf("Singular matrix...\n");
			printf("Singular value = %1.10lf\n", det(A));
			x = (vt1 + vt2) * 0.5;
			x[3] = vt1[3];
			x[4] = vt1[4];
		}
		else
			x = inv(A) * (-b);
				
		// ������ ������(v1)�� ��ġ�� �����Ѵ�.
		v1->m_Pos.Set(x[0], x[1], x[2]);

		// ������ ���������� �����ϴ� ��� ������ �ؽ�ó ��ǥ�� �����Ѵ�.
		normalize_uv(x[3], x[4]);
		for (EgEdge *e : v1->m_pEdges)
			e->m_pTexel->m_Coord = GVector2(x[3], x[4]);

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
			e->m_pTexel = pEdge->m_pTexel;
			v1->m_pEdges.push_back(e);
		}

		// v2���� �����ϴ� ���� ������ �����ϰ�,
		v2->m_pEdges.clear();

		// v1���� �����ϴ� ������ mate ������ ������ �����Ѵ�.
		m_pMesh->UpdateEdgeMate(v1);

		// v1�� QEM ����� �����Ѵ�.
		m_QEM[v1->m_Idx] += m_QEM[v2->m_Idx];
	}

	// �޽��� ũ�⸦ ������� �����Ѵ�.
	double scl = m_MeshSize / 350.0;
	v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		v->m_Pos[0] = v->m_Pos[0] * scl;
		v->m_Pos[1] = v->m_Pos[1] * scl;
		v->m_Pos[2] = v->m_Pos[2] * scl;
		v = v->m_pNext;
	}
}

/*!
*	\brief ������ ���� �������� �˻��Ѵ�.
*
*	\param pEdge ���� ���� ���θ� ������ ������ ���� ������
*
*	\return ������ ���Ű����� ��� true, �ƴϸ� false�� ��ȯ�Ѵ�.
*/
bool EgDecimatorT::IsCollapsible(EgEdge *pEdge)
{
	// ������ ������(v1)�� ����(v2)�� ���Ѵ�.
	EgVertex *v1 = pEdge->m_pVert;
	EgVertex *v2 = pEdge->m_pNext->m_pVert;

	// ������ ������ ��� ������ ���Եȴٸ� false�� ��ȯ�Ѵ�.
	if (m_BndryVertInfo[v1->m_Idx] == true || m_BndryVertInfo[v2->m_Idx] == true)
		return false;

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
*	\param pEdge ������ ������ ���� ������
*
*	\return �Բ� ���ŵ� �ﰢ���� ������ ��ȯ�Ѵ�(0: ���� �Ұ���, 1, 2)
*/
int EgDecimatorT::CollapseEdge(EgCollapseInfoT &info, int &sIdx, int &eIdx)
{
	// ���Ű� �Ұ����� ������� 0�� ��ȯ�Ѵ�.
	if (IsCollapsible(info.m_pEdge) == false)
		return 0;

	int RemovedFaces = 1;
	EgEdge *pEdge = info.m_pEdge;

	// ������ ������(v1)�� ����(v2)�� ���� ���ο� ��ġ�� �ؽ��� ��ǥ�� ���Ѵ�.
	EgVertex *v1 = pEdge->m_pVert;
	EgVertex *v2 = pEdge->m_pNext->m_pVert;
	v1->m_Pos = info.m_Pos;
	for (EgEdge *e : v1->m_pEdges)
		e->m_pTexel->m_Coord = info.m_Coord;
	
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

	// v2���� �����ϴ� ��� �������� �������� �ؽ��� ��ǥ�� v1���� �����ϰ� 
	for (EgEdge *&e : v2->m_pEdges)
	{
		e->m_pVert = v1;
		//e->m_pTexel->m_Coord = info.m_Coord;
		e->m_pTexel = info.m_pEdge->m_pTexel;
		v1->m_pEdges.push_back(e);
	}

	// v2���� �����ϴ� ���� ������ �����ϰ�,
	v2->m_pEdges.clear();

	// v1���� �����ϴ� ������ mate ������ ������ �����Ѵ�.
	m_pMesh->UpdateEdgeMate(v1);

	// v1�� QEM ����� �����Ѵ�.
	m_QEM[v1->m_Idx] += m_QEM[v2->m_Idx];

	// ���ŵǴ� ������ �������� ������ �ε����� �����Ѵ�.
	sIdx = v1->m_Idx;
	eIdx = v2->m_Idx;

	// v1�� ������ �ﰢ���� ��� ���� ����� �����Ѵ�.
	UpdateCollapseInfo(v1);

	// ���ŵ� �ﰢ���� ������ ��ȯ�Ѵ�.
	return RemovedFaces;
}

/*!
*	\brief ������ ������ ������ ���� ����� �����Ѵ�.
*
*	\param v ������ ���� ������
*/
void EgDecimatorT::UpdateCollapseInfo(EgVertex *v)
{
	// ���� v���� �����ϴ� ������ ������ ���Ͽ�
	for (EgEdge *e : v->m_pEdges)
	{
		// e�� mate ������ ��ǥ �������ٸ�, e�� ��ǥ ������ �����Ѵ�.
		if (e->m_pMate != NULL && e->m_pMate->m_Cost != 0.0)
			e->m_pMate->m_Cost = 0.0;

		// ��ǥ ������ �糡���� ���ϰ�,
		EgVertex *v0 = e->m_pVert;
		EgVertex *v1 = e->m_pNext->m_pVert;
		EgTexel *t0 = e->m_pTexel;
		EgTexel *t1 = e->m_pNext->m_pTexel;

		// ������ �糡��(position + texture coord)�� ���Ͽ�,
		GVector vt0(5, v0->m_Pos[0], v0->m_Pos[1], v0->m_Pos[2], t0->m_Coord[0], t0->m_Coord[1]);
		GVector vt1(5, v1->m_Pos[0], v1->m_Pos[1], v1->m_Pos[2], t1->m_Coord[0], t1->m_Coord[1]);

		// ���ο� ������ 6x6 QEM�� ���Ѵ�.
		GMatrix Q = m_QEM[v0->m_Idx] + m_QEM[v1->m_Idx];

		// ���ο� ������ 5���� ���� (position + texture coord)�� ���Ѵ�.
		GVector x(5), b(5);
		b.Set(Q[0][5], Q[1][5], Q[2][5], Q[3][5], Q[4][5]);

		// 6x6 K ����� �κ���� A�� ���Ͽ� ���ο� ������ ����Ѵ�.
		GMatrix A = sub(Q, 0, 0, 4, 4);
		if (EQ_ZERO(det(A), 1.0e-9))
		{
			printf("Singular value = %1.10lf\n", det(A));
			x = (vt0 + vt1) * 0.5;
			x[3] = vt0[3];
			x[4] = vt0[4];
		}
		else
			x = inv(A) * (-b);
		
		// ��ǥ ���� ���� ������ ���� �߰��Ѵ�.
		normalize_uv(x[3], x[4]);
		e->m_Cost = x * b + Q[5][5];
		EgCollapseInfoT info(e, cast_pt3(x), GVector2(x[3], x[4]));
		m_Heap.push(info);
	}
}
