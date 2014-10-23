/*
 * =====================================================================================
 * 
 *        Filename:  DPCMPredLin.h
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



#ifndef  DPCM_PRED_LIN_HH
#define  DPCM_PRED_LIN_HH

#include "DPCMPred.h"
#include "DynamicMesh.h"
#include "MyPredI.h"
#include "LDFrameTypeData.h"

class DPCMPredLin : public DPCMPred {
    //CONSTRUCTOR
    public:
	DPCMPredLin(const DynamicMesh* _dm_r) : 
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
	
	Point predP(int _f, const VertexContext2& _vc, int _f_r0) const {
	    return pred_(_f, _vc) + (*dm_r_)[_f_r0][_vc.to] - pred_(_f_r0, _vc);
	}
	
	Point predB(int _f, const VertexContext2& _vc, int _f_r0, int _f_r1) const { 
	    return pred_(_f, _vc) + 
		((*dm_r_)[_f_r0][_vc.to] - pred_(_f_r0, _vc) +
		 (*dm_r_)[_f_r1][_vc.to] - pred_(_f_r1, _vc)) * 0.5;
	}

    //MEMBER VARIABLES
    private:
	const DynamicMesh*	dm_r_;
	const MyPredI		pred_;
};

#endif   /* ----- #ifndef DPCM_PRED_LIN_HH  ----- */

