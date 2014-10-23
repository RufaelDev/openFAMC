/*!
 ************************************************************************
 *  \file
 *     AffineMotionPredictor.cpp
 *  \brief
 *     AffineMotionPredictor class.
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

#include "AffineMotionPredictor.h"
#include "Console.h"

extern CConsole logger;

AffineMotionPredictor::AffineMotionPredictor(void)
{
}

AffineMotionPredictor::~AffineMotionPredictor(void)
{
}

void AffineMotionPredictor::Init(Animation * anim, std::vector<int>  * vertices){
	anim_ = anim;
	vertices_ = vertices;
	int m = (int) vertices_->size();
	int n = 4;

	Mat_DP A(m,n);
	pinvA_ = Mat_DP(n,m);
	float coord[3];
	for( int v = 0; v < m; v++) {
		anim_->GetCoord(0, (*vertices_)[v], coord);
		for( int h = 0; h < 3; h++) {
			A[v][h] = coord[h];
		}
		A[v][3] = 1.0;
	}

    Vec_DP w(n);
    Mat_DP V(n,n);

//	logger.write_2_log("Computing the SVD decomposition ");
//	logger.tic();
	NR::svdcmp(A,w,V);
//	logger.toc();

//	logger.write_2_log("Computing pseudo-inverse");
//	logger.tic();
	for (int k = 0; k < w.size(); k++) {
		if ( fabs(w[k]) > TOL) {
			w[k] = 1.0 /w[k];
		}
		else {
			w[k] = 0.0;
		}
	}

	for (int k = 0; k < V.nrows(); k++) {
		for (int l = 0; l < V.ncols(); l++) {
			V[k][l] *= w[l];
		}
	}

	for (int k = 0; k < pinvA_.nrows(); k++) {
		for (int l = 0; l < pinvA_.ncols(); l++) {
			pinvA_[k][l] = 0.0;
			for (int h = 0; h < V.ncols(); h++) {
				pinvA_[k][l] += V[k][h] * A[l][h]; 
			}
		}
	}
//	logger.toc();
}

void AffineMotionPredictor::ComputeAffineTransform(int frame, AffineTransform &affineTransform) {

//	logger.write_2_log("Computing the affine transform for frame %i ", frame);
//	logger.tic();
	int m = (int) vertices_->size();
	Vec_DP x(m);
	Vec_DP y(m);
	Vec_DP z(m);


	float coord[3];
	for(int v = 0; v < m; v++) {
		anim_->GetCoord(frame, (*vertices_)[v], coord);
		x[v] = coord[0];
		y[v] = coord[1];
		z[v] = coord[2];
	}

	Vec_DP a1(4);
	Vec_DP a2(4);
	Vec_DP a3(4);

	for (int k = 0; k < 4; k++) {
		a1[k] = 0.0;
		a2[k] = 0.0;
		a3[k] = 0.0;
		for (int l = 0; l < m; l++) {
			a1[k] += pinvA_[k][l] * x[l];
			a2[k] += pinvA_[k][l] * y[l];
			a3[k] += pinvA_[k][l] * z[l];
		}
	}
	
	affineTransform.a_ = (float) a1[0];
	affineTransform.b_ = (float) a1[1];
	affineTransform.c_ = (float) a1[2];
	affineTransform.tx_ = (float) a1[3];

	affineTransform.d_ = (float) a2[0];
	affineTransform.e_ = (float) a2[1];
	affineTransform.f_ = (float) a2[2];
	affineTransform.ty_ = (float) a2[3];

	affineTransform.g_ = (float) a3[0];
	affineTransform.h_ = (float) a3[1];
	affineTransform.i_ = (float) a3[2];
	affineTransform.tz_ = (float) a3[3];
//	logger.toc();

}