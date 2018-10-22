#include "EgDecimator.h"


/*!
*	\brief 생성자
*/
EgCollapseInfo::EgCollapseInfo(EgEdge *pEdge, GPoint3 Pos, double Cost)
{
	m_pEdge = pEdge;
	m_Pos = Pos;
	m_Cost = Cost;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
EgCollapseInfo::EgCollapseInfo(const EgCollapseInfo &cpy)
{
	m_pEdge = cpy.m_pEdge;
	m_Pos = cpy.m_Pos;
	m_Cost = cpy.m_Cost;
}

/*!
*	\brief 소멸자
*/
EgCollapseInfo::~EgCollapseInfo()
{
}

/*!
*	\brief 대입 연산자
*
*	\param rhs 대입될 객체
*/
EgCollapseInfo &EgCollapseInfo::operator =(const EgCollapseInfo &rhs)
{
	m_pEdge = rhs.m_pEdge;
	m_Pos = rhs.m_Pos;
	m_Cost = rhs.m_Cost;
	return *this;
}

/*!
*	\brief 비교(>) 연산자
*
*	\param lhs 왼쪽 객체
*	\param rhs 오른쪽 객체
*/
bool operator >(const EgCollapseInfo &lhs, const EgCollapseInfo &rhs)
{
	return (lhs.m_Cost > rhs.m_Cost);
}




/*!
*	\brief 생성자
*
*	\param pMesh 단순화할 대상 메쉬에 대한 포인터
*/
EgDecimator::EgDecimator(EgMesh *pMesh)
{
	// 대상 메쉬를 설정하고, 메쉬의 정점의 인덱스를 갱신한다.
	m_pMesh = pMesh;
	m_pMesh->UpdateVertIdx();
}

/*!
*	\brief 소멸자
*/
EgDecimator::~EgDecimator()
{
}

/*!
*	\brief 메쉬의 각 정점에서 정의되는 QEM 행렬의 정보를 초기화 한다.
*/
void EgDecimator::InitVertQEM()
{
	// 정점의 개수만큼 4x4 QEM 행렬을 할당한다.
	m_QEM.assign(m_pMesh->GetNumVert(), GMatrix());

	// 각각의 삼각형에 대하여
	EgFace *f = m_pMesh->m_pFaces;
	while (f != NULL)
	{
		// 삼각형을 포함한 평면의 방정식을 구하여
		GVector3 n = f->GetFaceNormal();
		GPoint3 p = f->GetVertex(0)->m_Pos;
		double d = -n * cast_vec3(p);

		// QEM 행렬을 구하고
		GMatrix Q;
		Q[0][0] = n[0] * n[0];
		Q[0][1] = n[0] * n[1];
		Q[0][2] = n[0] * n[2];
		Q[0][3] = n[0] * d;

		Q[1][0] = Q[0][1];	// 대칭성
		Q[1][1] = n[1] * n[1];
		Q[1][2] = n[1] * n[2];
		Q[1][3] = n[1] * d;

		Q[2][0] = Q[0][2];	// 대칭성
		Q[2][1] = Q[1][2];	// 대칭성
		Q[2][2] = n[2] * n[2];
		Q[2][3] = n[2] * d;

		Q[3][0] = Q[0][3];	// 대칭성
		Q[3][1] = Q[1][3];	// 대칭성
		Q[3][2] = Q[2][3];	// 대칭성
		Q[3][3] = d * d;

		// 삼각형의 세 정점의 QEM에 누적한다.
		m_QEM[f->GetVertex(0)->m_Idx] += Q;
		m_QEM[f->GetVertex(1)->m_Idx] += Q;
		m_QEM[f->GetVertex(2)->m_Idx] += Q;

		// 다음 삼각형을 처리한다.
		f = f->m_pNext;
	}	
}

/*!
*	\brief 메쉬의 각각의 에지를 제거하는 비용을 초기화 한다.
*	\note 에지 (vi, vk)와 (vk, vi)는 동일한 에지로 판단하여 대표 에지에 대해서만 비용을 구한다.
*/
void EgDecimator::InitCollapseInfo()
{
	// 각각의 정점에 대하여
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		// 정점에서 시작하는 각각의 에지에 대하여
		for (EgEdge *e : v->m_pEdges)
		{
			// 대표 에지라면
			if (e->m_pMate == NULL || m_EdgeCostMap[e->m_pMate] == 0.0)
			{
				// 에지의 양끝점을 구하여,
				EgVertex *v0 = v;
				EgVertex *v1 = e->m_pNext->m_pVert;

				// 새로운 정점의 QEM을 구한다.
				GMatrix K = m_QEM[v->m_Idx] + m_QEM[v1->m_Idx];

				// 새로운 정점의 위치를 구한다.
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

				// 대표 에지 제거 정보를 힙에 추가한다.
				GVector y(4, x[0], x[1], x[2], 1.0);
				EgCollapseInfo c(e, cast_pt3(x), y * (K * y));
				m_Heap.push(c);

				// 대표 에지의 최신 비용을 기록한다.
				m_EdgeCostMap[e] = c.m_Cost;
			}
		}
		v = v->m_pNext;
	}
}

/*!
*	\brief 3행 3열의 대칭 행렬 A의 역행렬을 반환한다.
*
*	\param A 대상 행렬
*	\param det 행렬 A의 행렬식
*
*	\return 행렬 A의 역행렬을 반환한다.
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
*	\brief 반복적으로 에지 제거를 수행한다.
*
*	\prarm Ratio 제거할 삼각형의 비율
*/
void EgDecimator::Run(double Ratio)
{
	// 초기 삼각형의 개수를 구한다.
	int NumFace = m_pMesh->GetNumFace();
	int TarNumFace = (int)(NumFace * (1.0 - Ratio));

	printf("Size of heap = %zd\n", m_Heap.size());

	// 반복적으로 에지를 제거하여 삼각형의 개수를 줄인다.
	while (!m_Heap.empty())
	{
		// 최소 비용을 갖는 제거 정보를 뽑아서,
		EgCollapseInfo Info = m_Heap.top();
		m_Heap.pop();

		// 힙에서 중복된 제거 정보는 제거하고,
		while (!m_Heap.empty() && m_Heap.top().m_Cost == Info.m_Cost)
			m_Heap.pop();

		// 이미 제거된 에지이거나, 최신 비용을 갖는 에지가 아니라면 스킵한다. ??
		double cost = m_EdgeCostMap[Info.m_pEdge];
		if (cost == -1.0 || cost != Info.m_Cost)
			continue;

		// 선택된 에지를 제거하고, 삼각형 수를 줄인다.
		NumFace -= CollapseEdge(Info);

		// 삼각형의 개수가 원하는 수에 도달했다면 종료한다.
		if (NumFace < TarNumFace)
			break;
	}
}

/*!
*	\brief 메쉬의 제거 가능한 에지를 제거한다.
*
*	\param pEdge 제거할 에지에 대한 포인터
*
*	\return 함께 제거된 삼각형의 개수를 반환한다(0: 제거 불가능, 1, 2)
*/
int EgDecimator::CollapseEdge(EgCollapseInfo &Cost)
{
	// 제거가 불가능한 에지라면 false를 반환한다.
	if (!Cost.m_pEdge->IsCollapsible())
		return 0;

	int RemovedFaces = 1;
	EgEdge *pEdge = Cost.m_pEdge;
	// 에지의 시작점(v1)과 끝점(v2)을 구해 새로운 위치를 구한다.
	EgVertex *v1 = pEdge->m_pVert;
	EgVertex *v2 = pEdge->m_pNext->m_pVert;
	v1->m_Pos = Cost.m_Pos;

	// 삼각형의 에지들이 제거되었다는 것을 기록한다.
	m_EdgeCostMap[pEdge] = -1.0;
	m_EdgeCostMap[pEdge->m_pNext] = -1.0;
	m_EdgeCostMap[pEdge->m_pNext->m_pNext] = -1.0;

	// 에지를 공유한 삼각형을 제거한다(정점은 남는다).
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

	// v2에서 시작하는 모든 에지들의 시작점을 v1으로 변경한다.
	for (EgEdge *&e : v2->m_pEdges)
	{
		e->m_pVert = v1;
		v1->m_pEdges.push_back(e);
	}

	// v2에서 시작하는 에지 정보를 제거하고,
	v2->m_pEdges.clear();

	// v1에서 시작하는 에지의 mate 에지의 정보를 갱신한다.
	m_pMesh->UpdateEdgeMate(v1);

	// v1의 QEM 행렬을 갱신한다.
	m_QEM[v1->m_Idx] += m_QEM[v2->m_Idx];

	// v1을 공유한 삼각형의 모든 에지 비용을 갱신한다.
	UpdateCollapseInfo(v1);

	return RemovedFaces;
}

/*!
*	\brief 정점을 공유한 에지의 제거 비용을 갱신한다.
*
*	\param v 정점에 대한 포인터
*/
void EgDecimator::UpdateCollapseInfo(EgVertex *v)
{
	// 정점 v에서 시작하는 각각의 에지에 대하여
	for (EgEdge *e : v->m_pEdges)
	{
		// 대표 에지 e0를 구한다.
		EgEdge *e0 = e;
		if (e->m_pMate != NULL && m_EdgeCostMap[e->m_pMate] != 0.0)
			e0 = e->m_pMate;

		// 대표 에지의 양끝점을 구하고,
		EgVertex *v0 = e0->m_pVert;
		EgVertex *v1 = e0->m_pNext->m_pVert;

		// 새로운점의 QEM 행렬을 구하여,
		GMatrix K = m_QEM[v0->m_Idx] + m_QEM[v1->m_Idx];
		GVector x(3), b(3);		
		b[0] = -K[0][3];	
		b[1] = -K[1][3];	
		b[2] = -K[2][3];

		// 새로운 위치를 구하고,
		double det =
			(K[0][0] * K[1][1] * K[2][2] + 2 * K[0][1] * K[1][2] * K[0][2]) -
			(K[1][2] * K[1][2] * K[0][0] + K[0][2] * K[0][2] * K[1][1] + K[0][1] * K[0][1] * K[2][2]);
		if (EQ_ZERO(det, 1.0e-7))
			x = cast_vec(v0->m_Pos + (v1->m_Pos - v0->m_Pos) * 0.5);
		else
			x = Inv3(K, det) * b;

		// 대표 에지 제거 정보를 힙에 추가한다.
		GVector y(4, x[0], x[1], x[2], 1.0);
		EgCollapseInfo c(e0, cast_pt3(x), y * (K * y));
		m_Heap.push(c);

		// 대표 에지의 최신 비용을 기록한다.
		m_EdgeCostMap[e0] = MAX(m_EdgeCostMap[e0], c.m_Cost);
	}
}




