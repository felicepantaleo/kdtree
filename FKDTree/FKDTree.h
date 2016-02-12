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
#include <iostream>
#include "KDPoint.h"

template<class TYPE, int numberOfDimensions>
class FKDTree
{

public:

	FKDTree(const long int nPoints)
	{
		theDepth = std::floor(log2(nPoints));
		theMaxNumberOfNodes = (1 << (theDepth + 1)) - 1;
		for (auto& x : theDimensions)
			x.resize(theMaxNumberOfNodes);
		theIntervalLength.resize(theMaxNumberOfNodes, 0);
		theIntervalMin.resize(theMaxNumberOfNodes, 0);
		theIds.reserve(nPoints);
		thePoints.reserve(nPoints);
		std::cout << "kdtree allocated: depth " << theDepth
				<< " maxnumberofnodes: " << theMaxNumberOfNodes << std::endl;
	}

	void push_back(const KDPoint<TYPE, numberOfDimensions>& point)
	{

		thePoints.push_back(point);
		for (int i = 0; i < numberOfDimensions; ++i)
			theDimensions.at(i).push_back(point[i]);
		theIds.push_back(point.getId());

	}

	void add_at_position(const KDPoint<TYPE, numberOfDimensions>& point,
			const unsigned int position)
	{
		for (int i = 0; i < numberOfDimensions; ++i)
			theDimensions.at(i).at(position) = point[i];
		theIds.at(position) = point.getId();

	}

	void build()
	{
		//gather kdtree building
		int dimension;
		theIntervalMin[0] = 0;
		theIntervalLength[0] = thePoints.size();

		for (int depth = 0; depth < theDepth; ++depth)
		{

			dimension = depth % numberOfDimensions;
			std::cout << "\n\n\n checking dimension " << dimension << std::endl;
			unsigned int firstIndexInDepth = (1 << depth) - 1;
			for (int indexInDepth = 0; indexInDepth < (1 << depth);
					++indexInDepth)
			{
				unsigned int indexInArray = firstIndexInDepth + indexInDepth;
				unsigned int leftSonIndexInArray = 2* indexInArray +1;
				unsigned int rightSonIndexInArray = leftSonIndexInArray + 1;
				std::cout << "indexInDepth " <<indexInDepth << " indexInArray " << indexInArray << " leftSonIndexInArray " << leftSonIndexInArray << " rightSonIndexInArray " << rightSonIndexInArray<<std::endl;
//
//				std::cout << "searching for nth element, in position " <<  theIntervalMin.at(indexInArray)+ theIntervalLength.at(indexInArray) / 2 << std::endl;

				for(int p = theIntervalMin.at(indexInArray); p <  theIntervalMin.at(indexInArray)+ theIntervalLength.at(indexInArray); ++p)
				{
//					std::cout << "printing element" << p << std::endl;
//					thePoints.at(p).print();
				}

				unsigned int whichElementInInterval =  partition_complete_kdtree(theIntervalLength.at(indexInArray));
				std::cout << "whichElementInInterval " << whichElementInInterval << std::endl;
				std::nth_element(
						thePoints.begin() + theIntervalMin.at(indexInArray),
						thePoints.begin() + theIntervalMin.at(indexInArray)+ whichElementInInterval,
						thePoints.begin() + theIntervalMin.at(indexInArray)
								+ theIntervalLength.at(indexInArray),
						[dimension](const KDPoint<TYPE,numberOfDimensions> & a, const KDPoint<TYPE,numberOfDimensions> & b) -> bool
						{
							if(a[dimension] == b[dimension])
								return a.getId() < b.getId();
							else
							return a[dimension] < b[dimension];
						});
				add_at_position(
						thePoints.at(theIntervalMin.at(indexInArray)+ whichElementInInterval), indexInArray);

//				std::cout << "nth_element: "  << std::endl;
//				thePoints.at(
//						theIntervalMin.at(indexInArray)
//								+ theIntervalLength.at(indexInArray)
//										/ 2).print();

				theIntervalMin.at(leftSonIndexInArray) = theIntervalMin.at(
						indexInArray);
				theIntervalLength.at(leftSonIndexInArray) = whichElementInInterval;
				theIntervalMin.at(rightSonIndexInArray) =
						theIntervalMin.at(indexInArray)+ whichElementInInterval +1;
				theIntervalLength.at(rightSonIndexInArray) =
						(theIntervalLength.at(indexInArray) - 1 - whichElementInInterval);
			}
		}


		dimension = theDepth % numberOfDimensions;
		unsigned int firstIndexInDepth = (1 << theDepth) - 1;
		unsigned int indexInArray = firstIndexInDepth;
		for (unsigned int indexInArray = firstIndexInDepth;
				indexInArray < thePoints.size(); ++indexInArray)
		{
			add_at_position(thePoints.at(theIntervalMin.at(indexInArray)),
					indexInArray);

		}
		std::cout << " kd tree building complete: " << std::endl;
		for (int i = 0; i < thePoints.size(); ++i)
		{
			std::cout << "position: " << i << " id " << theIds.at(i)
					<< std::endl;
			std::cout << theDimensions[0].at(i) << std::endl;
			std::cout << theDimensions[1].at(i) << std::endl;
		}

	}


private:


	unsigned int partition_complete_kdtree(unsigned int length)
	{
		std::cout << "partitioning a number of nodes: " << length << std::endl;

		if(length == 1)
			return 0;
		int index = 1<<( (int)log2(length));

		std::cout << "the maximum power of two less than length is " << index << std::endl;

		if ((index/2) -1 <= length - index)
			return index -1;
		else
			return length- index/2;

	}



	int theDepth;
	int theMaxNumberOfNodes;

	std::vector<KDPoint<TYPE, numberOfDimensions> > thePoints;
	std::array<std::vector<TYPE>, numberOfDimensions> theDimensions;
	std::vector<unsigned int> theIntervalLength;
	std::vector<unsigned int> theIntervalMin;
	std::vector<unsigned int> theIds;

};

#endif /* FKDTREE_FKDTREE_H_ */
