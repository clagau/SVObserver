//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedPPQWriter
//* .File Name       : $Workfile:   SVSharedPPQWriter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   02 Sep 2014 13:18:18  $
//******************************************************************************
#include "StdAfx.h"
#include <cmath>
#include "SVSharedPPQWriter.h"
#include "SVSharedConfiguration.h"

namespace SeidenaderVision
{
	SVSharedPPQWriter::SVSharedPPQWriter()
		: sh(nullptr)
		, rsh(nullptr)
	{
	}

	SVSharedPPQWriter::~SVSharedPPQWriter()
	{
		Destroy();
	}

	SVSharedProduct & SVSharedPPQWriter::RequestNextProductSlot(long & idx)
	{
		idx = next_writable(sh);
		return sh->data[idx];
	}

	SVSharedProduct & SVSharedPPQWriter::RequestNextRejectSlot(long & idx)
	{
		idx = next_writable(rsh, true);
		return rsh->data[idx];
	}

	SVSharedProduct & SVSharedPPQWriter::GetProductSlot(long idx)
	{
		if (idx >= 0 && idx < static_cast<long>(sh->data.size()))
		{
			return sh->data[idx];
		}
		throw (std::exception("GetProductSlot Bad Index"));
	}

	void SVSharedPPQWriter::ReleaseProduct(SVSharedProduct & product)
	{
		product.m_Flags &= ~ds::writing;
		product.m_Flags |= ds::ready;
	}

	void SVSharedPPQWriter::ReleaseReject(SVSharedProduct & product)
	{
		product.m_Flags &= ~ds::writing;
		product.m_Flags |= ds::ready;
	}

	// get next writable index for the ppq Run or Reject data
	long SVSharedPPQWriter::next_writable(SVSharedProductStore * share, bool bReject)
	{
		long current_idx = share->current_idx;
		long idx = 0;
		size_t count = 0;
		long flags = ds::none;
		SVSharedProductVector & data = share->data;
		size_t size = data.size();
		do 
		{
			idx = (++current_idx) % size;
			flags = data[idx].m_Flags;
			if (count++ > size)
			{
				if (bReject)
				{
					throw std::exception("No Available Reject Slots for Writing\n");
				}
				else
				{
					throw std::exception("No Available Slots for Writing\n");
				}
			}
		} 
		while ((flags & ds::reading) || ::InterlockedCompareExchange(&data[idx].m_Flags, flags | ds::writing, flags) != flags);

		share->current_idx = idx;
		return idx;
	}

	void SVSharedPPQWriter::Init()
	{
		// remove previous share
		if (!m_ShareName.empty())
		{
			boost::interprocess::shared_memory_object::remove(m_ShareName.c_str());
		}
	}

	// Create
	HRESULT SVSharedPPQWriter::Create( const std::string& name, const InspectionIDs & inspections, const SVSharedMemorySettings& p_rSettings )
	{
		HRESULT l_result = S_OK;

		m_ShareName = name + "." + SVSharedConfiguration::GetShareName();
		try
		{
			Init();

			// Allocate new repositories
			size_t managedShareSize = p_rSettings.ProductStoreSize() * statics::M;
			shm = managed_shared_memory_shared_ptr(new boost::interprocess::managed_shared_memory(boost::interprocess::create_only, m_ShareName.c_str(), managedShareSize));

			// Allocate product store
			ProductStoreAllocator salloc = shm->get_allocator<SVSharedProductStore>();
			shm->construct<SVSharedProductStore>(SVSharedConfiguration::GetPPQName().c_str())(salloc, p_rSettings.NumProductSlots());

			// Adjust Number of Reject slots
			long numRejectSlots = p_rSettings.NumRejectSlots();
			if (numRejectSlots)
			{
				numRejectSlots += static_cast<long>(std::ceil(static_cast<float>(numRejectSlots) * 0.5f)) + 4; // allow 50% extra + 4 additional slots
			}
			// Construct adjusted settings
			SVSharedMemorySettings settings(p_rSettings.MonitorStoreSize(), p_rSettings.ProductStoreSize(), p_rSettings.DataStoreSize(), p_rSettings.NumProductSlots(), numRejectSlots);
			shm->construct<SVSharedProductStore>(SVSharedConfiguration::GetPPQRejectsName().c_str())(salloc, numRejectSlots);
			try
			{
				m_writers.resize(inspections.size());
				InspectionIDs::const_iterator it = inspections.begin();
				std::for_each(m_writers.begin(), m_writers.end(), 
					[&it, &settings](SVSharedInspectionWriter & wr) 
				{ 
					if (wr.Create(it->first, it->second, settings) != S_OK) throw std::exception("Failed to create inspection writer");
					++it;
				});
			}
			catch (std::exception & e)
			{
				SVSharedConfiguration::Log(e.what());
				l_result = E_FAIL;
			}
			if (S_OK == l_result)
			{
				// get pointers to the product and reject queues
				sh = shm->find<SVSharedProductStore>(SVSharedConfiguration::GetPPQName().c_str()).first;
				rsh = shm->find<SVSharedProductStore>(SVSharedConfiguration::GetPPQRejectsName().c_str()).first;
			}
		}
		catch (const boost::interprocess::interprocess_exception& bex)
		{
			SVSharedConfiguration::Log(bex.what());
			Destroy();
			l_result = E_FAIL;
		}
		return l_result;
	}

	void SVSharedPPQWriter::Destroy()
	{
		m_writers.clear();
		if (shm.get() != nullptr)
		{
			ReleaseAll();
			if (rsh)
			{
				shm->destroy_ptr(rsh);
				rsh = nullptr;
			}
			if (sh)
			{
				shm->destroy_ptr(sh);
				sh = nullptr;
			}
			// release managed_shared_memory
			shm.reset();
		}
		Init();
		m_ShareName.clear();
	}

	SVSharedInspectionWriter & SVSharedPPQWriter::operator[](const std::string & shareName)
	{
		InspectionWriters::iterator it = std::find_if(m_writers.begin(), m_writers.end(), 
			[&shareName](SVSharedInspectionWriter & iw)->bool 
		{ return iw.GetShareName() == shareName; });
		if (it != m_writers.end())
		{
			return *it;
		}
		throw std::exception("Inspection writer not found.");
	}

	SVSharedInspectionWriter & SVSharedPPQWriter::operator[](const GUID & guid)
	{
		InspectionWriters::iterator it = std::find_if(m_writers.begin(), m_writers.end(), 
			[&guid](SVSharedInspectionWriter & iw)->bool 
		{ return std::memcmp(&iw.GetGuid(), &guid, sizeof(GUID)) == 0; });
		if (it != m_writers.end())
		{
			return *it;
		}
		throw std::exception("Inspection writer not found.");
	}

	HRESULT SVSharedPPQWriter::CopyLastInspectedToReject(const SVSharedProduct& rProduct)
	{
		HRESULT hr = S_OK;
		long rejectIndex = 0;
		SVSharedProduct& rRejectProduct = RequestNextRejectSlot(rejectIndex);
		rRejectProduct.m_TriggerCount = rProduct.m_TriggerCount;

		for (SVSharedInspectionMap::const_iterator it = rProduct.m_Inspections.begin();it != rProduct.m_Inspections.end() && S_OK == hr;++it)
		{
			SVSharedInspectionWriter & rInspectionWriter = (*this)[it->second.m_ShareName.c_str()];
			hr = rInspectionWriter.CopyLastInspectedToReject(it->second.m_Index, rejectIndex);
			if (hr == S_OK)
			{

				std::pair< SVSharedInspectionMap::iterator,bool>  mRet; 	
				mRet = rRejectProduct.m_Inspections.insert(SVSharedInspectionPair(it->second.m_ShareName, SVSharedInspection(it->second.m_ShareName.c_str(), rejectIndex, rRejectProduct.m_Allocator)));


			}
		}
		ReleaseReject(rRejectProduct);
		return hr;
	}

	// this should only be called when there are no clients attached
	void SVSharedPPQWriter::ClearHeld(SVSharedProductStore* share)
	{
		if (share)
		{
			size_t size = share->data.size();
			for (size_t i = 0; i < size;i++)
			{
				share->data[i].m_Flags = ds::none;
			}
			share->current_idx = -1;
		}
	}

	// this should be called when closing  or the client disconnects
	void SVSharedPPQWriter::ReleaseAll()
	{
		// free all resources
		ClearHeld(sh);
		ClearHeld(rsh);
	}
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVSharedPPQWriter.cpp_v  $
 * 
 *    Rev 1.2   02 Sep 2014 13:18:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised next_writable method to throw an std::exception rather than call DebugBreak.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Aug 2014 18:32:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised Create method to add an additional 4 slots to the allocation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Aug 2014 17:09:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/