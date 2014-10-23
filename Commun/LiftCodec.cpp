/*!
 ************************************************************************
 *  \file
 *     LiftCodec.cpp
 *  \brief
 *     LiftCodec class.
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
#include "LiftCodec.h"
#include "LiftScheme.h"
#include "CABAC.h"
#include "Console.h"
#include <math.h>
extern CConsole logger;

LiftCodec::LiftCodec(int len, int nbr){
	len_ = len;
	nbr_ = nbr;
	function_ = new double*[nbr_];
	for (int v = 0; v < nbr_; v++){	
		function_[v] = new double[len_];
	}
	for (int v = 0; v < nbr_; v++){	
		for (int k = 0; k < len_; k++){
			function_[v][k] = 0.0;
		}
	}
}

LiftCodec::~LiftCodec(void){
	// cleraning the structure
	for (int v = 0; v < nbr_; v++){	
		delete[] function_[v];
	}
	delete[] function_;
}
void LiftCodec::LiftLS(bool directTransform) {
	LiftScheme lift(len_);
	for (int v = 0; v < nbr_; v++){
		for (int k = 0; k < len_; k++) {
			lift.Set(k, function_[v][k]);
		}
		lift.LiftLS(directTransform);
		for (int k = 0; k < len_; k++) {
			lift.Get(k, function_[v][k]);
		}
	}
}
int LiftCodec::Round(double a){
	if (a >= 0.0) return (int) (a+0.5);
	return -(int) (-a+0.5);
}




int LiftCodec::Quantize(unsigned char nbits, Vec3D<int> *pcIntVertices, int iStride, int iCoord, float *pfMax, float *pfMin){
	*pfMax = (float) function_[0][0];
	*pfMin = (float) function_[0][0];
	for (int k = 0; k < len_; k++) {
		for (int v = 0; v < nbr_; v++){
			if ( function_[v][k] >  *pfMax)  *pfMax = (float) function_[v][k];
			if ( function_[v][k] <  *pfMin)  *pfMin = (float) function_[v][k];
		}
	}

	// Constant for uniform quantization
	double delta = 1.0;
	if ((*pfMax-*pfMin) != 0.0) delta = (double) ((1 << nbits) - 1) / (*pfMax-*pfMin);

	// Quantize function
	for (int k = 0; k < len_; k++) {
		for (int v = 0; v < nbr_; v++){
			function_[v][k] = (double) Round((function_[v][k] - *pfMin) * delta);
		}
	}

	// we apply the lifting scheme
	LiftLS(true);

	for (int k = 0; k < len_; k++) {
		for (int v = 0; v < nbr_; v++){
			pcIntVertices[v * iStride + k].data[iCoord] = (int) function_[v][k];
		}
	}

	// we apply the unlifting scheme
	LiftLS(false);	

	// unquantize function
	for (int k = 0; k < len_; k++) {
		for (int v = 0; v < nbr_; v++){
			function_[v][k] = function_[v][k] / delta + *pfMin;
		}
	}
	return 0;
}

int LiftCodec::Unquantize(unsigned char nbits, Vec3D<int> *pcIntVertices, int iStride, int iCoord, float *pfMax, float *pfMin){
	if (nbits > 0) {
		for (int k = 0; k < len_; k++) {
			for (int v = 0; v < nbr_; v++){
				function_[v][k]=0.0f;
			}
		}

		for (int k = 0; k < len_; k++) {
			for (int v = 0; v < nbr_; v++){
				function_[v][k] = (double) pcIntVertices[v * iStride + k].data[iCoord];
			}
		}
		
		// we apply the unlifting scheme
		LiftLS(false);	


		// Constant for uniform quantization
		double delta = 1.0;
		if ((*pfMax-*pfMin) != 0.0) delta = (double) ((1 << nbits) - 1) / (*pfMax-*pfMin);

		// unquantize function
		for (int k = 0; k < len_; k++) {
			for (int v = 0; v < nbr_; v++){
				function_[v][k] = function_[v][k] / delta + *pfMin;
			}
		}
	}
	return 0;
}

int LiftCodec::Quantize2(unsigned char nbits, float &pfMax, float &pfMin){
	pfMax = (float) function_[0][0];
	pfMin = (float) function_[0][0];
	for (int k = 0; k < len_; k++) {
		for (int v = 0; v < nbr_; v++){
			if ( function_[v][k] >  pfMax)  pfMax = (float) function_[v][k];
			if ( function_[v][k] <  pfMin)  pfMin = (float) function_[v][k];
		}
	}

	// Constant for uniform quantization
	double delta = 1.0;
	if ((pfMax-pfMin) != 0.0) delta = (double) ((1 << nbits) - 1) / (pfMax-pfMin);

	// Quantize function
	for (int k = 0; k < len_; k++) {
		for (int v = 0; v < nbr_; v++){
			function_[v][k] = (double) Round((function_[v][k] - pfMin) * delta);
		}
	}
	return 0;
}
int LiftCodec::Unquantize2(unsigned char nbits, float pfMax, float pfMin){
	if (nbits > 0) {	
		// Constant for uniform quantization
		double delta = 1.0;
		if ((pfMax-pfMin) != 0.0) delta = (double) ((1 << nbits) - 1) / (pfMax-pfMin);

		// unquantize function
		for (int k = 0; k < len_; k++) {
			for (int v = 0; v < nbr_; v++){
				function_[v][k] = function_[v][k] / delta + pfMin;
			}
		}
	}
	return 0;
}
