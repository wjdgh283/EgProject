#include "EgDecimatorT.h"

/*!
*	\brief 생성자
*/
EgCollapseInfoT::EgCollapseInfoT(EgEdge *pEdge, GPoint3 Pos, GVector2 Coord)
{
	m_pEdge = pEdge;
	m_Pos = Pos;
	m_Coord = Coord;
	m_Cost = pEdge->m_Cost;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
EgCollapseInfoT::EgCollapseInfoT(const EgCollapseInfoT &cpy)
{
	m_pEdge = cpy.m_pEdge;
	m_Pos = cpy.m_Pos;
	m_Coord = cpy.m_Coord;
	m_Cost = cpy.m_Cost;
}

/*!
*	\brief 소멸자
*/
EgCollapseInfoT::~EgCollapseInfoT()
{
}

/*!
*	\brief 대입 연산자
*
*	\param rhs 대입될 객체
*/
EgCollapseInfoT &EgCollapseInfoT::operator =(const EgCollapseInfoT &rhs)
{
	m_pEdge = rhs.m_pEdge;
	m_Pos = rhs.m_Pos;
	m_Coord = rhs.m_Coord;
	m_Cost = rhs.m_Cost;
	return *this;
}

/*!
*	\brief 비교(>) 연산자
*
*	\param lhs 왼쪽 객체
*	\param rhs 오른쪽 객체
*/
bool operator >(const EgCollapseInfoT &lhs, const EgCollapseInfoT &rhs)
{
	return (lhs.m_Cost > rhs.m_Cost);
}

/*!
*	\brief 생성자
*
*	\param pMesh 단순화할 대상 메쉬에 대한 포인터
*/
EgDecimatorT::EgDecimatorT(EgMesh *pMesh)
{
	// 대상 메쉬를 설정한다.
	m_pMesh = pMesh;
	m_MeshSize = 0.0;
}

/*!
*	\brief 소멸자
*/
EgDecimatorT::~EgDecimatorT()
{
}

/*!
*	\brief 초기화 작업을 수행한다.
*/
void EgDecimatorT::Init()
{
	// 메쉬 정점의 인덱스를 갱신한다.
	int NumVert = m_pMesh->UpdateVertIdx();

	// 메쉬의 크기를 초기화 한다.
	InitMeshSize();

	// 경계 정점의 정보를 초기화 한다.
	InitBndryVertInfo(NumVert);

	// 메쉬 정점의 QEM 행렬을 초기화 한다.
	InitVertQEM(NumVert);

	// 에지의 제거 비용을 초기화 한다.
	InitCollapseInfo();
}

/*!
*	\brief 메쉬를 지정된 크기로 초기화 한다.
*/
void EgDecimatorT::InitMeshSize()
{
	GVector3 p, q;
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		GPoint3 r = v->m_Pos;
		p[0] = MIN(p[0], r[0]);
		p[1] = MIN(p[1], r[1]);
		p[2] = MIN(p[2], r[2]);
		q[0] = MAX(q[0], r[0]);
		q[1] = MAX(q[1], r[1]);
		q[2] = MAX(q[2], r[2]);
		v = v->m_pNext;
	}

	// 경계 상자 대각선의 길이를 초기화 한다.
	m_MeshSize = norm(q - p);

	double scl = 350.0 / m_MeshSize;
	v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		v->m_Pos[0] = v->m_Pos[0] * scl;
		v->m_Pos[1] = v->m_Pos[1] * scl;
		v->m_Pos[2] = v->m_Pos[2] * scl;
		v = v->m_pNext;
	}
}

/*!
*	\brief 메쉬의 경계 정점 정보를 구한다.
*
*	\param NumVert 메쉬 정점의 개수
*/
void EgDecimatorT::InitBndryVertInfo(int NumVert)
{
	// 경계 정점 정보를 초기화 한다.
	m_BndryVertInfo.clear();
	m_BndryVertInfo.assign(NumVert, false);
	
	// 메쉬 각각의 정점에 대하여
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		// 정점에서 시작하는 각각의 에지에 대하여
		EgTexel *pTexel = v->m_pEdges[0]->m_pTexel;
		for (EgEdge *e : v->m_pEdges)
		{
			// 하나라도 경계 에지가 있다면
			if (e->m_pMate == NULL)
			{
				// 경계 정점임을 기록한다.
				m_BndryVertInfo[v->m_Idx] = true;
				break;
			}

			if (e->m_pTexel->m_Coord != pTexel->m_Coord)
			{
				// 경계 정점임을 기록한다.
				m_BndryVertInfo[v->m_Idx] = true;
				break;
			}
		}
		v = v->m_pNext;
	}
}

/*!
*	\brief 일반화된 Quatric error 행렬을 구한다.
*
*	\param K 결과가 저장될 6 x 6 행렬
*	\param p 삼각형의 일반화된 정점 벡터
*	\param q 삼각형의 일반화된 정점 벡터
*	\param r 삼각형의 일반화된 정점 벡터
*/
void EgDecimatorT::GetErrorMatrix(GMatrix &K, GVector &p, GVector &q, GVector &r)
{
	// 수직한 5차원 벡터 e1, e2 정의한다.
	GVector e1 = (q - p).Normalize();
	GVector e2 = ((r - p) - ((r - p) * e1) * e1).Normalize();

	// 행렬 K는 5 x 5 행렬 A와 5차원 벡터 b와 상수 c로 구성된다.
	// K = | A    b |
	//     | b^t  c |

	// 행렬 K의 A부분을 구하고,		
	K[0][0] = 1.0 - e1[0] * e1[0] - e2[0] * e2[0];
	K[0][1] = -e1[0] * e1[1] - e2[0] * e2[1];
	K[0][2] = -e1[0] * e1[2] - e2[0] * e2[2];
	K[0][3] = -e1[0] * e1[3] - e2[0] * e2[3];
	K[0][4] = -e1[0] * e1[4] - e2[0] * e2[4];

	K[1][0] = K[0][1];
	K[1][1] = 1.0 - e1[1] * e1[1] - e2[1] * e2[1];
	K[1][2] = -e1[1] * e1[2] - e2[1] * e2[2];
	K[1][3] = -e1[1] * e1[3] - e2[1] * e2[3];
	K[1][4] = -e1[1] * e1[4] - e2[1] * e2[4];

	K[2][0] = K[0][2];
	K[2][1] = K[1][2];
	K[2][2] = 1.0 - e1[2] * e1[2] - e2[2] * e2[2];
	K[2][3] = -e1[2] * e1[3] - e2[2] * e2[3];
	K[2][4] = -e1[2] * e1[4] - e2[2] * e2[4];

	K[3][0] = K[0][3];
	K[3][1] = K[1][3];
	K[3][2] = K[2][3];
	K[3][3] = 1.0 - e1[3] * e1[3] - e2[3] * e2[3];
	K[3][4] = -e1[3] * e1[4] - e2[3] * e2[4];

	K[4][0] = K[0][4];
	K[4][1] = K[1][4];
	K[4][2] = K[2][4];
	K[4][3] = K[3][4];
	K[4][4] = 1.0 - e1[4] * e1[4] - e2[4] * e2[4];

	// 벡터 b를 구하고,
	GVector b = (p * e1) * e1 + (p * e2) * e2 - p;
	K[5][0] = K[0][5] = b[0];
	K[5][1] = K[1][5] = b[1];
	K[5][2] = K[2][5] = b[2];
	K[5][3] = K[3][5] = b[3];
	K[5][4] = K[4][5] = b[4];

	// 상수 c를 계산한다.
	double pe1 = p * e1;
	double pe2 = p * e2;
	K[5][5] = p * p - SQR(pe1) - SQR(pe2);
}

/*!
*	\brief 메쉬 정점에서 정의되는 QEM 행렬을 초기화 한다.
*
*	\param NumVert 메쉬 정점의 개수
*/
void EgDecimatorT::InitVertQEM(int NumVert)
{
	// QEM 리스트를 초기화 한다.
	m_QEM.clear();
	m_QEM.assign(NumVert, GMatrix(6, 6));

	// 각각의 삼각형에 대하여
	EgFace *f = m_pMesh->m_pFaces;
	while (f != NULL)
	{
		// 5차원의 세 점 p, q, r 정의한다.
		EgVertex *v0 = f->GetVertex(0);
		EgVertex *v1 = f->GetVertex(1);
		EgVertex *v2 = f->GetVertex(2);
		EgTexel *t0 = f->GetTexel(0);
		EgTexel *t1 = f->GetTexel(1);
		EgTexel *t2 = f->GetTexel(2);
		GVector p(5, v0->m_Pos[0], v0->m_Pos[1], v0->m_Pos[2], t0->m_Coord[0], t0->m_Coord[1]);
		GVector q(5, v1->m_Pos[0], v1->m_Pos[1], v1->m_Pos[2], t1->m_Coord[0], t1->m_Coord[1]);
		GVector r(5, v2->m_Pos[0], v2->m_Pos[1], v2->m_Pos[2], t2->m_Coord[0], t2->m_Coord[1]);

		// 텍스처 좌표를 포함한 Quadric error 행렬을 구한다.
		GMatrix K(6, 6);
		GetErrorMatrix(K, p, q, r);		

		// 삼각형의 세 정점의 QEM에 누적한다.
		m_QEM[f->GetVertex(0)->m_Idx] += K;
		m_QEM[f->GetVertex(1)->m_Idx] += K;
		m_QEM[f->GetVertex(2)->m_Idx] += K;

		// 다음 삼각형을 처리한다.
		f = f->m_pNext;
	}
}

/*!
*	\brief 메쉬의 각각의 에지를 제거하는 비용을 초기화 한다.
*	\note 에지 (vi, vk)와 (vk, vi)는 동일한 에지로 판단하여 대표 에지에 대해서만 비용을 구한다.
*/
void EgDecimatorT::InitCollapseInfo()
{
	// 각각의 정점에 대하여
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		// 경계 정점이 아니라면
		if (!m_BndryVertInfo[v->m_Idx])
		{
			// 정점에서 시작하는 각각의 에지에 대하여
			for (EgEdge *e : v->m_pEdges)
			{
				// 대표 에지(경계에지 또는 먼저 방문된 에지)라면
				if (e->m_pMate == NULL || e->m_pMate->m_Cost == 0.0)
				{
					// 에지의 양끝 정점과 텍스처 정점을 구한다.
					EgVertex *v0 = v;
					EgVertex *v1 = e->m_pNext->m_pVert;
					EgTexel *t0 = e->m_pTexel;
					EgTexel *t1 = e->m_pNext->m_pTexel;

					// 에지의 양끝점(position + texture coord)을 구하여,
					GVector vt0(5, v0->m_Pos[0], v0->m_Pos[1], v0->m_Pos[2], t0->m_Coord[0], t0->m_Coord[1]);
					GVector vt1(5, v1->m_Pos[0], v1->m_Pos[1], v1->m_Pos[2], t1->m_Coord[0], t1->m_Coord[1]);

					// 새로운 정점의 6x6 QEM을 구한다.
					GMatrix Q = m_QEM[v0->m_Idx] + m_QEM[v1->m_Idx];

					// 새로운 정점의 5차원 벡터 (position + texture coord)를 구한다.
					GVector x(5), b(5);
					b.Set(Q[0][5], Q[1][5], Q[2][5], Q[3][5], Q[4][5]);

					// 6x6 K 행렬의 부분행렬 A를 구하여 새로운 정점을 계산한다.
					GMatrix A = sub(Q, 0, 0, 4, 4);
					if (EQ_ZERO(det(A), 1.0e-9))
					{
						//printf("Singular matrix...\n");
						printf("Singular value = %1.10lf\n", det(A));
						x = (vt0 + vt1) * 0.5;
						x[3] = vt0[3];
						x[4] = vt0[4];
					}
					else
						x = inv(A) * (-b);

					// 대표 에지 제거 정보를 힙에 추가한다.
					normalize_uv(x[3], x[4]);
					e->m_Cost = x * b + Q[5][5];

					EgCollapseInfoT info(e, GPoint3(x[0], x[1], x[2]), GVector2(x[3], x[4]));
					m_Heap.push(info);
				}
			}
		}
		v = v->m_pNext;
	}
}

/*!
*	\brief 반복적으로 에지 제거를 수행한다.
*
*	\prarm Ratio 제거할 삼각형의 비율
*	\param FileName 제거되는 에지 정보를 기록할 파일명
*/
void EgDecimatorT::Execute(double Ratio, const char *FileName)
{
	// 초기 삼각형의 개수를 구한다.
	int NumFace = m_pMesh->GetNumFace();
	int TarNumFace = (int)(NumFace * (1.0 - Ratio));

	// 제거되는 에지의 순서를 기록할 파일을 생성한다.
	FILE *fp;
	if (FileName != NULL)
		fopen_s(&fp, FileName, "w");

	// 반복적으로 에지를 제거하여 삼각형의 개수를 줄인다.
	while (!m_Heap.empty())
	{
		// 최소 비용을 갖는 제거 정보를 뽑아서,
		EgCollapseInfoT Info = m_Heap.top();
		m_Heap.pop();

		// 이미 제거된 에지이거나, 에지 제거 비용이 변경되었다면 스킵한다.
		if (Info.m_pEdge->m_Cost == -1.0 ||
			Info.m_pEdge->m_Cost != Info.m_Cost)
			continue;

		// 선택된 에지를 제거하고, 삼각형 수를 줄인다.
		int sIdx, eIdx;
		int result = CollapseEdge(Info, sIdx, eIdx);
		if (result > 0)
		{
			// 제거된 삼각형 수를 줄이고,
			NumFace = NumFace - result;

			// 제거되는 에지의 정점 인덱스를 기록한다.
			if (FileName != NULL)
				fprintf_s(fp, "%d\t%d\n", sIdx, eIdx);
		}

		// 삼각형의 개수가 원하는 수에 도달했다면 종료한다.
		if (NumFace < TarNumFace)
			break;
	}

	// 파일의 기록을 종료한다.
	if (FileName != NULL)
		fclose(fp);

	// 제거된 삼각형을 정리한다.
	std::vector<EgFace *> DelFaces;
	EgFace *f = m_pMesh->m_pFaces;
	while (f != NULL)
	{
		if (f->m_pEdge->m_Cost == -1.0)
			DelFaces.push_back(f);
		f = f->m_pNext;
	}
	for (EgFace *f : DelFaces)
		DEL_FROM_PTR_LIST(m_pMesh->m_pFaces, f);

	// 메쉬의 크기를 원래대로 복원한다.
	double scl = m_MeshSize / 350.0;
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		v->m_Pos[0] = v->m_Pos[0] * scl;
		v->m_Pos[1] = v->m_Pos[1] * scl;
		v->m_Pos[2] = v->m_Pos[2] * scl;
		v = v->m_pNext;
	}
}

/*!
*	\brief 정해진 순서에 따라 에지 제거를 수행한다.
*
*	\param fname 에지 제거의 순서 정보가 저장된 파일
*/
void EgDecimatorT::Execute(const char *fname)
{
	// 파일을 읽기 모드로 오픈한다.
	FILE *fp;
	fopen_s(&fp, fname, "r");
	if (fp == NULL)
	{
		printf("File open failed...\n");
		return;
	}

	// 인덱스 접근을 위해 정점의 배열을 생성한다.
	std::vector<EgVertex *> Verts;
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		Verts.push_back(v);
		v = v->m_pNext;
	}

	// 파일에서 제거할 에지의 시작점과 끝점의 인덱스를 읽는다.
	int vidx1, vidx2;
	while (fscanf_s(fp, "%d%d", &vidx1, &vidx2) != EOF)
	{
		// 에지의 시작점과 끝점을 구하여,
		EgVertex *v1 = Verts[vidx1];
		EgVertex *v2 = Verts[vidx2];

		// 해당 에지를 찾는다.
		EgEdge *pEdge = NULL;
		for (EgEdge *e : v1->m_pEdges)
		{
			if (e->m_pNext->m_pVert == v2)
			{
				pEdge = e;
				break;
			}
		}

		// 에지의 양끝 정점과 텍스처 정점을 구한다.
		EgTexel *t1 = pEdge->m_pTexel;
		EgTexel *t2 = pEdge->m_pNext->m_pTexel;

		// 에지의 양끝점(position + texture coord)을 구하여,
		GVector vt1(5, v1->m_Pos[0], v1->m_Pos[1], v1->m_Pos[2], t1->m_Coord[0], t1->m_Coord[1]);
		GVector vt2(5, v2->m_Pos[0], v2->m_Pos[1], v2->m_Pos[2], t2->m_Coord[0], t2->m_Coord[1]);
		
		// 해당 에지를 찾지 못했다면 에러 메시지를 출력한다.
		if (pEdge == NULL)
			printf("Edge is not found...\n");

		// 새로운 정점의 QEM을 구한다.
		GMatrix Q = m_QEM[v1->m_Idx] + m_QEM[v2->m_Idx];

		// 새로운 정점의 5차원 벡터 (position + texture coord)를 구한다.
		GVector x(5), b(5);
		b.Set(Q[0][5], Q[1][5], Q[2][5], Q[3][5], Q[4][5]);

		// 6x6 K 행렬의 부분행렬 A를 구하여 새로운 정점을 계산한다.
		GMatrix A = sub(Q, 0, 0, 4, 4);
		if (EQ_ZERO(det(A), 1.0e-9))
		{
			//printf("Singular matrix...\n");
			printf("Singular value = %1.10lf\n", det(A));
			x = (vt1 + vt2) * 0.5;
			x[3] = vt1[3];
			x[4] = vt1[4];
		}
		else
			x = inv(A) * (-b);
				
		// 에지의 시작점(v1)의 위치를 갱신한다.
		v1->m_Pos.Set(x[0], x[1], x[2]);

		// 에지의 시작점에서 시작하는 모든 에지의 텍스처 좌표를 변경한다.
		normalize_uv(x[3], x[4]);
		for (EgEdge *e : v1->m_pEdges)
			e->m_pTexel->m_Coord = GVector2(x[3], x[4]);

		// 에지를 공유한 삼각형을 제거한다(정점은 남는다).
		std::vector<EgFace *> Faces;
		Faces.push_back(pEdge->m_pFace);
		if (pEdge->m_pMate != NULL)
			Faces.push_back(pEdge->m_pMate->m_pFace);
		m_pMesh->DelFaces(Faces);

		// v2에서 시작하는 모든 에지들의 시작점을 v1으로 변경한다.
		for (EgEdge *e : v2->m_pEdges)
		{
			e->m_pVert = v1;
			e->m_pTexel = pEdge->m_pTexel;
			v1->m_pEdges.push_back(e);
		}

		// v2에서 시작하는 에지 정보를 제거하고,
		v2->m_pEdges.clear();

		// v1에서 시작하는 에지의 mate 에지의 정보를 갱신한다.
		m_pMesh->UpdateEdgeMate(v1);

		// v1의 QEM 행렬을 갱신한다.
		m_QEM[v1->m_Idx] += m_QEM[v2->m_Idx];
	}

	// 메쉬의 크기를 원래대로 복원한다.
	double scl = m_MeshSize / 350.0;
	v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		v->m_Pos[0] = v->m_Pos[0] * scl;
		v->m_Pos[1] = v->m_Pos[1] * scl;
		v->m_Pos[2] = v->m_Pos[2] * scl;
		v = v->m_pNext;
	}
}

/*!
*	\brief 에지가 제거 가능한지 검사한다.
*
*	\param pEdge 제거 가능 여부를 조사할 에지에 대한 포인터
*
*	\return 에지가 제거가능할 경우 true, 아니면 false를 반환한다.
*/
bool EgDecimatorT::IsCollapsible(EgEdge *pEdge)
{
	// 에지의 시작점(v1)과 끝점(v2)을 구한다.
	EgVertex *v1 = pEdge->m_pVert;
	EgVertex *v2 = pEdge->m_pNext->m_pVert;

	// 에지가 끝점이 경계 정점에 포함된다면 false를 반환한다.
	if (m_BndryVertInfo[v1->m_Idx] == true || m_BndryVertInfo[v2->m_Idx] == true)
		return false;

	// v1과 v2의 1링 정점의 집합 A와 B를 구한다.
	std::vector<EgVertex *> A, B;
	v1->GetOneRingVert(A);
	v2->GetOneRingVert(B);

	// A와 B의 교집합을 구한다.
	std::vector<EgVertex *> C;
	for (EgVertex *a : A)
	{
		for (EgVertex *b : B)
		{
			if (a == b)
			{
				C.push_back(a);
				break;
			}
		}
	}

	// 에지 제거의 조건을 만족할 경우 true를 반환한다.
	if (pEdge->m_pMate == NULL && C.size() == 1)
		return true;
	if (pEdge->m_pMate != NULL && C.size() == 2)
		return true;

	// 에지 제거 조건을 만족하지 못하는 경우 false를 반환한다.
	return false;
}

/*!
*	\brief 메쉬의 제거 가능한 에지를 제거한다.
*
*	\param pEdge 제거할 에지에 대한 포인터
*
*	\return 함께 제거된 삼각형의 개수를 반환한다(0: 제거 불가능, 1, 2)
*/
int EgDecimatorT::CollapseEdge(EgCollapseInfoT &info, int &sIdx, int &eIdx)
{
	// 제거가 불가능한 에지라면 0을 반환한다.
	if (IsCollapsible(info.m_pEdge) == false)
		return 0;

	int RemovedFaces = 1;
	EgEdge *pEdge = info.m_pEdge;

	// 에지의 시작점(v1)과 끝점(v2)을 구해 새로운 위치와 텍스쳐 좌표를 구한다.
	EgVertex *v1 = pEdge->m_pVert;
	EgVertex *v2 = pEdge->m_pNext->m_pVert;
	v1->m_Pos = info.m_Pos;
	for (EgEdge *e : v1->m_pEdges)
		e->m_pTexel->m_Coord = info.m_Coord;
	
	// 삼각형의 에지들이 제거되었다는 것을 기록한다.
	pEdge->m_Cost = -1.0;
	pEdge->m_pNext->m_Cost = -1.0;
	pEdge->m_pNext->m_pNext->m_Cost = -1.0;

	// 에지를 공유한 삼각형을 제거한다(정점은 남는다).
	std::vector<EgFace *> Faces;
	Faces.push_back(pEdge->m_pFace);
	if (pEdge->m_pMate != NULL)
	{
		RemovedFaces = 2;
		EgEdge *pMate = pEdge->m_pMate;
		Faces.push_back(pMate->m_pFace);

		// 삼각형의 에지들이 제거되었다는 것을 기록한다.
		pMate->m_Cost = -1.0;
		pMate->m_pNext->m_Cost = -1.0;
		pMate->m_pNext->m_pNext->m_Cost = -1.0;
	}

	// 각각의 삼각형에 대하여
	for (EgFace *f : Faces)
	{
		// 각각의 에지의 반대편 에지의 정보를 갱신한다.
		for (int i = 0; i < 3; ++i)
		{
			EgEdge *e = f->GetEdge(i);
			if (e->m_pMate != NULL)
				e->m_pMate->m_pMate = NULL;

			// 정점에서 시작하는 에지 리스트에서 현재 에지를 제거한다.
			DEL_FROM_VEC(e->m_pVert->m_pEdges, e);
		}
	}

	// v2에서 시작하는 모든 에지들의 시작점과 텍스쳐 좌표를 v1으로 변경하고 
	for (EgEdge *&e : v2->m_pEdges)
	{
		e->m_pVert = v1;
		//e->m_pTexel->m_Coord = info.m_Coord;
		e->m_pTexel = info.m_pEdge->m_pTexel;
		v1->m_pEdges.push_back(e);
	}

	// v2에서 시작하는 에지 정보를 제거하고,
	v2->m_pEdges.clear();

	// v1에서 시작하는 에지의 mate 에지의 정보를 갱신한다.
	m_pMesh->UpdateEdgeMate(v1);

	// v1의 QEM 행렬을 갱신한다.
	m_QEM[v1->m_Idx] += m_QEM[v2->m_Idx];

	// 제거되는 에지의 시작점과 끝점의 인덱스를 저장한다.
	sIdx = v1->m_Idx;
	eIdx = v2->m_Idx;

	// v1을 공유한 삼각형의 모든 에지 비용을 갱신한다.
	UpdateCollapseInfo(v1);

	// 제거된 삼각형의 개수를 반환한다.
	return RemovedFaces;
}

/*!
*	\brief 정점을 공유한 에지의 제거 비용을 갱신한다.
*
*	\param v 정점에 대한 포인터
*/
void EgDecimatorT::UpdateCollapseInfo(EgVertex *v)
{
	// 정점 v에서 시작하는 각각의 에지에 대하여
	for (EgEdge *e : v->m_pEdges)
	{
		// e의 mate 에지가 대표 에지였다면, e를 대표 에지로 정의한다.
		if (e->m_pMate != NULL && e->m_pMate->m_Cost != 0.0)
			e->m_pMate->m_Cost = 0.0;

		// 대표 에지의 양끝점을 구하고,
		EgVertex *v0 = e->m_pVert;
		EgVertex *v1 = e->m_pNext->m_pVert;
		EgTexel *t0 = e->m_pTexel;
		EgTexel *t1 = e->m_pNext->m_pTexel;

		// 에지의 양끝점(position + texture coord)을 구하여,
		GVector vt0(5, v0->m_Pos[0], v0->m_Pos[1], v0->m_Pos[2], t0->m_Coord[0], t0->m_Coord[1]);
		GVector vt1(5, v1->m_Pos[0], v1->m_Pos[1], v1->m_Pos[2], t1->m_Coord[0], t1->m_Coord[1]);

		// 새로운 정점의 6x6 QEM을 구한다.
		GMatrix Q = m_QEM[v0->m_Idx] + m_QEM[v1->m_Idx];

		// 새로운 정점의 5차원 벡터 (position + texture coord)를 구한다.
		GVector x(5), b(5);
		b.Set(Q[0][5], Q[1][5], Q[2][5], Q[3][5], Q[4][5]);

		// 6x6 K 행렬의 부분행렬 A를 구하여 새로운 정점을 계산한다.
		GMatrix A = sub(Q, 0, 0, 4, 4);
		if (EQ_ZERO(det(A), 1.0e-9))
		{
			printf("Singular value = %1.10lf\n", det(A));
			x = (vt0 + vt1) * 0.5;
			x[3] = vt0[3];
			x[4] = vt0[4];
		}
		else
			x = inv(A) * (-b);
		
		// 대표 에지 제거 정보를 힙에 추가한다.
		normalize_uv(x[3], x[4]);
		e->m_Cost = x * b + Q[5][5];
		EgCollapseInfoT info(e, cast_pt3(x), GVector2(x[3], x[4]));
		m_Heap.push(info);
	}
}
