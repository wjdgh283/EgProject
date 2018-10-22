#pragma once

#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <cassert>
#include <stdarg.h>
#include "egmath_util.h"
#include "egmath_opt.h"

// ��ũ�� ���
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616

// ��ũ�� ����
#define SQRT(X)		sqrt((X))
#define SQR(X)		((X) * (X))
#define DEG2RAD(X)	((X) * (M_PI) / (180.0))
#define RAD2DEG(X)	((X) * (180.0) / (M_PI))
#define SWAP(type, x, y) { type temp = (x); (x) = (y); (y) = temp; }
#define MIN(x, y)	((x) > (y) ? (y) : (x))
#define MAX(x, y)	((x) > (y) ? (x) : (y))
#define ABS(X)		(((X) > 0.0) ? (X) : (-(X)))
#define SIGN(a)		((a) > 0.0 ? (1.0) : (-1.0))
#define SIGN1(a, b) ((b) > 0.0 ? ABS(a) : -ABS(a))
#define SIGN2(a, b)	((b) >= 0.0 ? fabs(a) : -fabs(a))
#define PYTHAG(a, b) SQRT((SQR(a) + SQR(b)))
#define EQ(X, Y, EPS)	(ABS((X) - (Y)) < EPS)
#define EQ_ZERO(X, EPS) (ABS(X) < EPS)
#define ARR_ZERO(A, N) memset((A), 0, sizeof(A[0]) * (N))
#define ARR_COPY(D, S, N) memmove((D), (S), sizeof(S[0]) * (N))
#define ARR_COPY_2D(D, S, M, N) \
	for (int i = 0; i < (M); ARR_COPY(D[i], S[i], N), ++i);
#define ARR_ALLOC(D, TYPE, N) D = new TYPE [(N)]
#define ARR_ALLOC_2D(D, TYPE, M, N) \
	D = new TYPE *[(M)]; \
	for (int i = 0; i < (M); D[i] = new TYPE [(N)], ++i);
#define ARR_FREE(D) \
	delete [] D; \
	D = NULL
#define ARR_FREE_2D(D, M) \
	for (int i = 0; i < (M); delete [] D[i], ++i); \
	delete [] D; \
	D = NULL
#define MTYPE_EPS	1.0e-15

// Ŭ���� ����
class GVector3;
class GVector4;
class GVector;
class GPoint3;
class GMatrix;
class GQuater;
class GTransf;
class GLine;
class GPlane;
class GCtrlPt;

/*!
*	\class GVector2
*	\brief 2���� ���͸� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001	
*/
class GVector2
{
	// ������ �Լ� �� Ŭ����
	friend GVector2 operator +(const GVector2 &lhs, const GVector2 &rhs);
	friend GVector2 operator -(const GVector2 &lhs, const GVector2 &rhs);
	friend GVector2 operator *(const GVector2 &lhs, const double &s);
	friend GVector2 operator *(const double &s, const GVector2 &rhs);
	friend double operator *(const GVector2 &lhs, const GVector2 &rhs);
	friend GVector2 operator /(const GVector2 &lhs, const double &s);
	friend bool operator ==(const GVector2 &lhs, const GVector2 &rhs);
	friend bool operator !=(const GVector2 &lhs, const GVector2 &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GVector2 &v);
	friend std::istream &operator >>(std::istream &is, GVector2 &v);
	friend GVector2 proj(const GVector2 &v, const GVector2 &w);
	friend double dist(const GVector2 &v, const GVector2 &w);
	friend double norm(const GVector2 &v);
	friend double angle(const GVector2 &v, const GVector2 &w, bool radian = false);
	friend GVector cast_vec(const GVector2 &v);
	friend GVector3 cast_vec3(const GVector2 &v);

public:
	// ������ �� �Ҹ���
	GVector2(double x = 0.0, double y = 0.0);
	GVector2(const GVector2 &cpy);
	~GVector2();

	// ���� �� ���մ��� ������
	GVector2 &operator =(const GVector2 &rhs);
	GVector2 &operator +=(const GVector2 &rhs);
	GVector2 &operator -=(const GVector2 &rhs);
	GVector2 &operator *=(const double &s);
	GVector2 &operator /=(const double &s);
	
	// ���� ������
	GVector2 operator +() const;
	GVector2 operator -() const;

	// ÷�� ������
	double &operator [](const int &idx);
	const double &operator [](const int &idx) const;

	// ����Լ�
	GVector2 &Set(const double &x, const double &y);
	GVector2 &Normalize();

	// ���� �ɹ��Լ�
	static void SetPrecision(double error);
	static double GetPrecision();

public:
	// ������ ���
	/*! \brief 2���� ������ ���Ҹ� �����ϴ� �Ǽ� �迭 */
	double V[2];

	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GVector3
*	\brief 3���� ���͸� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001	
*/
class GVector3
{
	// ������ �Լ� �� Ŭ����
	friend GVector3 operator +(const GVector3 &lhs, const GVector3 &rhs);
	friend GPoint3 operator +(const GVector3 &lhs, const GPoint3 &rhs);
	friend GVector3 operator -(const GVector3 &lhs, const GVector3 &rhs);
	friend GVector3 operator *(const GVector3 &lhs, const double &s);
	friend GVector3 operator *(const double &s, const GVector3 &rhs);
	friend double operator *(const GVector3 &lhs, const GVector3 &rhs);
	friend GVector3 operator /(const GVector3 &lhs, const double &s);
	friend GVector3 operator ^(const GVector3 &lhs, const GVector3 &rhs);
	friend bool operator ==(const GVector3 &lhs, const GVector3 &rhs);
	friend bool operator !=(const GVector3 &lhs, const GVector3 &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GVector3 &v);
	friend std::istream &operator >>(std::istream &is, GVector3 &v);
	friend GVector3 proj(const GVector3 &v, const GVector3 &w);
	friend GVector3 ortho(const GVector3 &v);
	friend double dist(const GVector3 &v, const GVector3 &w);
	friend double det(const GVector3 &u, const GVector3 &v, const GVector3 &w);
	friend double norm(const GVector3 &v);
	friend double angle(const GVector3 &v, const GVector3 &w, bool radian = false);
	friend double angle(const GVector3 &v, const GVector3 &w, const GVector3 &up, bool radian = false);
	friend GPoint3 cast_pt3(const GVector3 &v);
	friend GVector cast_vec(const GVector3 &v);
	friend GVector4 cast_vec4(const GVector3 &v, double w = 0.0);

public:
	// ������ �� �Ҹ���
	GVector3(double x = 0.0, double y = 0.0, double z = 0.0);
	GVector3(const GVector3 &cpy);
	~GVector3();

	// ���� �� ���մ��� ������
	GVector3 &operator =(const GVector3 &rhs);
	GVector3 &operator +=(const GVector3 &rhs);
	GVector3 &operator -=(const GVector3 &rhs);
	GVector3 &operator *=(const double &s);
	GVector3 &operator /=(const double &s);
	GVector3 &operator ^=(const GVector3 &rhs);

	// ���� ������
	GVector3 operator +() const;
	GVector3 operator -() const;

	// ÷�� ������
	double &operator [](const int &idx);
	const double &operator [](const int &idx) const;

	// ����Լ�
	GVector3 &Set(const double &x, const double &y, const double &z);
	GVector3 &Normalize();
		
	// ���� �ɹ��Լ�
	static void SetPrecision(double error);
	static double GetPrecision();

public:
	// ������ ���
	/*! \brief 3���� ������ ���Ҹ� �����ϴ� �Ǽ� �迭 */
	double V[3];

	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GVector4
*	\brief 4���� ���͸� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001	
*/
class GVector4
{
	// ������ �Լ� �� Ŭ����
	friend GVector4 operator +(const GVector4 &lhs, const GVector4 &rhs);
	friend GVector4 operator -(const GVector4 &lhs, const GVector4 &rhs);
	friend GVector4 operator *(const GVector4 &lhs, const double &s);
	friend GVector4 operator *(const double &s, const GVector4 &rhs);
	friend double operator *(const GVector4 &lhs, const GVector4 &rhs);
	friend GVector4 operator /(const GVector4 &lhs, const double &s);
	friend bool operator ==(const GVector4 &lhs, const GVector4 &rhs);
	friend bool operator !=(const GVector4 &lhs, const GVector4 &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GVector4 &v);
	friend std::istream &operator >>(std::istream &is, GVector4 &v);
	friend double norm(const GVector4 &v);
	friend double dist(const GVector4 &v, const GVector4 &w);
	friend GVector cast_vec(const GVector4 &v);
	friend GVector3 cast_vec3(const GVector4 &v);
	friend GPoint3 cast_pt3(const GVector4 &v);
	friend GVector4 homogenize(const GVector4 &v);
	friend GVector4 cartesianize(const GVector4 &v);
	friend GVector4 rationalize(const GVector4 &v);

public:
	// ������ �� �Ҹ���
	GVector4(double x = 0.0, double y = 0.0, double z = 0.0, double w = 0.0);
	GVector4(const GVector4 &cpy);
	~GVector4();

	// ���� �� ���մ��� ������
	GVector4 &operator =(const GVector4 &rhs);
	GVector4 &operator +=(const GVector4 &rhs);
	GVector4 &operator -=(const GVector4 &rhs);
	GVector4 &operator *=(const double &s);
	GVector4 &operator /=(const double &s);
	
	// ���� ������
	GVector4 operator +() const;
	GVector4 operator -() const;

	// ÷�� ������
	double &operator [](const int &idx);
	const double &operator [](const int &idx) const;

	// ����Լ�
	GVector4 &Set(double x, double y, double z, double w);
	GVector4 &Normalize();

	// �����ɹ��Լ�
	static void SetPrecision(double error);
	static double GetPrecision();

public:
	// ������ ���
	/*! \brief 4���� ������ ���Ҹ� �����ϴ� �Ǽ� �迭 */
	double V[4];

	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GVector
*	\brief \a n���� ���� �ڷ����� ��Ÿ���� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001	
*/
class GVector
{
	// ������ �Լ� �� Ŭ����
	friend GVector operator +(const GVector &lhs, const GVector &rhs);
	friend GVector operator -(const GVector &lhs, const GVector &rhs);
	friend double operator *(const GVector &lhs, const GVector &rsh);
	friend GVector operator *(const double &s, const GVector &rhs);
	friend GVector operator *(const GVector &lhs, const double &s);
	friend GVector operator *(const GMatrix &lhs, const GVector &rhs);
	friend GMatrix operator *(const GVector &lhs, const GMatrix &rhs);
	friend GVector operator /(const GVector &lhs, const double &s);
	friend bool operator >(const GVector &lhs, const GVector &rhs);
	friend bool operator >=(const GVector &lhs, const GVector &rhs);
	friend bool operator <(const GVector &lhs, const GVector &rhs);
	friend bool operator <=(const GVector &lhs, const GVector &rhs);
	friend bool operator ==(const GVector &lhs, const GVector &rhs);
	friend bool operator !=(const GVector &lhs, const GVector &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GVector &v);
	friend std::istream &operator >>(std::istream &is, GVector &v);
	friend double norm(const GVector &v);
	friend double dist(const GVector &v, const GVector &w);
	friend int dcmp_lu(GMatrix &A, GVector &Idx, double &d);
	friend void dcmp_sv(const GMatrix &A, GMatrix &U, GVector &S, GMatrix &V);
	friend void lin_sys_lu(const GMatrix &A, GVector &x, const GVector &b, const GVector &Idx);
	friend double lin_sys_sv(const GMatrix &A, GVector &x, const GVector &b);
	friend double lin_sys_sv(const GMatrix &U, const GVector &S, const GMatrix &V, GVector &x, const GVector &b);
	friend int lin_sys_cg(const GMatrix &A, GVector &x, const GVector &b, int iter);
	friend int lin_sys_qr(const GMatrix &A, GVector &x, const GVector &b);
	friend int eigen_sys(const GMatrix &A, GVector &D, GMatrix &V);
	friend void pca(GMatrix &Data, GMatrix &PC, GVector &SigVal, GVector &Mean);
	friend double *cast_arr(const GVector &v);
	friend GPoint3 cast_pt3(const GVector &v);
	friend GVector3 cast_vec3(const GVector &v);
	friend GQuater cast_quat(const GVector &v);
	friend class GMatrix;
	friend class RBFinterp;

public:
	// ������ �� �Ҹ���
	GVector(int dim = 3);
	GVector(int dim, double x, ...);
	GVector(int dim, int x, ...);
	GVector(const GVector &cpy);
	~GVector();

	// ���Կ����� �� ���մ��� ������
	GVector &operator =(const GVector &rhs);
	GVector &operator +=(const GVector &rhs);
	GVector &operator -=(const GVector &rhs);
	GVector &operator *=(const double &s);
	GVector	&operator /=(const double &s);

	// ���� ������
	GVector operator +() const;
	GVector operator -() const;

	// �ε��� ������
	double &operator [](const int &idx);
	const double &operator [](const int &idx) const;

	// ����Լ�
	GVector	&Set(double x, ...);
	GVector	&Set(int x, ...);
	GVector &Set(double *pVal);
	GVector	&Normalize();
	GVector &SetZeros();
	int GetDim() const;

	// �����Լ�
	static void	SetPrecision(double error);
	static double GetPrecision();

public:
	// ������ ���
	/*! \brief ������ ���� */
	int N;

	/*! \brief ������ ���Ҹ� �����ϴ� �Ǽ� �迭 */
	double *V;

	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GPoint3
*	\brief 3���� ����Ʈ �ڷ����� ��Ÿ���� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001	
*/
class GPoint3
{
	// ������ �Լ� �� Ŭ����
	friend GVector3 operator -(const GPoint3 &lhs, const GPoint3 &rhs);
	friend GPoint3 operator -(const GPoint3 &lhs, const GVector3 &rhs);
	friend GPoint3 operator +(const GPoint3 &lhs, const GVector3 &rhs);
	friend GPoint3 operator +(const GVector3 &lhs, const GPoint3 &rhs);
	friend bool operator ==(const GPoint3 &lhs, const GPoint3 &rhs);
	friend bool operator !=(const GPoint3 &lhs, const GPoint3 &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GPoint3 &p);
	friend std::istream &operator >>(std::istream &is, GPoint3 &p);
	friend double norm(const GPoint3 &p);
	friend double dist(const GPoint3 &p, const GPoint3 &q);
	friend double dist_sq(const GPoint3 &p, const GPoint3 &q);
	friend GPoint3 affine_sum(int N, ...);
	friend GPoint3 affine_sum(GPoint3 *Points, double *Weights, const int Size);
	friend GVector3 cast_vec3(const GPoint3 &p);
	friend GVector4 cast_vec4(const GPoint3 &p, double w = 0.0);
	friend GVector cast_vec(const GPoint3 &p);
	friend GPoint3 round_off(const GPoint3 &p, int place);
	friend class GLScene;

public:
	// ������ �� �Ҹ���
	GPoint3(double x = 0.0, double y = 0.0, double z = 0.0);
	GPoint3(const GPoint3 &cpy);
	~GPoint3();

	// ���� ������
	GPoint3 &operator =(const GPoint3 &rhs);
	GPoint3 &operator +=(const GVector3 &rhs);

	// ÷�� ������
	double &operator [](const int &idx);
	const double &operator [](const int &idx) const;

	// ����Լ�
	GPoint3 &Set(const double &x, const double &y, const double &z);
	
	// ���� ����Լ�
	static void SetPrecision(double error);
	static double GetPrecision();
   
public:
	/*! \brief 3���� ������ ���Ҹ� �����ϴ� �Ǽ��迭 */	
	double V[3];	
	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;	
};

/*!
*	\class GMatrix
*	\brief \a m �� \a n ���� ũ�⸦ ���� ����� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001
*/
class GMatrix
{
	// ������ �Լ� �� Ŭ����
	friend GMatrix operator +(const GMatrix &lhs, const GMatrix &rhs);
	friend GMatrix operator -(const GMatrix &lhs, const GMatrix &rhs);
	friend GMatrix operator *(const GMatrix &lhs, const GMatrix &rhs);
	friend GMatrix operator /(const GMatrix &lhs, const double &s);
	friend bool operator ==(const GMatrix &lhs, const GMatrix &rhs);
	friend bool operator !=(const GMatrix &lhs, const GMatrix &rhs);
	friend GVector operator *(const GMatrix &lhs, const GVector &rhs);
	friend GMatrix operator *(const GVector &lhs, const GMatrix &rhs);
	friend GMatrix operator *(const GMatrix &lhs, const double &s);
	friend GMatrix operator *(const double &s, const GMatrix &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GMatrix &m);
	friend GMatrix tr(const GMatrix &m);
	friend GMatrix ref(const GMatrix &m);
	friend GMatrix rref(const GMatrix &m);
	friend GMatrix basis_null(const GMatrix &m);
	friend double *cast_arr(const GMatrix &m);
	friend int rank(const GMatrix &m);
	friend int nullity(const GMatrix &m);
	friend double det(const GMatrix &A);
	friend GMatrix inv(const GMatrix &A);
	friend GMatrix sub(const GMatrix &A, int r0, int c0, int r1, int c1);
	friend int dcmp_lu(const GMatrix &A, GMatrix &L, GMatrix &U);
	friend int dcmp_lu(GMatrix &A, GVector &Idx, double &d);
	friend int dcmp_qr(const GMatrix &A, GMatrix &Q, GMatrix &R);
	friend void dcmp_sv(const GMatrix &A, GMatrix &U, GVector &S, GMatrix &V);
	friend void lin_sys_lu(const GMatrix &A, GVector &x, const GVector &b, const GVector &Idx);
	friend double lin_sys_sv(const GMatrix &A, GVector &x, const GVector &b);
	friend double lin_sys_sv(const GMatrix &U, const GVector &S, const GMatrix &V, GVector &x, const GVector &b);
	friend int lin_sys_cg(const GMatrix &A, GVector &x, const GVector &b, int iter);
	friend int lin_sys_qr(const GMatrix &A, GVector &x, const GVector &b);
	friend int eigen_sys(const GMatrix &A, GVector &D, GMatrix &V);
	friend void pca(GMatrix &Data, GMatrix &PC, GVector &SigVal, GVector &Mean);

public:
	// ������ �� �Ҹ���
	GMatrix(int row = 4, int col = 4, double *elem = NULL);
	GMatrix(const GMatrix &cpy);
	~GMatrix();

	// ���� ������ �� ���մ��� ������
	GMatrix &operator =(const GMatrix &rhs);
	GMatrix &operator +=(const GMatrix &rhs);
	GMatrix &operator -=(const GMatrix &rhs);
	GMatrix &operator *=(const GMatrix &rhs);
	GMatrix &operator *=(const double &s);
	GMatrix &operator /=(const double &s);

	// ���� ������
	GMatrix operator +() const;
	GMatrix operator -() const;

	// ÷�� ������
	double *operator [](const int idx);
	const double *operator [](const int idx) const;

	// ����Լ�
	GMatrix &SetTranspose();
	GMatrix &SetIdentity();
	GMatrix &SetZeros();
	GMatrix &SetRowVec(const int idx, const GVector &v);
	GMatrix &SetColVec(const int idx, const GVector &v);
	GMatrix &ExchangeRows(const int idx0, const int idx1);
	GMatrix &ExchangeCols(const int idx0, const int idx1);
	int GetRowNum() const;
	int GetColNum() const;
	GVector GetRowVec(const int idx) const;
	GVector GetColVec(const int idx) const;
	bool IsSquare() const;

	// �����Լ�
	static void SetPrecision(double error);
	static double GetPrecision();

public:
	/*! \brief ����� ���� ���� */	
	int r;
	/*! \brief ����� ���� ���� */	
	int c;
	/*! \brief ����� ���Ҹ� �����ϴ� 1���� �Ǽ� �迭 */	
	double *M;
	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GQuater		   
*	\brief ���ʹϿ� (q = W + iX + jY + kZ)�� ��Ÿ���� Ŭ����.
*	\warning �������ʹϿ��� �ƴѰ�쵵 ����.
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001	
*/
enum TypeEuler
{
	EULER_XYZ = 0,
	EULER_ZYX = 1,
};

enum TypeArcBall
{ 
	ARCBALL_FREE = 0, 
	ARCBALL_X, 
	ARCBALL_Y, 
	ARCBALL_Z 
};

class GQuater
{
	// ����Ʈ �Լ� �� Ŭ����
	friend GQuater operator +(const GQuater &lhs, const GQuater &rhs);
	friend GQuater operator -(const GQuater &lhs, const GQuater &rhs);
	friend GQuater operator *(const GQuater &lhs, const GQuater &rhs);
	friend GQuater operator /(const GQuater &lhs, const GQuater &rhs);
	friend GQuater operator /(const GQuater &lhs, const double s);
	friend GPoint3 operator *(const GQuater &lhs, const GPoint3 &rhs);
	friend GVector3 operator *(const GQuater &lhs, const GVector3 &rhs);
	friend bool operator ==(const GQuater &lhs, const GQuater &rhs);
	friend bool operator !=(const GQuater &lhs, const GQuater &rhs);
	friend GQuater operator *(const GQuater &lhs, const double &s);
	friend GQuater operator *(const double &s, const GQuater &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GQuater &q);
	friend std::istream &operator >>(std::istream &is, GQuater &q);
	friend double norm(const GQuater &q);
	friend GQuater inv(const GQuater &q);
	friend GQuater exp(const GVector3 &v);
	friend GVector3 log(const GQuater &q);
	friend double quat_dot(const GQuater &q1, const GQuater &q2);
	friend GQuater slerp(GQuater q1, GQuater q2, double t);
	friend GMatrix cast_mat(const GQuater &q, bool isGL = true);
	friend GVector cast_vec(const GQuater &q);
	friend GVector4 cast_vec4(const GQuater &q);
	friend GQuater cast_qt(const GVector4 &v);

public:
	// ������ �� �Ҹ���
	GQuater(double w = 1.0, double x = 0.0, double y = 0.0, double z = 0.0);
	GQuater(const GQuater &cpy);
	GQuater(const double *q, const bool invOrder = false);
	GQuater(GVector3 axis, double theta, bool radian = false);
	GQuater(double theta1, double theta2, double theta3, TypeEuler eulerType = EULER_XYZ);
	GQuater(GVector3 x_axis, GVector3 y_axis);
	~GQuater();

	// ���� ������
	GQuater &operator =(const GQuater &rhs);

	// ���մ��� ������
	GQuater &operator +=(const GQuater &rhs);
	GQuater &operator -=(const GQuater &rhs);
	GQuater &operator *=(const GQuater &rhs);
	GQuater &operator /=(const GQuater &rhs);
	GQuater &operator *=(const double s);
	GQuater &operator /=(const double s);

	// ���� ������
	GQuater operator +() const;
	GQuater operator -() const;

	// Set ����Լ�
	GQuater &Set(const double w, const double x, const double y, const double z);
	GQuater &Set(double *q, bool invOrder = false);
	GQuater &SetIdentity();
	GQuater &SetInverse();
	GQuater &SetConjugate();
	GQuater &SetNegate();
	GQuater &SetFromArcBall(GVector3 p, GVector3 q, TypeArcBall arcType = ARCBALL_FREE);
	GQuater &SetFromAngleAxis(const double theta, GVector3 axis, bool radian = false);
	GQuater &SetFromEulerAngle(double theta1, double theta2, double theta3, TypeEuler eulerType = EULER_XYZ);
	GQuater &SetFromFrameXY(const GVector3 &x_axis, const GVector3 &y_axis);
	GQuater &SetFromFrameYZ(const GVector3 &y_axis, const GVector3 &z_axis);
	GQuater &SetFromFrameZX(const GVector3 &z_axis, const GVector3 &x_axis);
	GQuater &SetFromMatrix(double *mat, bool isGL);
	GQuater &Normalize();

	// Get ����Լ�
	void	GetAngleAxis(GVector3 &axis, double &angle, bool radian = false) const;
	void	GetEulerAngle(double &theta1, double &theta2, double &theta3, bool radian = false, TypeEuler eulerType = EULER_XYZ);
	double	GetW() const;
	double	GetX() const;
	double	GetY() const;
	double	GetZ() const;
	
	// ����Լ�
	bool IsUnitQuater() const;
	bool IsIdentity() const;
	
	// ���� ����Լ�
	static void SetPrecision(double error);
	static double GetPrecision();
	
public:
	// ������ ���
	/*! \brief ���ʹϿ��� �Ǽ��� */
	double W;
	/*! \brief ���ʹϿ��� ����� */
	double X, Y, Z;
	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GLine
*	\brief 3���� �������� ������ ��Ÿ���� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001	
*/
class GLine
{
	// ������ �Լ� �� Ŭ����
	friend bool operator ==(const GLine &lhs, const GLine &rhs);
	friend bool operator !=(const GLine &lhs, const GLine &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GLine &l);
	friend double dist(const GLine &l, const GPoint3 &q);
	friend double dist(const GLine &l0, const GLine &l1, double &t1, double &t2);
	friend bool intersect_line_line(GPoint3 &p, const GLine &l0, const GLine &l1, double &t0, double &t1);
	friend bool intersect_line_line2(GPoint3 &p, GLine &l0, GLine &l1);
	friend bool intersect_line_plane(GPoint3 &p, const GLine &l, const GPlane &pi);
	friend bool intersect_line_triangle(const GPoint3 &p1, const GPoint3 &p2, const GPoint3 &p3, const GLine &l, double &t, GPoint3 &q, double &alpha, double &beta, double &gamma, const bool bCull = true);
	friend bool intersect_line_triangle(const GPoint3 &p1, const GPoint3 &p2, const GPoint3 &p3, const GLine &l, GPoint3 &ispt, const bool bCull = true);
	friend bool intersect_planes(GLine &l, const GPlane &pi1, const GPlane &pi2);
	friend GLine operator *(const GTransf &X, const GLine &l);

public:
	// ������ �� �Ҹ���
	GLine(const GPoint3 &_p = GPoint3(0, 0, 0), const GVector3 &_v = GVector3(0, 0, 0));
	GLine(const GPoint3 &_p, const GPoint3 &_q);
	GLine(const GLine &cpy);
	~GLine();

	// ���� ������
	GLine &operator =(const GLine &rhs);
	
	// ����Լ�
	GLine &SetPt(const GPoint3 &_p);
	GLine &SetDir(const GVector3 &_v);
	GPoint3 GetPt() const;
	GVector3 GetDir() const;
	bool IsOnLine(const GPoint3 &q) const;
	GPoint3 Eval(const double t) const;

	// ��������Լ�
	static void SetPrecision(double err);
	static double GetPrecision();

public:
	// ������ ���.
	/*! \brief �������� �� �� */
	GPoint3 p;
	/*! \brief ������ ���� ���� l(t) = p + v * t */
	GVector3 v;
	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GPlane
*	\brief ����� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu)
*	\date 01 Jan 2001	
*/
class GPlane
{
	// ������ �Լ� �� Ŭ����
	friend bool operator ==(const GPlane &lhs, const GPlane &rhs);
	friend bool operator !=(const GPlane &lhs, const GPlane &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GPlane &pi);
	friend GPlane operator *(const GTransf &X, const GPlane &pi);
	friend double dist(const GPlane &pi, const GPoint3 &p);
	friend GPoint3 proj(const GPoint3 &p, const GPlane &pi);
	friend bool intersect_line_plane(GPoint3 &p, const GLine &l, const GPlane &pi);
	friend bool intersect_planes(GLine &l, const GPlane &pi1, const GPlane &pi2);
	friend bool intersect_planes(GPoint3 &p, const GPlane &pi1, const GPlane &pi2, const GPlane &pi3);

public:
	// ������ �� �Ҹ���
	GPlane(const GVector3 &_n, const GPoint3 &_p);
	GPlane(const GPoint3 &p1, const GPoint3 &p2, const GPoint3 &p3);
	GPlane(const GVector3 &_n, const double &_d);
	GPlane(const double &a, const double &b, const double &c, const double &d);
	GPlane(const GPlane &cpy);
	~GPlane();

	// ���� ������
	GPlane &operator =(const GPlane &rhs);
	
	// ����Լ�
	GVector3 GetNormal() const;
    bool IsOnPlane(const GPoint3 &p) const;
	bool IsAbovePlane(const GPoint3 &p) const;
	bool IsBelowPlane(const GPoint3 &p) const;

	// ���� ����Լ�
	static void SetPrecision(const double err);
	static double GetPrecision();
    
public:
	// ������ ���
	/*! \brief ����� ������(ax + by + cz + d = 0)���� ���� ���� n=(a, b, c) */
	GVector3 n;
	/*! \brief ����� ������(ax + by + cz + d = 0)���� ��� d(= -n * p) */
	double d;
	/*! \brief ��ȣ �� �ε�ȣ �������� �����Ѱ� */
	static double Precision;
};

/*!
*	\class GTransf
*	\brief ��ü��ȯ(rigid-body transformation)�� ��Ÿ���� Ŭ����.
*
*	\author ������(shyun@dongguk.edu)
*	\date 2001�� 1�� 1��.
*/
class GTransf
{
	// ������ �Լ� �� Ŭ����
	friend GTransf operator *(const GTransf &lhs, const GTransf &rhs);
	friend bool operator ==(const GTransf &lhs, const GTransf &rhs);
	friend bool operator !=(const GTransf &lhs, const GTransf &rhs);
	friend std::ostream &operator <<(std::ostream &os, const GTransf &X);
	friend GTransf operator *(const GTransf &lhs, const double &s);
	friend GTransf operator *(const double &s, const GTransf &rhs);
	friend GPoint3 operator *(const GTransf &X, const GPoint3 &p);
	friend GLine operator *(const GTransf &X, const GLine &l);
	friend GPlane operator *(const GTransf &X, const GPlane &pi);
	friend GVector3 operator *(const GTransf &X, const GVector3 &v);
	friend GTransf inv(const GTransf &X);
	friend GVector3 part_trans(const GTransf &X);
	friend GQuater part_rot(const GTransf &X);
	friend GMatrix cast_mat(const GTransf &X, bool isGL = true);
	friend double norm(const GTransf &X);
	friend double dist(const GTransf &X1, const GTransf &X2);
	friend GTransf ad(const GTransf &X1, const GTransf &dX, const GTransf &X2);
	friend GTransf lerp(GTransf X1, GTransf X2, const double t);
	
public:
	// ������ �� �Ҹ���
	GTransf(const GVector3 &_t = GVector3(0.0, 0.0, 0.0), const GQuater &_q = GQuater(1.0, 0.0, 0.0, 0.0));
	GTransf(double *mat, bool isGL);
	GTransf(const GTransf &cpy);
	~GTransf();

	// ���� ������ �� ���մ��� ������
	GTransf &operator =(const GTransf &rhs);
	GTransf &operator *=(const GTransf &rhs);
	GTransf &operator *=(const double &s);

	// ����Լ�
	void SetNegate();
	void SetTrans(const GVector3 &_t);
	void SetRot(const GQuater &_q);
   
public:
	// ������ ���
	/*! \brief 3���� �̵���ȯ */
    GVector3 t;
	/*! \brief 3���� ȸ����ȯ */
	GQuater q;
};

/*!
*	\class RBFinterp
*	\brief Radial basis�� ����� �����Լ��� ǥ���ϴ� Ŭ����
*
*	\author ������(shyun@dongguk.edu) 
*	\date 03 Dec. 2008	
*/
enum RBFType
{ 
	RBF_GAUSSIAN = 0, 
	RBF_BIHARMONIC, 
	RBF_TRIHARMONIC, 
	RBF_MULTIQUADRIC, 
	RBF_THINPLATE,
};

class RBFinterp
{
public:
	// ������ �� �Ҹ���
	RBFinterp(const int &_m = 0, const int &_n = 0, RBFType _type = RBF_GAUSSIAN);
	RBFinterp(const RBFinterp &cpy);
	~RBFinterp();

	// ���� ������
	RBFinterp &operator =(const RBFinterp &rhs);
	
	// ����Լ�
	int GetDimParam() const;
	int GetDimValue() const;
	RBFType GetBasisType() const;
	GMatrix GetCoeffMatrix() const;
	std::vector<GVector> &GetParamList();
	std::vector<GVector> &GetValueList();
	void SetDimParam(const int &_m);
	void SetDimValue(const int &_n);
	void SetBasisType(RBFType _type);
	void AddData(const GVector &Param, const GVector &Value);
	void Interpolate();
	GVector Eval(double x0, ...);
	GVector Eval(const GVector &X);

protected:
	// ���λ��
	double basis(double r2);

public:
	// ������ ���
	/*! \brief �Ķ���� ������ ���� */
	int m;

	/*! \brief ������ ������ ���� */
	int n;

	/*! \brief Radial basis �Լ��� Ÿ�� */
	RBFType type;

	/*! \brief �������� ����� ����� ��� */
	GMatrix C;

	/*! \brief �����Ķ������ \a m ���� ���� */
	std::vector<GVector> Params;

	/*! \brief ������������ \a n ���� ���� */
	std::vector<GVector> Values;	
};
