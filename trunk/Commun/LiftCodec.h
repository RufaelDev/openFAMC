/*!
 ************************************************************************
 *  \file
 *     LiftCodec.h
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
#pragma once
#include <vector>
#include "FAMCCommon.h"

class LiftCodec
{
	int len_; // dim for wavelet computation
	int nbr_;// nbr of vectors
public:
	double ** function_;

	LiftCodec(int len, int nbr);
	void LiftLS(bool directTransform);
	int Unquantize(unsigned char nbits, Vec3D<int> *pcIntVertices, int iStride, int iCoord, float *pfMax, float *pfMin);
	int Unquantize2(unsigned char nbits, float pfMax, float pfMin);
	int Quantize(unsigned char nbits, Vec3D<int> *pcIntVertices, int iStride, int iCoord, float *pfMax, float *pfMin);
	int Quantize2(unsigned char nbits, float &pfMax, float &pfMin);
	float ComputeCost(int ** tab, int v1, int v2, int dimp, int dim);
	int Round(double a);
public:
	~LiftCodec(void);
};
