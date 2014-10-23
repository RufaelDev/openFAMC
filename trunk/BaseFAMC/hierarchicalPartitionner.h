/*!
 ************************************************************************
 *  \file
 *     HierarchicalPartitionner.h
 *  \brief
 *     HierarchicalPartitionner class.
 *  \author
 *     Copyright (C) 2007  ARTEMIS Department INT/GET, Paris, France.
 *
 *     Khaled MAMOU               <khaled.mamou@int-evry.fr>
 *
 *	   Institut National des Telecommunications		tel.: +33 (0)1 60 76 40 94
 *     9, Rue Charles Fourier,						fax.: +33 (0)1 60 76 43 81
 *     91011 Evry Cedex France
 *
 ************************************************************************
 */
#pragma once
#include "Animation.h"
#include <vector>

class HierarchicalPartitionner
{

	int K_;					// nbr of clusters
	std::vector<int> partition_;
public:
	inline std::vector<int> * GetPartition() { return &partition_;};
	inline int GetNClusters() { return K_;};
	void CopyPartition(std::vector<int> &partition1, std::vector<int> &partition2);

	void PartitionTrivial(Animation * anim);

	bool SavePartition(char * file);
	bool LoadPartition(char * file);
	HierarchicalPartitionner(void);
	void ComputeClustersNumber();
	void UpdatePartition();
public:
	~HierarchicalPartitionner(void);
};
