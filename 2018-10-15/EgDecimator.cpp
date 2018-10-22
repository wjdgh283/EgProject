#include "EgDecimator.h"


/*!
*	\brief 생성자
*
*	\param pEdge 제거할 에지의 포인터
*	\param Pos 에지의 양 끝점이 병합되는 위치
*/
EgCollapseInfo::EgCollapseInfo(EgEdge *pEdge, GPoint3 Pos)
{
	m_pEdge = pEdge;
	m_Pos = Pos;
	m_Cost = pEdge->m_Cost;
}

/*!
*	\brief 복사 생성자
*
*	\param cpy 복사될 객체
*/
EgCollapseInfo::EgCollapseInfo(const EgCollapseInfo &cpy)
{
	m_pEdge = cpy.m_pEdge;
	m_Pos = cpy.m_Pos;
	m_Cost = cpy.m_Cost;
}

/*!
*	\brief 소멸자
*/
EgCollapseInfo::~EgCollapseInfo()
{
}

/*!
*	\brief 대입 연산자
*
*	\param rhs 대입될 객체
*/
EgCollapseInfo &EgCollapseInfo::operator =(const EgCollapseInfo &rhs)
{
	m_pEdge = rhs.m_pEdge;
	m_Pos = rhs.m_Pos;
	m_Cost = rhs.m_Cost;
	return *this;
}

/*!
*	\brief 비교(>) 연산자
*
*	\param lhs 왼쪽 객체
*	\param rhs 오른쪽 객체
*/
bool operator >(const EgCollapseInfo &lhs, const EgCollapseInfo &rhs)
{
	return (lhs.m_Cost > rhs.m_Cost);
}

/*!
*	\brief 생성자
*
*	\param pMesh 단순화할 대상 메쉬에 대한 포인터
*/
EgDecimator::EgDecimator(EgMesh *pMesh)
{
	// 대상 메쉬를 설정한다.
	m_pMesh = pMesh;
}

/*!
*	\brief 소멸자
*/
EgDecimator::~EgDecimator()
{
}

/*!
*	\brief 평면 ax + by + cz + d = 0에서 점 p까지 거리의 제곱을 계산하는 행렬을 구한다.
*
*	\param K 계산된 행렬이 저장된다.
*	\param a 평면의 방정식의 상수
*	\param b 평면의 방정식의 상수
*	\param c 평면의 방정식의 상수
*	\param d 평면의 방정식의 상수
*/
void EgDecimator::GetErrorMatrix(GMatrix &K, double a, double b, double c, double d)
{
	K[0][0] = a * a; 
	K[0][1] = a * b; 
	K[0][2] = a * c; 
	K[0][3] = a * d;

	K[1][0] = K[0][1];	// 대칭성
	K[1][1] = b * b;
	K[1][2] = b * c;
	K[1][3] = b * d;

	K[2][0] = K[0][2];	// 대칭성
	K[2][1] = K[1][2];	// 대칭성
	K[2][2] = c * c;
	K[2][3] = c * d;

	K[3][0] = K[0][3];	// 대칭성
	K[3][1] = K[1][3];	// 대칭성
	K[3][2] = K[2][3];	// 대칭성
	K[3][3] = d * d;
}

/*
*	\brief 3행 3열의 대칭 행렬 A의 역행렬을 반환한다.
*
*	\param A 대상 행렬
*	\param det 행렬 A의 행렬식
*
*	\return 행렬 A의 역행렬을 반환한다.
*/
GMatrix EgDecimator::Inv3(GMatrix &A, double det)
{
	GMatrix ret(3, 3);
	ret[0][0] = (A[1][1] * A[2][2] - A[1][2] * A[2][1]) / det;
	ret[0][1] = -(A[0][1] * A[2][2] - A[0][2] * A[2][1]) / det;
	ret[0][2] = (A[0][1] * A[1][2] - A[0][2] * A[1][1]) / det;

	ret[1][1] = (A[0][0] * A[2][2] - A[0][2] * A[2][0]) / det;
	ret[1][2] = -(A[0][0] * A[1][2] - A[0][2] * A[1][0]) / det;

	ret[2][2] = (A[0][0] * A[1][1] - A[0][1] * A[1][0]) / det;

	ret[1][0] = ret[0][1];
	ret[2][0] = ret[0][2];
	ret[2][1] = ret[1][2];

	return ret;
}

/*!
*	\brief 초기화 작업을 수행한다.
*/
void EgDecimator::Init()
{
	// 메쉬 정점의 인덱스를 갱신한다.
	int NumVert = m_pMesh->UpdateVertIdx();

	// 메쉬 정점의 QEM 행렬을 초기화 한다.
	InitVertQEM(NumVert);

	// 에지의 제거 비용을 초기화 한다.
	InitCollapseInfo();
}

/*!
*	\brief 메쉬 정점에서 정의되는 QEM 행렬을 초기화 한다.
*
*	\param NumVert 메쉬 정점의 개수
*/
void EgDecimator::InitVertQEM(int NumVert)
{
	// QEM 리스트를 초기화 한다.
	m_QEM.clear();
	m_QEM.assign(NumVert, GMatrix(4, 4));

	// 각각의 삼각형에 대하여
	EgFace *f = m_pMesh->m_pFaces;
	while (f != NULL)
	{
		// 삼각형을 포함한 평면의 방정식 (ax + by + cz + d = 0)을 구하여
		GVector3 n = f->GetFaceNormal(); // n = (a, b, c)
		double d = -n * cast_vec3(f->GetVertex(0)->m_Pos);

		// 삼각형 f에 대한 fundamental error quadric 행렬을 구하고
		GMatrix K(4, 4);
		GetErrorMatrix(K, n[0], n[1], n[2], d);

		// 삼각형의 세 정점에 K를 누적한다.
		m_QEM[f->GetVertex(0)->m_Idx] += K;
		m_QEM[f->GetVertex(1)->m_Idx] += K;
		m_QEM[f->GetVertex(2)->m_Idx] += K;

		// 삼각형의 각각의 에지에 대하여
		for (int i = 0; i < 3; ++i)
		{
			EgEdge *e = f->GetEdge(i);

			// 경계 에지라면,
			if (e->m_pMate == NULL)
			{
				// 에지를 지나고 삼각형에 수직한 평면을 구하고, 
				GVector3 ei = (e->m_pNext->m_pVert->m_Pos - e->m_pVert->m_Pos).Normalize();
				GVector3 ni = (n ^ ei).Normalize();
				double di = -ni * cast_vec3(e->m_pVert->m_Pos);

				// 평면으로 정의되는 quadric error 행렬을 구한다.
				GMatrix Ki(4, 4);
				GetErrorMatrix(Ki, ni[0], ni[1], ni[2], di);

				// 에지의 양끝점에 더한다.
				Ki *= 100.0;
				m_QEM[e->m_pVert->m_Idx] += Ki;
				m_QEM[e->m_pNext->m_pVert->m_Idx] += Ki;
			}
		}

		// 다음 삼각형을 처리한다.
		f = f->m_pNext;
	}	
}

/*!
*	\brief 각각의 에지를 제거하는 초기 비용을 계산한다.
*	\note 에지 (vi, vk)와 (vk, vi)는 동일한 에지로 간주하여 대표 에지에 대해서만 비용을 구한다.
*/
void EgDecimator::InitCollapseInfo()
{
	// 각각의 정점에 대하여
	EgVertex *v = m_pMesh->m_pVerts;
	while (v != NULL)
	{
		// 정점에서 시작하는 각각의 에지에 대하여
		for (EgEdge *e : v->m_pEdges)
		{
			// 대표 에지(경계에지 또는 먼저 방문된 에지)라면
			if (e->m_pMate == NULL || e->m_pMate->m_Cost == 0.0)
			{
				// 에지의 양끝점을 구하여,
				EgVertex *v1 = v;
				EgVertex *v2 = e->m_pNext->m_pVert;

				// 새로운 정점의 QEM을 구한다.
				GMatrix Q = m_QEM[v1->m_Idx] + m_QEM[v2->m_Idx];

				// 새로운 정점의 위치를 구한다.
				GVector x(3), b(3);
				b.Set(Q[0][3], Q[1][3], Q[2][3]);
				double det = (Q[0][0] * Q[1][1] * Q[2][2] + 2 * Q[0][1] * Q[1][2] * Q[0][2])
					- (Q[1][2] * Q[1][2] * Q[0][0] + Q[0][2] * Q[0][2] * Q[1][1] + Q[0][1] * Q[0][1] * Q[2][2]);

				if (EQ_ZERO(det, 1.0e-7))
					x = cast_vec(v1->m_Pos + (v2->m_Pos - v1->m_Pos) * 0.5);
				else
					x = Inv3(Q, det) * (-b);

				// 대표 에지의 제거 정보를 힙에 추가한다.
				e->m_Cost = x * b + Q[3][3];
				EgCollapseInfo info(e, cast_pt3(x));
				m_Heap.push(info);
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
void EgDecimator::Execute(double Ratio, const char *FileName)
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
		EgCollapseInfo Info = m_Heap.top();
		m_Heap.pop();

		// 이미 제거된 에지이거나, 오래된 제거 정보라면 스킵한다.
		if (Info.m_pEdge->m_Cost == -1.0 || 
			Info.m_pEdge->m_Cost != Info.m_Cost)
			continue;

		// 선택된 에지를 제거하고, 
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
		if (NumFace <= TarNumFace)
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
}

/*!
*	\brief 정해진 순서에 따라 에지 제거를 수행한다.
*
*	\param fname 에지 제거의 순서 정보가 저장된 파일
*/
void EgDecimator::Execute(const char *fname)
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

		// 해당 에지를 찾지 못했다면 에러 메시지를 출력한다.
		if (pEdge == NULL)
			printf("Edge is not found...\n");
		
		// 새로운 정점의 QEM을 구한다.
		GMatrix Q = m_QEM[v1->m_Idx] + m_QEM[v2->m_Idx];

		// 새로운 정점의 위치를 구한다.
		GVector x(3), b(3);
		b.Set(Q[0][3], Q[1][3], Q[2][3]);
		double det =
			(Q[0][0] * Q[1][1] * Q[2][2] + 2 * Q[0][1] * Q[1][2] * Q[0][2]) -
			(Q[1][2] * Q[1][2] * Q[0][0] + Q[0][2] * Q[0][2] * Q[1][1] + Q[0][1] * Q[0][1] * Q[2][2]);

		if (EQ_ZERO(det, 1.0e-7))
			x = cast_vec(v1->m_Pos + (v2->m_Pos - v1->m_Pos) * 0.5);
		else
			x = Inv3(Q, det) * (-b);

		// 에지의 시작점(v1)의 위치를 갱신한다.
		v1->m_Pos = cast_pt3(x);

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
			v1->m_pEdges.push_back(e);
		}

		// v2에서 시작하는 에지 정보를 제거하고,
		v2->m_pEdges.clear();

		// v1에서 시작하는 에지의 mate 에지의 정보를 갱신한다.
		m_pMesh->UpdateEdgeMate(v1);

		// v1의 QEM 행렬을 갱신한다.
		m_QEM[v1->m_Idx] += m_QEM[v2->m_Idx];
	}	
}

/*!
*	\brief 에지가 제거 가능한지 검사한다.
*
*	\param pEdge 제거 가능 여부를 조사할 에지에 대한 포인터 
*
*	\return 에지가 제거가능할 경우 true, 아니면 false를 반환한다.
*/
bool EgDecimator::IsCollapsible(EgEdge *pEdge)
{
	// 에지의 시작점(v1)과 끝점(v2)을 구한다.
	EgVertex *v1 = pEdge->m_pVert;
	EgVertex *v2 = pEdge->m_pNext->m_pVert;

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
*	\param Info 제거 정보
*	\param sIdx 제거되는 에지의 시작 정점의 인덱스가 저장된다.
*	\param eIdx 제거되는 에지의 끝 정점의 인덱스가 저장된다.
*
*	\return 제거된 삼각형의 개수를 반환한다(0: 제거 불가능, 1, 2)
*/
int EgDecimator::CollapseEdge(EgCollapseInfo &Info, int &sIdx, int &eIdx)
{
	// 제거가 불가능한 에지라면 0을 반환한다.
	if (IsCollapsible(Info.m_pEdge) == false)
		return 0;

	int RemovedFaces = 1;
	// 에지의 시작점(v1)의 위치를 갱신한다.
	EgEdge *pEdge = Info.m_pEdge;
	EgVertex *v1 = pEdge->m_pVert;
	EgVertex *v2 = pEdge->m_pNext->m_pVert;
	v1->m_Pos = Info.m_Pos;

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

	// 에지의 끝점(v2)에서 시작하는 모든 에지들의 시작점을 v1으로 변경한다.
	for (EgEdge *e : v2->m_pEdges)
	{
		e->m_pVert = v1;
		v1->m_pEdges.push_back(e);
	}

	// 끝점(v2)의 에지 정보를 제거하고,
	v2->m_pEdges.clear();

	// 시작점(v1)의 mate 에지의 정보를 갱신한다.
	m_pMesh->UpdateEdgeMate(v1);

	// 시작점(v1)의 QEM 행렬을 갱신한다.
	m_QEM[v1->m_Idx] = m_QEM[v1->m_Idx] + m_QEM[v2->m_Idx];

	// 제거되는 에지의 시작점과 끝점의 인덱스를 저장한다.
	sIdx = v1->m_Idx;
	eIdx = v2->m_Idx;

	// 시작점(v1)을 공유한 삼각형의 모든 에지 비용을 갱신한다.
	UpdateCollapseInfo(v1);

	// 제거된 삼각형의 개수를 반환한다.
	return RemovedFaces;
}

/*!
*	\brief 정점을 공유한 에지의 제거 비용을 갱신한다.
*
*	\param v 정점에 대한 포인터
*/
void EgDecimator::UpdateCollapseInfo(EgVertex *v)
{
	// 정점 v에서 시작하는 각각의 에지에 대하여
	for (EgEdge *e : v->m_pEdges)
	{
		// e의 mate 에지가 대표 에지였다면, e를 대표 에지로 정의한다.
		if (e->m_pMate != NULL && e->m_pMate->m_Cost != 0.0)
			e->m_pMate->m_Cost = 0.0;
		
		// 대표 에지의 양끝점을 구하고,
		EgVertex *v1 = e->m_pVert;
		EgVertex *v2 = e->m_pNext->m_pVert;

		// 새로운점의 QEM 행렬을 구하여,
		GMatrix Q = m_QEM[v1->m_Idx] + m_QEM[v2->m_Idx];
		GVector x(3), b(3);
		b.Set(Q[0][3], Q[1][3], Q[2][3]);

		// 새로운 위치를 구하고,
		double det =
			(Q[0][0] * Q[1][1] * Q[2][2] + 2 * Q[0][1] * Q[1][2] * Q[0][2]) -
			(Q[1][2] * Q[1][2] * Q[0][0] + Q[0][2] * Q[0][2] * Q[1][1] + Q[0][1] * Q[0][1] * Q[2][2]);
		if (EQ_ZERO(det, 1.0e-7))
			x = cast_vec(v1->m_Pos + (v2->m_Pos - v1->m_Pos) * 0.5);
		else
			x = Inv3(Q, det) * (-b);

		// 대표 에지 제거 정보를 힙에 추가한다.
		e->m_Cost = x * b + Q[3][3];
		EgCollapseInfo info(e, cast_pt3(x));
		m_Heap.push(info);
	}
}
