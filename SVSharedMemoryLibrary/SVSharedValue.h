//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedValue
//* .File Name       : $Workfile:   SVSharedValue.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:10:08  $
//******************************************************************************
#pragma once
#include "SVShared.h"

namespace SeidenaderVision
{
	struct SVSharedValue
	{
		enum ResultTypeEnum
		{
			UnknownType = 0,
			StringType = 1,
			IntType = 2,
			FloatType = 3,
			BooleanType = 4
		};
		ResultTypeEnum m_ResultType;	// this is the native type
		char_string m_Result;

		int m_Status;

		void_allocator m_Allocator;

		SVSharedValue( int p_Status, const void_allocator& p_rAlloc );
		SVSharedValue( ResultTypeEnum p_ResultType, const char* p_szResult, int p_Status, const void_allocator& p_rAlloc );
		SVSharedValue( const void_allocator& p_rAlloc );
		SVSharedValue( const SVSharedValue& p_rData );

		const SVSharedValue& operator=( const SVSharedValue& p_rData );
	};

	struct SVValue
	{
		long trigger;
		long status;
		std::string name;
		std::string value;

		SVValue(const std::string & n, long trg, long sts, const std::string & val): 
			name(n), trigger(trg), status(sts), value(val)
			{
			}
		SVValue() {}

		bool empty() const { return name.empty(); }
	};

}

typedef SeidenaderVision::SVSharedValue SVSharedValue;
typedef boost::interprocess::allocator< SVSharedValue, segment_manager_t > SVSharedValueAllocator;
typedef std::pair< const char_string, SVSharedValue > SVSharedValuePair;
typedef boost::interprocess::allocator< SVSharedValuePair, segment_manager_t > SVSharedValuePairAllocator;
typedef boost::interprocess::map< char_string, SVSharedValue, std::less< char_string >, SVSharedValuePairAllocator > SVSharedValueMap;
typedef boost::interprocess::allocator< SVSharedValueMap, segment_manager_t > SVSharedValueMapAllocator;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedValue.h_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:10:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:40:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
