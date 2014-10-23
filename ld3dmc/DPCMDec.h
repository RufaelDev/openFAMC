/*
 * =====================================================================================
 * 
 *        Filename:  DPCMDec.h
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


#ifndef  DPCM_DEC_HH
#define  DPCM_DEC_HH

#include "DynamicMesh.h"
#include "DPCMPred.h"
#include "LDRecon3D.h"
#include "LDQuant3D.h"
#include "LDFrameTypeData.h"
#include <vector>

class DPCMDec {

    //CONSTRUCTORS
    public:
	DPCMDec(const DPCMPred* _pred, double _delta) : pred_(_pred), recon_(_delta) {}//unbounded no. of levels

    //MEMBER FUNCTIONS
    public:
	Point operator()(IPoint _res_q, int _f, const VertexContext2& _vc, LDFrameType _f_type, int _f_r0, int _f_r1){
	    
	    //get predicted vertex location
	    Point p_p = (*pred_)(_f, _vc, _f_type, _f_r0, _f_r1);
	    
	    //get reconstructed residual
	    Point res_r = recon_(_res_q);

	    //get reconstructed vertex location
	    Point p_r = p_p + res_r;

	    return p_r;
	}

    //MEMBER VARIABLES
    private:
	const DPCMPred*	      pred_;
	const LDRecon3D		recon_;
};

class DPCMDecLS {

    //CONSTRUCTORS
    public:
	DPCMDecLS(const DPCMPred* _pred) : pred_(_pred), quant_(1.0) {}//unbounded no. of levels

    //MEMBER FUNCTIONS
    public:
	IPoint operator()(IPoint _ires, int _f, const VertexContext2& _vc, LDFrameType _f_type, int _f_r0, int _f_r1){
	    
	    //get predicted vertex location
	    Point p_p = (*pred_)(_f, _vc, _f_type, _f_r0, _f_r1);
		IPoint p_ip = quant_(p_p);
	    
	    //get reconstructed vertex location
	    IPoint ip_r = p_ip + _ires;

	    return ip_r;
	}

    //MEMBER VARIABLES
    private:
	const DPCMPred*     pred_;
	const LDQuant3D		quant_;
};

#endif   /* ----- #ifndef DPCM_DEC_HH  ----- */
