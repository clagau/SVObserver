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
	const std::string g_shName = "MonitorListStore.";

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
		if (nullptr != shm.get())
		{
			if (m_lists)
			{
				shm->destroy_ptr(m_lists);
				m_lists = nullptr;
			}
			shm.reset();
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
			boost::interprocess::shared_memory_object::remove(m_ShareName.c_str());
		}
	}

	bool SVMonitorListWriter::IsCreated() const
	{
		SVSharedConfiguration::Log("SVMonitorListWriter::IsCreated");
		return (nullptr != shm.get() && m_lists);
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
				shm = managed_shared_memory_shared_ptr(new boost::interprocess::managed_shared_memory(boost::interprocess::create_only, m_ShareName.c_str(), managedShareSize));

				// Allocate monitor list store
				MonitorListStoreAllocator salloc = shm->get_allocator<MonitorListStoreAllocator>();
				auto rslt = shm->construct<SVMonitorListStore>("MonitorListStore")(salloc);

				if (rslt)
				{
					// get pointer to the monitor list store
					m_lists = shm->find<SVMonitorListStore>("MonitorListStore").first;
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
		catch (const boost::interprocess::interprocess_exception& e)
		{
			SVSharedConfiguration::Log(e.what());
			Release();
			l_result = E_FAIL;
		}
		return l_result;
	}

	void SVMonitorListWriter::AddList(const std::string & listName, const std::string & ppqName, int rejectDepth, bool isActive )
	{
		SVSharedConfiguration::Log("SVMonitorListWriter::AddList");
		if (!m_lists)
		{
			throw std::exception("MonitorListStore not created yet.");
		}
		MonitorListAllocator alloc = shm->get_allocator<SVSharedMonitorList>();
		SVSharedMonitorList list(alloc);
		isActive?   list.Activate(): list.Deactivate();  
		list.SetNames(listName, ppqName);
		list.SetRejectDepth(rejectDepth);
		m_lists->Add(list);
	}

	void SVMonitorListWriter::FillList(const std::string & listName, listType type, const std::vector<std::string> & list)
	{
		SVSharedConfiguration::Log("SVMonitorListWriter::FillList");
		if (!m_lists)
		{
			throw std::exception("MonitorListStore not created yet.");
		}

		SVSharedMonitorList & mlist = (*m_lists)[listName];
		switch(type)
		{
			case productItems:
				mlist.SetProductItems(list);
				break;
			case rejectCondition:
				mlist.SetRejectCond(list);
				break;
			case failStatus:
				mlist.SetFailStatus(list);
				break;
			default:
				throw std::exception("Invalid list type.");
		}
	}

	void SVMonitorListWriter::SetProductFilter(const std::string & listName, SVProductFilterEnum filter)
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
		catch (boost::interprocess::interprocess_exception& e)
		{
			::OutputDebugString(e.what());

		}
#else
		catch (boost::interprocess::interprocess_exception& )
		{
			throw std::exception("MonitorList SetProductFilter failed.");
		}
#endif
	}

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
