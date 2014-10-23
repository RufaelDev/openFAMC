/*
 * =====================================================================================
 * 
 *        Filename:  VertexContext.h
 * 
 *     Description: A class describing the "local" vertex context
 * 
 *         Version:  1.0
 *         Created:  02/05/07 14:33:08 CET
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


#ifndef  VERTEX_CONTEXT_HH
#define  VERTEX_CONTEXT_HH

#include <vector>

class VertexContext{
    //TYPE DEFINITIONS
    public:
	enum Id{
	    Invalid,
	    Nil2Vertex,
	    Vertex2Vertex,
	    Tri2Vertex,	    //parallelogram
	    MultiVertex2Vertex,
	    Ring2Vertex,
	};
	typedef std::vector<int>	  VertexGroup;

    //CONSTRUCTORS
    public:
	VertexContext() : id_(Invalid){};

	//VertexContext(Id _id, const VertexGroups& _from, int _to) : 
	//    id_(_id), from_(_from), to_(_to){}
	
	VertexContext(Id _id, const VertexGroup& _from, int _to) : 
	    id_(_id), from_(_from), to_(_to){}
	
	//copy
	VertexContext(const VertexContext& _vc) :
	    id_(_vc.id()), from_(_vc.from()), to_(_vc.to()){}
	

    //MEMBER FUNCTIONS
    public:
		Id		id()		const { return id_; }
	const	VertexGroup&	from()		const { return from_; }
	const	int		to()		const { return to_; }
	
    private:
	
    //MEMBER VARIABLES
    protected:
	Id	      id_;
	VertexGroup   from_;
	int	      to_;

};

#endif   /* ----- #ifndef VERTEX_CONTEXT_HH  ----- */
