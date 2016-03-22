//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBidirectionalMap
//* .File Name       : $Workfile:   SVBidirectionalMap.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:44:02  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <utility>
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/multi_index_container.hpp>
//Moved to precompiled header: #include <boost/multi_index/member.hpp>
//Moved to precompiled header: #include <boost/multi_index/ordered_index.hpp>
#include "SVBimapDeclarations.h"

// The class template bidirectional_map wraps the specification
// of a bidirectional map based on multi_index_container.
template<typename FromType, typename ToType>
struct SVBidirectionalMap
{
	typedef struct SVPair{
		FromType first; 
		ToType second; 
		SVPair(const FromType& p_rFirst, const ToType& p_rSecond): first(p_rFirst), second(p_rSecond) {}
		SVPair(const std::pair<FromType, ToType> & p): first(p.first), second(p.second) {}
	} value_type;

	// A bidirectional map can be simulated as a multi_index_container
	// of pairs of (FromType, ToType) with two unique indices, one
	// for each member of the pair.
	typedef boost::multi_index_container<
		value_type,
		boost::multi_index::indexed_by<
			boost::multi_index::ordered_unique<
				boost::multi_index::tag<from>, 
				boost::multi_index::member<value_type, FromType, &value_type::first> 
			>,
			boost::multi_index::ordered_non_unique<
				boost::multi_index::tag<to>,  
				boost::multi_index::member<value_type, ToType, &value_type::second> 
			>
		>
	> type;
};

