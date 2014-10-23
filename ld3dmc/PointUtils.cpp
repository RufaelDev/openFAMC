#include "PointUtils.h"

Point PointUtils::get_face_normal(Point p1, Point p2, Point p3){
    Point p12 = p2-p1;
    Point p13 = p3-p1;
    Point n = p12%p13;
    //assert(n.norm()!=0);
    n.normalize();//add comment for area-weighted vertex normal
    return n;
}

Point PointUtils::get_vertex_normal(Point m, const PointGroup& vp) {
    assert(vp.size()>=3);
    Point n(0,0,0);
    std::vector<Point> face_normal;
    for (int i=0; i< (int) vp.size()-1; ++i){
	face_normal.push_back(get_face_normal(m, vp[i], vp[i+1]));
    }
    face_normal.push_back(get_face_normal(m, vp[vp.size()-1], vp[0]));
    for (int i=0; i< (int) face_normal.size(); ++i){
	n+=face_normal[i];
    }
    assert(face_normal.size()>=3);
    //assert(n.norm()!=0);
    n.normalize();
    return n;
}


void PointUtils::get_basis(Point m, const PointGroup& vp, Point& x, Point& y, Point& z) {

    //init basis vectors
    x=Point(0,0,0);
    y=Point(0,0,0);
    z=Point(0,0,0);
    
    //calculate basis vectors
    z=get_vertex_normal(m, vp);
    if(z.norm()!=0){
	z.normalize();
    }
    
    Point mvp=vp[0]-m;
    Point mvp_n=mvp-z*(z|mvp);
    if(mvp_n.norm()!=0){
	x=mvp_n;
	x.normalize();
    }

    y=z%x;
    if (y.norm()!=0){
	y.normalize();
    }

    if (z.norm()==0 || x.norm()==0 || y.norm()==0){
	get_canonical_basis(x, y, z);
    }
}

void PointUtils::get_canonical_basis(Point& x, Point& y, Point& z) {
	x=Point(1,0,0);
	y=Point(0,1,0);
	z=Point(0,0,1);
}

bool PointUtils::is_canonical_basis(Point x, Point y, Point z) {
    return 
	(x==Point(1,0,0) && 
	 y==Point(0,1,0) &&
	 z==Point(0,0,1));
}

Point PointUtils::cannonical2xyz(Point p, Point x, Point y, Point z) {
    return Point(p|x, p|y, p|z);
}

Point PointUtils::xyz2cannonical(Point p, Point x, Point y, Point z) {
    return x*p[0]+y*p[1]+z*p[2];
}

