/*
 * =====================================================================================
 * 
 *        Filename:  MySimplePreds.h
 * 
 *     Description:  My Simple Predictors
 * 
 *         Version:  1.0
 *         Created:  03/15/07 11:50:45 CET
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */



#ifndef  MY_SIMPLE_PREDS_HH
#define  MY_SIMPLE_PREDS_HH

#include "MeshTypes.h"

class MySimplePreds{
    //MEMBER VARIABLES
    public:
	static Point nil2vertex(){ return Point(0,0,0); }
	
	static Point vertex2vertex(Point _p){ return _p; }

	static Point tri2vertex(const PointGroup& _pg){
	    assert( _pg.size()==3 );
	    return _pg[0]+_pg[1]-_pg[2];
	}

	static Point multi_vertex2vertex(const PointGroup& _pg){
	    assert(!_pg.empty());
	    return mean(_pg);
	}
	
	static Point ring2vertex(const PointGroup& _pg){
	    if (_pg.empty()) return Point(0,0,0);
	    else return mean(_pg);
	}
	static Point mean(const PointGroup& _pg){
	    assert(!_pg.empty());
	    Point sum(0,0,0);
	    for (int i=0; i<(int) _pg.size(); ++i){
		sum+=_pg[i];
	    }	

	    return sum/static_cast<double>(_pg.size());
	}    

};

#endif   /* ----- #ifndef MY_SIMPLE_PREDS_HH  ----- */
