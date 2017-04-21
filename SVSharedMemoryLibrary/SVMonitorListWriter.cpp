//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMonitorListWriter
//* .File Name       : $Workfile:   SVMonitorListWriter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   28 Aug 2014 18:41:56  $
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "SVMonitorListWriter.h"
#include "SVSharedConfiguration.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	const SVString g_shName = "MonitorListStore.";

	SVMonitorListWriter::SVMonitorListWriter()
	: m_lists(nullptr)
	{
		SVSharedConfiguration::Log("SVMonitorListWriter::Constructor");
	}

	SVMonitorListWriter::~SVMonitorListWriter()
	{
		SVSharedConfiguration::Log("SVMonitorListWriter::Destructor");
		Release();
	}

	void SVMonitorListWriter::Release()
	{
		SVSharedConfiguration::Log("SVMonitorListWriter::Release");
		if (nullptr != m_pManagedSharedMemory.get())
		{
			if (m_lists)
			{
				m_pManagedSharedMemory->destroy_ptr(m_lists);
				m_lists = nullptr;
			}
			m_pManagedSharedMemory.reset();
		}
		Init();
	}

	void SVMonitorListWriter::Init()
	{
		SVSharedConfiguration::Log("SVMonitorListWriter::Init");
		// remove previous share
		if (!m_ShareName.empty())
		{
			if (m_lists)
			{
				m_lists->Clear();
				m_lists = nullptr;
			}
			bip::shared_memory_object::remove(m_ShareName.c_str());
		}
	}

	bool SVMonitorListWriter::IsCreated() const
	{
		SVSharedConfiguration::Log("SVMonitorListWriter::IsCreated");
		return (nullptr != m_pManagedSharedMemory.get() && m_lists);
	}

	HRESULT SVMonitorListWriter::Create(const SVSharedMemorySettings& settings, size_t requiredSize)
	{
		SVSharedConfiguration::Log("SVMonitorListWriter::Create");
		HRESULT l_result = S_OK;

		m_ShareName = g_shName + SVSharedConfiguration::GetShareName();
		try
		{
			Init();

			// check required size
			size_t managedShareSize = settings.MonitorStoreSize() * statics::M;
			if (requiredSize < managedShareSize)
			{
				// Allocate new repositories
				m_pManagedSharedMemory = std::shared_ptr<bip::managed_shared_memory>(new bip::managed_shared_memory(bip::create_only, m_ShareName.c_str(), managedShareSize));

				// Allocate monitor list store
				MonitorListStoreAllocator salloc = m_pManagedSharedMemory->get_allocator<MonitorListStoreAllocator>();
				auto rslt = m_pManagedSharedMemory->construct<SVMonitorListStore>("MonitorListStore")(salloc);

				if (rslt)
				{
					// get pointer to the monitor list store
					m_lists = m_pManagedSharedMemory->find<SVMonitorListStore>("MonitorListStore").first;
				}
				else
				{
					m_lists = nullptr;
					SVSharedConfiguration::Log("SVMonitorListWriter::Create failed to construct MonitorListStore");
				}
			}
			else
			{
				l_result = E_INVALIDARG;
			}
		}
		catch (const bip::interprocess_exception& e)
		{
			SVSharedConfiguration::Log(e.what());
			Release();
			l_result = E_FAIL;
		}
		return l_result;
	}

	void SVMonitorListWriter::AddList(const SVString & listName, const SVString & ppqName, int rejectDepth, bool isActive )
	{
		SVSharedConfiguration::Log("SVMonitorListWriter::AddList");
		if (!m_lists)
		{
			throw std::exception("MonitorListStore not created yet.");
		}
		MonitorListAllocator alloc = m_pManagedSharedMemory->get_allocator<SVSharedMonitorList>();
		SVSharedMonitorList list(alloc);
		isActive?   list.Activate(): list.Deactivate();  
		list.SetNames(listName, ppqName);
		list.SetRejectDepth(rejectDepth);
		m_lists->Add(list);
	}

	void SVMonitorListWriter::FillList(const SVString & listName, ListType::typ type, const MonitorEntries & monEntries)
	{
		SVSharedConfiguration::Log("SVMonitorListWriter::FillList");
		if (!m_lists)
		{
			throw std::exception("MonitorListStore not created yet.");
		}

		SVSharedMonitorList & mlist = (*m_lists)[listName];
		switch(type)
		{
			case  ListType::productItemsData:
				mlist.SetEntries(ListType::productItemsData,monEntries);
				break;
			case ListType::productItemsImage:
				mlist.SetEntries(ListType::productItemsImage,monEntries);
				break;
			case ListType::rejectCondition:
				mlist.SetEntries(ListType::rejectCondition,monEntries);
				break;
			case ListType::failStatus:
				mlist.SetEntries(ListType::failStatus,monEntries);
				break;
			default:
				throw std::exception("Invalid list type.");
		}
	}

	void SVMonitorListWriter::SetProductFilter(const SVString & listName, SVProductFilterEnum filter)
	{
		SVSharedConfiguration::Log("SVMonitorListWriter::SetProductFilter");
		if (!m_lists)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
			::OutputDebugString("MonitorListStore not created yet.");
#endif
			throw std::exception("MonitorListStore not created yet.");
		}
		try
		{
			SVSharedMonitorList & mlist = (*m_lists)[listName];
			mlist.SetProductFilter(filter);
		}
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
		catch (bip::interprocess_exception& e)
		{
			::OutputDebugString(e.what());

		}
#else
		catch (bip::interprocess_exception& )
		{
			throw std::exception("MonitorList SetProductFilter failed.");
		}
#endif
	}

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
