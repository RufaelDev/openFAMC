/*!
 ************************************************************************
 *  \file
 *     AffineMotionPredictor.h
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
#pragma once
#include "nr.h"
#include <vector>
#include "Animation.h"
#include "AffineTransform.h"

#define TOL 0.00001 //0.001	
class AffineMotionPredictor
{
	Mat_DP pinvA_;
	std::vector<int> * vertices_;
	Animation * anim_;

public:
	void Init(Animation * anim, std::vector<int>  * vertices);
	void ComputeAffineTransform(int frame, AffineTransform &affineTransform);
	AffineMotionPredictor(void);
public:
	~AffineMotionPredictor(void);
};
