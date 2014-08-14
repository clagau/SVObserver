//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMonitorReader
//* .File Name       : $Workfile:   SVMonitorListReader.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:07:26  $
//******************************************************************************
#include "StdAfx.h"
#include "SVMonitorListReader.h"
#include "SVSharedConfiguration.h"

const std::string g_shName = "MonitorListStore";

using namespace SeidenaderVision;
 
SVMonitorListReader::SVMonitorListReader(): m_store(nullptr), m_isOpen(false)
{
	SVSharedConfiguration::Log("SVMonitorListReader::Constructor");
}

SVMonitorListReader::~SVMonitorListReader()
{
	SVSharedConfiguration::Log("SVMonitorListReader::Destructor");
	Close();
}

bool SVMonitorListReader::Open()
{
	SVSharedConfiguration::Log("SVMonitorListReader::Open");
	bool retVal = false;
	try
	{
		m_ShareName = g_shName + "." + SVSharedConfiguration::GetShareName();
		shm = DataSharedMemPtr(new boost::interprocess::managed_shared_memory(boost::interprocess::open_read_only, m_ShareName.c_str()));

		// get a pointer to the monitor list segment
		auto store = shm->find<SVMonitorListStore>(g_shName.c_str());
		//if (store.second)
		if (store.first)
		{	
			m_store = store.first;
			retVal = true;
			m_isOpen = true;
		}
	}
	catch (boost::interprocess::interprocess_exception & e)
	{
		SVSharedConfiguration::Log(e.what());
		m_isOpen = false;
		m_store = nullptr;
	}
	return retVal;
}

void SVMonitorListReader::Close()
{
	SVSharedConfiguration::Log("SVMonitorListReader::Close");
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
	m_store = nullptr;
	m_ShareName.clear();
	m_isOpen = false;
}

bool SVMonitorListReader::IsOpen() const
{
	SVSharedConfiguration::Log("SVMonitorListReader::IsOpen");
	return m_isOpen && nullptr != m_store;
}

const SVSharedMonitorList & SVMonitorListReader::operator[](const std::string & listName) const
{
	SVSharedConfiguration::Log("SVMonitorListReader::operator[]");
	if (!IsOpen())
	{
		throw std::exception("operator[]: MonitorListStore not open yet.");
	}
	return (*m_store)[listName];
}

const std::vector<std::string> SVMonitorListReader::GetListNames() const
{
	SVSharedConfiguration::Log("SVMonitorListReader::GetListNames");
	if (!IsOpen())
	{
		throw std::exception("GetListNames: MonitorListStore not open yet.");
	}
	return m_store->GetListNames();
}

bool SVMonitorListReader::HasList(const std::string & name) const
{
	SVSharedConfiguration::Log("SVMonitorListReader::HasList");
	if (!IsOpen())
	{
		throw std::exception("HasList: MonitorListStore not open yet.");
	}
	return m_store->HasList(name);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVMonitorListReader.cpp_v  $
 * 
 *    Rev 1.0   14 Aug 2014 17:07:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/