/*
 * =====================================================================================
 * 
 *        Filename:  TraversalDecomposer.hh
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  07/12/07 15:54:08 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst), stefanos@tnt.uni-hannover.de
 *         Company:  Institut für Informationsverarbeitung
 * 
 * =====================================================================================
 */


#ifndef  TRAVERSAL_DECOMPOSER_HH
#define  TRAVERSAL_DECOMPOSER_HH

#include "SConnectivity.h"
#include "LayeredDecomposition.h"
#include <vector>


class TraversalDecomposer {
    //CONSTRUCTORS
    public:
	TraversalDecomposer(const SConnectivity& _conn) : 
	    conn_(_conn),
       faceTag(conn_.nFaces(), false),
       vertexTag(conn_.nVertices(), false) {
	   for (int f=0; f<conn_.nFaces(); ++f){
	       if (conn_.isFaceDeleted(f)) faceTag[f]=true;
	   }
	   for (int v=0; v<conn_.nVertices(); ++v){
	       if (conn_.isVertexDeleted(v)) vertexTag[v]=true;
	   }
       }

    //MEMBER FUNCTIONS
    public:
	void createLayerContext(LayerContext& _lc);
	bool isConsistent(const LayerContext& _lc) const;

    private:
	void traverse(std::vector<int>& _vertexIndices);
	int getNextSeed() const;
	void addVertices(int _f, std::vector<int>& _newVertices);
	void addRemainingVertices(std::vector<int>& _vertexIndices);
	void createVertexContext(int _to, VertexContext2& _vc, const std::vector<bool>& _isVisited) const;
	    
    //MEMBER VARIABLES
    private:
	const SConnectivity& conn_;
	std::vector<bool> faceTag;
	std::vector<bool> vertexTag;
    
};

#endif   /* ----- #ifndef TRAVERSAL_DECOMPOSER_HH  ----- */
