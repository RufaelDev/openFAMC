/*!
 ************************************************************************
 *  \file
 *     DCTEncoder.h
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

#pragma once
#include <vector>
#include "FAMCCommon.h"

#include "CABAC.h"

class DCTEncoder  
{
public:
	float ** function_;
	float ** spectrum_;

	float ** base_;
	
	
	int len_; // dim for DCT computation
	int nbr_;// nbr of vectors

	float dot(float * vect, int n);	// retourne la projection sur le nième vecteur de la base
	void DCT();
	void IDCT();

	int Quantize(unsigned char nbits, Vec3D<int> *pcIntVertices, int iStride, int iCoord, float *pfMax, float *pfMin);
	int Unquantize(unsigned char nbits, Vec3D<int> *pcIntVertices, int iStride, int iCoord, float *pfMax, float *pfMin);

	void Save(char *fileName);
	void SaveS(char *fileName);

	void LoadS(char *fileName);
	void Load(char *fileName);

	DCTEncoder(int len, int nbr);

	static int Round(double a);

	virtual ~DCTEncoder();

};
