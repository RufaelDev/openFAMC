/*
* =====================================================================================
* 
*        Filename:  DPCMPredNonLin.hh
* 
*     Description:  DPCM-Predictor 
* 
*         Version:  1.0
*         Created:  03/04/07 19:08:46 CEST
*        Revision:  none
*        Compiler:  gcc
* 
*          Author:  Nikolce Stefanoski (nst)
*         Company:  Institut für Informationsverarbeitung
*           Email:  stefanos@tnt.uni-hannover.de
* 
* =====================================================================================
*/



#ifndef  DPCM_PRED_NON_LIN_HH
#define  DPCM_PRED_NON_LIN_HH

#include "DPCMPred.h"
#include "DynamicMesh.h"
#include "MyPredI.h"
#include "MySimplePreds.h"
#include "PointUtils.h"
#include "LDFrameTypeData.h"

class DPCMPredNonLin : public DPCMPred {
	//CONSTRUCTOR
public:
	DPCMPredNonLin(const DynamicMesh* _dm_r) : 
	  dm_r_(_dm_r), pred_(_dm_r){} 

	  //MEMBER FUNCTIONS
public:
	Point operator()(
		int _f,
		const VertexContext2& _vc,
		LDFrameType _f_type, 
		int _f_r0,
		int _f_r1) const {

			Point  p(0,0,0);
			switch ( _f_type )
			{
			case LDFrameType::I:	
				p=predI(_f, _vc);
				//			p = IPoint(0,0,0);
				break;

			case LDFrameType::P:	
				p=predP(_f, _vc, _f_r0);
				break;

			case LDFrameType::B:
				p=predB(_f, _vc, _f_r0, _f_r1);
				break;
			}				/* -----  end switch  ----- */
			return p;
	}

	//helper functions
private:

	Point predI(int _f, const VertexContext2& _vc) const { 
		return pred_(_f, _vc); 
	}

	Point predP(int _f, const VertexContext2& _vc, int _f_r) const {
		Point vec = (*dm_r_)[_f_r][_vc.to] - predI(_f_r, _vc);

		//non-linear prediction for ring contextes
		//if (_vc.id()==VertexContext::Ring2Vertex){
		vec = reproject_vector(vec, _f, _vc, _f_r);
		//}

		return predI(_f, _vc) + vec;
	}

	Point predB(int _f, const VertexContext2& _vc, int _f_r0, int _f_r1) const { 
		return  
			(predP(_f, _vc, _f_r0)+predP(_f, _vc, _f_r1))*0.5;
	}

	Point reproject_vector(Point _vec, int _f, const VertexContext2& _vc, int _f_r) const {
		if (_vc.from.size()<3) return _vec;

		PointGroup pg_r; 
		get_point_group(_f_r, _vc.from, pg_r);
		Point m_r  = MySimplePreds::mean(pg_r);
		Point x_r,y_r,z_r;
		get_face_basis(m_r, pg_r, x_r,y_r,z_r);
		Point proj = PointUtils::cannonical2xyz(_vec, x_r, y_r, z_r);

		PointGroup pg; get_point_group(_f, _vc.from, pg);
		Point m  = MySimplePreds::mean(pg);
		Point x,y,z;
		get_face_basis(m, pg, x,y,z);
		Point reproj =  PointUtils::xyz2cannonical(proj, x, y, z);

		if (!PointUtils::is_canonical_basis(x,y,z) && !PointUtils::is_canonical_basis(x_r, y_r, z_r)){
			return reproj;
		}
		else{
			return _vec;
		}
	}

	void get_point_group(int _f, const std::vector<int>& _vg, std::vector<Point>& _pg) const {
		assert(_pg.empty());
		//fill group with points
		for (int v=0; v<(int) _vg.size(); ++v){
			_pg.push_back( (*dm_r_)[_f][_vg[v]] );
		}
	}

	void get_face_basis(Point m, const PointGroup& vp, Point& x, Point& y, Point& z) const {
		if (vp.size()<2){
			PointUtils::get_canonical_basis(x, y, z);
			return;
		}
		else {
			z = PointUtils::get_face_normal(m, vp[0], vp[1]);
			x = vp[0]-m;
			x.normalize();
			y = z%x;
			y.normalize();

			if (z.norm()==0 || x.norm()==0 || y.norm()==0){
				PointUtils::get_canonical_basis(x, y, z);
			}
		}
	}


	//MEMBER VARIABLES
private:
	const DynamicMesh*	dm_r_;
	const MyPredI		pred_;
};

#endif   /* ----- #ifndef DPCM_PRED_NON_LIN_HH  ----- */

