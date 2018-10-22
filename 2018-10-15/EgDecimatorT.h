#pragma once

#include <vector>
#include <map>
#include <queue>
#include <functional>
#include "egmath.h"
#include "egmesh.h"

/*!
*	\class EgCollapseInfoT
*	\brief 에지 제거에 대한 정보를 표현하는 클래스
*
*	\author 박정호, 윤승현
*	\date 01 Oct. 2018
*/
class EgCollapseInfoT
{
	// 프렌드 함수
	friend bool operator >(const EgCollapseInfoT &lhs, const EgCollapseInfoT &rhs);

public:
	/*! \brief 제거할 에지에 대한 포인터 */
	EgEdge *m_pEdge;

	/*! \brief 에지 제거 비용 */
	double m_Cost;

	/*! \brief 에지 제거 후, 두 끝점의 위치 */
	GPoint3 m_Pos;

	/*! \brief 에지 제거 후, 두 끝점의 텍스쳐 좌표 */
	GVector2 m_Coord;

public:
	// 생성자 및 소멸자
	EgCollapseInfoT(EgEdge *pEdge, GPoint3 Pos, GVector2 Coord);
	EgCollapseInfoT(const EgCollapseInfoT &cpy);
	~EgCollapseInfoT();

	// 대입 연산자
	EgCollapseInfoT &operator =(const EgCollapseInfoT &rhs);
};

/*!
*	\class EgDecimatorT
*	\brief 메쉬의 단순화를 위한 클래스
*
*	\author 박정호, 윤승현
*	\date 01 Oct. 2018
*/
class EgDecimatorT
{
public:
	/*! \brief 단순화할 대상 메쉬에 대한 포인터 */
	EgMesh *m_pMesh;

	/*! \brief 단순화할 대상 메쉬의 경계 상자의 대각선 길이 */
	double m_MeshSize;

	/*! \brief 메쉬의 각 정점의 경계 정점 여부를 저장하는 변수 */
	std::vector<bool> m_BndryVertInfo;

	/*! \brief 대상 메쉬의 각 정점에서 정의된 QEM 행렬 */
	std::vector<GMatrix> m_QEM;	

	/*! \brief 에지 제거 비용에 따라 에지 제거 정보를 저장하는 최소힙 */
	std::priority_queue<EgCollapseInfoT, std::vector<EgCollapseInfoT>, std::greater<EgCollapseInfoT>> m_Heap;

protected:
	void GetErrorMatrix(GMatrix &K, GVector &p, GVector &q, GVector &r);

public:
	// 생성자 및 소멸자
	EgDecimatorT(EgMesh *pMesh);
	~EgDecimatorT();

	// 멤버 함수
	void Init();
	void InitMeshSize();
	void InitBndryVertInfo(int NumVert);
	void InitVertQEM(int NumVert);
	void InitCollapseInfo();
	void Execute(double Ratio, const char *FileName);
	void Execute(const char *fname);
	bool IsCollapsible(EgEdge *pEdge);
	int CollapseEdge(EgCollapseInfoT &cost, int &sIdx, int &eIdx);	
	void UpdateCollapseInfo(EgVertex *v);
};
