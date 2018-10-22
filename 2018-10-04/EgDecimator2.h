#pragma once

#include <vector>
#include <map>
#include <queue>
#include <functional>
#include "egmath.h"
#include "egmesh.h"

class EgCollapseInfo2
{
	// ������ �Լ�
	friend bool operator >(const EgCollapseInfo2 &lhs, const EgCollapseInfo2 &rhs);

public:
	/*! \brief ������ ������ ���� ������ */
	EgEdge * m_pEdge;

	/*! \brief ���� ���� ��� */
	double m_Cost;

	/*! \brief ���� ���� ��, �� ������ ��ġ */
	GPoint3 m_Pos;

	/*! \brief ���� ���� ��, �� ������ �ؽ��� ��ǥ */
	GVector2 m_Coord;

public:
	// ������ �� �Ҹ���
	EgCollapseInfo2(EgEdge *pEdge, GPoint3 Pos, GVector2 Coord, double Cost);
	EgCollapseInfo2(const EgCollapseInfo2 &cpy);
	~EgCollapseInfo2();

	// ���� ������
	EgCollapseInfo2 &operator =(const EgCollapseInfo2 &rhs);
};

class EgDecimator2
{
public:
	/*! \brief �ܼ�ȭ�� ��� �޽��� ���� ������ */
	EgMesh * m_pMesh;

	/*! \brief ��� �޽��� �� �������� ���ǵ� QEM ��� */
	std::vector<GMatrix> m_QEM;

	/*! \brief ������ ���� ����� �����ϴ� ���� */
	std::map<EgEdge *, double> m_EdgeCostMap;

	/*! \brief ���� ���� ��뿡 ���� ���� ���� ������ �����ϴ� �ּ��� */
	std::priority_queue<EgCollapseInfo2, std::vector<EgCollapseInfo2>, std::greater<EgCollapseInfo2>> m_Heap;

public:
	// ������ �� �Ҹ���
	EgDecimator2(EgMesh *pMesh);
	~EgDecimator2();

	// ��� �Լ�
	void InitVertQEM();
	void InitCollapseInfo();
	GMatrix Part5(GMatrix &K);
	void Run(double Ratio);
	int CollapseEdge(EgCollapseInfo2 &cost);
	void UpdateCollapseInfo(EgVertex *v);
};
