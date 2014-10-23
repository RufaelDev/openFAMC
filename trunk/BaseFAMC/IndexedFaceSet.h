/*!
 ************************************************************************
 *  \file
 *     IndexedFaceSet.h
 *  \brief
 *     IndexedFaceSet class. It supports only triangulair meshes.
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
#include <map>

typedef std::vector<int> IntVect;
typedef std::map<int, IntVect> IntMultiVect;

class IndexedFaceSet
{
	// See VRML 2.0 specifications
	float * coord_;			
	float * color_;
	float * normal_;
	float * texCoord_;
	int nCoord_;
	int nColor_;
	int nNormal_;
	int nTexCoord_;
	
	bool ccw_;
	bool solid_;
	bool convex_;
	bool colorPerVertex_;
	bool normalPerVertex_;

	float creaseAngle_;

	int ** coordIndex_;
	int ** colorIndex_;
	int ** normalIndex_;
	int ** texCoordIndex_;
	int nCoordIndex_;
	int nColorIndex_;
	int nNormalIndex_;
	int nTexCoordIndex_;
	
	int * traversal_;

public:
	IntMultiVect vertex2Vertex_;	//	clusters adjacency

	inline int * GetVerticesTraversal() { return traversal_;};
	inline float GetCreaseAngle(){return creaseAngle_;};
	inline void SetCreaseAngle(float creaseAngle){creaseAngle_ = creaseAngle;};
	//------------------
	inline int GetNCoordIndex(){ return nCoordIndex_;};
	inline int GetNNormalIndex(){ return nNormalIndex_;};
	inline int GetNColorIndex(){ return nColorIndex_;};
	inline int GetNTexCoordIndex(){ return nTexCoordIndex_;};
	inline void SetNCoordIndex(int nCoordIndex){ nCoordIndex_ = nCoordIndex;};
	inline void SetNNormalIndex(int nNormalIndex){ nNormalIndex_ = nNormalIndex;};
	inline void SetNColorIndex(int nColorIndex){ nColorIndex_ = nColorIndex;};
	inline void SetNTexCoordIndex(int nTexCoordIndex){ nTexCoordIndex_ = nTexCoordIndex;};
	//------------------
	inline int GetNCoord(){ return nCoord_;};
	inline int GetNNormal(){ return nNormal_;};
	inline int GetNColor(){ return nColor_;};
	inline int GetNTexCoord(){ return nTexCoord_;};
	inline void SetNCoord(int nCoord){nCoord_ = nCoord;};
	inline void SetNNormal(int nNormal){nNormal_ = nNormal;};
	inline void SetNColor(int nColor){nColor_ = nColor;};
	inline void SetNTexCoord(int nTexCoord){nTexCoord_ = nTexCoord;};
	//------------------
	inline bool GetCCW() { return ccw_;}
	inline bool GetSolid() { return solid_;};
	inline bool GetConvex() { return convex_;};
	inline bool GetColorPerVertex() { return colorPerVertex_;};
	inline bool GetNormalPerVertex() { return normalPerVertex_;};
	inline void SetCCW(bool ccw) { ccw_ = ccw;}
	inline void SetSolid(bool solid) { solid_ = solid;};
	inline void SetConvex(bool convex) { convex_ = convex;};
	inline void SetColorPerVertex(bool colorPerVertex) { colorPerVertex_ = colorPerVertex;};
	inline void SetNormalPerVertex(bool normalPerVertex) {normalPerVertex_ = normalPerVertex;};
	//------------------
	bool GetCoordIndex(int pos, int * coordIndex);
	bool GetNormalIndex(int pos, int * normalIndex);
	bool GetColorIndex(int pos, int * colorIndex);
	bool GetTexCoordIndex(int pos, int * texCoordIndex);
	bool GetCoord(int vertex, float * coord);
	bool GetNormal(int pos, float * normal);
	bool GetColor(int pos, float * color);
	bool GetTexCoord(int pos, float * texCoord);

	bool SetCoordIndex(int pos, int * coordIndex);
	bool SetNormalIndex(int pos, int * normalIndex);
	bool SetColorIndex(int pos, int * colorIndex);
	bool SetTexCoordIndex(int pos, int * texCoordIndex);
	bool SetCoord(int vertex, float * coord);
	bool SetNormal(int pos, float * normal);
	bool SetColor(int pos, float * color);
	bool SetTexCoord(int pos, float * texCoord);

	//------------------
	inline int ** GetCoordIndex(){return coordIndex_;};
	inline int ** GetNormalIndex(){return normalIndex_;};
	inline int ** GetColorIndex(){return colorIndex_;};
	inline int ** GetTexCoordIndex(){return texCoordIndex_;};
	//------------------
	inline float * GetCoord(){return coord_;};
	inline float * GetNormal(){return normal_;};
	inline float * GetColor(){return color_;};
	inline float * GetTexCoord(){return texCoord_;};



	void FreeInt(int ** tab, int dim);
	void FreeMem();
	bool AllocateMem();

	bool LoadIFSVRML2(char * fileName);
	bool SaveIFSVRML2(char * fileName);

	void AddNeighborVertex2Vertex(int v1, int v2);
	void ComputeVertex2Vertex();
	void ComputeVerticesTraversal(std::vector<int> * partition);

	IndexedFaceSet(void);
	~IndexedFaceSet(void);
};
