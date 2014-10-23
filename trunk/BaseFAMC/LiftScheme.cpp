/*!
 ************************************************************************
 *  \file
 *     LiftScheme.cpp
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
#include "LiftScheme.h"
#include <stdio.h>
#include <vector>
#include <math.h>
int LiftScheme::Round(double a){
	if (a >= 0.0) return (int) (a+0.5);
	return -(int) (-a+0.5);
}

LiftScheme::LiftScheme(int n)
{
	dim_ = n;
	vector_ = new double [dim_];

}

LiftScheme::~LiftScheme(void)
{
	if (vector_ != NULL) {
		delete [] vector_;
	}
}

void LiftScheme::Print()
{
	for (int k = 0; k < dim_; k++) {
		printf("%i\t%f\n", k, vector_[k]);
	}
	printf("\n");
}

int LiftScheme::Split(int n) {
	double * temp = new double [n];

	if ( n%2 == 1) { // odd
		for (int k = 0; k < n/2+1; k++) {
			temp[k] = vector_[2*k];
			if (k != n/2) {
				temp[n/2+k+1] = vector_[2*k+1];
			}
		}
	}
	else {
		for (int k = 0; k < n/2; k++) {
			temp[k] = vector_[2*k];
			temp[n/2+k] = vector_[2*k+1];
		}
	}

	for (int k = 0; k < n; k++) {
		vector_[k] = temp[k];
	}
	delete [] temp;
	return 0;
}

int LiftScheme::Merge(int n) {
	double * temp = new double [n];

	if ( n%2 == 1) { // odd
		for (int k = 0; k < n/2+1; k++) {
			temp[2*k] = vector_[k];
			if (k != n/2) {
				temp[2*k+1] = vector_[n/2+k+1];
			}
		}
	}
	else {
		for (int k = 0; k < n/2; k++) {
			temp[2*k] = vector_[k];
			temp[2*k+1] = vector_[n/2+k];
		}
	}

	for (int k = 0; k < n; k++) {
		vector_[k] = temp[k];
	}
	delete [] temp;
	return 0;
}

int LiftScheme::Predict4(int n, bool directTransform) {	// we suppose that n >= 4
	int posd = n/2;
	if ( n%2 == 1) { // odd
		posd++;
	}

	double pv = 0.0;
	int done = 0;
	for (int k = 0; k < n/2; k++) {
		done  = 0;
		if (k == 0) {
			pv = vector_[0] * 0.3125 + vector_[1] * 0.9375 + vector_[2] * (-0.3125) + vector_[3] * 0.0625; 
			done = 1;
		}
		if (k == posd-1) {
			pv = vector_[posd-4] * (-0.3125) + vector_[posd-3] * 1.3125 + vector_[posd-2] * (-2.1875) + vector_[posd-1] * 2.1875;
			done = 1;
		}
		if (k == posd-2) {
			pv = vector_[posd-4] * 0.0625 + vector_[posd-3] * -0.3125 + vector_[posd-2] * 0.9375 + vector_[posd-1] * 0.3125; 
			done = 1;
		}
		if (done == 0) {
				pv = vector_[k-1] * (-0.0625) + vector_[k] * 0.5625 + vector_[k+1] * 0.5625 + vector_[k+2] * -0.0625; 
		}

		if ( directTransform ) {
			vector_[posd+k] -= pv;
		}
		else {
			vector_[posd+k] += pv;
		}
	}
	return 0;
}

int LiftScheme::Update2(int n, bool directTransform) {	// we suppose that n >= 4
	int posd = n/2;
	if ( n%2 == 1) { // odd
		posd++;
	}

	double uv = 0.0;
	for (int k = 1; k < posd-1; k++) {
		uv = vector_[posd+k-1] * 0.25 + vector_[posd+k] * 0.25 ; 
		if ( directTransform ) {
			vector_[k] += uv;
		}
		else {
			vector_[k]-= uv;
		}
	}
	return 0;
}



int LiftScheme::Lift(bool directTransform) {

	if ( directTransform ) {
		int n = dim_;
		while (n>= 7) {
			Split(n);
			Predict4(n, true);
			Update2(n, true);
			if ( n%2 == 1) { // odd
				n = n/2 + 1;
			}
			else {
				n = n/2;
			}
		}
	}
	else {
		int n = dim_;
		std::vector<int> p;
		while (n>= 7) {
			p.push_back(n);
			if ( n%2 == 1) { // odd
				n = n/2 + 1;
			}
			else {
				n = n/2;
			}
		}
		for (int i = (int) p.size()-1; i >= 0 ; i--) {
			n = p[i];
			Update2(n, false);
			Predict4(n, false);
			Merge(n);
		}
	}
	
	return 0;
}


// for lossless compression
int LiftScheme::Predict4LS(int n, bool directTransform) {	// we suppose that n >= 4
	int posd = n/2;
	if ( n%2 == 1) { // odd
		posd++;
	}

	double pv = 0.0;
	int done = 0;
	for (int k = 0; k < n/2; k++) {
		done  = 0;
		if (k == 0) {
			pv = vector_[0] * 0.3125 + vector_[1] * 0.9375 + vector_[2] * (-0.3125) + vector_[3] * 0.0625; 
			done = 1;
		}
		if (k == posd-1) {
			pv = vector_[posd-4] * (-0.3125) + vector_[posd-3] * 1.3125 + vector_[posd-2] * (-2.1875) + vector_[posd-1] * 2.1875;
//			pv = vector_[posd-1];
			done = 1;
		}
		if (k == posd-2) {
			pv = vector_[posd-4] * 0.0625 + vector_[posd-3] * (-0.3125) + vector_[posd-2] * 0.9375 + vector_[posd-1] * 0.3125; 
			done = 1;
		}
		if (done == 0) {
				pv = vector_[k-1] * (-0.0625) + vector_[k] * 0.5625 + vector_[k+1] * 0.5625 + vector_[k+2] * (-0.0625); 
		}

		if ( directTransform ) {
			vector_[posd+k] -= Round(pv);
		}
		else {
			vector_[posd+k] += Round(pv);
		}
	}
	return 0;
}

int LiftScheme::Update2LS(int n, bool directTransform) {	// we suppose that n >= 4
	int posd = n/2;
	if ( n%2 == 1) { // odd
		posd++;
	}

	double uv = 0.0;
	for (int k = 1; k < posd-1; k++) {
		uv = vector_[posd+k-1] * 0.25 + vector_[posd+k] * 0.25 ; 
		if ( directTransform ) {
			vector_[k] += Round(uv);
		}
		else {
			vector_[k] -= Round(uv);
		}
	}
	return 0;
}



int LiftScheme::LiftLS(bool directTransform) {

	if ( directTransform ) {
		int n = dim_;
		while (n>= 7) {
			Split(n);
			Predict4LS(n, true);
			Update2LS(n, true);
			if ( n%2 == 1) { // odd
				n = n/2 + 1;
			}
			else {
				n = n/2;
			}
		}
	}
	else {
		int n = dim_;
		std::vector<int> p;
		while (n>= 7) {
			p.push_back(n);
			if ( n%2 == 1) { // odd
				n = n/2 + 1;
			}
			else {
				n = n/2;
			}
		}
		for (int i = (int) p.size()-1; i >= 0 ; i--) {
			n = p[i];
			Update2LS(n, false);
			Predict4LS(n, false);
			Merge(n);
		}
	}
	
	return 0;
}
