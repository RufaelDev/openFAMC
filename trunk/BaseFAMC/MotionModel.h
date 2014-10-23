/*!
 ************************************************************************
 *  \file
 *     MotionModel.h
 *  \brief
 *     MotionModel class.
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
#include "AffineTransform.h"
#include<vector>

class MotionModel
{
	AffineTransform ** affineTransforms_;
	float ** weights_;
	int K_;
	int V_;
	int F_;
public:
	
	MotionModel(int K, int V, int F);
	
	bool GetAffineTransform(AffineTransform & at, int frame, int cluster);
	bool SetAffineTransform(AffineTransform & at, int frame, int cluster);
	
	inline bool SetWeight(float w, int vertex, int cluster) {
		if ((vertex < V_) && (cluster < K_)){
			weights_[vertex][cluster] = w;
			return true;
		}
		return false;
	};

	inline bool GetWeight(float &w, int vertex, int cluster) {
		if ((vertex < V_) && (cluster < K_)){
			w = weights_[vertex][cluster];
			return true;
		}
		return false;
	};


	inline int GetNClusters() {return K_;};
	inline int GetNVertices() {return V_;};
	inline int GetNFrames() { return F_; };	
	inline void InitWeights() {
		for (int v = 0; v < V_; v++) {
			for (int k = 0; k < K_; k++) {
				weights_[v][k] = 0.0;
			}
		}
	};
	void SetWeightFromPartiton(std::vector<int> & partition);
	void SetWeightFromPartiton(int vertex, std::vector<int> & partition);

	bool Get(float * coord0, int vertex, int frame, float * coord1);
	bool GetN(float * coord0, int vertex, int frame, float * coord1);
	bool GetNO(float * coord0, int vertex, int frame, float * coord1);


public:
	~MotionModel(void);
};
