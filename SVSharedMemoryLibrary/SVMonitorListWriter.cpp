//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMonitorListWriter
//* .File Name       : $Workfile:   SVMonitorListWriter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:07:30  $
//******************************************************************************
#include "StdAfx.h"
#include "SVMonitorListWriter.h"
#include "SVSharedConfiguration.h"

using namespace SeidenaderVision;

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
	if (shm.get() != nullptr)
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
	return (shm.get() != nullptr && m_lists);
}

HRESULT SVMonitorListWriter::Create(const SVSharedMemorySettings & settings)
{
	SVSharedConfiguration::Log("SVMonitorListWriter::Create");
	HRESULT l_result = S_OK;

	m_ShareName = g_shName + SVSharedConfiguration::GetShareName();
	try
	{
		Init();

		// Allocate new repositories
		size_t managedShareSize = settings.MonitorStoreSize() * statics::M;
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
	catch (const boost::interprocess::interprocess_exception & e)
	{
		SVSharedConfiguration::Log(e.what());
		Release();
		l_result = E_FAIL;
	}
	return l_result;
}

void SVMonitorListWriter::AddList(const std::string & listName, const std::string & ppqName, int rejectDepth)
{
	SVSharedConfiguration::Log("SVMonitorListWriter::AddList");
	if (!m_lists)
	{
		throw std::exception("MonitorListStore not created yet.");
	}
	MonitorListAllocator alloc = shm->get_allocator<SVSharedMonitorList>();
	SVSharedMonitorList list(alloc);
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVMonitorListWriter.cpp_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:07:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/