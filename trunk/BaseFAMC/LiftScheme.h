/*!
 ************************************************************************
 *  \file
 *     LiftScheme.h
 *  \brief
 *     LiftScheme class.
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

class LiftScheme
{
	double * vector_;
	int dim_;
public:
	LiftScheme(int n);
	void Print();

	int LiftScheme::Round(double a);

	inline int Set(int pos, double v) {
		if ((pos < dim_) && (pos>=0) ){
			vector_[pos] = v;
			return 0;
		}
		return -1;
	}
	inline int Get(int pos, double &v) {
		if ((pos < dim_) && (pos>=0) ){
			v = vector_[pos];
			return 0;
		}
		return -1;
	}

	int Lift(bool directTransform);
	int Split(int n);
	int Merge(int n);
	int Predict4(int n, bool directTransform);
	int Update2(int n, bool directTransform);
//	for lossless compression
	int LiftLS(bool directTransform);
	int Predict4LS(int n, bool directTransform);
	int Update2LS(int n, bool directTransform);


public:
	~LiftScheme(void);
};
