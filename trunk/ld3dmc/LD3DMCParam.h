/*
 * =====================================================================================
 * 
 *        Filename:  LD3DMCParam.h
 * 
 *     Description:  Golbal parameters of Layered Dynamic 3D Mesh Coder
 * 
 *         Version:  1.0
 *         Created:  05/15/07 10:29:14 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

/*===============================================*\
 * Copyright (C) 2006-2007 by Nikolce Stefanoski * 
\*===============================================*/

#ifndef  LD3DMC_PARAM_HH
#define  LD3DMC_PARAM_HH

struct LD3DMCParam {

    //MEMBER VARIABLS
    int		  L;		//no. of decomposed layers
    int           l;            //no. of encoded layers
    int		  N_P;		//frame order parameter1    
    int		  N_B;		//frame order parameter2    
    double	  Qbin;		//quantization bin size     
    short	  O;		//global prediction mode    

};

#endif   /* ----- #ifndef LD3DMC_PARAM_HH  ----- */

