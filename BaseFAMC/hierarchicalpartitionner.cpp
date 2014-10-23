/*!
 ************************************************************************
 *  \file
 *     HierarchicalPartitionner.cpp
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
#include "hierarchicalPartitionner.h"
#include "AffineTransform.h"
#include "AffineMotionPredictor.h"
#include "Animation.h"
#include "MotionModel.h"
#include <math.h>
#include "Console.h"

extern CConsole logger;

HierarchicalPartitionner::HierarchicalPartitionner(void)
{
	K_ = 0;
}

HierarchicalPartitionner::~HierarchicalPartitionner(void)
{
}

bool HierarchicalPartitionner::SavePartition(char * file) {
	FILE * fPartition = fopen(file, "w");
	for (int vertex = 0; vertex < (int) partition_.size(); vertex++){	
		fprintf(fPartition, "%i\n", partition_[vertex]);
	}
	fclose(fPartition);
	return true;
}
void HierarchicalPartitionner::ComputeClustersNumber() {
	K_ = 0;
	for (int i = 0; i < (int) partition_.size(); i++) {
		K_ = MAX(K_, partition_[i]+1);
	}
}
bool HierarchicalPartitionner::LoadPartition(char * file) {
	FILE * fPartition = fopen(file, "r");
	partition_.clear();
	int cluster;
	while(!feof(fPartition)){
		fscanf(fPartition, "%i", &cluster);
		partition_.push_back(cluster);
	}
	fclose(fPartition);
	partition_.pop_back();
	ComputeClustersNumber();
	return true;
}
void HierarchicalPartitionner::UpdatePartition() {
	std::map<int, int> mapp;

	int cluster = 0;
	for (int i = 0; i < (int) partition_.size(); i++) {
		if (mapp.find(partition_[i]) == mapp.end()) {
			mapp[partition_[i]] = cluster;
			cluster++;
		}
		partition_[i] = mapp[partition_[i]];
	}
}
void HierarchicalPartitionner::CopyPartition(std::vector<int> &partition1, std::vector<int> &partition2) {
	partition1.clear();
	for (int u = 0; u < (int) partition2.size(); u++) {
		partition1.push_back(partition2[u]);
	}
}
void HierarchicalPartitionner::PartitionTrivial(Animation * anim){
	partition_.clear();
	for (int v = 0; v < anim->GetNCoord(); v++){
		partition_.push_back(0);
	}
	ComputeClustersNumber();
}
