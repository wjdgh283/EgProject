#include "EgSmoother.h"

/*!
*	\brief 생성자
*
*	\param pMesh 스무딩할 대상 메쉬에 대한 포인터
*/
EgSmoother::EgSmoother(EgMesh *pMesh)
{
	m_pMesh = pMesh;
}

/*!
*	\brief 소멸자
*/
EgSmoother::~EgSmoother()
{
}

/*!
*	\brief 정점의 위치를 인접한 정점의 평균위치로 설정하여 메쉬를 부드럽게 만든다(라플라시안 스무딩).
*/
void EgSmoother::Execute()
{
	// 무게 중심 배열을 정의한다. 
	std::vector<GVector3> Cogs;

	// 각각의 정점에 대하여
	EgVertex *pVert = m_pMesh->m_pVerts;
	while (pVert != NULL)
	{
		// 정점의 1링 이웃 정점의 무게중심을 구한다.
		GVector3 cog;
		std::vector<EgVertex *> Verts;
		int k = pVert->GetOneRingVert(Verts);
		for (EgVertex *v : Verts)
			cog += cast_vec3(v->m_Pos);
		if (k != 0)
			cog /= (double)k;

		// 구한 무게중심을 배열에 저장한다.
		Cogs.push_back(cog);

		pVert = pVert->m_pNext;
	} 

	// 각각의 정점의 위치를 무게중심으로 이동한다.
	pVert = m_pMesh->m_pVerts;
	int vidx = 0;
	do
	{
		pVert->m_Pos = cast_pt3(Cogs[vidx]);
		pVert = pVert->m_pNext;
		vidx++;
	} while (pVert != NULL);

	printf("Mesh Smoothing is completed...\n");

}