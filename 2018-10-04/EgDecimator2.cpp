#include "EgDecimator2.h"

/*!
*	\brief 생성자
*/
EgCollapseInfo2::EgCollapseInfo2(EgEdge *pEdge, GPoint3 Pos, GVector2 Coord, double Cost)
{
	m_pEdge = pEdge;
	m_Pos = Pos;
	m_Coord = Coord;
	m_Cost = Cost;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
EgCollapseInfo2::EgCollapseInfo2(const EgCollapseInfo2 &cpy)
{
	m_pEdge = cpy.m_pEdge;
	m_Pos = cpy.m_Pos;
	m_Coord = cpy.m_Coord;
	m_Cost = cpy.m_Cost;
}

/*!
*	\brief 소멸자
*/
EgCollapseInfo2::~EgCollapseInfo2()
{
}

/*!
*	\brief 대입 연산자
*
*	\param rhs 대입될 객체
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
*	\brief 비교(>) 연산자
*
*	\param lhs 왼쪽 객체
*	\param rhs 오른쪽 객체
*/
bool operator >(const EgCollapseInfo2 &lhs, const EgCollapseInfo2 &rhs)
{
	return (lhs.m_Cost > rhs.m_Cost);
}


/*!
*	\brief 생성자
*
*	\param pMesh 단순화할 대상 메쉬에 대한 포인터
*/
EgDecimator2::EgDecimator2(EgMesh *pMesh)
{
	// 대상 메쉬를 설정하고, 메쉬의 정점의 인덱스를 갱신한다.
	m_pMesh = pMesh;
	m_pMesh->UpdateVertIdx();
}

/*!
*	\brief 소멸자
*/
EgDecimator2::~EgDecimator2()
{
}

/*!
*	\brief 메쉬의 각 정점에서 정의되는 QEM 행렬의 정보를 초기화 한다.
*/
void EgDecimator2::InitVertQEM()
{
	// 정점의 개수만큼 6x6 QEM 행렬을 할당한다.
	m_QEM.assign(m_pMesh->GetNumVert(), GMatrix(6, 6));

	// 각각의 삼각형에 대하여
	EgFace *f = m_pMesh->m_pFaces;
	while (f != NULL)
	{

		GVector3 n = f->GetFaceNormal();
		//세 점 p, q, r 정의
		GVector p = GVector(5, f->GetVertex(0)->m_Pos.V[0], f->GetVertex(0)->m_Pos.V[1], f->GetVertex(0)->m_Pos.V[2], f->GetEdge(0)->m_pTexel->m_Coord.V[0], f->GetEdge(0)->m_pTexel->m_Coord.V[1]);
		GVector q = GVector(5, f->GetVertex(1)->m_Pos.V[0], f->GetVertex(1)->m_Pos.V[1], f->GetVertex(1)->m_Pos.V[2], f->GetEdge(1)->m_pTexel->m_Coord.V[0], f->GetEdge(1)->m_pTexel->m_Coord.V[1]);
		GVector r = GVector(5, f->GetVertex(2)->m_Pos.V[0], f->GetVertex(2)->m_Pos.V[1], f->GetVertex(2)->m_Pos.V[2], f->GetEdge(2)->m_pTexel->m_Coord.V[0], f->GetEdge(2)->m_pTexel->m_Coord.V[1]);

		//5차원 벡터 e1, e2 정의
		GVector e1 = (q - p).Normalize();
		GVector e2 = (r - p - ((e1*(r - p))*e1)).Normalize();


		//5x5 A 행렬 정의
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

		//5차원 벡터 b 정의
		GVector b = (p*e1)*e1 + (p*e2)*e2 - p;

		//상수 c 정의
		double c = p * p - (p*e1)*(p*e1) - (p*e2)*(p*e2);

		// A,b,c로 구성되는 Q 행렬을 구하고
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
void EgDecimator2::InitCollapseInfo()
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
				// 에지의 양끝점을 구한다.
				EgVertex *v0 = v;
				EgVertex *v1 = e->m_pNext->m_pVert;

				// 에지의 양끝점(위치+texture coord)을 구하여,
				GVector vt0(5, v->m_Pos.V[0], v->m_Pos.V[1], v->m_Pos.V[2], e->m_pTexel->m_Coord.V[0], e->m_pTexel->m_Coord.V[1]);
				GVector vt1(5, e->m_pNext->m_pVert->m_Pos.V[0], e->m_pNext->m_pVert->m_Pos.V[1], e->m_pNext->m_pVert->m_Pos.V[2],
					e->m_pNext->m_pTexel->m_Coord.V[0], e->m_pNext->m_pTexel->m_Coord.V[1]);
				
				// 새로운 정점의 6x6 QEM을 구한다.
				GMatrix K = m_QEM[v0->m_Idx] + m_QEM[v1->m_Idx];

				// 새로운 정점의 5차원 벡터 (위치 + texture coord)를 구한다.
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

				// 대표 에지 제거 정보를 힙에 추가한다.
				GVector y(6, x[0], x[1], x[2], x[3], x[4], 1.0);
				EgCollapseInfo2 c(e, cast_pt3(x), GVector2(x.V[3], x.V[4]), y * (K * y));
				m_Heap.push(c);

				// 대표 에지의 최신 비용을 기록한다.
				m_EdgeCostMap[e] = c.m_Cost;
			}
		}
		v = v->m_pNext;
	}
}


/*!
*	\brief 6행 6열의 대칭 행렬 K의 5x5 부분행렬 A를 반환한다.
*
*	\param K 대상 행렬
*
*	\return 5x5 부분행렬 A를 반환한다.
*/
GMatrix EgDecimator2::Part5(GMatrix &K)
{
	//6x6 대칭 행렬 K의 5x5 부분행렬 A를 구한다
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

	A[1][0] = K[0][1];	//대칭성
	A[2][0] = K[0][2];	//대칭성
	A[3][0] = K[0][3];	//대칭성
	A[4][0] = K[0][4];	//대칭성

	A[2][1] = K[1][2];	//대칭성
	A[3][1] = K[1][3];	//대칭성
	A[4][1] = K[1][4];	//대칭성

	A[3][2] = K[2][3];	//대칭성
	A[4][2] = K[2][4];	//대칭성

	A[4][3] = K[3][4];	//대칭성

	return A;
}

/*!
*	\brief 반복적으로 에지 제거를 수행한다.
*
*	\prarm Ratio 제거할 삼각형의 비율
*/
void EgDecimator2::Run(double Ratio)
{
	// 초기 삼각형의 개수를 구한다.
	int NumFace = m_pMesh->GetNumFace();
	int TarNumFace = (int)(NumFace * (1.0 - Ratio));

	printf("Size of heap = %zd\n", m_Heap.size());

	// 반복적으로 에지를 제거하여 삼각형의 개수를 줄인다.
	while (!m_Heap.empty())
	{
		// 최소 비용을 갖는 제거 정보를 뽑아서,
		EgCollapseInfo2 Info = m_Heap.top();
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
int EgDecimator2::CollapseEdge(EgCollapseInfo2 &Cost)
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
	pEdge->m_pTexel->m_Coord = Cost.m_Coord;

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
		e->m_pTexel->m_Coord = Cost.m_Coord;
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
void EgDecimator2::UpdateCollapseInfo(EgVertex *v)
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

		// 에지의 양끝점(위치+texture coord)을 구하여,
		GVector vt0(5, v->m_Pos.V[0], v->m_Pos.V[1], v->m_Pos.V[2], e0->m_pTexel->m_Coord.V[0], e0->m_pTexel->m_Coord.V[1]);
		GVector vt1(5, e0->m_pNext->m_pVert->m_Pos.V[0], e0->m_pNext->m_pVert->m_Pos.V[1], e0->m_pNext->m_pVert->m_Pos.V[2],
			e0->m_pNext->m_pTexel->m_Coord.V[0], e0->m_pNext->m_pTexel->m_Coord.V[1]);

		// 새로운 정점의 6x6 QEM을 구한다.
		GMatrix K = m_QEM[v0->m_Idx] + m_QEM[v1->m_Idx];

		// 새로운 정점의 5차원 벡터 (위치 + texture coord)를 구한다.
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

		// 대표 에지 제거 정보를 힙에 추가한다.
		GVector y(6, x[0], x[1], x[2], x[3], x[4], 1.0);
		EgCollapseInfo2 c(e, cast_pt3(x), GVector2(x.V[3], x.V[4]), y * (K * y));
		m_Heap.push(c);

		// 대표 에지의 최신 비용을 기록한다.
		m_EdgeCostMap[e0] = MAX(m_EdgeCostMap[e0], c.m_Cost);
	}
}
