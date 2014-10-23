/*
 * =====================================================================================
 * 
 *        Filename:  SConnectivity.hh
 * 
 *     Description:  A simple connectivity class supporting simplification operations
 * 
 *         Version:  1.0
 *         Created:  07/09/07 17:15:18 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */


#ifndef  CONNECTIVITY_H
#define  CONNECTIVITY_H

#include <vector>

class SConnectivity{
    //MEMBER VARIABLES
    private:
	std::vector< std::vector<int> > fv_;
	std::vector< std::vector<int> > vf_;
	std::vector<bool> isVertexDeleted_;
	std::vector<bool> isFaceDeleted_;
	
    //TYPE DEFS
    public:
	typedef std::vector< std::vector<int> > IFS;

    //CONSTRUCTORS
    public:
	SConnectivity(const IFS& _ifs);
	SConnectivity(const IFS& _ifs, int _nVertices);
	
    //MEMBER FUNCTIONS
    public:

	//get functions
	IFS getIFS() const ;
	int nVertices() const { return vf_.size(); }
	int nFaces() const { return fv_.size(); }
	const std::vector<int>& getFV(int _f) const; 
	const std::vector<int>& getVF(int _v) const; 
	std::vector<int> getOneRingVertices(int _v) const ;
	std::vector<int> getNeighboringFaces(int _f) const;
	int degree(int _v) const { return vf_[_v].size(); }
	
	// predicates
	bool isVertexDeleted(int _v) const; 
	bool isFaceDeleted(int _f) const; 
	bool isEdgeCollapseAllowed(int _vertexIndex, int _vertexIndexCollapse) const;
	bool isRegularSimplificationAllowed(int _vertexIndex, int _vertexIndexCollapse) const;
	bool hasOrientableOneRingVertices(int _v) const;

	//manipulation functions
	bool simplify(int _vertexIndex, int _simplificationMode);

    private:
	//initialize vf_
	void initVF(int _size);

	//get functions
	std::vector<int> getOneRingVerticesInc(int _v) const ;
	std::vector<int> getOneRingVerticesCCW(int _v) const;

	//predicates
	bool areNeighboringVertices(int _v0, int _v1) const;
	bool areNeighboringFaces(int _f0, int _f1) const;
	bool isNeighborFaceCW(int _f, int _fl, int _v) const;
	bool isNeighborFaceCCW(int _f, int _fr, int _v) const;
	bool isDegeneratedFace(int _f) const;
	bool faceContainsVertex(int _f, int _v) const;
	bool equalFaceVertices(int _f0, int _f1) const;
	bool isFanCCW(const std::vector<int>& _faces, int _vAnchor) const;
	bool isClosedFanCCW(const std::vector<int>& _faces, int _vAnchor) const;

	//manipulation functions
	bool edgeCollapse(int _vertexIndex, int _vertexIndexCollapse);
	bool regularSimplification(int _vertexIndex, int _vertexIndexCollapse);
	bool regularSimplification6(int _vertexIndex);
	bool regularSimplification7(int _vertexIndex);
	bool deleteFace(int _f);
	bool deleteDoubleFaces(int _vertexIndex);
	bool deleteDegeneratedFaces(int _vertexIndex);
	bool deleteVertex(int _v);

	//manupulative helper functions
	void replaceFaceVertex(int _f, int _v1, int _v2);
	void flipFacesCCW(int _f0, int _f1, int _vAnchor);

	//non manupulative helper functions
	bool sortToFanCCW(std::vector<int>& _faces, int _vAnchor) const;
	void sortIncreasing(std::vector<int>& _vec) const;
	int maxIdx(const IFS& _ifs) const;
	bool adjustToIdx(std::vector<int>& _vec, int _idx) const;

};


#endif   /* ----- #ifndef CONNECTIVITY_H  ----- */
