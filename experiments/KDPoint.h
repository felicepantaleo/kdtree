/*
 * KDPoint.h
 *
 *  Created on: Feb 9, 2016
 *      Author: fpantale
 */

#ifndef FKDPOINT_H_
#define FKDPOINT_H_

/* A utility function to construct a Point from a range of iterators. */
template <size_t N, typename IteratorType>
Point<N> PointFromRange(IteratorType begin, IteratorType end) {
  Point<N> result;
  copy(begin, end, result.begin());
  return result;
}

/* Utility functions to create 1-, 2-, 3-, or 4-Points from values. */
Point<1> MakePoint(double x) {
  Point<1> result;
  result[0] = x;
  return result;
}
Point<2> MakePoint(double x, double y) {
  Point<2> result;
  result[0] = x;
  result[1] = y;
  return result;
}
Point<3> MakePoint(double x, double y, double z) {
  Point<3> result;
  result[0] = x;
  result[1] = y;
  result[2] = z;
  return result;
}
Point<4> MakePoint(double x, double y, double z, double w) {
  Point<4> result;
  result[0] = x;
  result[1] = y;
  result[2] = z;
  result[3] = w;
  return result;
}





#endif /* FKDPOINT_H_ */
