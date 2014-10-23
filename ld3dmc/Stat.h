/*
 * =====================================================================================
 * 
 *        Filename:  Stat.h
 * 
 *     Description:  Some functions often used in statistical evaluation
 * 
 *         Version:  1.0
 *         Created:  03/13/07 13:10:54 CET
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

/*===============================================*\
 * Copyright (C) 2006-2007 by Nikolce Stefanoski * 
\*===============================================*/

#ifndef  STAT_HH
#define  STAT_HH

#include <cmath>
#include <vector>
#include "MeshTypes.h"
#include "DynamicMesh.h"

class Stat{
    public:
	
    //------------------------1D MEMBER FUNCTIONS-------------------------------------------------------

	//MEAN
	template<typename T> 
	    static double mean(const std::vector<T>& _v){
		double mean(0);
		for (int i=0; i<_v.size();i++){
		    mean+=(double) _v[i];
		}
		return mean/static_cast<double>(_v.size());
	    }

	//DEV
	template<typename T> 
	    static double dev(const std::vector<T>& _v, const double _p){
		double dev(0);
		for (int i=0; i<_v.size();i++){
		    dev += (_p-(double)_v[i])*(_p-(double)_v[i]);
		}
		return dev/static_cast<double>(_v.size());
	    }

	//VAR
	template<typename T> 
	    static double var(const std::vector<T>& _v){
		return dev(_v, mean(_v));
	    }

	//STD_DEV
	template<typename T> 
	    static double std_dev(const std::vector<T>& _v){
		return sqrt( var(_v) );
	    }

	//SSD
	template<typename T> 
	    static double ssd(const std::vector<T>& _v1, const std::vector<T>& _v2){
		double ssd=0;
		assert(_v1.size()==_v2.size());
		for (int i=0; i<_v1.size(); ++i){
		    ssd+=(_v1[i]-_v2[i])*(_v1[i]-_v2[i]);
		}
		return ssd;
	    }

	//SAD
	template<typename T> 
	    static double sad(const std::vector<T>& _v1, const std::vector<T>& _v2){
		double sad=0;
		assert(_v1.size()==_v2.size());
		for (int i=0; i<_v1.size(); ++i){
		    sad+=abs( _v1[i]-_v2[i] );
		}
		return sad;
	    }

	//MSE
	template<typename T> 
	    static double mse(const std::vector<T>& _v1, const std::vector<T>& _v2){
		assert(_v1.size()==_v2.size());
		return ssd(_v1, _v2)/static_cast<double>(_v1.size());
	    }
	
	//RMSE
	template<typename T> 
	    static double rmse(const std::vector<T>& _v1, const std::vector<T>& _v2){
		assert(_v1.size()==_v2.size());
		return sqrt( mse(_v1, _v2) );
	    }

	//MAE
	template<typename T> 
	    static double mae(const std::vector<T>& _v1, const std::vector<T>& _v2){
		assert(_v1.size()==_v2.size());
		return sad(_v1, _v2)/static_cast<double>(_v1.size());
	    }

	//HISTO
	static void histo(const std::vector<int>& _v, std::vector<int>& _histo, int& _min);
	
	//PROB
	static void prob(const std::vector<int>& _v, std::vector<double>& _prob, int& _min);
	
	//CDF (Cumul. Density Func.)
	static void cdf(const std::vector<int>& _v, std::vector<double>& _cdf, int& _min);

	//MEDIAN
	static int median(const std::vector<int>& _v);

	//ENTROPY
	static double entropy(const std::vector<int>& _v);
	
    //------------------------3D MEMBER FUNCTIONS---------------------------------------------------------
    
	//MEAN3D
	template<typename T> 
	    static Point mean3D(const std::vector<T>& _v){
		Point mean(0, 0, 0);
		for (int i=0; i<(int) _v.size();i++){
		    mean+= Point(_v[i]);
		}
		return mean/=static_cast<double>(_v.size());
	    }
	    
	//DEV3D
	template<typename T> 
	    static Point dev3D(const std::vector<T>& _v, const Point _p){
		Point dev(0, 0, 0);
		for (int i=0; i<_v.size();i++){
		    dev += (_p-Point(_v[i]))*(_p-Point(_v[i]));
		}
		return dev/static_cast<double>(_v.size());
	    }

	//VAR3D
	template<typename T> 
	    static Point var3D(const std::vector<T>& _v){
		return dev3D(_v, mean3D(_v));
	    }

	//STD_DEV3D
	template<typename T> 
	    static Point std_dev3D(const std::vector<T>& _v){
		Point var =  var3D(_v);
		return Point(
			sqrt( var[0]), 
			sqrt( var[1]),
			sqrt( var[2]));
	    }

	//SSD3D
	template<typename T> 
	    static Point ssd3D(const std::vector<T>& _v1, const std::vector<T>& _v2){
		Point ssd(0, 0, 0);
		assert(_v1.size()==_v2.size());
		for (int i=0; i<(int) _v1.size(); ++i){
		    ssd+=(Point(_v1[i])-Point(_v2[i])) * (Point(_v1[i])-Point(_v2[i]));
		}
		return ssd;
	    }

	//SAD3D
	template<typename T> 
	    static Point sad3D(const std::vector<T>& _v1, const std::vector<T>& _v2){
		Point sad(0, 0, 0);
		Point diff(0, 0, 0);
		assert(_v1.size()==_v2.size());
		for (int i=0; i<(int) _v1.size(); ++i){
		    diff=Point(_v1[i])-Point(_v2[i]);
		    sad+=Point(
			    fabs(diff[0]),
			    fabs(diff[1]),
			    fabs(diff[2]));
		}
		return sad;
	    }

	//MSE3D
	template<typename T> 
	    static Point mse3D(const std::vector<T>& _v1, const std::vector<T>& _v2){
		assert(_v1.size()==_v2.size());
		return ssd3D(_v1, _v2)/static_cast<double>(_v1.size());
	    }
	
	//MAE3D
	template<typename T> 
	    static Point mae3D(const std::vector<T>& _v1, const std::vector<T>& _v2){
		assert(_v1.size()==_v2.size());
		return sad3D(_v1, _v2)/static_cast<double>(_v1.size());
	    }

	//MIN3D
	template<typename T> 
	    static T min3D(const std::vector<T>& _v){
		assert(!_v.empty());
		T min=_v[0];
		for (int i=1; i<(int) _v.size(); ++i){
		    if (min[0]>_v[i][0]) min[0]=_v[i][0];
		    if (min[1]>_v[i][1]) min[1]=_v[i][1];
		    if (min[2]>_v[i][2]) min[2]=_v[i][2];
		    //min.minimize(_v[i]);
		}
		return min;
	    }
	
	//MAX3D
	template<typename T> 
	    static T max3D(const std::vector<T>& _v){
		assert(!_v.empty());
		T max=_v[0];
		for (int i=1; i<(int) _v.size(); ++i){
		    if (max[0]<_v[i][0]) max[0]=_v[i][0];
		    if (max[1]<_v[i][1]) max[1]=_v[i][1];
		    if (max[2]<_v[i][2]) max[2]=_v[i][2];
		    //max.maximize(_v[i]);
		}
		return max;
	    }
	
	//VEC3D_TO_VEC1D
	template<typename T> 
	    static void vec3D_to_vec1D(int _c, const std::vector<T>& _v3d, std::vector<typename T::value_type>& _v1d){
		assert(_c>=0 && _c<=2);
		assert(_v1d.empty());
		_v1d.resize(_v3d.size());
		for (int i=0; i<(int) _v3d.size(); ++i){
		    _v1d[i]=_v3d[i][_c];
		}
	    }

	//VEC1D_TO_VEC3D
	template<typename T> 
	    static void vec1D_to_vec3D(
		    const std::vector<typename T::value_type>& _v1d_0, 
		    const std::vector<typename T::value_type>& _v1d_1, 
		    const std::vector<typename T::value_type>& _v1d_2, 
		    std::vector<T>& _v3d){
		assert(_v1d_0.empty());
		assert(_v1d_1.empty());
		assert(_v1d_2.empty());
		_v3d.resize(_v1d_0.size());
		for (int i=0; i<_v1d_0.size(); ++i){
		    _v3d[i]=Point(_v1d_0[i],_v1d_1[i],_v1d_2[i]);
		}
	    }
	
	//MEDIAN3D
	static IPoint median3D(const std::vector<IPoint>& _v){
	    std::vector<int> v0;
	    std::vector<int> v1;
	    std::vector<int> v2;
	    vec3D_to_vec1D(0, _v, v0);
	    vec3D_to_vec1D(1, _v, v1);
	    vec3D_to_vec1D(2, _v, v2);
	    return IPoint(median(v0), median(v1), median(v2));
	}

	//ENTROPY3D
	static Point entropy3D(const std::vector<IPoint>& _v){
	    std::vector<int> v0;
	    std::vector<int> v1;
	    std::vector<int> v2;
	    vec3D_to_vec1D(0, _v, v0);
	    vec3D_to_vec1D(1, _v, v1);
	    vec3D_to_vec1D(2, _v, v2);
	    return Point(entropy(v0), entropy(v1), entropy(v2));
	}


    //------------------------MESH FUNCTIONS---------------------------------------------------------

	static Point ssdDM(const DynamicMesh& _dm1, const DynamicMesh& _dm2){
	    assert(_dm1.n_frames()==_dm2.n_frames());
	    assert(_dm1.n_vertices()==_dm2.n_vertices());
	    Point ssd(0,0,0);
	    for (int f=0; f<_dm1.n_frames(); ++f){
		ssd+=ssd3D(_dm1[f], _dm2[f]);
	    }
	    return ssd;
	}
	
	static Point sadDM(const DynamicMesh& _dm1, const DynamicMesh& _dm2){
	    assert(_dm1.n_frames()==_dm2.n_frames());
	    assert(_dm1.n_vertices()==_dm2.n_vertices());
	    Point sad(0,0,0);
	    for (int f=0; f<_dm1.n_frames(); ++f){
		sad+=sad3D(_dm1[f], _dm2[f]);
	    }
	    return sad;
	}
	
	static Point mseDM(const DynamicMesh& _dm1, const DynamicMesh& _dm2){
	    assert(_dm1.n_frames()==_dm2.n_frames());
	    assert(_dm1.n_vertices()==_dm2.n_vertices());
	    Point mse = ssdDM(_dm1, _dm2);
	    mse*=1/static_cast<double>(_dm1.n_frames()*_dm1.n_vertices());
	    return mse;
	}

	static double errorKG(const DynamicMesh& _dm, const DynamicMesh& _dm_r){
	    Point ssd3d_n = ssdDM(_dm, _dm_r);
	    double ssd_n = ssd3d_n[0]+ssd3d_n[1]+ssd3d_n[2];
	    double numerator = sqrt(ssd_n);

	    Point ssd3d_d(0,0,0);
	    for (int v=0; v<_dm.n_vertices(); v++){
		std::vector<Point> path(_dm.n_frames());
		_dm.get_path(v, 0, path);
		std::vector<Point> path_avg(_dm.n_frames(), mean3D(path));
		ssd3d_d+=ssd3D(path, path_avg);
	    }
	    double ssd_d = ssd3d_d[0]+ssd3d_d[1]+ssd3d_d[2];
	    double denominator = sqrt(ssd_d);

	    return 100.0*numerator/denominator;
	}

};

#endif   /* ----- #ifndef STAT_HH  ----- */
