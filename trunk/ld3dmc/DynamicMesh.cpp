/*===============================================*\
 * Copyright (C) 2006-2007 by Nikolce Stefanoski * 
\*===============================================*/

#include "DynamicMesh.h"

Point DynamicMesh::get_point(int _f, int _v) const{
    check_f(_f); check_v(_v);
    return dyn_geo_[_f][_v];
}

void DynamicMesh::set_point(Point _p, int _f, int _v){
    check_f(_f); check_v(_v);
    dyn_geo_[_f][_v]=_p;
}

const Geometry& DynamicMesh::operator[](int _f) const{
    check_f(_f);
    return dyn_geo_[_f];
}

Geometry& DynamicMesh::operator[](int _f){
    check_f(_f);
    return dyn_geo_[_f];
}
void DynamicMesh::get_path(int _v, int _begin, Geometry& _path) const{
    assert(!_path.empty());
    int end = (int) (_begin+_path.size());
    for (int f=_begin; f<end; ++f){
	_path[f-_begin]=get_point(f, _v);
    }
}

void DynamicMesh::set_path(int _v, int _begin, const Geometry& _path){
    assert(!_path.empty());
    int end = (int) (_begin+_path.size());
    for (int f=_begin; f<end; ++f){
	set_point(_path[f-_begin], f, _v);
    }
}
