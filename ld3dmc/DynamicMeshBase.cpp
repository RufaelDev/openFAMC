/*===============================================*\
 * Copyright (C) 2006-2007 by Nikolce Stefanoski * 
\*===============================================*/

#include "DynamicMeshBase.h"

      int   DynamicMeshBase::n_frames()	      const { return (int) dyn_geo_.size(); }
      int   DynamicMeshBase::n_vertices()     const { return n_vertices_; }
const IFS&  DynamicMeshBase::get_ifs()	      const { return ifs_; }
      void  DynamicMeshBase::check_v(int _v)  const { assert(_v>=0); assert(_v<n_vertices()); }
      void  DynamicMeshBase::check_f(int _f)  const { assert(_f>=0); assert(_f<n_frames()); }

