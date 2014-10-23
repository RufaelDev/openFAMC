/*!
 ************************************************************************
 *  \file
 *     MotionModel.cpp
 *  \brief
 *     MotionModel class.
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
#include "MotionModel.h"
#include "AffineTransform.h"
#include "MathTools.h"
#include <math.h>

MotionModel::MotionModel(int K, int V, int F)
{
	K_ = K;
	V_ = V;
	F_ = F;
	affineTransforms_ = new AffineTransform * [F_];
	for(int i = 0; i < F_; i++) {
		affineTransforms_[i] = new AffineTransform[K_];
	}
	weights_ = new float * [V_];
	for(int i = 0; i < V_; i++) {
		weights_[i] = new float[K_];
	}
}

MotionModel::~MotionModel(void)
{
	for(int i = 0; i < F_; i++) {
		delete [] affineTransforms_[i];
	}
	delete [] affineTransforms_;

	for(int i = 0; i < V_; i++) {
		delete [] weights_[i];
	}
	delete [] weights_;
	
}

bool MotionModel::SetAffineTransform(AffineTransform & at, int frame, int cluster){
	if ((frame < F_) && (cluster < K_)){
		affineTransforms_[frame][cluster].a_ = at.a_;
		affineTransforms_[frame][cluster].b_ = at.b_;
		affineTransforms_[frame][cluster].c_ = at.c_;
		affineTransforms_[frame][cluster].d_ = at.d_;
		affineTransforms_[frame][cluster].e_ = at.e_;
		affineTransforms_[frame][cluster].f_ = at.f_;
		affineTransforms_[frame][cluster].g_ = at.g_;
		affineTransforms_[frame][cluster].h_ = at.h_;
		affineTransforms_[frame][cluster].i_ = at.i_;
		affineTransforms_[frame][cluster].tx_ = at.tx_;
		affineTransforms_[frame][cluster].ty_ = at.ty_;
		affineTransforms_[frame][cluster].tz_ = at.tz_;
		return true;
	}
	return false;
}

bool MotionModel::GetAffineTransform(AffineTransform & at, int frame, int cluster){
	if ((frame < F_) && (cluster < K_)){
		at.a_ = affineTransforms_[frame][cluster].a_;
		at.b_ = affineTransforms_[frame][cluster].b_ ;
		at.c_ = affineTransforms_[frame][cluster].c_;
		at.d_ = affineTransforms_[frame][cluster].d_;
		at.e_ = affineTransforms_[frame][cluster].e_;
		at.f_ = affineTransforms_[frame][cluster].f_;
		at.g_ = affineTransforms_[frame][cluster].g_;
		at.h_ = affineTransforms_[frame][cluster].h_;
		at.i_ = affineTransforms_[frame][cluster].i_;
		at.tx_ = affineTransforms_[frame][cluster].tx_;
		at.ty_ = affineTransforms_[frame][cluster].ty_;
		at.tz_ = affineTransforms_[frame][cluster].tz_;
		return true;
	}
	return false;
}

bool MotionModel::Get(float * coord0, int vertex, int frame, float * coord1) {
	if ((frame < F_) && (vertex < V_)){
		float coordTemp[3];
		// we set coord1 to 0
		for (int c = 0; c < 3; c++) {
			coord1[c] = 0.0f;
		}

		for (int k = 0; k < K_; k++) {
			affineTransforms_[frame][k].Get(coord0, coordTemp);
			for (int c = 0; c < 3; c++) {
				coord1[c] += coordTemp[c] * weights_[vertex][k];
			}
		}

		return true;
	}
	return false;
}
bool MotionModel::GetN(float * coord0, int vertex, int frame, float * coord1) {
	if ((frame < F_) && (vertex < V_)){
		float coordTemp[3];
		// we set coord1 to 0
		for (int c = 0; c < 3; c++) {
			coord1[c] = 0.0f;
		}

		for (int k = 0; k < K_; k++) {
			affineTransforms_[frame][k].GetN(coord0, coordTemp);
			for (int c = 0; c < 3; c++) {
				coord1[c] += coordTemp[c] * weights_[vertex][k];
			}
		}

		return true;
	}
	return false;


}

bool MotionModel::GetNO(float * coord0, int vertex, int frame, float * coord1) {
	if ((frame < F_) && (vertex < V_)){
		float u[3] = {1.0,0.0,0.0};
		float v[3] = {0.0,1.0,0.0};
		float n[3] = {0.0,0.0,1.0};

		for (int h = 0; h < 3; h++) {
			n[h] = coord0[h];
		}

		MathTools::vectorBasis(n, u, v);

		float us[3] = {0.0,0.0,0.0};
		float vs[3] = {0.0,0.0,0.0};
		GetN(u, vertex, frame, us);
		GetN(v, vertex, frame, vs);

		MathTools::vectorUnitary(us);
		MathTools::vectorUnitary(vs);

		MathTools::vectoriel(us,vs,coord1);


		return true;
	}
	return false;


}

void MotionModel::SetWeightFromPartiton(std::vector<int> & partition) {
	for(int v = 0; v < V_; v++) {
		for(int k = 0; k < K_; k++) {
			weights_[v][k] = 0.0f;
		}
	}

	for (int i = 0; i < (int) partition.size(); i++) {
		weights_[i][partition[i]] = 1.0f;
	}
}

void MotionModel::SetWeightFromPartiton(int vertex, std::vector<int> & partition) {
	for(int k = 0; k < K_; k++) {
		weights_[vertex][k] = 0.0f;
	}
	weights_[vertex][partition[vertex]] = 1.0f;
}

