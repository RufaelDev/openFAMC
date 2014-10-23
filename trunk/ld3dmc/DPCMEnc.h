/*
 * =====================================================================================
 * 
 *        Filename:  DPCMEnc.h
 * 
 *     Description:  A Layer Compressor exploiting strictly linear dependencies
 * 
 *         Version:  1.0
 *         Created:  03/04/07 15:58:04 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

#ifndef  DPCM_ENC_HH
#define  DPCM_ENC_HH

#include "DynamicMesh.h"
#include "DPCMPred.h"
#include "LDQuant3D.h"
#include <vector>

class DPCMEnc {

    //CONSTRUCTORS
    public:
	DPCMEnc(
		const DPCMPred* _pred,
		double _delta) : 
	    pred_(_pred),
	    quant_(_delta){}

    //MEMBER VARIABLES
    public:
	IPoint operator()(
		Point _p,
		int _f,
		const VertexContext2& _vc,
		LDFrameType _f_type,
		int _f_r0,
		int _f_r1){
	    
	    //get predicted vertex location
	    Point p_p = (*pred_)(_f, _vc, _f_type, _f_r0, _f_r1);
	    
	    //calculate residual
	    Point res = _p-p_p;

	    //quantize residual
	    IPoint res_q = quant_(res);

	    return res_q;
	}
	
    //MEMBER VARIABLES
    private:
	const DPCMPred*	    pred_;
	LDQuant3D			quant_;
};

class DPCMEncLS {

    //CONSTRUCTORS
    public:
	DPCMEncLS(
		const DPCMPred* _pred) : 
	    pred_(_pred),
	    quant_(1.0){}

	//MEMBER VARIABLES
    public:
	IPoint operator()(
		IPoint _ip,
		int _f,
		const VertexContext2& _vc,
		LDFrameType _f_type,
		int _f_r0,
		int _f_r1){
	    
	    //get predicted vertex location
	    Point p_p = (*pred_)(_f, _vc, _f_type, _f_r0, _f_r1);
		IPoint p_ip = quant_(p_p);
	    
	    //calculate residual
	    IPoint ires = _ip-p_ip;

	    return ires;
	}

    //MEMBER VARIABLES
    private:
	const DPCMPred*	    pred_;
	LDQuant3D			quant_;
};

#endif   /* ----- #ifndef DPCM_ENC_HH  ----- */
