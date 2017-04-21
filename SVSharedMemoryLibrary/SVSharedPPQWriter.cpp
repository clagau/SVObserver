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
#pragma region Includes
#include "StdAfx.h"
//Moved to precompiled header: #include <cmath>
#include "SVSharedPPQWriter.h"
#include "SVSharedConfiguration.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	SVSharedPPQWriter::SVSharedPPQWriter()
		: m_pProductStore(nullptr)
		, m_pRejectStore(nullptr)
	{
	}

	SVSharedPPQWriter::~SVSharedPPQWriter()
	{
		Destroy();
	}

	SVSharedProduct& SVSharedPPQWriter::RequestNextProductSlot(long& idx)
	{
		idx = next_writable(m_pProductStore);
		return m_pProductStore->data[idx];
	}

	SVSharedProduct& SVSharedPPQWriter::RequestNextRejectSlot(long& idx)
	{
		idx = next_writable(m_pRejectStore, true);
		return m_pRejectStore->data[idx];
	}

	SVSharedProduct& SVSharedPPQWriter::GetProductSlot(long idx)
	{
		if (idx >= 0 && idx < static_cast<long>(m_pProductStore->data.size()))
		{
			return m_pProductStore->data[idx];
		}
		throw (std::exception("GetProductSlot Bad Index"));
	}

	void SVSharedPPQWriter::ReleaseProduct(SVSharedProduct& product)
	{
		_ReadWriteBarrier();
		long flag,newflag;
		do 
		{
			flag = product.m_Flags;
			newflag =  flag &  ~ds::writing;
			newflag |= ds::ready;
		}
		while ( InterlockedCompareExchange(&(product.m_Flags),newflag, flag) != flag) ;
		// If no other thread updated product.m_Flags, then newflag is stored and the loop ends.
    }

	void SVSharedPPQWriter::ReleaseReject(SVSharedProduct& product)
	{
		
		_ReadWriteBarrier();
		long flag,newflag;
		do 
		{
			flag = product.m_Flags;
			newflag =  flag &  ~ds::writing;
			newflag |= ds::ready;
		}
		while ( InterlockedCompareExchange(&(product.m_Flags),newflag, flag) != flag) ;
	}

	// get next writable index for the ppq Run or Reject data
	long SVSharedPPQWriter::next_writable(SVSharedProductStore* share, bool bReject)
	{
		long current_idx = share->current_idx;
		long idx = 0;
		size_t count = 0;
		long flags = ds::none;
		SVSharedProductVector& data = share->data;
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
			bip::shared_memory_object::remove(m_ShareName.c_str());
		}
	}

	// Create
	HRESULT SVSharedPPQWriter::Create( const SVString& name, const InspectionWriterCreationInfos& inspections, const SVSharedMemorySettings& rSettings, const long ProductSlots, const long RejectSlots, size_t size )
	{
		HRESULT l_result = S_OK;

		m_ShareName = name + "." + SVSharedConfiguration::GetShareName();
		try
		{
			Init();

			//@Todo[MEC][7.50] [21.02.2017] find out why this number was changed and try to avoid it 
			// Adjust Number of Reject slots 
			long numRejectSlots(RejectSlots);
			if (numRejectSlots)
			{
				
				numRejectSlots += static_cast<long>(std::ceil(static_cast<float>(numRejectSlots) * 0.5f)) + 4; // allow 50% extra + 4 additional slots
			}
			// check required size
			size_t requiredSize = (ProductSlots * size);
			requiredSize += (numRejectSlots * size);
			size_t managedShareSize = rSettings.ProductStoreSize() * statics::M;

			if (requiredSize < managedShareSize)
			{
				// Allocate new repositories
				m_pManagedSharedMemory = std::shared_ptr<bip::managed_shared_memory>(new bip::managed_shared_memory(bip::create_only, m_ShareName.c_str(), managedShareSize));

				// Allocate product store
				ProductStoreAllocator salloc = m_pManagedSharedMemory->get_allocator<SVSharedProductStore>();
				m_pManagedSharedMemory->construct<SVSharedProductStore>(SVSharedConfiguration::GetPPQName().c_str())(salloc, ProductSlots);
			
				m_pManagedSharedMemory->construct<SVSharedProductStore>(SVSharedConfiguration::GetPPQRejectsName().c_str())(salloc, numRejectSlots);
				try
				{
					m_writers.resize(inspections.size());
					InspectionWriterCreationInfos::const_iterator it = inspections.begin();
					std::for_each(m_writers.begin(), m_writers.end(), 
						[&it, &rSettings, &ProductSlots, &numRejectSlots](SVSharedInspectionWriter& wr) 
					{ 
						if (S_OK != wr.Create(it->inspectionID.first, it->inspectionID.second, rSettings, ProductSlots, numRejectSlots, it->imagesCreationInfo, it->valuesCreationInfo))
						{
							throw std::exception("Failed to create inspection writer");
						}
						++it;
					});
				}
				catch (std::exception& e)
				{
					SVSharedConfiguration::Log(e.what());
					l_result = E_FAIL;
				}
				if (S_OK == l_result)
				{
					// get pointers to the product and reject queues
					m_pProductStore = m_pManagedSharedMemory->find<SVSharedProductStore>(SVSharedConfiguration::GetPPQName().c_str()).first;
					m_pRejectStore = m_pManagedSharedMemory->find<SVSharedProductStore>(SVSharedConfiguration::GetPPQRejectsName().c_str()).first;
				}
			}
			else
			{
				l_result = E_INVALIDARG;
			}
		}
		catch (const bip::interprocess_exception& bex)
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
		if (nullptr != m_pManagedSharedMemory.get())
		{
			ReleaseAll();
			if (m_pRejectStore)
			{
				m_pManagedSharedMemory->destroy_ptr(m_pRejectStore);
				m_pRejectStore = nullptr;
			}
			if (m_pProductStore)
			{
				m_pManagedSharedMemory->destroy_ptr(m_pProductStore);
				m_pProductStore = nullptr;
			}
			// release managed_shared_memory
			m_pManagedSharedMemory.reset();
		}
		Init();
		m_ShareName.clear();
	}

	SVSharedInspectionWriter& SVSharedPPQWriter::operator[](const SVString& shareName)
	{
		std::vector<SVSharedInspectionWriter>::iterator it = std::find_if(m_writers.begin(), m_writers.end(), 
			[&shareName](SVSharedInspectionWriter& iw)->bool 
		{ return iw.GetShareName() == shareName; });
		if (it != m_writers.end())
		{
			return *it;
		}
		throw std::exception("Inspection writer not found.");
	}

	SVSharedInspectionWriter& SVSharedPPQWriter::operator[](const GUID& guid)
	{
		std::vector<SVSharedInspectionWriter>::iterator it = std::find_if(m_writers.begin(), m_writers.end(), 
			[&guid](SVSharedInspectionWriter& iw)->bool 
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
			SVSharedInspectionWriter& rInspectionWriter = (*this)[it->second.m_ShareName.c_str()];
			hr = rInspectionWriter.CopyLastInspectedToReject(it->second.m_Index, rejectIndex);
			if (S_OK == hr)
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
		ClearHeld(m_pProductStore);
		ClearHeld(m_pRejectStore);
	}
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
