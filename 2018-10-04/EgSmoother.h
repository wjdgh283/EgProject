#pragma once

#include<vector>
#include"egmath.h"
#include"egmesh.h"

/*!
*	\class EgSmoother
*	\brief 메쉬의 스무딩(Smoothing)을 위한 클래스
*
*	\author 윤승현, 박정호
*	\date 13 Aug. 2018
*/
class EgSmoother
{
public:
	/*! \brief 대상 메쉬에 대한 포인터 */
	EgMesh * m_pMesh;

public:
	// 생성자 및 소멸자
	EgSmoother(EgMesh *pMesh);
	~EgSmoother();

	// 멤버함수
	void Execute();

};
