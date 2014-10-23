/*!
 ************************************************************************
 *  \file
 *     DCTEncoder.cpp
 *  \brief
 *     DCTEncoder class.
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
#include "DCTEncoder.h"
#include "CABAC.h"
#include "Console.h"
#include <math.h>
extern CConsole logger;
#define PI 3.141592653589 

DCTEncoder::DCTEncoder(int len, int nbr)
{
	len_ = len;
	nbr_ = nbr;
	function_ = new float*[nbr_];
	spectrum_ = new float*[nbr_];
	base_ = new float*[len_];

	for (int v = 0; v < nbr_; v++){	
		function_[v] = new float[len_];
		spectrum_[v] = new float[len_];
	}

	for (int k = 0; k < len_; k++) {
		for (int v = 0; v < nbr_; v++){			
			spectrum_[v][k] = 0.0;
			function_[v][k] = 0.0;
		}
	}


	for (int n = 0; n < len_; n++){
		base_[n] = new float[len_];
		float norm = 0.0;
		for (int k = 0; k < len_; k++){
			if (n == 0) {
				base_[n][k] = (float) (sqrt(1.0/len_) * cos(PI*(2*k+1.0)*n/(2.0 * len_)));
			}
			else {
				base_[n][k] = (float) (sqrt(2.0/len_) *  cos(PI*(2*k+1.0)*n/(2.0 * len_)));
			}
		}
	}
}



DCTEncoder::~DCTEncoder()
{
	// cleraning the structure
	for (int v = 0; v < nbr_; v++){	
		delete[] function_[v];
		delete[] spectrum_[v];
	}
	for (int n = 0; n < len_; n++){
		delete[] base_[n];
	}

	delete[] base_;
	delete[] function_;
	delete[] spectrum_;
}


float DCTEncoder::dot(float * vect, int n) {
	float sum = 0.0;
	for (int k = 0; k < len_; k++){
		sum += base_[n][k] * vect[k];
	}
	return sum;
}

void DCTEncoder::DCT() {
	for (int v = 0; v < nbr_; v++){	
		for (int n = 0; n < len_; n++) {
			float d = dot(function_[v], n);
			 spectrum_[v][n] = d;
		}
	}
}

void DCTEncoder::IDCT() {
	for (int v = 0; v < nbr_; v++){			
		for (int k = 0; k < len_; k++) {
			function_[v][k] = 0.0;
			for (int n = 0; n < len_; n++) {
				function_[v][k] += spectrum_[v][n] * base_[n][k];
			}
		}
	}
}

void  DCTEncoder::Save(char *fileName) {
	FILE* file = fopen(fileName, "w");
	fprintf(file, "%i %i\n", nbr_, len_);
	for (int v = 0; v < nbr_; v++){			
		for (int k = 0; k < len_; k++) {
			fprintf(file, "%3.6f\t", function_[v][k]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}
void  DCTEncoder::SaveS(char *fileName) {
	FILE* file = fopen(fileName, "w");
	fprintf(file, "%i %i\n", nbr_, len_);
	for (int v = 0; v < nbr_; v++){			
		for (int k = 0; k < len_; k++) {
			fprintf(file, "%f\t", spectrum_[v][k]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

void  DCTEncoder::LoadS(char *fileName) {
	FILE* file = fopen(fileName, "r");
	int nbr = 0;
	int len = 0;
	fscanf(file, "%i", &nbr);
	fscanf(file, "%i", &len);
	for (int v = 0; v < nbr; v++){			
		for (int k = 0; k < len; k++) {
			fscanf(file, "%f", &spectrum_[v][k]);
		}
	}
	fclose(file);
}
void  DCTEncoder::Load(char *fileName) {
	FILE* file = fopen(fileName, "r");
	int nbr = 0;
	int len = 0;
	fscanf(file, "%i", &nbr);
	fscanf(file, "%i", &len);
	for (int v = 0; v < nbr; v++){			
		for (int k = 0; k < len; k++) {
			fscanf(file, "%f", &function_[v][k]);
		}
	}
	fclose(file);
}
int DCTEncoder::Round(double a){
	if (a >= 0.0) return (int) (a+0.5);
	return -(int) (-a+0.5);
}


int DCTEncoder::Quantize(unsigned char nbits, Vec3D<int> *pcIntVertices, int iStride, int iCoord, float *pfMax, float *pfMin){
	// we compute the max for abs(DCT)
	*pfMax = fabs(spectrum_[0][0]);
	*pfMin = 0.0f;
	for (int k = 0; k < len_; k++) {
		for (int v = 0; v < nbr_; v++){
			if ( fabs(spectrum_[v][k]) >  *pfMax)  *pfMax = fabs(spectrum_[v][k]);
		}
	}

	// Constant for uniform quantization
	double delta = 1.0;
	if (*pfMax != 0.0) delta = (double) ((1 << nbits) - 1) / *pfMax;

	// Unquantize spectrum
	for (int k = 0; k < len_; k++) {
		for (int v = 0; v < nbr_; v++){
			pcIntVertices[v * iStride + k].data[iCoord] = Round(delta * spectrum_[v][k]);
			spectrum_[v][k] =  (float)((double) pcIntVertices[v * iStride + k].data[iCoord] /  delta);
		}
	}
	return 0;
}
int DCTEncoder::Unquantize(unsigned char nbits, Vec3D<int> *pcIntVertices, int iStride, int iCoord, float *pfMax, float *pfMin){
	// we compute the max for abs(DCT)
	if (nbits > 0) {
		for (int k = 0; k < len_; k++) {
			for (int v = 0; v < nbr_; v++){
				spectrum_[v][k]=0.0f;
			}
		}

		// Constant for uniform quantization
		double delta = 1.0;
		if (*pfMax != 0.0) delta = (double) ((1 << nbits) - 1) / *pfMax;

		// Unquantize spectrum
		for (int k = 0; k < len_; k++) {
			for (int v = 0; v < nbr_; v++){
				spectrum_[v][k] =  (float)((double) pcIntVertices[v * iStride + k].data[iCoord] /  delta);
			}	
		}
	}
	return 0;
}