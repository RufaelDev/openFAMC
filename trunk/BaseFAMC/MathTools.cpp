/*!
 ************************************************************************
 *  \file
 *     MathTools.cpp
 *  \brief
 *     MathTools class.
 *  \author
 *     Copyright (C) 2007  ARTEMIS Department INT/GET, Paris, France.
 *
 *     Khaled MAMOU               <khaled.mamou@int-evry.fr>
 *
 *	   Institut National des Telecommunications		tel.: +33 (0)1 60 76 40 94
 *     9, Rue Charles Fourier,						fax.: +33 (0)1 60 76 43 81
 *     91011 Evry Cedex France
 *
 ************************************************************************
 */
#include "MathTools.h"
#include <math.h>

#define EPS 0.00000001
#define PI 3.141592653589 

MathTools::MathTools(void)
{
}

MathTools::~MathTools(void)
{
}

void MathTools::vecteur(float *pt1, float *pt2, float *vect, int n) {
	for(int i = 0; i < n; i++) {
		vect[i] = pt2[i] - pt1[i];
	}
}

void MathTools::vectoriel(float vect1[3], float vect2[3], float vect[3]) {
	vect[0] = vect1[1] * vect2[2] - vect1[2] * vect2[1];
	vect[1] = vect1[2] * vect2[0] - vect1[0] * vect2[2];
	vect[2] = vect1[0] * vect2[1] - vect1[1] * vect2[0];
}

float MathTools::vectorNorm(float vect[3]) {	
	return (float) pow( (double)  vect[0]*vect[0] + vect[1]*vect[1] + vect[2]*vect[2] ,0.5);
}

void MathTools::vectorUnitary(float vect[3]) {	
	float normVect = vectorNorm(vect);
	if ( normVect > EPS) {
		for( int k = 0; k < 3; k++) {
			vect[k] /= normVect;
		}
	}
}

float MathTools::vectorDot(float vect1[3], float vect2[3]) {	
	return vect1[0] * vect2[0] + vect1[1] * vect2[1] + vect1[2] * vect2[2];
}

void MathTools::vectorBasis(float vect1[3], float vect2[3], float vect3[3]) {
	vectorUnitary(vect1);

	float b[3][3] = {{1.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f,1.0f}};

	int k = 0;
	double d0 = fabs(vectorDot(vect1, b[0]));
	for (int p = 1; p < 3; p++) {
		double d = fabs(vectorDot(vect1, b[p]));
		if ( d > d0 ) {
			k = p;
			d0 = d;
		}
	}

	int k1 = (k+1)%3;
	float d = vectorDot(vect1, b[k1]);

	for (int p = 0; p < 3; p++) {
		vect2[p] = b[k1][p] - d * vect1[p];
	}

	vectorUnitary(vect2);

	vectoriel(vect1, vect2, vect3);
}

void MathTools::cartesian2Spherical(float coord[3], float& r, float& tetha, float& phi) {	
	double x = coord[0];
	double y = coord[1];
	double z = coord[2];

	r = (float) pow(x*x+y*y+z*z, 0.5);	
	phi = 0.0f;
	tetha = 0.0f;

	if ( (x == 0.0) && (y == 0.0) ) {
		tetha = 0.0f;
		phi = 0.0f;
	} else {
		if (x == 0.0) {
			tetha = (float) PI/2;
		}
		else {
			tetha = (float) atan(fabs(y/x));
		}
	}

	if ((x<=0.0) && (y<=0.0)) {
		tetha = (float) (PI + tetha);
	}
	else if ((x<=0.0) && (y>=0.0)) {
		tetha = (float) PI - tetha;
	}
	else if ((x>=0.0) && (y<=0.0)) tetha = (float) (2*PI - tetha);

	if (r > EPS) {
		phi = (float) acos(z/r);
	}
}

void MathTools::spherical2Cartesian(float r, float tetha, float phi, float coord[3]) {	
	coord[0] = (float) r* cos(tetha)*sin(phi);
	coord[1] = (float) r* sin(tetha)*sin(phi);
	coord[2] = (float) r* cos(phi);
}

float MathTools::Signe(float x) {
	if ( x > 0.0) return 1.0f;
	return -1.0f;
}

int MathTools::nBinaryBits(unsigned int m){
	int n=0;
	while (m!=0) {
		m = (m>>1);
		n++;
	}
	return n;
}
