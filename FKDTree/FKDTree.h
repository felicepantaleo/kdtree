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
#include <deque>
#include <cassert>

#include "KDPoint.h"
template<class TYPE, int numberOfDimensions>
class FKDTree
{

public:

	FKDTree(const long int nPoints)
	{
		theNumberOfPoints = nPoints;
		theDepth = std::floor(log2(nPoints));
		theMaxNumberOfNodes = (1 << (theDepth + 1)) - 1;
		for (auto& x : theDimensions)
			x.resize(theNumberOfPoints);
		theIntervalLength.resize(theNumberOfPoints, 0);
		theIntervalMin.resize(theNumberOfPoints, 0);
		theIds.reserve(theNumberOfPoints);
		thePoints.reserve(theNumberOfPoints);

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

	void add_at_position(KDPoint<TYPE, numberOfDimensions> && point,
			const unsigned int position)
	{
		for (int i = 0; i < numberOfDimensions; ++i)
			theDimensions.at(i).at(position) = point[i];
		theIds.at(position) = point.getId();

	}

	KDPoint<TYPE, numberOfDimensions> getPoint(unsigned int index) const
	{
		KDPoint<TYPE, numberOfDimensions> point;
		for (int i = 0; i < numberOfDimensions; ++i)
			point.setDimension(i, theDimensions.at(i).at(index));
		point.setId(theIds.at(index));
		return point;
	}

	void build()
	{
		//gather kdtree building
		int dimension;
		theIntervalMin[0] = 0;
		theIntervalLength[0] = theNumberOfPoints;

		for (int depth = 0; depth < theDepth; ++depth)
		{

			dimension = depth % numberOfDimensions;
			unsigned int firstIndexInDepth = (1 << depth) - 1;
			for (int indexInDepth = 0; indexInDepth < (1 << depth);
					++indexInDepth)
			{
				unsigned int indexInArray = firstIndexInDepth + indexInDepth;
				unsigned int leftSonIndexInArray = 2 * indexInArray + 1;
				unsigned int rightSonIndexInArray = leftSonIndexInArray + 1;

				unsigned int whichElementInInterval = partition_complete_kdtree(
						theIntervalLength.at(indexInArray));
				std::nth_element(
						thePoints.begin() + theIntervalMin.at(indexInArray),
						thePoints.begin() + theIntervalMin.at(indexInArray)
								+ whichElementInInterval,
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
						thePoints.at(
								theIntervalMin.at(indexInArray)
										+ whichElementInInterval),
						indexInArray);

				if (leftSonIndexInArray < theNumberOfPoints)
				{
					theIntervalMin.at(leftSonIndexInArray) = theIntervalMin.at(
							indexInArray);
					theIntervalLength.at(leftSonIndexInArray) =
							whichElementInInterval;
				}

				if (rightSonIndexInArray < theNumberOfPoints)
				{
					theIntervalMin.at(rightSonIndexInArray) = theIntervalMin.at(
							indexInArray) + whichElementInInterval + 1;
					theIntervalLength.at(rightSonIndexInArray) =
							(theIntervalLength.at(indexInArray) - 1
									- whichElementInInterval);
				}
			}
		}

		dimension = theDepth % numberOfDimensions;
		unsigned int firstIndexInDepth = (1 << theDepth) - 1;
		unsigned int indexInArray = firstIndexInDepth;
		for (unsigned int indexInArray = firstIndexInDepth;
				indexInArray < theNumberOfPoints; ++indexInArray)
		{
			add_at_position(thePoints.at(theIntervalMin.at(indexInArray)),
					indexInArray);

		}

	}

	inline
	unsigned int leftSonIndex(unsigned int index)
	{
		return 2 * index + 1;
	}

	inline
	unsigned int rightSonIndex(unsigned int index)
	{
		return 2 * index + 2;
	}

	inline
	bool intersects(unsigned int index,
			const KDPoint<TYPE, numberOfDimensions>& minPoint,
			const KDPoint<TYPE, numberOfDimensions>& maxPoint, int dimension)
	{
		return (theDimensions.at(dimension).at(index) <= maxPoint[dimension]
				&& theDimensions.at(dimension).at(index) >= minPoint[dimension]);
	}

	inline
	bool isInTheBox(unsigned int index,
			const KDPoint<TYPE, numberOfDimensions>& minPoint,
			const KDPoint<TYPE, numberOfDimensions>& maxPoint)
	{
		bool inTheBox = true;
		for (int i = 0; i < numberOfDimensions; ++i)
		{
			inTheBox &= (theDimensions.at(i).at(index) <= maxPoint[i]
					&& theDimensions.at(i).at(index) >= minPoint[i]);
		}
		return inTheBox;
	}

	std::vector<KDPoint<TYPE, numberOfDimensions> > search_in_the_box(
			const KDPoint<TYPE, numberOfDimensions>& minPoint,
			const KDPoint<TYPE, numberOfDimensions>& maxPoint)
	{
		std::deque<unsigned int> indecesToVisit;
		std::vector<KDPoint<TYPE, numberOfDimensions> > result;

		indecesToVisit.push_back(0);
		for (int depth = 0; depth < theDepth + 1; ++depth)
		{
			int dimension = depth % numberOfDimensions;
			unsigned int numberOfIndecesToVisitThisDepth =
					indecesToVisit.size();
			for (unsigned int visitedIndecesThisDepth = 0;
					visitedIndecesThisDepth < numberOfIndecesToVisitThisDepth;
					visitedIndecesThisDepth++)
			{
				unsigned int index = indecesToVisit.at(visitedIndecesThisDepth);
				assert(index >= 0 && index < theNumberOfPoints);
				bool intersection = intersects(index, minPoint, maxPoint,
						dimension);
				if (intersection && isInTheBox(index, minPoint, maxPoint))
					result.push_back(getPoint(index));

				bool isLowerThanBoxMin = theDimensions.at(dimension).at(index)
						< minPoint[dimension];

				int startSon = isLowerThanBoxMin; //left son = 0, right son =1

				int endSon = isLowerThanBoxMin || intersection;

				for (int whichSon = startSon; whichSon < endSon + 1; ++whichSon)
				{
					unsigned int indexToAdd = leftSonIndex(index) + whichSon;
					if (indexToAdd < theNumberOfPoints)
					{

//						assert(
//								indexToAdd >= (1 << (depth + 1)) - 1
//										&& leftSonIndex(index) + whichSon
//												< ((1 << (depth + 2)) - 1));
						indecesToVisit.push_back(indexToAdd);
					}

				}

			}

			indecesToVisit.erase(indecesToVisit.begin(),
					indecesToVisit.begin() + numberOfIndecesToVisitThisDepth);

		}
		return result;
	}

	bool test_correct_build(unsigned int index, int dimension)
	{

		unsigned int leftSonIndexInArray = 2 * index + 1;
		unsigned int rightSonIndexInArray = leftSonIndexInArray + 1;
		if (rightSonIndexInArray >= theNumberOfPoints
				&& leftSonIndexInArray >= theNumberOfPoints)
		{
			return true;
		}
		else
		{
			if (leftSonIndexInArray < theNumberOfPoints)
			{
				if (theDimensions.at(dimension).at(index)
						>= theDimensions.at(dimension).at(leftSonIndexInArray))
				{
					test_correct_build(leftSonIndexInArray,
							(dimension + 1) % numberOfDimensions);

				}
				else
					return false;
			}

			if (rightSonIndexInArray < theNumberOfPoints)
			{
				if (theDimensions.at(dimension).at(index)
						<= theDimensions.at(dimension).at(rightSonIndexInArray))
				{
					test_correct_build(rightSonIndexInArray,
							(dimension + 1) % numberOfDimensions);

				}
				else
					return false;

			}

		}

	}
private:

	unsigned int partition_complete_kdtree(unsigned int length)
	{
		if (length == 1)
			return 0;
		int index = 1 << ((int) log2(length));

		if ((index / 2) - 1 <= length - index)
			return index - 1;
		else
			return length - index / 2;

	}

	long int theNumberOfPoints;
	int theDepth;
	long int theMaxNumberOfNodes;
	std::vector<KDPoint<TYPE, numberOfDimensions> > thePoints;
	std::array<std::vector<TYPE>, numberOfDimensions> theDimensions;
	std::vector<unsigned int> theIntervalLength;
	std::vector<unsigned int> theIntervalMin;
	std::vector<unsigned int> theIds;

};

#endif /* FKDTREE_FKDTREE_H_ */
