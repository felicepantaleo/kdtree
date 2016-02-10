/*
 * FKDTree.h
 *
 *  Created on: Feb 9, 2016
 *      Author: fpantale
 */

#ifndef FKDTREE_H_
#define FKDTREE_H_


#include <vector>
#include <tuple>
#include <algorithm>
#include <cmath>
#include <utility>

template<typename... AcceptedTypes>
class FKDTree {

public:

	FKDTree(const long int nPoints)
	{
		theDepth = std::floor(log2(nPoints));
		theMaxNumberOfNodes = (1 << (theDepth+1)) -1;
		for_each(theDimensions, [this](auto& x){x.reserve(theMaxNumberOfNodes);});
	}

	FKDTree()
	{
		theDepth = 0;
		theMaxNumberOfNodes = 1;
		for_each(theDimensions, [this](auto& x){x.reserve(theMaxNumberOfNodes);});

	}


	void push_back(AcceptedTypes...)
	{
		for_each(theDimensions, [this](auto& x){x.reserve(theMaxNumberOfNodes);});
	}


	template<typename TupleType, typename FunctionType>
	void for_each(TupleType&&, FunctionType
	            , std::integral_constant<size_t, std::tuple_size<typename std::remove_reference<TupleType>::type >::value>) {}

	template<std::size_t I, typename TupleType, typename FunctionType
	       , typename = typename std::enable_if<I!=std::tuple_size<typename std::remove_reference<TupleType>::type>::value>::type >
	void for_each(TupleType&& t, FunctionType f, std::integral_constant<size_t, I>)
	{
	    f(std::get<I>(t));
	    for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, I + 1>());
	}

	template<typename TupleType, typename FunctionType>
	void for_each(TupleType&& t, FunctionType f)
	{
	    for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, 0>());
	}

private:

	int theDepth;
	int theMaxNumberOfNodes;
	std::tuple<std::vector<AcceptedTypes>...> theDimensions;
	const unsigned short int theNumberOfDimensions= sizeof...(AcceptedTypes);


};

#endif /* FKDTREE_H_ */
