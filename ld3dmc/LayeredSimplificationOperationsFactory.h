/*
 * =====================================================================================
 * 
 *        Filename:  LayeredSimplificationOperationsFactory.hh
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  15/07/07 13:08:45 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst), stefanos@tnt.uni-hannover.de
 *         Company:  Institut für Informationsverarbeitung
 * 
 * =====================================================================================
 */

#include <vector>
#include "LayeredDecomposition.h"
#include "SimplificationOperationsFactory.h"
#include "SConnectivity.h"
#include "Animation.h"

class LayeredSimplificationOperationsFactory{
public:
	static void getSimplificationOperationsAutomatic(std::vector< std::vector<SimplificationOperation> >& lsop, const SConnectivity& _conn){
		SimplificationOperationsFactory sof(_conn);
		for (int l=0; l<lsop.size(); ++l){
			printf("lsop produced automatically for layer %d\n", l);
			sof.produce(lsop[l]);
		}
	}

	static void getSimplificationOperationsAutomaticOrderedByVertexIndex(std::vector< std::vector<SimplificationOperation> >& lsop, const SConnectivity& _conn){
		SimplificationOperationsFactory sof(_conn);
		for (int l=0; l<lsop.size(); ++l){
			printf("lsop produced automatically ordered by vertex index for layer %d\n", l);
			sof.produceOrderedByVertexIndex(lsop[l]);
		}
	}

	static void getSimplificationOperationsAutomaticOrderedByVertexIndex(std::vector< std::vector<SimplificationOperation> >& lsop, IndexedFaceSet& _ifs){
		int nFaces = _ifs.GetNCoordIndex();
		std::vector< std::vector<int> > sifs(nFaces, std::vector<int>(3,0));
		int coordIndex[3] = {-1, -1, -1};
		for (int t=0; t<nFaces; ++t){
			_ifs.GetCoordIndex(t, coordIndex);
			sifs[t][0]=coordIndex[0];
			sifs[t][1]=coordIndex[1];
			sifs[t][2]=coordIndex[2];
		}
		SConnectivity conn(sifs);
		SimplificationOperationsFactory sof(conn);
		for (int l=0; l<lsop.size(); ++l){
			printf("lsop produced automatically ordered by vertex index for layer %d\n", l);
			sof.produceOrderedByVertexIndex(lsop[l]);
		}
	}
};
