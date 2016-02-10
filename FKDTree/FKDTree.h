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
#include "KDPoint.h"
template< class TYPE, int numberOfDimensions>
class FKDTree {

public:

	FKDTree(const long int nPoints)
	{
		theDepth = std::floor(log2(nPoints));
		theMaxNumberOfNodes = (1 << (theDepth+1)) -1;
		for(auto& x : theDimensions)
			x.reserve(theMaxNumberOfNodes);
		theIds.reserve(theMaxNumberOfNodes);
		isValid.resize(theMaxNumberOfNodes, false);
		thePoints.reserve(nPoints);
	}

	void push_back(const KDPoint<TYPE,numberOfDimensions>& point, const long int id)
	{
		for(int i = 0; i< numberOfDimensions; ++i)
			theDimensions.at(i).push_back(point[i]);
		theIds.push_back(id);

	}


	void build()
	{

	}


private:
	int theDepth;
	int theMaxNumberOfNodes;

	std::vector<KDPoint<TYPE,numberOfDimensions> > thePoints;
	std::array<std::vector<TYPE>, numberOfDimensions> theDimensions;
	std::vector<long int> theIds;
	std::vector<bool> isValid;

};




#endif /* FKDTREE_FKDTREE_H_ */
