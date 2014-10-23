/*
 * =====================================================================================
 * 
 *        Filename:  ContextPredictor.h
 * 
 *     Description:  Abstract VertexContext-based Predictor class
 * 
 *         Version:  1.0
 *         Created:  03/15/07 09:23:49 CET
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */


#ifndef  PRED_I_HH
#define  PRED_I_HH

#include "DynamicMesh.h"
#include "MeshTypes.h"
#include "LayeredDecomposition.h"

class PredI{
	
    //MEMBER FUNCTIONS
    public:
	virtual Point operator()(int _f, const VertexContext2& _vc) const = 0;
};

#endif   /* ----- #ifndef PRED_I_HH  ----- */
