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
#ifndef SVBIDIRECTIONALMAP_H
#define SVBIDIRECTIONALMAP_H

#include <boost/config.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVContainerLibrary\SVBidirectionalMap.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:44:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Jan 2012 14:41:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated containers to update data structure to be more discriptive and not dependent on std::pair.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:13:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Moved from SVSoftwareTriggerDevice project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:25:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
