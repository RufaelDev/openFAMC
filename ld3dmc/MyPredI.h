/*
 * =====================================================================================
 * 
 *        Filename:  MyPredI.h
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  03/15/07 11:02:45 CET
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */


#ifndef  MY_PRED_I_HH
#define  MY_PRED_I_HH

#include "PredI.h"
#include "DynamicMesh.h"
#include "LayeredDecomposition.h"


class MyPredI : public PredI{
    //CONSTRUCTORS
    public:
	MyPredI(const DynamicMesh* _dm) : dm_(_dm){} 
	
    //MEMBER FUNCTIONS
    public:
	virtual Point operator()(int _f, const VertexContext2& _vc) const;

    private:
		void get_point_group(int _f, const std::vector<int>& _vg, std::vector<Point>& _pg) const;


    //MEMBER VARIABLES
    private:
	const DynamicMesh* dm_;
};

#endif   /* ----- #ifndef MY_CONTEXT_PREDICTOR_HH  ----- */
