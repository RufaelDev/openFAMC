/*
 * =====================================================================================
 * 
 *        Filename:  LayeredDecomposerUserBased.hh
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

#ifndef  LAYERED_DECOMPOSER_USER_BASED_HH
#define  LAYERED_DECOMPOSER_USER_BASED_HH

#include "LayeredDecomposition.h"
#include "SimplificationOperationsFactory.h"
#include "SConnectivity.h"

class LayeredDecomposerUserBased {
    //CONSTRUCTOR
    public:
	LayeredDecomposerUserBased(const std::vector< std::vector<SimplificationOperation> >& _simplificationOperation, const SConnectivity& _conn) : 
	    simplificationOperation_(_simplificationOperation),
	conn_(_conn){};

    //MEMBER FUNCTIONS
    public:
	bool produce(LayeredDecomposition& _ld);
	bool isConsistent(const LayeredDecomposition& _ld) const;

    //MEMBER VARIABLES
    private:
	const std::vector< std::vector<SimplificationOperation> >& simplificationOperation_;
	SConnectivity conn_;
};

#endif   /* ----- #ifndef LAYERED_DECOMPOSER_USER_BASED_HH  ----- */

