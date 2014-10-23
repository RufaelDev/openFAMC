/*!
 ************************************************************************
 *  \file
 *     MathTools.h
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
#pragma once

class MathTools
{
public:
static void vecteur(float *pt1, float *pt2, float *vect, int n);
static void vectoriel(float vect1[3], float vect2[3], float vect[3]);
static float vectorNorm(float vect[3]);
static void vectorUnitary(float vect[3]);
static float vectorDot(float vect1[3], float vect2[3]);
static void vectorBasis(float vect1[3], float vect2[3], float vect3[3]);
static void cartesian2Spherical(float coord[3], float& r, float& tetha, float& phi);
static void spherical2Cartesian(float r, float tetha, float phi, float coord[3]);
static float Signe(float x);
static int nBinaryBits(unsigned int m);

MathTools(void);
public:
	~MathTools(void);
};
