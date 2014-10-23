/*
 * =====================================================================================
 * 
 *        Filename:  LayeredDecomposerAutomatic.hh
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  15/07/07 12:39:10 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst), stefanos@tnt.uni-hannover.de
 *         Company:  Institut für Informationsverarbeitung
 * 
 * =====================================================================================
 */


#ifndef  LAYERED_DECOMPOSER_AUTOMATIC_HH
#define  LAYERED_DECOMPOSER_AUTOMATIC_HH

#include "LayeredDecomposition.h"
#include "SConnectivity.h"

class LayeredDecomposerAutomatic{
    //CONSTRUCTORS
    public:
    LayeredDecomposerAutomatic(int _nLayers, const SConnectivity& _conn) : nLayers_(_nLayers), conn_(_conn) {}

    //MEMBER FUNCTIONS
    public:
    bool produce(LayeredDecomposition& _ld);
    bool isConsistent(const LayeredDecomposition& _ld) const;

    //MEMBER VARIABLES
    const int nLayers_;
    const SConnectivity& conn_;
};

#endif   /* ----- #ifndef LAYERED_DECOMPOSER_AUTOMATIC_HH  ----- */
