/*===============================================*\
 * Copyright (C) 2006-2007 by Nikolce Stefanoski * 
\*===============================================*/

#ifndef  MESH_TYPES_HH
#define  MESH_TYPES_HH

#include <vector>
#include "PointT.h"

typedef PointT<double>		Point;
typedef std::vector<Point>	PointGroup;
typedef std::vector<PointGroup> PointGroups;

typedef std::vector<Point>	Geometry;

typedef std::vector<int>	IF;
typedef std::vector<IF>		IFS;


typedef PointT<int>		IPoint; //point with integer valued coordinates
#endif   /* ----- #ifndef MESH_TYPES_HH----- */
