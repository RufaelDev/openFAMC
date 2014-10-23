/*
 * =====================================================================================
 * 
 *        Filename:  FAMCVertexInfoDecoder.h
 * 
 *     Description:  A cabac based decoder for a partition
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

#ifndef FAMC_VERTEX_INFO_DECODER_H
#define FAMC_VERTEX_INFO_DECODER_H

#include <vector>

class FAMCVertexInfoDecoder {
	unsigned char* buffer_;

public:
	FAMCVertexInfoDecoder(unsigned char* _buffer);
	int decode (int _nC, int _nV, std::vector<int>& _partition);
};

#endif //FAMC_VERTEX_INFO_DECODER_H