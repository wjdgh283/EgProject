#include "EgDecimator2.h"

/*!
*	\brief ������
*/
EgCollapseInfo2::EgCollapseInfo2(EgEdge *pEdge, GPoint3 Pos, GVector2 Coord, double Cost)
{
	m_pEdge = pEdge;
	m_Pos = Pos;
	m_Coord = Coord;
	m_Cost = Cost;
}

/*!
*	\brief ���� ������
*
*	\param cpy ����� ��ü
*/
EgCollapseInfo2::EgCollapseInfo2(const EgCollapseInfo2 &cpy)
{
	m_pEdge = cpy.m_pEdge;
	m_Pos = cpy.m_Pos;
	m_Coord = cpy.m_Coord;
	m_Cost = cpy.m_Cost;
}

/*!
*	\brief �Ҹ���
*/
EgCollapseInfo2::~EgCollapseInfo2()
{
}

/*!
*	\brief ���� ������
*
*	\param rhs ���Ե� ��ü
*/
EgCollapseInfo2 &EgCollapseInfo2::operator =(const EgCollapseInfo2 &rhs)
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
bool operator >(const EgCollapseInfo2 &lhs, const EgCollapseInfo2 &rhs)
{
	return (lhs.m_Cost > rhs.m_Cost);
}


/*!
*	\brief ������
*
*	\param pMesh �ܼ�ȭ�� ��� �޽��� ���� ������
*/
EgDecimator2::EgDecimator2(EgMesh *pMesh)
{
	// ��� �޽��� �����ϰ�, �޽��� ������ �ε����� �����Ѵ�.
	m_pMesh = pMesh;
	m_pMesh->UpdateVertIdx();
}

/*!
*	\brief �Ҹ���
*/
EgDecimator2::~EgDecimator2()
{
}

/*!
*	\brief �޽��� �� �������� ���ǵǴ� QEM ����� ������ �ʱ�ȭ �Ѵ�.
*/
void EgDecimator2::InitVertQEM()
{
	// ������ ������ŭ 6x6 QEM ����� �Ҵ��Ѵ�.
	m_QEM.assign(m_pMesh->GetNumVert(), GMatrix(6, 6));

	// ������ �ﰢ���� ���Ͽ�
	EgFace *f = m_pMesh->m_pFaces;
	while (f != NULL)
	{

		GVector3 n = f->GetFaceNormal();
		//�� �� p, q, r ����
		GVector p = GVector(5, f->GetVertex(0)->m_Pos.V[0], f->GetVertex(0)->m_Pos.V[1], f->GetVertex(0)->m_Pos.V[2], f->GetEdge(0)->m_pTexel->m_Coord.V[0], f->GetEdge(0)->m_pTexel->m_Coord.V[1]);
		GVector q = GVector(5, f->GetVertex(1)->m_Pos.V[0], f->GetVertex(1)->m_Pos.V[1], f->GetVertex(1)->m_Pos.V[2], f->GetEdge(1)->m_pTexel->m_Coord.V[0], f->GetEdge(1)->m_pTexel->m_Coord.V[1]);
		GVector r = GVector(5, f->GetVertex(2)->m_Pos.V[0], f->GetVertex(2)->m_Pos.V[1], f->GetVertex(2)->m_Pos.V[2], f->GetEdge(2)->m_pTexel->m_Coord.V[0], f->GetEdge(2)->m_pTexel->m_Coord.V[1]);

		//5���� ���� e1, e2 ����
		GVector e1 = (q - p).Normalize();
		GVector e2 = (r - p - ((e1*(r - p))*e1)).Normalize();


		//5x5 A ��� ����
		GMatrix A = GMatrix(5, 5);
		A[0][0] = 1.0f - e1.V[0] * e1.V[0] - e2.V[0] * e2.V[0];
		A[0][1] = -e1.V[0] * e1.V[1] - e2.V[0] * e2.V[1];
		A[0][2] = -e1.V[0] * e1.V[2] - e2.V[0] * e2.V[2];
		A[0][3] = -e1.V[0] * e1.V[3] - e2.V[0] * e2.V[3];
		A[0][4] = -e1.V[0] * e1.V[4] - e2.V[0] * e2.V[4];

		A[1][0] = -e1.V[1] * e1.V[0] - e2.V[1] * e2.V[0];
		A[1][1] = 1.0f - e1.V[1] * e1.V[1] - e2.V[1] * e2.V[1];
		A[1][2] = -e1.V[1] * e1.V[2] - e2.V[1] * e2.V[2];
		A[1][3] = -e1.V[1] * e1.V[3] - e2.V[1] * e2.V[3];
		A[1][4] = -e1.V[1] * e1.V[4] - e2.V[1] * e2.V[4];

		A[2][0] = -e1.V[2] * e1.V[0] - e2.V[2] * e2.V[0];
		A[2][1] = -e1.V[2] * e1.V[1] - e2.V[2] * e2.V[1];
		A[2][2] = 1.0f - e1.V[2] * e1.V[2] - e2.V[2] * e2.V[2];
		A[2][3] = -e1.V[2] * e1.V[3] - e2.V[2] * e2.V[3];
		A[2][4] = -e1.V[2] * e1.V[4] - e2.V[2] * e2.V[4];

		A[3][0] = -e1.V[3] * e1.V[0] - e2.V[3] * e2.V[0];
		A[3][1] = -e1.V[3] * e1.V[1] - e2.V[3] * e2.V[1];
		A[3][2] = -e1.V[3] * e1.V[2] - e2.V[3] * e2.V[2];
		A[3][3] = 1.0f - e1.V[3] * e1.V[3] - e2.V[3] * e2.V[3];
		A[3][4] = -e1.V[3] * e1.V[4] - e2.V[3] * e2.V[4];

		A[4][0] = -e1.V[4] * e1.V[0] - e2.V[4] * e2.V[0];
		A[4][1] = -e1.V[4] * e1.V[1] - e2.V[4] * e2.V[1];
		A[4][2] = -e1.V[4] * e1.V[2] - e2.V[4] * e2.V[2];
		A[4][3] = -e1.V[4] * e1.V[3] - e2.V[4] * e2.V[3];
		A[4][4] = 1.0f - e1.V[4] * e1.V[4] - e2.V[4] * e2.V[4];

		//5���� ���� b ����
		GVector b = (p*e1)*e1 + (p*e2)*e2 - p;

		//��� c ����
		double c = p * p - (p*e1)*(p*e1) - (p*e2)*(p*e2);

		// A,b,c�� �����Ǵ� Q ����� ���ϰ�
		GMatrix Q = GMatrix(6, 6);
		Q[0][0] = A[0][0];
		Q[0][1] = A[0][1];
		Q[0][2] = A[0][2];
		Q[0][3] = A[0][3];
		Q[0][4] = A[0][4];
		Q[0][5] = b[0];

		Q[1][0] = A[1][0];
		Q[1][1] = A[1][1];
		Q[1][2] = A[1][2];
		Q[1][3] = A[1][3];
		Q[1][4] = A[1][4];
		Q[1][5] = b[1];

		Q[2][0] = A[2][0];
		Q[2][1] = A[2][1];
		Q[2][2] = A[2][2];
		Q[2][3] = A[2][3];
		Q[2][4] = A[2][4];
		Q[2][5] = b[2];

		Q[3][0] = A[3][0];
		Q[3][1] = A[3][1];
		Q[3][2] = A[3][2];
		Q[3][3] = A[3][3];
		Q[3][4] = A[3][4];
		Q[3][5] = b[3];

		Q[4][0] = A[4][0];
		Q[4][1] = A[4][1];
		Q[4][2] = A[4][2];
		Q[4][3] = A[4][3];
		Q[4][4] = A[4][4];
		Q[4][5] = b[4];

		Q[5][0] = b[0];
		Q[5][1] = b[1];
		Q[5][2] = b[2];
		Q[5][3] = b[3];
		Q[5][4] = b[4];
		Q[5][5] = c;

		// �ﰢ���� �� ������ QEM�� �����Ѵ�.
		m_QEM[f->GetVertex(0)->m_Idx] += Q;
		m_QEM[f->GetVertex(1)->m_Idx] += Q;
		m_QEM[f->GetVertex(2)->m_Idx] += Q;

		// ���� �ﰢ���� ó���Ѵ�.
		f = f->m_pNext;
	}
}

/*!
*	\brief �޽��� ������ ������ �����ϴ� ����� �ʱ�ȭ �Ѵ�.
*	\note ���� (vi, vk)�� (vk, vi)�� ������ ������ �Ǵ��Ͽ� ��ǥ ������ ���ؼ��� ����� ���Ѵ�.
*/
void EgDecimator2::InitCollapseInfo()
{
	// ������ ������ ���Ͽ�
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		// �������� �����ϴ� ������ ������ ���Ͽ�
		for (EgEdge *e : v->m_pEdges)
		{
			// ��ǥ �������
			if (e->m_pMate == NULL || m_EdgeCostMap[e->m_pMate] == 0.0)
			{
				// ������ �糡���� ���Ѵ�.
				EgVertex *v0 = v;
				EgVertex *v1 = e->m_pNext->m_pVert;

				// ������ �糡��(��ġ+texture coord)�� ���Ͽ�,
				GVector vt0(5, v->m_Pos.V[0], v->m_Pos.V[1], v->m_Pos.V[2], e->m_pTexel->m_Coord.V[0], e->m_pTexel->m_Coord.V[1]);
				GVector vt1(5, e->m_pNext->m_pVert->m_Pos.V[0], e->m_pNext->m_pVert->m_Pos.V[1], e->m_pNext->m_pVert->m_Pos.V[2],
					e->m_pNext->m_pTexel->m_Coord.V[0], e->m_pNext->m_pTexel->m_Coord.V[1]);
				
				// ���ο� ������ 6x6 QEM�� ���Ѵ�.
				GMatrix K = m_QEM[v0->m_Idx] + m_QEM[v1->m_Idx];

				// ���ο� ������ 5���� ���� (��ġ + texture coord)�� ���Ѵ�.
				GVector x(5), b(5);
				b.Set(-K[0][5], -K[1][5], -K[2][5], -K[3][5], -K[4][5]);
				
				GMatrix A = Part5(K);
				if (EQ_ZERO(det(A), 1.0e-7))
				{
					x = (vt0 + vt1) / 2.0f;
				}
				else
				{
					x = inv(A) * b;
				}

				// ��ǥ ���� ���� ������ ���� �߰��Ѵ�.
				GVector y(6, x[0], x[1], x[2], x[3], x[4], 1.0);
				EgCollapseInfo2 c(e, cast_pt3(x), GVector2(x.V[3], x.V[4]), y * (K * y));
				m_Heap.push(c);

				// ��ǥ ������ �ֽ� ����� ����Ѵ�.
				m_EdgeCostMap[e] = c.m_Cost;
			}
		}
		v = v->m_pNext;
	}
}


/*!
*	\brief 6�� 6���� ��Ī ��� K�� 5x5 �κ���� A�� ��ȯ�Ѵ�.
*
*	\param K ��� ���
*
*	\return 5x5 �κ���� A�� ��ȯ�Ѵ�.
*/
GMatrix EgDecimator2::Part5(GMatrix &K)
{
	//6x6 ��Ī ��� K�� 5x5 �κ���� A�� ���Ѵ�
	GMatrix A(5, 5);
	A[0][0] = K[0][0];
	A[0][1] = K[0][1];
	A[0][2] = K[0][2];
	A[0][3] = K[0][3];
	A[0][4] = K[0][4];

	A[1][1] = K[1][1];
	A[1][2] = K[1][2];
	A[1][3] = K[1][3];
	A[1][4] = K[1][4];

	A[2][2] = K[2][2];
	A[2][3] = K[2][3];
	A[2][4] = K[2][4];

	A[3][3] = K[3][3];
	A[3][4] = K[3][4];

	A[4][4] = K[4][4];

	A[1][0] = K[0][1];	//��Ī��
	A[2][0] = K[0][2];	//��Ī��
	A[3][0] = K[0][3];	//��Ī��
	A[4][0] = K[0][4];	//��Ī��

	A[2][1] = K[1][2];	//��Ī��
	A[3][1] = K[1][3];	//��Ī��
	A[4][1] = K[1][4];	//��Ī��

	A[3][2] = K[2][3];	//��Ī��
	A[4][2] = K[2][4];	//��Ī��

	A[4][3] = K[3][4];	//��Ī��

	return A;
}

/*!
*	\brief �ݺ������� ���� ���Ÿ� �����Ѵ�.
*
*	\prarm Ratio ������ �ﰢ���� ����
*/
void EgDecimator2::Run(double Ratio)
{
	// �ʱ� �ﰢ���� ������ ���Ѵ�.
	int NumFace = m_pMesh->GetNumFace();
	int TarNumFace = (int)(NumFace * (1.0 - Ratio));

	printf("Size of heap = %zd\n", m_Heap.size());

	// �ݺ������� ������ �����Ͽ� �ﰢ���� ������ ���δ�.
	while (!m_Heap.empty())
	{
		// �ּ� ����� ���� ���� ������ �̾Ƽ�,
		EgCollapseInfo2 Info = m_Heap.top();
		m_Heap.pop();

		// ������ �ߺ��� ���� ������ �����ϰ�,
		while (!m_Heap.empty() && m_Heap.top().m_Cost == Info.m_Cost)
			m_Heap.pop();

		// �̹� ���ŵ� �����̰ų�, �ֽ� ����� ���� ������ �ƴ϶�� ��ŵ�Ѵ�. ??
		double cost = m_EdgeCostMap[Info.m_pEdge];
		if (cost == -1.0 || cost != Info.m_Cost)
			continue;

		// ���õ� ������ �����ϰ�, �ﰢ�� ���� ���δ�.
		NumFace -= CollapseEdge(Info);

		// �ﰢ���� ������ ���ϴ� ���� �����ߴٸ� �����Ѵ�.
		if (NumFace < TarNumFace)
			break;
	}
}

/*!
*	\brief �޽��� ���� ������ ������ �����Ѵ�.
*
*	\param pEdge ������ ������ ���� ������
*
*	\return �Բ� ���ŵ� �ﰢ���� ������ ��ȯ�Ѵ�(0: ���� �Ұ���, 1, 2)
*/
int EgDecimator2::CollapseEdge(EgCollapseInfo2 &Cost)
{
	// ���Ű� �Ұ����� ������� false�� ��ȯ�Ѵ�.
	if (!Cost.m_pEdge->IsCollapsible())
		return 0;

	int RemovedFaces = 1;
	EgEdge *pEdge = Cost.m_pEdge;
	// ������ ������(v1)�� ����(v2)�� ���� ���ο� ��ġ�� ���Ѵ�.
	EgVertex *v1 = pEdge->m_pVert;
	EgVertex *v2 = pEdge->m_pNext->m_pVert;
	v1->m_Pos = Cost.m_Pos;
	pEdge->m_pTexel->m_Coord = Cost.m_Coord;

	// �ﰢ���� �������� ���ŵǾ��ٴ� ���� ����Ѵ�.
	m_EdgeCostMap[pEdge] = -1.0;
	m_EdgeCostMap[pEdge->m_pNext] = -1.0;
	m_EdgeCostMap[pEdge->m_pNext->m_pNext] = -1.0;

	// ������ ������ �ﰢ���� �����Ѵ�(������ ���´�).
	std::vector<EgFace *> Faces;
	Faces.push_back(pEdge->m_pFace);
	if (pEdge->m_pMate != NULL)
	{
		RemovedFaces++;
		EgEdge *pMate = pEdge->m_pMate;
		Faces.push_back(pMate->m_pFace);

		m_EdgeCostMap[pMate] = -1.0;
		m_EdgeCostMap[pMate->m_pNext] = -1.0;
		m_EdgeCostMap[pMate->m_pNext->m_pNext] = -1.0;
	}
	m_pMesh->DelFaces(Faces);

	// v2���� �����ϴ� ��� �������� �������� v1���� �����Ѵ�.
	for (EgEdge *&e : v2->m_pEdges)
	{
		e->m_pVert = v1;
		e->m_pTexel->m_Coord = Cost.m_Coord;
		v1->m_pEdges.push_back(e);
	}

	// v2���� �����ϴ� ���� ������ �����ϰ�,
	v2->m_pEdges.clear();

	// v1���� �����ϴ� ������ mate ������ ������ �����Ѵ�.
	m_pMesh->UpdateEdgeMate(v1);

	// v1�� QEM ����� �����Ѵ�.
	m_QEM[v1->m_Idx] += m_QEM[v2->m_Idx];

	// v1�� ������ �ﰢ���� ��� ���� ����� �����Ѵ�.
	UpdateCollapseInfo(v1);

	return RemovedFaces;
}

/*!
*	\brief ������ ������ ������ ���� ����� �����Ѵ�.
*
*	\param v ������ ���� ������
*/
void EgDecimator2::UpdateCollapseInfo(EgVertex *v)
{
	// ���� v���� �����ϴ� ������ ������ ���Ͽ�
	for (EgEdge *e : v->m_pEdges)
	{
		// ��ǥ ���� e0�� ���Ѵ�.
		EgEdge *e0 = e;
		if (e->m_pMate != NULL && m_EdgeCostMap[e->m_pMate] != 0.0)
			e0 = e->m_pMate;

		// ��ǥ ������ �糡���� ���ϰ�,
		EgVertex *v0 = e0->m_pVert;
		EgVertex *v1 = e0->m_pNext->m_pVert;

		// ������ �糡��(��ġ+texture coord)�� ���Ͽ�,
		GVector vt0(5, v->m_Pos.V[0], v->m_Pos.V[1], v->m_Pos.V[2], e0->m_pTexel->m_Coord.V[0], e0->m_pTexel->m_Coord.V[1]);
		GVector vt1(5, e0->m_pNext->m_pVert->m_Pos.V[0], e0->m_pNext->m_pVert->m_Pos.V[1], e0->m_pNext->m_pVert->m_Pos.V[2],
			e0->m_pNext->m_pTexel->m_Coord.V[0], e0->m_pNext->m_pTexel->m_Coord.V[1]);

		// ���ο� ������ 6x6 QEM�� ���Ѵ�.
		GMatrix K = m_QEM[v0->m_Idx] + m_QEM[v1->m_Idx];

		// ���ο� ������ 5���� ���� (��ġ + texture coord)�� ���Ѵ�.
		GVector x(5), b(5);
		b.Set(-K[0][5], -K[1][5], -K[2][5], -K[3][5], -K[4][5]);

		GMatrix A = Part5(K);
		if (EQ_ZERO(det(A), 1.0e-7))
		{
			x = (vt0 + vt1) / 2.0f;
		}
		else
		{
			x = inv(A) * b;
		}

		// ��ǥ ���� ���� ������ ���� �߰��Ѵ�.
		GVector y(6, x[0], x[1], x[2], x[3], x[4], 1.0);
		EgCollapseInfo2 c(e, cast_pt3(x), GVector2(x.V[3], x.V[4]), y * (K * y));
		m_Heap.push(c);

		// ��ǥ ������ �ֽ� ����� ����Ѵ�.
		m_EdgeCostMap[e0] = MAX(m_EdgeCostMap[e0], c.m_Cost);
	}
}
