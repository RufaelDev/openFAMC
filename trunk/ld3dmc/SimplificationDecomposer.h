/*
 * =====================================================================================
 * 
 *        Filename:  SimplificationDecomposer.hh
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  07/13/07 12:59:40 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst), stefanos@tnt.uni-hannover.de
 *         Company:  Institut für Informationsverarbeitung
 * 
 * =====================================================================================
 */


#ifndef  SIMPLIFICATION_DECOMPOSER_HH
#define  SIMPLIFICATION_DECOMPOSER_HH

#include "SConnectivity.h"
#include "LayeredDecomposition.h"
#include <vector>

struct SimplificationOperation{
    int vertexIndex;
    int mode;
};

class SimplificationDecomposer{
    //CONSTRUCTORS
    public:
	SimplificationDecomposer(SConnectivity& _conn) : 
	    conn_(_conn){}

    //MEMBER FUNCTIONS
    public:
	bool createLayerContext(const std::vector<SimplificationOperation>& _op, LayerContext& _lc);
	bool isConsistent(const LayerContext& _lc) const;

    //MEMBER VARIABLES
    private:
	SConnectivity& conn_;

};

#endif   /* ----- #ifndef SIMPLIFICATION_DECOMPOSER_HH  ----- */
