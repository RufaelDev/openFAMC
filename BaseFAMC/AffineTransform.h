/*!
 ************************************************************************
 *  \file
 *     AffineTransform.h
 *  \brief
 *     AffineTransform class.
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

class AffineTransform
{
public:
	float a_, b_, c_, tx_;
	float d_, e_, f_, ty_;
	float g_, h_, i_, tz_;

	inline void Get(float * coord0, float * coord1) {
		coord1[0] = a_*coord0[0] + b_*coord0[1] + c_*coord0[2] + tx_;
		coord1[1] = d_*coord0[0] + e_*coord0[1] + f_*coord0[2] + ty_;
		coord1[2] = g_*coord0[0] + h_*coord0[1] + i_*coord0[2] + tz_;
	};
	inline void GetN(float * coord0, float * coord1) {
		coord1[0] = a_*coord0[0] + b_*coord0[1] + c_*coord0[2];
		coord1[1] = d_*coord0[0] + e_*coord0[1] + f_*coord0[2];
		coord1[2] = g_*coord0[0] + h_*coord0[1] + i_*coord0[2];
	};

	AffineTransform(void);
public:
	~AffineTransform(void);
};
