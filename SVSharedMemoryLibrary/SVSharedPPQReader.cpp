//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSharedPPQReader
//* .File Name       : $Workfile:   SVSharedPPQReader.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   17 Oct 2014 13:02:36  $
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "SVSharedPPQReader.h"
#include "SVSharedConfiguration.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	

	SVSharedPPQReader::SVSharedPPQReader()
	: m_DataSharedMemPtr(nullptr), m_SharedProductStorePPQ(nullptr), m_SharedProductStorePPQReject(nullptr), m_ShareName(""), m_isOpen(false)
	{
	}

	SVSharedPPQReader::~SVSharedPPQReader()
	{
		Close();
	}

	bool SVSharedPPQReader::Open(const SVString& name)
	{
		bool retVal = false;
		try
		{
			m_ShareName = name + "." + SVSharedConfiguration::GetShareName();
			m_DataSharedMemPtr = std::shared_ptr<bip::managed_shared_memory>(new bip::managed_shared_memory(bip::open_only, m_ShareName.c_str()));

			// get pointers to the product and reject segments
			m_SharedProductStorePPQ = m_DataSharedMemPtr->find<SVSharedProductStore>(SVSharedConfiguration::GetPPQName().c_str()).first;
			m_SharedProductStorePPQReject = m_DataSharedMemPtr->find<SVSharedProductStore>(SVSharedConfiguration::GetPPQRejectsName().c_str()).first;
			if (m_SharedProductStorePPQ && m_SharedProductStorePPQReject)
			{
				retVal = true;
				m_isOpen = true;
			}
		}
		catch (bip::interprocess_exception& e)
		{
			SVSharedConfiguration::Log(e.what());
			m_isOpen = false;
		}
		return retVal;
	}

	void SVSharedPPQReader::Close()
	{
		try
		{
			if (m_DataSharedMemPtr)
			{
				m_DataSharedMemPtr.reset();
			}
		}
		catch(bip::interprocess_exception& e)
		{
			SVSharedConfiguration::Log(e.what());
		}
		m_SharedProductStorePPQ = nullptr;
		m_SharedProductStorePPQReject = nullptr;

		m_isOpen = false;
	}

	bool SVSharedPPQReader::IsOpen() const
	{
		return m_isOpen;
	}

	ProductPtr SVSharedPPQReader::RequestNextProduct(long& idx) const
	{
		if (idx < 0) // negative idx requests next product, positive one asks for a specific index
		{
			idx = next_readable();
		}
		else if (idx >= static_cast<long>(m_SharedProductStorePPQ->data.size()))
		{
			throw std::exception("Index out of bounds.");
		}

		ProductPtr ret(new SVProductBundle(m_SharedProductStorePPQ->data[idx]));
		InspReaderMap& readers = m_inspReaders;
		const SVSharedInspectionMap& inspections = ret->product.m_Inspections;
		std::for_each(inspections.begin(), inspections.end(), 
			[&readers, ret](const SVSharedInspectionPair& pair)
		{
			SVString name = pair.first.c_str();
			if (readers.find(name.c_str()) == readers.end())
			{
				InspectionReaderPtr reader(new SVSharedInspectionReader());
				reader->Open(name.substr(0, name.find_first_of('.')));
				readers.insert(InspReaderMap::value_type(name.c_str(), reader));
			}
			ret->inspections[name.c_str()] =  (SVSharedData*)(
				&(readers[name.c_str()])->GetInspectedSlot(ret->product.InspectionSlotIndex(name.c_str()))
				);
		}
		);
		return ret;
	}

	void SVSharedPPQReader::ReleaseProduct(const ProductPtr product, long idx) const
	{
		long size = static_cast<long>(m_SharedProductStorePPQ->data.size());
		if (product && (idx >= 0 && idx < size))
		{
			try
			{
				SVSharedProductVector* pData = &m_SharedProductStorePPQ->data;
	
				short flags = static_cast<short>((*pData)[idx].m_Flags);
				if (flags > 0) // check for a reader lock
				{
					short x = _InterlockedDecrement16((volatile short *)&((*pData)[idx].m_Flags));
					assert(x >= 0);
					long inFlags(0),Toflags(0);
					do
					{
						inFlags = (*pData)[idx].m_Flags;
						Toflags =  inFlags | ds::ready;
					}
					while (inFlags != _InterlockedCompareExchange(&((*pData)[idx].m_Flags ),Toflags,flags)); 
					// If no other thread updated m_Flags, then Toflags is stored and the loop ends.					
				}
				else
				{
					// When SVObserver has gone offline and the RRS hasn't detected the state change yet,
					// it could still be in the process of handling a GetProduct command,
					// and current_idx could be -1 (See ClearHeld in SVSharedPPQWriter) 
					if (m_SharedProductStorePPQ->current_idx >= 0)
					{
						// log exception
						SvStl::MessageMgrStd Exception( SvStl::LogOnly );
						Exception.setMessage(SVMSG_SHAREDMEMORY_READER_RELEASE_NO_READER_LOCK, SvOi::Tid_ErrorReleaseProductNoReaderLock, SvStl::SourceFileParams(StdMessageParams), SVMSG_SHAREDMEMORY_READER_RELEASE_NO_READER_LOCK );
					}
				}
			}
			catch (std::exception& e)
			{
				// log exception
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage(SVMSG_SHAREDMEMORY_READER_ACCESS_VIOLATION, e.what(), SvStl::SourceFileParams(StdMessageParams), SVMSG_SHAREDMEMORY_READER_ACCESS_VIOLATION );
			}
		}
	}

	long SVSharedPPQReader::next_readable() const
	{
		long flags = ds::none;
		long start = m_SharedProductStorePPQ->current_idx;
		if (start < 0)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_OTHER)
			::OutputDebugString("ppq next_readable - No Data\n");
#endif
			throw std::exception("ppq next_readable No Data");
		}
		long idx = start + 1;
		long count = 0;
		long size = static_cast<long>(m_SharedProductStorePPQ->data.size());

		SVSharedProductVector* data = &m_SharedProductStorePPQ->data;

		do
		{
			idx = (--idx + size) % size;
			flags = (*data)[idx].m_Flags;
			if (count++ > size)
			{
				throw std::exception("ppq next_readable out of slots");
			}
		} 
		while((flags & ds::writing) == ds::writing || _InterlockedCompareExchange(&(*data)[idx].m_Flags, flags + 1, flags) != flags);

		return idx;
	}

	long SVSharedPPQReader::next_reject_readable() const
	{
		int count = 0;
		long flags = ds::none;
		long start = m_SharedProductStorePPQReject->current_idx;
		long idx = start + 1;
		long size = static_cast<long>(m_SharedProductStorePPQReject->data.size());
		if (idx < 0) // -1 means No rejects are available (none exist)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
			::OutputDebugString("ppq next_reject_readable - No Data\n");
#endif
			throw std::exception("No Rejects exist");
		}
		SVSharedProductVector* data = &m_SharedProductStorePPQReject->data;
		do
		{
			idx = (--idx + size) % size;
			flags = (*data)[idx].m_Flags;
			if (count++ > size)
			{
				throw std::exception("next_reject_readable out of slots");
			}
		} 
		while((flags & ds::writing) == ds::writing || _InterlockedCompareExchange(&(*data)[idx].m_Flags, flags + 1, flags) != flags);

		return idx;
	}

	long SVSharedPPQReader::FindRejectSlot(long trigger) const
	{
		SVSharedProductVector& store = m_SharedProductStorePPQReject->data;
		if (trigger < 0) // negative idx requests next product, positive one asks for a specific trigger count
		{
			return next_reject_readable();
		}

		for (long i = 0; i < static_cast<long>(store.size()); ++i)
		{
			if (store[i].m_TriggerCount == trigger)
			{
				long flags = ds::none;
				do
				{
					flags = store[i].m_Flags;
				} 
				while((flags & ds::writing) == ds::writing || _InterlockedCompareExchange(&store[i].m_Flags, flags + 1, flags) != flags);

				return i;
			}
		}
		throw std::exception("Reject with a specified trigger count not found.");
	} 

	ProductPtr SVSharedPPQReader::GetReject(long idx) const
	{
		ProductPtr ret(new SVProductBundle(m_SharedProductStorePPQReject->data[idx]));
		if (ret->product.m_TriggerCount < 0)
		{
			throw std::exception("No reject at this index");
		}
		InspReaderMap& readers = m_inspReaders;
		const SVSharedInspectionMap& inspections = ret->product.m_Inspections;
		std::for_each(inspections.begin(), inspections.end(), 
			[&readers, ret](const SVSharedInspectionPair& pair)
		{
			if (readers.find(pair.first.c_str()) == readers.end())
			{
				InspectionReaderPtr reader(new SVSharedInspectionReader());
				SVString name = SVString(pair.first.c_str());
				reader->Open(name.substr(0, name.find_first_of('.')));
				readers.insert(InspReaderMap::value_type(pair.first.c_str(), reader));
			}
			typedef std::map<const SVString,  SVSharedData*> InspValueType;
			ret->inspections[SVString(pair.first.c_str())] =  (SVSharedData*)(
				&(readers[pair.first.c_str()])->GetRejectSlot(ret->product.InspectionSlotIndex(pair.first.c_str()))
				);
		}
		);
		return ret;
	}

	ProductPtr SVSharedPPQReader::RequestReject(long trig, long& idx) const
	{
		idx = FindRejectSlot(trig);
		return GetReject(idx);
	}

	void SVSharedPPQReader::ReleaseReject(const ProductPtr product, long idx) const
	{
		long size = static_cast<long>(m_SharedProductStorePPQReject->data.size());
		if (product && (idx >= 0 && idx < size))
		{
			try
			{
				SVSharedProductVector* pData = &m_SharedProductStorePPQReject->data;
	
				short flags = static_cast<short>((*pData)[idx].m_Flags);
				if (flags > 0) // check for a reader lock
				{
					short x = _InterlockedDecrement16((volatile short *)&((*pData)[idx].m_Flags));
					assert(x >= 0);
					long inFlags(0), toFlags(0);
					do
					{
						inFlags = (*pData)[idx].m_Flags;
						toFlags =  inFlags | ds::ready;
					}
					while (inFlags != _InterlockedCompareExchange(&((*pData)[idx].m_Flags ),toFlags,flags)); 
					// If no other thread updated m_Flags, then Toflags is stored and the loop ends.					
				}
				else
				{
					// When SVObserver has gone offline and the RRS hasn't detected the state change yet,
					// it could still be in the process of handling a GetReject command,
					// and current_idx could be -1 (See ClearHeld in SVSharedPPQWriter) 
					if (m_SharedProductStorePPQReject->current_idx >= 0)
					{
						// log exception
						SvStl::MessageMgrStd Exception( SvStl::LogOnly );
						Exception.setMessage(SVMSG_SHAREDMEMORY_READER_RELEASE_NO_READER_LOCK, SvOi::Tid_ErrorReleaseProductNoReaderLock, SvStl::SourceFileParams(StdMessageParams), SVMSG_SHAREDMEMORY_READER_RELEASE_NO_READER_LOCK );
					}
				}
			}
			catch (std::exception& e)
			{
				// log exception
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage(SVMSG_SHAREDMEMORY_READER_ACCESS_VIOLATION, e.what(), SvStl::SourceFileParams(StdMessageParams), SVMSG_SHAREDMEMORY_READER_ACCESS_VIOLATION );
			}
		}
	}

	FailStatusMap SVSharedPPQReader::GetFailStatus(const MonitorEntries & rEntries) const
	{
		FailStatusMap ret;
		const SVSharedPPQReader& reader = *this;
		for (long idx = 0; idx < static_cast<long>(m_SharedProductStorePPQReject->data.size()); ++idx) // read all rejects and fill in FailStatusMap
		{
			const SVSharedProduct& prod = m_SharedProductStorePPQReject->data[idx];
			reader.lock(prod);
			long trig = prod.m_TriggerCount;
			if (trig >= 0)
			{
				ProductPtr pp = reader.GetReject(idx);
				ret.insert(std::make_pair(trig, SVValueVector()));
				SVValueVector& values = ret[trig];
				values.reserve(rEntries.size());
				
				MonitorEntries::const_iterator it;
				for(it = rEntries.begin(); it != rEntries.end(); ++it)
				{
					SVValue val = pp->find(it->get()->name);
					if (!val.empty())
					{
						val.trigger = trig;
						values.push_back(val);
					}

				}
			}
			reader.unlock(prod);
		}
		return ret;
	}

	// This method is only called from GetFailStatus
	SVValue SVProductBundle::find(const SVString& name) const
	{
		SVValue val;
		// find the Inspection Share for this item
		SVString inspectionName = name.substr(0, name.find_first_of('.'));
		SVSharedDataPtrMap::const_iterator it = std::find_if(inspections.begin(), inspections.end(),
			[&inspectionName](const SVSharedDataPtrMap::value_type& entry)
		{
			return inspectionName == entry.first.substr(0, entry.first.find_first_of('.'));
		}
		);
		if (it != inspections.end())
		{
			SVValue v = it->second->FindValue(name);
			if (!v.empty())
			{
				val = v;
			}
		}
		else
		{
			throw std::exception((name + " not found.").c_str());
		}
		return val;
	}
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

