#pragma once

#include <vector>
#include <map>
#include <queue>
#include <functional>
#include "egmath.h"
#include "egmesh.h"

/*!
*	\class EgCollapseInfoT
*	\brief ���� ���ſ� ���� ������ ǥ���ϴ� Ŭ����
*
*	\author ����ȣ, ������
*	\date 01 Oct. 2018
*/
class EgCollapseInfoT
{
	// ������ �Լ�
	friend bool operator >(const EgCollapseInfoT &lhs, const EgCollapseInfoT &rhs);

public:
	/*! \brief ������ ������ ���� ������ */
	EgEdge *m_pEdge;

	/*! \brief ���� ���� ��� */
	double m_Cost;

	/*! \brief ���� ���� ��, �� ������ ��ġ */
	GPoint3 m_Pos;

	/*! \brief ���� ���� ��, �� ������ �ؽ��� ��ǥ */
	GVector2 m_Coord;

public:
	// ������ �� �Ҹ���
	EgCollapseInfoT(EgEdge *pEdge, GPoint3 Pos, GVector2 Coord);
	EgCollapseInfoT(const EgCollapseInfoT &cpy);
	~EgCollapseInfoT();

	// ���� ������
	EgCollapseInfoT &operator =(const EgCollapseInfoT &rhs);
};

/*!
*	\class EgDecimatorT
*	\brief �޽��� �ܼ�ȭ�� ���� Ŭ����
*
*	\author ����ȣ, ������
*	\date 01 Oct. 2018
*/
class EgDecimatorT
{
public:
	/*! \brief �ܼ�ȭ�� ��� �޽��� ���� ������ */
	EgMesh *m_pMesh;

	/*! \brief �ܼ�ȭ�� ��� �޽��� ��� ������ �밢�� ���� */
	double m_MeshSize;

	/*! \brief �޽��� �� ������ ��� ���� ���θ� �����ϴ� ���� */
	std::vector<bool> m_BndryVertInfo;

	/*! \brief ��� �޽��� �� �������� ���ǵ� QEM ��� */
	std::vector<GMatrix> m_QEM;	

	/*! \brief ���� ���� ��뿡 ���� ���� ���� ������ �����ϴ� �ּ��� */
	std::priority_queue<EgCollapseInfoT, std::vector<EgCollapseInfoT>, std::greater<EgCollapseInfoT>> m_Heap;

protected:
	void GetErrorMatrix(GMatrix &K, GVector &p, GVector &q, GVector &r);

public:
	// ������ �� �Ҹ���
	EgDecimatorT(EgMesh *pMesh);
	~EgDecimatorT();

	// ��� �Լ�
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
