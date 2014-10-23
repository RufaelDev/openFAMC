#ifndef _NR_UTIL_H_
#define _NR_UTIL_H_

#include <string>
#include <cmath>
#include <complex>
#include <iostream>
using namespace std;

typedef double DP;

template<class T>
inline const T SQR(const T a) {return a*a;}

template<class T>
inline const T MAX(const T &a, const T &b)
        {return b > a ? (b) : (a);}

inline float MAX(const double &a, const float &b)
        {return b > a ? (b) : float(a);}

inline float MAX(const float &a, const double &b)
        {return b > a ? float(b) : (a);}

template<class T>
inline const T MIN(const T &a, const T &b)
        {return b < a ? (b) : (a);}

inline float MIN(const double &a, const float &b)
        {return b < a ? (b) : float(a);}

inline float MIN(const float &a, const double &b)
        {return b < a ? float(b) : (a);}

template<class T>
inline const T SIGN(const T &a, const T &b)
	{return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

inline float SIGN(const float &a, const double &b)
	{return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

inline float SIGN(const double &a, const float &b)
	{return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}

template<class T>
inline void SWAP(T &a, T &b)
	{T dum=a; a=b; b=dum;}

namespace NR {
	inline void nrerror(const string error_text)
	// Numerical Recipes standard error handler
	{
		cerr << "Numerical Recipes run-time error..." << endl;
		cerr << error_text << endl;
		cerr << "...now exiting to system..." << endl;
		exit(1);
	}
}

#include "mtl/mtl.h"
using namespace mtl;

// mtl Wrapper File
// This is the file that "joins" the mtl Vector<> and Matrix<> classes
// to the NRVec and NRMat classes by the Wrapper Class Method

// NRVec contains a Vector and a &Vector. All its constructors, except the
// conversion constructor, create the Vector and point the &Vector to it.
// The conversion constructor only points the &Vector. All operations
// (size, subscript) are through the &Vector, which as a reference
// (not pointer) has no indirection overhead.

template<class T>
class NRVec {
// Use the std::vector based dense1D for our Vector type
typedef dense1D<T> Vector;
protected:		//access required in NRVec<bool> below
	Vector myvec;
	Vector &myref;
public:
	NRVec<T>() : myvec(), myref(myvec) {}
	explicit NRVec<T>(const int n) : myvec(n), myref(myvec) {}
	NRVec<T>(const T &a, int n) : myvec(n), myref(myvec) {
		for (int i=0; i<n; i++) myvec[i] = a;}
	NRVec<T>(const T *a, int n) : myvec(n), myref(myvec) {
		for (int i=0; i<n; i++) myvec[i] = *a++;}
	NRVec<T>(Vector &rhs) : myref(rhs) {}
	// conversion constructor makes a special NRVec pointing to Vector's data
	// this handles Vector actual args sent to NRVec formal args in functions
	NRVec(const NRVec<T>& rhs) : myvec(rhs.myref.size()), myref(myvec)
		{copy(rhs.myref,myref);}
	// copy constructor. mtl copy constructor
	// does shallow copy only. so use copy() instead
	inline NRVec& operator=(const NRVec& rhs) {
		if (myref.size() != rhs.myref.size())
			myref.resize(rhs.myref.size());
		copy(rhs.myref,myref); return *this;}
	inline int size() const {return myref.size();}
	inline T & operator[](const int i) const {return myref[i];}
	inline operator Vector() const {return myref;}
	// conversion operator to Vector
	// handles NRVec function return types when used in Vector expressions
	~NRVec() {}
};

//The std:vector class has a specialization for vector<bool> that doesn't
//work with the above wrapper class scheme. So implement our own
//specialization as a derived class of NRVec<int>. This could cause
//problems if you mix mtl::Vector<bool> with NRVec<bool>!

template <> class NRVec<bool> : public NRVec<int> {
public:
	NRVec() : NRVec<int>() {}
	explicit NRVec(const int n) : NRVec<int>(n) {}
	NRVec(const bool &a, int n) : NRVec<int>(int(a),n) {}
	NRVec(const bool *a, int n) : NRVec<int>(n) {
		for (int i=0; i<n; i++) myvec[i] = *a++;}
//note: defaults OK for copy constructor and assignment
};

template <class T>
class NRMat {
// Use the matrix generator to select a matrix type
typedef matrix< T,
	rectangle<>,
	dense<>,
	row_major>::type Matrix;
protected:
	Matrix mymat;
	Matrix &myref;
public:
	NRMat() : mymat(), myref(mymat) {}
	NRMat(int n, int m) : mymat(n,m), myref(mymat) {}
	NRMat(const T& a, int n, int m) : mymat(n,m), myref(mymat) {
		for (int i=0; i< n; i++)
			for (int j=0; j<m; j++)
				mymat[i][j] = a;}
	NRMat(const T* a, int n, int m) : mymat(n,m), myref(mymat) {
		for (int i=0; i< n; i++)
			for (int j=0; j<m; j++)
				mymat[i][j] = *a++;}
	NRMat<T>(Matrix &rhs) : myref(rhs) {}
	NRMat(const NRMat& rhs) :
		mymat(rhs.myref.nrows(),rhs.myref.ncols()), myref(mymat)
		{copy(rhs.myref,myref);}
	inline NRMat& operator=(const NRMat& rhs) {
		if (myref.nrows() != rhs.myref.nrows() && myref.ncols() !=
			rhs.myref.ncols()) {
				cerr << "assignment with incompatible matrix sizes\n";
				abort();
		}
		copy(rhs.myref,myref); return *this;
	}
	typename Matrix::OneD operator[](const int i) const {return myref[i];}
	//return type is whatever Matrix returns for a single [] dereference
	inline int nrows() const {return myref.nrows();}
	inline int ncols() const {return myref.ncols();}
	inline operator Matrix() const {return myref;}
	~NRMat() {}
};

template <> class NRMat<bool> : public NRMat<int> {
public:
	NRMat() : NRMat<int>() {}
	explicit NRMat(int n, int m) : NRMat<int>(n,m) {}
	NRMat(const bool &a, int n, int m) : NRMat<int>(int(a),n,m) {}
	NRMat(const bool *a, int n, int m) : NRMat<int>(n,m) {
		for (int i=0; i< n; i++)
			for (int j=0; j<m; j++)
				mymat[i][j] = *a++;}
//note: defaults OK for copy constructor and assignment
};

template <class T>
class NRMat3d {
private:
	int nn;
	int mm;
	int kk;
	T ***v;
public:
	NRMat3d();
	NRMat3d(int n, int m, int k);
	inline T** operator[](const int i);	//subscripting: pointer to row i
	inline const T* const * operator[](const int i) const;
	inline int dim1() const;
	inline int dim2() const;
	inline int dim3() const;
	~NRMat3d();
};

template <class T>
NRMat3d<T>::NRMat3d(): nn(0), mm(0), kk(0), v(0) {}

template <class T>
NRMat3d<T>::NRMat3d(int n, int m, int k) : nn(n), mm(m), kk(k), v(new T**[n])
{
	int i,j;
	v[0] = new T*[n*m];
	v[0][0] = new T[n*m*k];
	for(j=1; j<m; j++)
		v[0][j] = v[0][j-1] + k;
	for(i=1; i<n; i++) {
		v[i] = v[i-1] + m;
		v[i][0] = v[i-1][0] + m*k;
		for(j=1; j<m; j++)
			v[i][j] = v[i][j-1] + k;
	}
}

template <class T>
inline T** NRMat3d<T>::operator[](const int i) //subscripting: pointer to row i
{
	return v[i];
}

template <class T>
inline const T* const * NRMat3d<T>::operator[](const int i) const
{
	return v[i];
}

template <class T>
inline int NRMat3d<T>::dim1() const
{
	return nn;
}

template <class T>
inline int NRMat3d<T>::dim2() const
{
	return mm;
}

template <class T>
inline int NRMat3d<T>::dim3() const
{
	return kk;
}

template <class T>
NRMat3d<T>::~NRMat3d()
{
	if (v != 0) {
		delete[] (v[0][0]);
		delete[] (v[0]);
		delete[] (v);
	}
}

//The next 3 classes are used in artihmetic coding, Huffman coding, and
//wavelet transforms respectively. This is as good a place as any to put them!

class arithcode {
private:
	NRVec<unsigned long> *ilob_p,*iupb_p,*ncumfq_p;
public:
	NRVec<unsigned long> &ilob,&iupb,&ncumfq;
	unsigned long jdif,nc,minint,nch,ncum,nrad;
	arithcode(unsigned long n1, unsigned long n2, unsigned long n3)
		: ilob_p(new NRVec<unsigned long>(n1)),
		iupb_p(new NRVec<unsigned long>(n2)),
		ncumfq_p(new NRVec<unsigned long>(n3)),
		ilob(*ilob_p),iupb(*iupb_p),ncumfq(*ncumfq_p) {}
	~arithcode() {
		if (ilob_p != 0) delete ilob_p;
		if (iupb_p != 0) delete iupb_p;
		if (ncumfq_p != 0) delete ncumfq_p;
	}
};

class huffcode {
private:
	NRVec<unsigned long> *icod_p,*ncod_p,*left_p,*right_p;
public:
	NRVec<unsigned long> &icod,&ncod,&left,&right;
	int nch,nodemax;
	huffcode(unsigned long n1, unsigned long n2, unsigned long n3,
		unsigned long n4) :
		icod_p(new NRVec<unsigned long>(n1)),
		ncod_p(new NRVec<unsigned long>(n2)),
		left_p(new NRVec<unsigned long>(n3)),
		right_p(new NRVec<unsigned long>(n4)),
		icod(*icod_p),ncod(*ncod_p),left(*left_p),right(*right_p) {}
	~huffcode() {
		if (icod_p != 0) delete icod_p;
		if (ncod_p != 0) delete ncod_p;
		if (left_p != 0) delete left_p;
		if (right_p != 0) delete right_p;
	}
};

class wavefilt {
private:
	NRVec<DP> *cc_p,*cr_p;
public:
	int ncof,ioff,joff;
	NRVec<DP> &cc,&cr;
	wavefilt() : cc(*cc_p),cr(*cr_p) {}
	wavefilt(const DP *a, const int n) :  //initialize to array
		cc_p(new NRVec<DP>(n)),cr_p(new NRVec<DP>(n)),
		ncof(n),ioff(-(n >> 1)),joff(-(n >> 1)),cc(*cc_p),cr(*cr_p) {
			int i;
			for (i=0; i<n; i++)
				cc[i] = *a++;
			DP sig = -1.0;
			for (i=0; i<n; i++) {
				cr[n-1-i]=sig*cc[i];
				sig = -sig;
			}
	}
	~wavefilt() {
		if (cc_p != 0) delete cc_p;
		if (cr_p != 0) delete cr_p;
	}
};

//Overloaded complex operations to handle mixed float and double
//This takes care of e.g. 1.0/z, z complex<float>

inline const complex<float> operator+(const double &a,
	const complex<float> &b) { return float(a)+b; }

inline const complex<float> operator+(const complex<float> &a,
	const double &b) { return a+float(b); }

inline const complex<float> operator-(const double &a,
	const complex<float> &b) { return float(a)-b; }

inline const complex<float> operator-(const complex<float> &a,
	const double &b) { return a-float(b); }

inline const complex<float> operator*(const double &a,
	const complex<float> &b) { return float(a)*b; }

inline const complex<float> operator*(const complex<float> &a,
	const double &b) { return a*float(b); }

inline const complex<float> operator/(const double &a,
	const complex<float> &b) { return float(a)/b; }

inline const complex<float> operator/(const complex<float> &a,
	const double &b) { return a/float(b); }

//some compilers choke on pow(float,double) in single precision. also atan2

inline float pow (float x, double y) {return pow(double(x),y);}
inline float pow (double x, float y) {return pow(x,double(y));}
inline float atan2 (float x, double y) {return atan2(double(x),y);}
inline float atan2 (double x, float y) {return atan2(x,double(y));}
#endif /* _NR_UTIL_H_ */
