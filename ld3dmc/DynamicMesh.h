/*
 * =====================================================================================
 * 
 *        Filename:  ChangeableDynamicMesh.h
 * 
 *     Description:  A dynamic mesh
 * 
 *         Version:  1.0
 *         Created:  02/01/07 16:26:23 CET
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Theoretische Nachrichtentechnik und Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

/*===============================================*\
 * Copyright (C) 2006-2007 by Nikolce Stefanoski * 
\*===============================================*/

#ifndef  DYNAMIC_MESH_HH
#define  DYNAMIC_MESH_HH

#include "DynamicMeshBase.h"

class DynamicMesh : public DynamicMeshBase {

    //CONSTRUCTORS
    public: 
	DynamicMesh(int _n_vertices, int _n_frames) : DynamicMeshBase(IFS(0), _n_vertices, _n_frames){}
	DynamicMesh(const IFS& _ifs, int _n_vertices, int _n_frames) : DynamicMeshBase(_ifs, _n_vertices, _n_frames){}
    private:
	DynamicMesh(const DynamicMesh& _dm);

    //MEMBER FUNCTIONS
    public:
	virtual const	Geometry& operator[](int _f) const;
	virtual		Geometry& operator[](int _f);
	Point	  get_point(int _f, int _v) const;
	void	  set_point(Point _p, int _f, int _v);
	virtual		void	  get_path(int _v, int _begin, Geometry& _path) const;
	virtual		void	  set_path(int _v, int _begin, const Geometry& _path);

};

#endif   /* ----- #ifndef DYNAMIC_MESH_HH  ----- */
