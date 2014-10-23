/*
 * =====================================================================================
 * 
 *        Filename:  PointUtils.h
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  19/05/07 19:24:51 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

#ifndef  POINT_UTILS_HH
#define  POINT_UTILS_HH

#include "MeshTypes.h"
#include "VertexContext.h"
#include "DynamicMesh.h"

class PointUtils{
    public:

	static Point get_face_normal(Point p1, Point p2, Point p3);
	
	static Point get_vertex_normal(Point m, const PointGroup& vp);

	static void get_basis(Point m, const PointGroup& vp, Point& x, Point& y, Point& z); 

	static void get_canonical_basis(Point& x, Point& y, Point& z);

	static bool is_canonical_basis(Point x, Point y, Point z);

	static Point cannonical2xyz(Point p, Point x, Point y, Point z);

	static Point xyz2cannonical(Point p, Point x, Point y, Point z);

};

#endif   /* ----- #ifndef POINT_UTILS_HH----- */
