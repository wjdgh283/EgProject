#include "EgDecimator.h"


/*!
*	\brief ������
*/
EgCollapseInfo::EgCollapseInfo(EgEdge *pEdge, GPoint3 Pos, double Cost)
{
	m_pEdge = pEdge;
	m_Pos = Pos;
	m_Cost = Cost;
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
	// ��� �޽��� �����ϰ�, �޽��� ������ �ε����� �����Ѵ�.
	m_pMesh = pMesh;
	m_pMesh->UpdateVertIdx();
}

/*!
*	\brief �Ҹ���
*/
EgDecimator::~EgDecimator()
{
}

/*!
*	\brief �޽��� �� �������� ���ǵǴ� QEM ����� ������ �ʱ�ȭ �Ѵ�.
*/
void EgDecimator::InitVertQEM()
{
	// ������ ������ŭ 4x4 QEM ����� �Ҵ��Ѵ�.
	m_QEM.assign(m_pMesh->GetNumVert(), GMatrix());

	// ������ �ﰢ���� ���Ͽ�
	EgFace *f = m_pMesh->m_pFaces;
	while (f != NULL)
	{
		// �ﰢ���� ������ ����� �������� ���Ͽ�
		GVector3 n = f->GetFaceNormal();
		GPoint3 p = f->GetVertex(0)->m_Pos;
		double d = -n * cast_vec3(p);

		// QEM ����� ���ϰ�
		GMatrix Q;
		Q[0][0] = n[0] * n[0];
		Q[0][1] = n[0] * n[1];
		Q[0][2] = n[0] * n[2];
		Q[0][3] = n[0] * d;

		Q[1][0] = Q[0][1];	// ��Ī��
		Q[1][1] = n[1] * n[1];
		Q[1][2] = n[1] * n[2];
		Q[1][3] = n[1] * d;

		Q[2][0] = Q[0][2];	// ��Ī��
		Q[2][1] = Q[1][2];	// ��Ī��
		Q[2][2] = n[2] * n[2];
		Q[2][3] = n[2] * d;

		Q[3][0] = Q[0][3];	// ��Ī��
		Q[3][1] = Q[1][3];	// ��Ī��
		Q[3][2] = Q[2][3];	// ��Ī��
		Q[3][3] = d * d;

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
void EgDecimator::InitCollapseInfo()
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
				// ������ �糡���� ���Ͽ�,
				EgVertex *v0 = v;
				EgVertex *v1 = e->m_pNext->m_pVert;

				// ���ο� ������ QEM�� ���Ѵ�.
				GMatrix K = m_QEM[v->m_Idx] + m_QEM[v1->m_Idx];

				// ���ο� ������ ��ġ�� ���Ѵ�.
				GVector x(3), b(3);
				b.Set(-K[0][3], -K[1][3], -K[2][3]);
				double det =
					(K[0][0] * K[1][1] * K[2][2] + 2 * K[0][1] * K[1][2] * K[0][2]) -
					(K[1][2] * K[1][2] * K[0][0] + K[0][2] * K[0][2] * K[1][1] + K[0][1] * K[0][1] * K[2][2]);

				if (EQ_ZERO(det, 1.0e-7))
				{
					x = cast_vec(v0->m_Pos + (v1->m_Pos - v0->m_Pos) * 0.5);

				}
				else
				{
					x = Inv3(K, det) * b;
				}

				// ��ǥ ���� ���� ������ ���� �߰��Ѵ�.
				GVector y(4, x[0], x[1], x[2], 1.0);
				EgCollapseInfo c(e, cast_pt3(x), y * (K * y));
				m_Heap.push(c);

				// ��ǥ ������ �ֽ� ����� ����Ѵ�.
				m_EdgeCostMap[e] = c.m_Cost;
			}
		}
		v = v->m_pNext;
	}
}

/*!
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
*	\brief �ݺ������� ���� ���Ÿ� �����Ѵ�.
*
*	\prarm Ratio ������ �ﰢ���� ����
*/
void EgDecimator::Run(double Ratio)
{
	// �ʱ� �ﰢ���� ������ ���Ѵ�.
	int NumFace = m_pMesh->GetNumFace();
	int TarNumFace = (int)(NumFace * (1.0 - Ratio));

	printf("Size of heap = %zd\n", m_Heap.size());

	// �ݺ������� ������ �����Ͽ� �ﰢ���� ������ ���δ�.
	while (!m_Heap.empty())
	{
		// �ּ� ����� ���� ���� ������ �̾Ƽ�,
		EgCollapseInfo Info = m_Heap.top();
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
int EgDecimator::CollapseEdge(EgCollapseInfo &Cost)
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
void EgDecimator::UpdateCollapseInfo(EgVertex *v)
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

		// ���ο����� QEM ����� ���Ͽ�,
		GMatrix K = m_QEM[v0->m_Idx] + m_QEM[v1->m_Idx];
		GVector x(3), b(3);		
		b[0] = -K[0][3];	
		b[1] = -K[1][3];	
		b[2] = -K[2][3];

		// ���ο� ��ġ�� ���ϰ�,
		double det =
			(K[0][0] * K[1][1] * K[2][2] + 2 * K[0][1] * K[1][2] * K[0][2]) -
			(K[1][2] * K[1][2] * K[0][0] + K[0][2] * K[0][2] * K[1][1] + K[0][1] * K[0][1] * K[2][2]);
		if (EQ_ZERO(det, 1.0e-7))
			x = cast_vec(v0->m_Pos + (v1->m_Pos - v0->m_Pos) * 0.5);
		else
			x = Inv3(K, det) * b;

		// ��ǥ ���� ���� ������ ���� �߰��Ѵ�.
		GVector y(4, x[0], x[1], x[2], 1.0);
		EgCollapseInfo c(e0, cast_pt3(x), y * (K * y));
		m_Heap.push(c);

		// ��ǥ ������ �ֽ� ����� ����Ѵ�.
		m_EdgeCostMap[e0] = MAX(m_EdgeCostMap[e0], c.m_Cost);
	}
}




