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

#pragma region Includes
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <boost/utility.hpp>
#include "SVSharedLastInspectedCache.h"
#include "SVSharedRejectCache.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
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

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;

