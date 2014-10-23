/*
 * =====================================================================================
 * 
 *        Filename:  FAMCLayeredDecompositionDecoder.h
 * 
 *     Description:  A cabac based decoder for a layered decomposition
 * 
 *         Version:  1.0
 *         Created:  10/04/07 17:15:18 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

#ifndef FAMC_LAYERED_DECOMPOSITION_DECODER_H
#define FAMC_LAYERED_DECOMPOSITION_DECODER_H

class FAMCLayeredDecompositionDecoder {
	FAMCLayeredDecompositionDecoder(){}
	int decode (int _nValues, std::vector<SimplificationOperation>& _sop) const;
};

#endif //FAMC_LAYERED_DECOMPOSITION_DECODER_H