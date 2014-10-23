/*
 * =====================================================================================
 * 
 *        Filename:  DynamicMeshBase.h
 * 
 *     Description:  A base class for a dynamic mesh
 * 
 *         Version:  1.0
 *         Created:  02/01/07 15:12:12 CET
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

/*===============================================*\
 * Copyright (C) 2006-2007 by Nikolce Stefanoski * 
\*===============================================*/

#ifndef DYNAMIC_MESH_BASE_HH
#define DYNAMIC_MESH_BASE_HH

#include <vector>
#include <assert.h>
#include "MeshTypes.h"

class DynamicMeshBase{

    //CONSTRUCTORS
    protected:
	DynamicMeshBase(const IFS& _ifs, int _n_vertices, int _n_frames) :
	  n_vertices_(_n_vertices), dyn_geo_(_n_frames, Geometry(_n_vertices, Point(0,0,0))), ifs_(_ifs) {} 
    private:
	DynamicMeshBase(const DynamicMeshBase& _dmb);

    //MEMBER VARIABLES
    protected:
	const int		    n_vertices_;
	      std::vector<Geometry> dyn_geo_; 
	      IFS		    ifs_;
	
    //MEMBER FUNCTIONS
    public:
	virtual	      int   n_frames()    const;
	virtual	      int   n_vertices()  const;
	virtual const IFS&  get_ifs()     const;
    protected:
	virtual void check_v(int _v) const;
	virtual void check_f(int _f) const;
};

#endif //DYNAMIC_MESH_BASE_HH
