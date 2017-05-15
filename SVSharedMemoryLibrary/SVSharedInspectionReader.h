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
#include "SharedImageStore.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace SvSml
{
	class SVSharedInspectionReader : public boost::noncopyable // one reader per inspection
	{
		
	public:
		SVSharedInspectionReader();
		~SVSharedInspectionReader();

		bool Open(const SVString& name);
		void Close();
		bool IsOpen() const;

		SVSharedData& GetInspectedSlot(long index); // for the product reader's use
		SVSharedData& GetRejectSlot(long index);

		
	private:
		void Init();
		
		bool m_bOpened;
		std::shared_ptr<bip::managed_shared_memory> m_pManagedSharedMemory;
		SVSharedLastInspectedCache* m_pSharedLastInspectedCache;
		SVSharedRejectCache* m_pSharedRejectCache;
		
		SharedImageStore  m_ProductImages; //< shared image buffer for the inspection
		SharedImageStore  m_RejectImages;   //<shared reject image buffer for the inspection

		SVString m_ShareName;
	};
	typedef std::shared_ptr<SVSharedInspectionReader> InspectionReaderPtr;

} //namespace SvSml
