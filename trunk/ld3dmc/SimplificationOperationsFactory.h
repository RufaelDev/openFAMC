/*
 * =====================================================================================
 * 
 *        Filename:  SimplificationOperationsFactory.hh
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  07/13/07 16:01:43 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst), stefanos@tnt.uni-hannover.de
 *         Company:  Institut für Informationsverarbeitung
 * 
 * =====================================================================================
 */


#ifndef  SIMPLIFICATION_OPERATIONS_FACTORY_HH
#define  SIMPLIFICATION_OPERATIONS_FACTORY_HH

#include "LayeredDecomposition.h"
#include "SConnectivity.h"
#include "SimplificationDecomposer.h"

class SimplificationOperationsFactory {
    //CONSTRUCTORS
    public:
	SimplificationOperationsFactory(const SConnectivity& _conn); 

    //MEMBER FUNCTIONS
    public:
	void produce(std::vector<SimplificationOperation>& _op);
	void produceOrderedByVertexIndex(std::vector<SimplificationOperation>& _op);

    //private:
	void initTags();
	void traverse(std::vector<int>& _patches);
	int getNextSeed() const;
	bool isFreePatch(int _v) const;
	void conquerPatch(int _v);
	int determineSimplificationMode(int _v) const;
	int edgeCollapseCost(int _vertexIndex, int _simplificationMode) const;
	int regularSimpificationCost(int _vertexIndex, int _simplificationMode) const;
	int sumAbs(std::vector<int> _vec) const;
	bool adjustToIdx(std::vector<int>& _vec, int _idx) const ;
	
    //MEMBER VARIABLES
    private:
	SConnectivity conn_;
	std::vector<bool> faceTag;
	std::vector<bool> vertexTag1;
	std::vector<bool> vertexTag2;

};

#endif   /* ----- #ifndef SIMPLIFICATION_OPERATIONS_FACTORY_HH  ----- */
