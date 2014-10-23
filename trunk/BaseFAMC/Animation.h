/*!
 ************************************************************************
 *  \file
 *     Animation.h
 *  \brief
 *     Animation class.
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
#include "IndexedFaceSet.h"
#include "MotionModel.h"

typedef std::vector<int> IntVect;
typedef std::map<int, IntVect> IntMultiVect;

class Animation
{
	float ** coord_;
	float ** normal_;			
	float ** color_;			
	float ** attributes_;

	float * coordKey_;			
	float * normalKey_;			
	float * colorKey_;			
	float * attributesKey_;



	int nCoord_;			
	int nNormal_;			
	int nColor_;			
	int nAttributes_;
	int nKeyCoord_;			
	int nKeyNormal_;			
	int nKeyColor_;			
	int nKeyAttributes_;

	double duration_;

	IntMultiVect adj_;	//	clusters adjacency


public:
	Animation(void);
	~Animation(void);
	inline double GetDuration(){return duration_;};
	inline void SetDuration(double duration ){duration_ = duration;};
	inline int GetNCoord(){ return nCoord_;};
	inline int GetNNormal(){ return nNormal_;};
	inline int GetNColor(){ return nColor_;};
	inline int GetNAttributes(){ return nAttributes_;};

	inline int GetNKeyCoord(){ return nKeyCoord_;};
	inline int GetNKeyNormal(){ return nKeyNormal_;};
	inline int GetNKeyColor(){ return nKeyColor_;};
	inline int GetNKeyAttributes(){ return nKeyAttributes_;};

	inline void SetNCoord(int nCoord){  nCoord_ = nCoord;};
	inline void SetNNormal(int nNormal){  nNormal_ = nNormal;};
	inline void SetNColor(int nColor){  nColor_ = nColor;};
	inline void SetNAttributes(int nAttributes){  nAttributes_= nAttributes;};

	inline void SetNKeyCoord(int nKeyCoord){  nKeyCoord_ = nKeyCoord;};
	inline void SetNKeyNormal(int nKeyNormal){  nKeyNormal_ = nKeyNormal;};
	inline void SetNKeyColor(int nKeyColor){  nKeyColor_= nKeyColor;};
	inline void SetNKeyAttributes(int nKeyAttributes){  nKeyAttributes_= nKeyAttributes;};

	inline float ** GetCoord(){return coord_;};
	inline float ** GetNormal(){return normal_;};
	inline float ** GetColor(){return color_;};
	inline float ** GetAttributes(){return attributes_;};
	
	inline float * GetKeyCoord(){return coordKey_;};
	inline float * GetKeyNormal(){return normalKey_;};
	inline float * GetKeyColor(){return colorKey_;};
	inline float * GetKeyAttributes(){return attributesKey_;};

	inline IntMultiVect * GetAdj() { return &adj_;};
	
	bool GetCoord(int frame, int vertex, float * coord);
	bool GetColor(int frame, int vertex, float * color);
	bool GetNormal(int frame, int vertex, float * normal);
	bool SetCoord(int frame, int vertex, float * coord);
	bool SetColor(int frame, int vertex, float * color);
	bool SetNormal(int frame, int vertex, float * normal);

	double ComputeFirstFrameBBDiagonal();

	void FreeMem();
	void FreeFloat(float ** tab, int dim);
	void AllocateMem();
	bool LoadInterpolatorsVRML2(char * fileName);
	bool SaveInterpolatorVRML2(char * fileName, IndexedFaceSet & ifs);
	int SaveFrame(char * fileName, IndexedFaceSet & ifs, int frame, float ** color = NULL);
	int SavePartitionnedFirstFrame(char * file, IndexedFaceSet & ifs, std::vector<int> * partition, int k);
	void ComputeClustersAdjacency(std::vector<int> * partition, int ** connec, int T);
	void AddNeighbor(int v1, int v2);
	void ComputeErrorDistribution(MotionModel &motionModel, std::vector<float> & errors,
										 std::vector<int> * partition, float & errMM, float & errAM);
	float ComputeVertexErrorsDiff(int v, MotionModel &motionModel, std::vector<int> * partition);
	bool FindNearestCoordKey(float t, int & frame);
	void AnimToIFS(IndexedFaceSet &ifs, int frame);
	void IFSToAnim(IndexedFaceSet &ifs, int frame);
	void GetAnimationSegment(Animation & segment, int frameStart, int frameEnd);
	void GetAnimationSegment(Animation & segment, IndexedFaceSet &ifs, int frameStart, int frameEnd);
	int GetAnimationData(std::vector<float> &keys, std::vector<float> & coord, std::vector<float> &normal, std::vector<float> &color, int start);
	void CreateAnimation(std::vector<float> &keys, std::vector<float> & coord, std::vector<float> &normal, std::vector<float> &color);
};
