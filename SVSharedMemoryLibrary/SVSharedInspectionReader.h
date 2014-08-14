//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedInspectionReader
//* .File Name       : $Workfile:   SVSharedInspectionReader.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:08:38  $
//******************************************************************************
#pragma once

#include <memory>
#include <boost/utility.hpp>
#include "SVSharedLastInspectedCache.h"
#include "SVSharedRejectCache.h"
//#include "Stats.h"

namespace SeidenaderVision
{
	class SVSharedInspectionReader : public boost::noncopyable // one reader per inspection
	{
		bool m_bOpened;
		typedef std::shared_ptr<boost::interprocess::managed_shared_memory> DataSharedMemPtr;

	public:
		SVSharedInspectionReader();
		~SVSharedInspectionReader();

		bool Open(const std::string& name);
		void Close();
		bool IsOpen() const;

		//Stats m_stats;

		SVSharedData& GetInspectedSlot(long index); // for the product reader's use
		SVSharedData& GetRejectSlot(long index);

	private:
		void Init();
	
		DataSharedMemPtr shm;
		SVSharedLastInspectedCache* sh;
		SVSharedRejectCache* rsh;

		std::string m_ShareName;
	};

	typedef std::shared_ptr<SVSharedInspectionReader> InspectionReaderPtr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedInspectionReader.h_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:08:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
