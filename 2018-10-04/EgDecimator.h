#pragma once

#include <vector>
#include <map>
#include <queue>
#include <functional>
#include "egmath.h"
#include "egmesh.h"

/*!
*	\class EgCollapseInfo
*	\brief 에지 제거에 대한 정보를 표현하는 클래스
*
*	\author 윤승현, 박정호
*	\date 31 July 2018
*/
class EgCollapseInfo
{
	// 프렌드 함수
	friend bool operator >(const EgCollapseInfo &lhs, const EgCollapseInfo &rhs);

public:
	/*! \brief 제거할 에지에 대한 포인터 */
	EgEdge *m_pEdge;

	/*! \brief 에지 제거 비용 */
	double m_Cost;

	/*! \brief 에지 제거 후, 두 끝점의 위치 */
	GPoint3 m_Pos;

public:
	// 생성자 및 소멸자
	EgCollapseInfo(EgEdge *pEdge, GPoint3 Pos, double Cost);
	EgCollapseInfo(const EgCollapseInfo &cpy);
	~EgCollapseInfo();

	// 대입 연산자
	EgCollapseInfo &operator =(const EgCollapseInfo &rhs);
};

/*!
*	\class EgDecimator
*	\brief 메쉬의 단순화를 위한 클래스
*
*	\author 윤승현, 박정호
*	\date 31 July 2018
*/
class EgDecimator
{
public:
	/*! \brief 단순화할 대상 메쉬에 대한 포인터 */
	EgMesh *m_pMesh;

	/*! \brief 대상 메쉬의 각 정점에서 정의된 QEM 행렬 */
	std::vector<GMatrix> m_QEM;

	/*! \brief 에지의 제거 비용을 저장하는 변수 */
	std::map<EgEdge *, double> m_EdgeCostMap;

	/*! \brief 에지 제거 비용에 따라 에지 제거 정보를 저장하는 최소힙 */
	std::priority_queue<EgCollapseInfo, std::vector<EgCollapseInfo>, std::greater<EgCollapseInfo>> m_Heap;

public:
	// 생성자 및 소멸자
	EgDecimator(EgMesh *pMesh);
	~EgDecimator();

	// 멤버 함수
	void InitVertQEM();
	void InitCollapseInfo();
	GMatrix Inv3(GMatrix &A, double det);	
	void Run(double Ratio);
	int CollapseEdge(EgCollapseInfo &cost);
	void UpdateCollapseInfo(EgVertex *v);
};
