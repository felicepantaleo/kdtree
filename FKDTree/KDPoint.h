/*
 * FKDPoint.h
 *
 *  Created on: Feb 10, 2016
 *      Author: fpantale
 */

#ifndef FKDTREE_KDPOINT_H_
#define FKDTREE_KDPOINT_H_
#include <type_traits>
#include <array>
#include <utility>


template<class TYPE, int numberOfDimensions>
class KDPoint {
public:
	KDPoint(): theElements(), theId(0) {}

	KDPoint(const KDPoint< TYPE, numberOfDimensions >& otherPoint)
	{
		theId = otherPoint.getId();
		for(int i=0; i<numberOfDimensions; ++i)
			theElements[i] = otherPoint[i];
	}


//	KDPoint(KDPoint< TYPE, numberOfDimensions >&& otherPoint)
//	{
//		*this = std::move(otherPoint);
//	}


	KDPoint( TYPE x, TYPE y)
    {
        static_assert( numberOfDimensions == 2, "Point dimensionality differs from the number of passed arguments." );
        theId= 0;
        theElements[0] = x;
        theElements[1] = y;
    }

	KDPoint( TYPE x, TYPE y, TYPE z )
    {
        static_assert( numberOfDimensions == 3, "Point dimensionality differs from the number of passed arguments." );
        theId= 0;
        theElements[0] = x;
        theElements[1] = y;
        theElements[2] = z;
    }

	KDPoint( TYPE x, TYPE y, TYPE z, TYPE w)
    {
        static_assert( numberOfDimensions == 4, "Point dimensionality differs from the number of passed arguments." );
        theId= 0;
        theElements[0] = x;
        theElements[1] = y;
        theElements[2] = z;
        theElements[3] = w;
    }

// the user should check that i < numberOfDimensions
    TYPE& operator[]( int const i )
    {
        return theElements[i];
    }

    TYPE const& operator[]( int const i ) const
    {
        return theElements[i];
    }

    void setDimension(int i, const TYPE& value)
    {
    	theElements[i] = value;
    }

    void setId(const long int id)
    {
    	theId = id;
    }


    long int getId () const
    {
    	return theId;


    }

    void print()
    {
    	std::cout << "point id: " << theId << std::endl;
    	for(auto i : theElements)
    	{
    		std::cout << i << std::endl;
    	}
    }



private:
    std::array< TYPE, numberOfDimensions > theElements;
    long int theId;
};



/* A utility function to construct a Point from a range of iterators. */
template < typename TYPE, size_t N, typename IteratorType>
KDPoint<TYPE, N > KDPointFromRange(IteratorType begin, IteratorType end) {
	KDPoint<TYPE, N > result;
  copy(begin, end, result.begin());
  return result;
}

/* Utility functions to create 1-, 2-, 3-, or 4-Points from values. */
template <typename TYPE>
KDPoint<TYPE, 1> MakeKDPoint(TYPE x) {
  KDPoint<TYPE, 1> result;
  result[0] = x;
  return result;
}

template <typename TYPE>
KDPoint<TYPE, 2> MakeKDPoint(TYPE x, TYPE y) {
  KDPoint<TYPE, 2> result;
  result[0] = x;
  result[1] = y;
  return result;
}


template <typename TYPE>
KDPoint<TYPE, 3> MakeKDPoint(TYPE x, TYPE y, TYPE z) {
  KDPoint<TYPE, 3> result;
  result[0] = x;
  result[1] = y;
  result[2] = z;
  return result;
}

template <typename TYPE>
KDPoint<TYPE, 4> MakeKDPoint(TYPE x, TYPE y, TYPE z, TYPE w) {
  KDPoint<TYPE, 4> result;
  result[0] = x;
  result[1] = y;
  result[2] = z;
  result[3] = w;
  return result;
}


#endif /* FKDTREE_KDPOINT_H_ */
