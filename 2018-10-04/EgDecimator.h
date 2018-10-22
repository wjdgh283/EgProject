#pragma once

#include <vector>
#include <map>
#include <queue>
#include <functional>
#include "egmath.h"
#include "egmesh.h"

/*!
*	\class EgCollapseInfo
*	\brief ���� ���ſ� ���� ������ ǥ���ϴ� Ŭ����
*
*	\author ������, ����ȣ
*	\date 31 July 2018
*/
class EgCollapseInfo
{
	// ������ �Լ�
	friend bool operator >(const EgCollapseInfo &lhs, const EgCollapseInfo &rhs);

public:
	/*! \brief ������ ������ ���� ������ */
	EgEdge *m_pEdge;

	/*! \brief ���� ���� ��� */
	double m_Cost;

	/*! \brief ���� ���� ��, �� ������ ��ġ */
	GPoint3 m_Pos;

public:
	// ������ �� �Ҹ���
	EgCollapseInfo(EgEdge *pEdge, GPoint3 Pos, double Cost);
	EgCollapseInfo(const EgCollapseInfo &cpy);
	~EgCollapseInfo();

	// ���� ������
	EgCollapseInfo &operator =(const EgCollapseInfo &rhs);
};

/*!
*	\class EgDecimator
*	\brief �޽��� �ܼ�ȭ�� ���� Ŭ����
*
*	\author ������, ����ȣ
*	\date 31 July 2018
*/
class EgDecimator
{
public:
	/*! \brief �ܼ�ȭ�� ��� �޽��� ���� ������ */
	EgMesh *m_pMesh;

	/*! \brief ��� �޽��� �� �������� ���ǵ� QEM ��� */
	std::vector<GMatrix> m_QEM;

	/*! \brief ������ ���� ����� �����ϴ� ���� */
	std::map<EgEdge *, double> m_EdgeCostMap;

	/*! \brief ���� ���� ��뿡 ���� ���� ���� ������ �����ϴ� �ּ��� */
	std::priority_queue<EgCollapseInfo, std::vector<EgCollapseInfo>, std::greater<EgCollapseInfo>> m_Heap;

public:
	// ������ �� �Ҹ���
	EgDecimator(EgMesh *pMesh);
	~EgDecimator();

	// ��� �Լ�
	void InitVertQEM();
	void InitCollapseInfo();
	GMatrix Inv3(GMatrix &A, double det);	
	void Run(double Ratio);
	int CollapseEdge(EgCollapseInfo &cost);
	void UpdateCollapseInfo(EgVertex *v);
};
