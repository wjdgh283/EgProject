#pragma once

#include<vector>
#include"egmath.h"
#include"egmesh.h"

/*!
*	\class EgSmoother
*	\brief �޽��� ������(Smoothing)�� ���� Ŭ����
*
*	\author ������, ����ȣ
*	\date 13 Aug. 2018
*/
class EgSmoother
{
public:
	/*! \brief ��� �޽��� ���� ������ */
	EgMesh * m_pMesh;

public:
	// ������ �� �Ҹ���
	EgSmoother(EgMesh *pMesh);
	~EgSmoother();

	// ����Լ�
	void Execute();

};
