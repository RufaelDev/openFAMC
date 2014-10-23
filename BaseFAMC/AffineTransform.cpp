/*!
 ************************************************************************
 *  \file
 *     AffineTransform.cpp
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
#include "AffineTransform.h"

AffineTransform::AffineTransform(void)
{
	a_ = e_ = i_ = 1.0f;
	b_ = c_ = d_ = f_ = g_ = h_ = tx_ = ty_ = tz_ = 0.0;
}

AffineTransform::~AffineTransform(void)
{
}
