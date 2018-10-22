#pragma once

#include <vector>
#include <map>
#include <queue>
#include <functional>
#include "egmath.h"
#include "egmesh.h"

class EgCollapseInfo2
{
	// 프렌드 함수
	friend bool operator >(const EgCollapseInfo2 &lhs, const EgCollapseInfo2 &rhs);

public:
	/*! \brief 제거할 에지에 대한 포인터 */
	EgEdge * m_pEdge;

	/*! \brief 에지 제거 비용 */
	double m_Cost;

	/*! \brief 에지 제거 후, 두 끝점의 위치 */
	GPoint3 m_Pos;

	/*! \brief 에지 제거 후, 두 끝점의 텍스쳐 좌표 */
	GVector2 m_Coord;

public:
	// 생성자 및 소멸자
	EgCollapseInfo2(EgEdge *pEdge, GPoint3 Pos, GVector2 Coord, double Cost);
	EgCollapseInfo2(const EgCollapseInfo2 &cpy);
	~EgCollapseInfo2();

	// 대입 연산자
	EgCollapseInfo2 &operator =(const EgCollapseInfo2 &rhs);
};

class EgDecimator2
{
public:
	/*! \brief 단순화할 대상 메쉬에 대한 포인터 */
	EgMesh * m_pMesh;

	/*! \brief 대상 메쉬의 각 정점에서 정의된 QEM 행렬 */
	std::vector<GMatrix> m_QEM;

	/*! \brief 에지의 제거 비용을 저장하는 변수 */
	std::map<EgEdge *, double> m_EdgeCostMap;

	/*! \brief 에지 제거 비용에 따라 에지 제거 정보를 저장하는 최소힙 */
	std::priority_queue<EgCollapseInfo2, std::vector<EgCollapseInfo2>, std::greater<EgCollapseInfo2>> m_Heap;

public:
	// 생성자 및 소멸자
	EgDecimator2(EgMesh *pMesh);
	~EgDecimator2();

	// 멤버 함수
	void InitVertQEM();
	void InitCollapseInfo();
	GMatrix Part5(GMatrix &K);
	void Run(double Ratio);
	int CollapseEdge(EgCollapseInfo2 &cost);
	void UpdateCollapseInfo(EgVertex *v);
};
