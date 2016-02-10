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
template<class TYPE, int numberOfDimensions>
class KDPoint {

	KDPoint(): theElements() {}

	KDPoint( TYPE x, TYPE y)
    {
        std::static_assert( nDimensions == 2, "Point dimensionality differs from the number of passed arguments." );
        theElements[0] = x;
        theElements[1] = y;

    }


	KDPoint( TYPE x, TYPE y, TYPE z )
    {
        std::static_assert( nDimensions == 3, "Point dimensionality differs from the number of passed arguments." );
        theElements[0] = x;
        theElements[1] = y;
        theElements[2] = z;
    }

	KDPoint( TYPE x, TYPE y, TYPE z, TYPE w)
    {
        std::static_assert( nDimensions == 4, "Point dimensionality differs from the number of passed arguments." );
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


private:
    std::array< TYPE, numberOfDimensions > theElements;

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
