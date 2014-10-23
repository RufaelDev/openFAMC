/*
 * =====================================================================================
 * 
 *        Filename:  DPCMPredDelta.h
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



#ifndef  DPCM_PRED_DELTA_HH
#define  DPCM_PRED_DELTA_HH

#include "DPCMPred.h"
#include "DynamicMesh.h"
#include "LDFrameTypeData.h"

class DPCMPredDelta : public DPCMPred {
    //CONSTRUCTOR
    public:
	DPCMPredDelta(const DynamicMesh* _dm_r) : 
	    dm_r_(_dm_r){} 

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
		    p=Point(0,0,0);
		    break;

		case LDFrameType::P:	
			p=(*dm_r_)[_f_r0][_vc.to];
		    break;

		case LDFrameType::B:
		    p=((*dm_r_)[_f_r0][_vc.to]+(*dm_r_)[_f_r1][_vc.to])*0.5;
		    break;
	    }				/* -----  end switch  ----- */
	    return p;
	}

    //MEMBER VARIABLES
    private:
	const DynamicMesh*	dm_r_;
};

#endif   /* ----- #ifndef DPCM_PRED_DELTA_HH  ----- */