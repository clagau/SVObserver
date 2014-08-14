//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedPPQReader
//* .File Name       : $Workfile:   SVSharedPPQReader.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:09:52  $
//******************************************************************************
#pragma once
#include <map>
#include <string>
#include "SVSharedProductStore.h"
#include "SVSharedMonitorList.h"
#include "SVSharedMemorySettings.h"
#include "SVSharedInspectionReader.h"

#pragma intrinsic (_InterlockedCompareExchange)
#pragma intrinsic (_InterlockedDecrement16)
#pragma intrinsic (_InterlockedIncrement16)

namespace SeidenaderVision
{
	typedef std::shared_ptr<boost::interprocess::managed_shared_memory> DataSharedMemPtr;

	typedef SVValue Value;
	typedef std::vector<Value> Values;
	// maps trigger count into collection of fully qualified name/value pairs
	typedef std::map<long, Values > FailStatusMap;

	// holds product and its inspections
	struct SVProductBundle
	{
		const SVSharedProduct & product;
		std::map<std::string, InspectionDataPtr> inspections;
		SVProductBundle(const SVSharedProduct & prod):
			product(prod), inspections()
			{
			}
		Value find(const std::string & name) const;
	};

	typedef std::shared_ptr<SVProductBundle> ProductPtr;
	typedef std::map<std::string, InspectionReaderPtr> InspReaderMap;

	class SVSharedPPQReader // one reader per ppq
	{
	public:
		SVSharedPPQReader();
		~SVSharedPPQReader();

		bool Open(const std::string& name);
		void Close();
		bool IsOpen() const;

		ProductPtr RequestNextProduct(long & idx) const;
		void ReleaseProduct(const ProductPtr product, long & idx) const;
		// request reject for a given trigger
		ProductPtr RequestReject(long trig, long & idx) const;
		void ReleaseReject(const ProductPtr product, long idx) const;

		FailStatusMap GetFailStatus(const std::vector<std::string> & names) const;

	private:
		long next_readable() const;
		long next_reject_readable() const;

		DataSharedMemPtr shm;

		SVSharedProductStore * sh;
		SVSharedProductStore * rsh;
		bool m_isOpen;
		std::string m_ShareName;
		mutable InspReaderMap m_inspReaders;

		ProductPtr GetReject(long idx) const;

		// find the reject slot with the specified trigger
		long FindRejectSlot(long trigger) const;

		void lock(const SVSharedProduct & prod) const
		{
			long flags = ds::none;
			do
			{
				flags = prod.m_Flags;
			} 
			while((flags & ds::writing) == ds::writing || _InterlockedCompareExchange(&prod.m_Flags, flags + 1, flags) != flags);
		}

		void unlock(const SVSharedProduct & prod) const
		{
			_InterlockedDecrement16((volatile short *)&(prod.m_Flags));		
		}
	};
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedPPQReader.h_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:09:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/