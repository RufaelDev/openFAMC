/*
 * =====================================================================================
 * 
 *        Filename:  Vertex.h
 * 
 *     Description:  A Vertex Item
 * 
 *         Version:  1.0
 *         Created:  05/24/07 08:27:20 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst), stefanos@tnt.uni-hannover.de
 *         Company:  Institut für Informationsverarbeitung
 * 
 * =====================================================================================
 */

/*===============================================*\
 * Copyright (C) 2006-2007 by Nikolce Stefanoski * 
\*===============================================*/

#ifndef  VERTEX_HH
#define  VERTEX_HH

#include "HandleTypes.h"
#include "BaseItem.h"

struct Vertex : public BaseItem {

    Vertex(){}

    VFH& vfhs() { return vfh_; }
    const VFH& vfhs() const { return vfh_; }

    private:
    VFH vfh_;
	

};

#endif   /* ----- #ifndef VERTEX_HH  ----- */
