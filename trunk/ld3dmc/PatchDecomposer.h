/*
 * =====================================================================================
 * 
 *        Filename:  PatchDecomposer.h
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  27/05/07 23:14:06 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst), stefanos@tnt.uni-hannover.de
 *         Company:  Institut für Informationsverarbeitung
 * 
 * =====================================================================================
 */


#ifndef  PATCH_DECOMPOSER_HH
#define  PATCH_DECOMPOSER_HH

#include "Connectivity.h"
#include "PatchRetriangulator.h"

class PatchDecomposer{
    public:
	PatchDecomposer(const Connectivity& _conn) : conn_(_conn), pr_(&conn_) {}

	void decompose(VVH& _vvh);

    private:
	VH next_seed() const;
	bool is_valid_patch(VH _vh) const;
	bool is_free_patch(VH _vh) const;
	void conq_patch(VH _vh);

    private:
	Connectivity conn_;
	PatchRetriangulator pr_;
};

#endif   /* ----- #ifndef PATCH_DECOMPOSER_HH  ----- */
