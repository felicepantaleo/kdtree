/*
 * FKDTree.h
 *
 *  Created on: Feb 10, 2016
 *      Author: fpantale
 */

#ifndef FKDTREE_FKDTREE_H_
#define FKDTREE_FKDTREE_H_


#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <utility>

template< class TYPE, int numberOfDimensions>
class FKDTree {

public:

	FKDTree(const long int nPoints)
	{
		theDepth = std::floor(log2(nPoints));
		theMaxNumberOfNodes = (1 << (theDepth+1)) -1;
		for(auto& x : theDimensions)
			x.reserve(theMaxNumberOfNodes);
	}

	FKDTree()
	{
		theDepth = 0;
		theMaxNumberOfNodes = 1;
		for(auto& x : theDimensions)
			x.reserve(theMaxNumberOfNodes);
	}


	void push_back()
	{

	}


private:

	int theDepth;
	int theMaxNumberOfNodes;
	std::array<std::vector<TYPE>, numberOfDimensions> theDimensions;
};




#endif /* FKDTREE_FKDTREE_H_ */
