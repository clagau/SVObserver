//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedPPQReader
//* .File Name       : $Workfile:   SVSharedPPQReader.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 Oct 2014 17:49:58  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVSharedProductStore.h"
#include "SVSharedMonitorList.h"
#include "SVSharedMemorySettings.h"
#include "SVSharedInspectionReader.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

#pragma intrinsic (_InterlockedCompareExchange)
#pragma intrinsic (_InterlockedDecrement16)
#pragma intrinsic (_InterlockedIncrement16)

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	typedef std::vector<SVValue> SVValueVector; 
	
	/// maps trigger count into collection of fully qualified name/value pairs
	typedef std::map<long, SVValueVector > FailStatusMap; 
	
	/// holds product and its inspections
	struct SVProductBundle
	{
		const SVSharedProduct& product;
		typedef std::map<SVString,  SVSharedData*> SVSharedDataPtrMap;
		SVSharedDataPtrMap inspections;
		SVProductBundle(const SVSharedProduct& prod)
		: product(prod), inspections()
		{
		}
		SVValue find(const SVString& name) const;
	};

	typedef std::shared_ptr<SVProductBundle> ProductPtr;
	typedef std::map<SVString, InspectionReaderPtr> InspReaderMap;

	class SVSharedPPQReader // one reader per ppq
	{
	public:
		SVSharedPPQReader();
		~SVSharedPPQReader();

		bool Open(const SVString& name);
		void Close();
		bool IsOpen() const;

		ProductPtr RequestNextProduct(long& idx) const;
		void ReleaseProduct(const ProductPtr product, long idx) const;
		// request reject for a given trigger
		ProductPtr RequestReject(long trig, long& idx) const;
		void ReleaseReject(const ProductPtr product, long idx) const;

		FailStatusMap GetFailStatus(const SvSml::MonitorEntries& Entries) const;
		
		
		const SharedImageStore*  GetImageStore(LPCTSTR InspectionName, SharedImageStore::StoreType t ) const;
		void AdInspectionReader(LPCTSTR InspectionName);


	private:
		long next_readable() const;
		long next_reject_readable() const;

		std::shared_ptr<bip::managed_shared_memory> m_DataSharedMemPtr;

		SVSharedProductStore* m_SharedProductStorePPQ;
		SVSharedProductStore* m_SharedProductStorePPQReject;
		bool m_isOpen;
		SVString m_ShareName;
		mutable InspReaderMap m_inspReaders;

		ProductPtr GetReject(long idx) const;

		// find the reject slot with the specified trigger
		long FindRejectSlot(long trigger) const;

		void lock(const SVSharedProduct& prod) const
		{
			long flags = ds::none;
			do
			{
				flags = prod.m_Flags;
			} 
			while((flags & ds::writing) == ds::writing || _InterlockedCompareExchange(&prod.m_Flags, flags + 1, flags) != flags);
		}

		void unlock(const SVSharedProduct& prod) const
		{
			_InterlockedDecrement16((volatile short *)&(prod.m_Flags));		
		}
	};

	///unique Ptr to SVSharedPPQReader
	typedef std::unique_ptr<SVSharedPPQReader> upSharedPPQReader;         

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;

