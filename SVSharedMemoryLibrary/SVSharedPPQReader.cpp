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
#include "StdAfx.h"
#include "SVSharedPPQReader.h"
#include "SVSharedConfiguration.h"

namespace SeidenaderVision
{
	SVSharedPPQReader::SVSharedPPQReader(): 
shm(nullptr), sh(nullptr), rsh(nullptr), m_ShareName(""), m_isOpen(false)
{
}

SVSharedPPQReader::~SVSharedPPQReader()
{
	Close();
}

bool SVSharedPPQReader::Open(const std::string& name)
{
	bool retVal = false;
	try
	{
		m_ShareName = name + "." + SVSharedConfiguration::GetShareName();
		shm = DataSharedMemPtr(new boost::interprocess::managed_shared_memory(boost::interprocess::open_only, m_ShareName.c_str()));

		// get pointers to the product and reject segments
		sh = shm->find<SVSharedProductStore>(SVSharedConfiguration::GetPPQName().c_str()).first;
		rsh = shm->find<SVSharedProductStore>(SVSharedConfiguration::GetPPQRejectsName().c_str()).first;
		if (sh && rsh)
		{
			retVal = true;
			m_isOpen = true;
		}
	}
	catch (boost::interprocess::interprocess_exception& e)
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
		if (shm)
		{
			shm.reset();
		}
	}
	catch(boost::interprocess::interprocess_exception& e)
	{
		SVSharedConfiguration::Log(e.what());
	}
	sh = nullptr;
	rsh = nullptr;

	m_isOpen = false;
}

bool SVSharedPPQReader::IsOpen() const
{
	return m_isOpen;
}

typedef std::map<std::string, InspectionReaderPtr> Readers;

ProductPtr SVSharedPPQReader::RequestNextProduct(long & idx) const
{
	if (idx < 0) // negative idx requests next product, positive one asks for a specific index
	{
		idx = next_readable();
	}
	else if (idx >= static_cast<long>(sh->data.size()))
	{
		throw std::exception("Index out of bounds.");
	}

	ProductPtr ret(new SVProductBundle(sh->data[idx]));
	Readers & readers = m_inspReaders;
	const SVSharedInspectionMap & inspections = ret->product.m_Inspections;
	std::for_each(inspections.begin(), inspections.end(), 
		[&readers, ret](const SVSharedInspectionPair & pair)
	{
		std::string name = pair.first.c_str();
		if (readers.find(name.c_str()) == readers.end())
		{
			InspectionReaderPtr reader(new SVSharedInspectionReader());
			reader->Open(name.substr(0, name.find_first_of('.')));
			readers.insert(Readers::value_type(name.c_str(), reader));
		}
		ret->inspections[name.c_str()] = InspectionDataPtr(
			&(readers[name.c_str()])->GetInspectedSlot(ret->product.InspectionSlotIndex(name.c_str()))
			);
	}
	);
	return ret;
}

void SVSharedPPQReader::ReleaseProduct(const ProductPtr product, long & idx) const
{
	if (product)
	{
		short x = _InterlockedDecrement16((volatile short *)&(product->product.m_Flags));
		assert(x >= 0);
		product->product.m_Flags |= ds::ready;
	}
}

long SVSharedPPQReader::next_readable() const
{
	long flags = ds::none;
	long start = sh->current_idx;
	if (start < 0)
	{
		::OutputDebugString("ppq next_readable - No Data\n");
		throw std::exception("ppq next_readable No Data");
	}
	long idx = start + 1;
	long count = 0;
	long size = static_cast<long>(sh->data.size());

	SVSharedProductVector * data = &sh->data;

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
	long start = rsh->current_idx;
	long idx = start + 1;
	long size = static_cast<long>(rsh->data.size());
	if (idx < 0) // -1 means No rejects are available (none exist)
	{
		::OutputDebugString("ppq next_reject_readable - No Data\n");
		throw std::exception("No Rejects exist");
	}
	SVSharedProductVector* data = &rsh->data;
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
	SVSharedProductVector & store = rsh->data;
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
	ProductPtr ret(new SVProductBundle(rsh->data[idx]));
	if (ret->product.m_TriggerCount < 0)
	{
		throw std::exception("No reject at this index");
	}
	Readers & readers = m_inspReaders;
	const SVSharedInspectionMap & inspections = ret->product.m_Inspections;
	std::for_each(inspections.begin(), inspections.end(), 
		[&readers, ret](const SVSharedInspectionPair & pair)
	{
		if (readers.find(pair.first.c_str()) == readers.end())
		{
			InspectionReaderPtr reader(new SVSharedInspectionReader());
			std::string name = std::string(pair.first.c_str());
			reader->Open(name.substr(0, name.find_first_of('.')));
			readers.insert(Readers::value_type(pair.first.c_str(), reader));
		}
		typedef std::map<const std::string, InspectionDataPtr> InspValueType;
		ret->inspections[std::string(pair.first.c_str())] = InspectionDataPtr(
			&(readers[pair.first.c_str()])->GetRejectSlot(ret->product.InspectionSlotIndex(pair.first.c_str()))
			);
	}
	);
	return ret;
}

ProductPtr SVSharedPPQReader::RequestReject(long trig, long & idx) const
{
	idx = FindRejectSlot(trig);
	return GetReject(idx);
}

void SVSharedPPQReader::ReleaseReject(const ProductPtr product, long idx) const
{
	if (product)
	{
		short x = _InterlockedDecrement16((volatile short *)&(product->product.m_Flags));
		assert(x >= 0);
		product->product.m_Flags |= ds::ready;
	}
}

FailStatusMap SVSharedPPQReader::GetFailStatus(const std::vector<std::string> & names) const
{
	FailStatusMap ret;
	const SVSharedPPQReader & reader = *this;
	for (long idx = 0; idx < static_cast<long>(rsh->data.size()); ++idx) // read all rejects and fill in FailStatusMap
	{
		const SVSharedProduct & prod = rsh->data[idx];
		reader.lock(prod);
		long trig = prod.m_TriggerCount;
		if (trig >= 0)
		{
			ProductPtr pp = reader.GetReject(idx);
			ret.insert(std::make_pair(trig, Values()));
			Values & values = ret[trig];
			values.reserve(names.size());
			std::for_each(names.begin(), names.end(), //init values with fail status names/triggers
				[&values, trig, pp](const std::string & name)
			{
				Value val = pp->find(name);
				if (!val.empty())
				{
					val.trigger = trig;
					values.push_back(val);
				}
			}
			);
		}
		reader.unlock(prod);
	}
	return ret;
}

// This method is only called from GetFailStatus
Value SVProductBundle::find(const std::string & name) const
{
	Value val;
	// find the Inspection Share for this item
	std::string inspectionName = name.substr(0, name.find_first_of('.'));
	InspectionDataPtrMap::const_iterator it = std::find_if(inspections.begin(), inspections.end(),
		[&inspectionName](const InspectionDataPtrMap::value_type& entry)
	{
		return inspectionName == entry.first.substr(0, entry.first.find_first_of('.'));
	}
	);
	if (it != inspections.end())
	{
		Value v = it->second->FindValue(name);
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
}

