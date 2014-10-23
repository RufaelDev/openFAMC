/*
 * =====================================================================================
 * 
 *        Filename:  PointT.hh
 * 
 *     Description:  Template class for 3D Points compatible to 
 *                   Arrays of Scalar values of size 3          
 * 
 *         Version:  1.0
 *         Created:  16/05/07 21:11:05 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */


#ifndef  POINT_T_HH
#define  POINT_T_HH

#include <cstdio>
#include <cmath>
#include <cassert>

template<typename Scalar>
class PointT{

    //TYPE DEFINITIONS
    public:
	typedef Scalar value_type;
    
    //CONSTRUCTORS
    public:
	inline PointT(){
	    data_[0]=Scalar(0);
	    data_[1]=Scalar(0);
	    data_[2]=Scalar(0);
	}

	inline PointT(const Scalar& _s0, const Scalar& _s1, const Scalar& _s2){
	    data_[0]=_s0;
	    data_[1]=_s1;
	    data_[2]=_s2;
	}

	inline explicit PointT(const Scalar _data[3]){
	    data_[0]=_data[0];
	    data_[1]=_data[1];
	    data_[2]=_data[2];
	}
	
	template<typename OtherScalar>
	inline explicit PointT(const PointT<OtherScalar>& _p){
	    operator=(_p);
	}
	
    //MEMBER FUNCTIONS
    public:

	//assignement (also with a different scalar type possible)
	template<typename OtherScalar>
	inline	      PointT& operator=(const PointT<OtherScalar>& _p){
	    data_[0]=Scalar(_p[0]);
	    data_[1]=Scalar(_p[1]);
	    data_[2]=Scalar(_p[2]);
	    return *this;
	}
	
	//get component
	inline	      Scalar& operator[](int _i)	{ return data_[_i]; }
	inline const  Scalar& operator[](int _i) const	{ return data_[_i]; }

	//cast to array
	inline operator	      Scalar*()	      { return this->data_;}
	inline operator const Scalar*() const { return this->data_;}
	
	//comparisons
	inline bool operator==(const PointT& _p) const { 
	    return 
		(data_[0]==_p[0]) &
		(data_[1]==_p[1]) &
		(data_[2]==_p[2]);
	}
	
	inline bool operator!=(const PointT& _p) const { 
	    return !operator==(_p);
	}
	
	//scalar operations
	inline PointT& operator*=(const Scalar& _s) { 
	    data_[0]*=_s;
	    data_[1]*=_s;
	    data_[2]*=_s;
	    return *this;
	}
	
	inline PointT& operator/=(const Scalar& _s) { 
	    data_[0]/=_s;
	    data_[1]/=_s;
	    data_[2]/=_s;
	    return *this;
	}

	inline PointT operator*(const Scalar& _s) const { 
	    return PointT(*this).operator*=(_s);
	}
	
	inline PointT operator/(const Scalar& _s) const { 
	    return PointT(*this).operator/=(_s);
	}
	
	//vector operations
	inline PointT& operator*=(const PointT& _p) { 
	    data_[0]*=_p[0];
	    data_[1]*=_p[1];
	    data_[2]*=_p[2];
	    return *this;
	}
	
	inline PointT& operator/=(const PointT& _p) { 
	    data_[0]/=_p[0];
	    data_[1]/=_p[1];
	    data_[2]/=_p[2];
	    return *this;
	}
	
	inline PointT& operator+=(const PointT& _p) { 
	    data_[0]+=_p[0];
	    data_[1]+=_p[1];
	    data_[2]+=_p[2];
	    return *this;
	}
	
	inline PointT& operator-=(const PointT& _p) { 
	    data_[0]-=_p[0];
	    data_[1]-=_p[1];
	    data_[2]-=_p[2];
	    return *this;
	}
	
	inline PointT operator*(const PointT& _p) const { 
	     return PointT(*this).operator*=(_p);
	}
	
	inline PointT operator/(const PointT& _p) const { 
	     return PointT(*this).operator/=(_p);
	}
	
	inline PointT operator+(const PointT& _p) const { 
	     return PointT(*this).operator+=(_p);
	}
	
	inline PointT operator-(const PointT& _p) const { 
	     return PointT(*this).operator-=(_p);
	}

	//unary minus
	inline PointT operator-(void) const { 
	     PointT p(*this);
	     p[0]=-p[0];
	     p[1]=-p[1];
	     p[2]=-p[2];
	     return p;
	}

	//cross product
	inline PointT operator%(const PointT& _p) const { 
	    return PointT(
		    data_[1]*_p[2]-data_[2]*_p[1],
		    data_[2]*_p[0]-data_[0]*_p[2],
		    data_[0]*_p[1]-data_[1]*_p[0]);
	}

	//scalar product
	inline Scalar operator|(const PointT& _p) const { 
	    Scalar s(0);
	    s+=data_[0]*_p[0];
	    s+=data_[1]*_p[1];
	    s+=data_[2]*_p[2];
	    return s;
	}
	
	//eucledian norm
	inline Scalar norm() const { return Scalar(sqrt( sqrnorm() )); }
	
	inline Scalar length() const { return norm(); }
	
	inline Scalar sqrnorm() const { return (*this)|(*this); }

	//normalization
	inline PointT& normalize() { 
	    Scalar n=norm();

	    //assert(n != Scalar(0.0));

	    //prevent div by zero!
	    if (n != Scalar(0.0)){
		return operator*=( Scalar(1.0/n) );
	    }
	    else{
		return *this;
	    }
	}

	//min, max
	inline Scalar min() const { 
	    Scalar m = data_[0];
	    if (data_[1]<m) m = data_[1];
	    if (data_[2]<m) m = data_[2];
	    return m;
	}
	
	inline Scalar max() const { 
	    Scalar m = data_[0];
	    if (data_[1]>m) m = data_[1];
	    if (data_[2]>m) m = data_[2];
	    return m;
	}
	inline Scalar max_abs() const { 
	    Scalar m = abs(data_[0]);
	    if (abs(data_[1])>m) m = abs(data_[1]);
	    if (abs(data_[2])>m) m = abs(data_[2]);
	    return m;
	}	
	inline Scalar min_abs() const { 
	    Scalar m = abs(data_[0]);
	    if (abs(data_[1])<m) m = abs(data_[1]);
	    if (abs(data_[2])<m) m = abs(data_[2]);
	    return m;
	}	
	//minimize and maximize, change current object
	inline PointT& minimize(const PointT& _p) { 
	    if (data_[0] > _p[0]) data_[0]=_p[0];
	    if (data_[1] > _p[1]) data_[1]=_p[1];
	    if (data_[2] > _p[2]) data_[2]=_p[2];
	    return *this;
	}
	
	inline PointT& maximize(const PointT& _p) { 
	    if (data_[0] < _p[0]) data_[0]=_p[0];
	    if (data_[1] < _p[1]) data_[1]=_p[1];
	    if (data_[2] < _p[2]) data_[2]=_p[2];
	    return *this;
	}

	//like minimize and maximize, but doesn't change current object
	inline PointT min(const PointT& _p) const { 
	    return PointT(*this).minimize(_p);
	}
	
	inline PointT max(const PointT& _p) const { 
	    return PointT(*this).maximize(_p);
	}
	
	//arithmetic mean
	inline Scalar mean() const { 
	    return data_[0]+data_[1]+data_[2]/Scalar(3.0);
	}

    //MEMBER VARIABLES
    private:
	Scalar data_[3];
};

#endif   /* ----- #ifndef POINT_T_HH  ----- */
