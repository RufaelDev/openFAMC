/*
 * =====================================================================================
 * 
 *        Filename:  MyPredI.cc
 * 
 *     Description:  The specific VertexContext Predictors
 * 
 *         Version:  1.0
 *         Created:  03/15/07 11:34:44 CET
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst), stefanos@tnt.uni-hannover.de
 *         Company:  Institut für Informationsverarbeitung
 * 
 * =====================================================================================
 */


#include "MyPredI.h"
#include "MySimplePreds.h"

using namespace std;

Point MyPredI::operator()(int _f, const VertexContext2& _vc) const{
    assert( _f>=0 );
    assert( _f<dm_->n_frames() );

    //Point p(0,0,0);
	std::vector<Point> from_points;
    get_point_group(_f, _vc.from, from_points); 

	//assert(_vc.id()!=VertexContext::Invalid);

    return MySimplePreds::ring2vertex(from_points);

 //   switch ( _vc.id() )
 //   {
	//case VertexContext::Nil2Vertex:	
	//    p = MySimplePreds::nil2vertex();
	//    break;

	//case VertexContext::Vertex2Vertex:	
	//    assert(from_points.size()==1);
	//    p = MySimplePreds::vertex2vertex(from_points[0]);
	//    break;

	//case VertexContext::Tri2Vertex:	
	//    p = MySimplePreds::tri2vertex(from_points);
	//    break;

	//case VertexContext::MultiVertex2Vertex:	
	//    p = MySimplePreds::multi_vertex2vertex(from_points);
	//    break;

	//case VertexContext::Ring2Vertex:	
	//    p = MySimplePreds::ring2vertex(from_points);
	//    break;

	//default:	
	//    printf("Error: MyPredI Id NOT recognized!\n");
	//    exit(1);
	//    break;
 //   }				/* -----  end switch  ----- */
 //   return p;
}

void MyPredI::get_point_group(int _f, const std::vector<int>& _vg, std::vector<Point>& _pg) const {
    assert(_pg.empty());
    //fill group with points
    for (int v=0; v<(int) _vg.size(); ++v){
	_pg.push_back( (*dm_)[_f][_vg[v]] );
    }
}

