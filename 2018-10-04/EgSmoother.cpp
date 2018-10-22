#include "EgSmoother.h"

/*!
*	\brief ������
*
*	\param pMesh �������� ��� �޽��� ���� ������
*/
EgSmoother::EgSmoother(EgMesh *pMesh)
{
	m_pMesh = pMesh;
}

/*!
*	\brief �Ҹ���
*/
EgSmoother::~EgSmoother()
{
}

/*!
*	\brief ������ ��ġ�� ������ ������ �����ġ�� �����Ͽ� �޽��� �ε巴�� �����(���ö�þ� ������).
*/
void EgSmoother::Execute()
{
	// ���� �߽� �迭�� �����Ѵ�. 
	std::vector<GVector3> Cogs;

	// ������ ������ ���Ͽ�
	EgVertex *pVert = m_pMesh->m_pVerts;
	while (pVert != NULL)
	{
		// ������ 1�� �̿� ������ �����߽��� ���Ѵ�.
		GVector3 cog;
		std::vector<EgVertex *> Verts;
		int k = pVert->GetOneRingVert(Verts);
		for (EgVertex *v : Verts)
			cog += cast_vec3(v->m_Pos);
		if (k != 0)
			cog /= (double)k;

		// ���� �����߽��� �迭�� �����Ѵ�.
		Cogs.push_back(cog);

		pVert = pVert->m_pNext;
	} 

	// ������ ������ ��ġ�� �����߽����� �̵��Ѵ�.
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