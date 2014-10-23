/*
 * =====================================================================================
 * 
 *        Filename:  DPCMPred.h
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



#ifndef  DPCM_PRED_HH
#define  DPCM_PRED_HH

#include "LayeredDecomposition.h"
#include "LDFrameTypeData.h"

class DPCMPred{

    //MEMBER FUNCTIONS
    public:
	virtual Point operator()(
		int _f,
		const VertexContext2& _vc,
		LDFrameType _f_type, 
		int _f_r0,
		int _f_r1) const = 0;
};

#endif   /* ----- #ifndef DPCM_PRED_HH  ----- */

