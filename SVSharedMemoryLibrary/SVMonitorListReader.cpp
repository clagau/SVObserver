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
#pragma region Includes
#include "StdAfx.h"
#include "SVMonitorListReader.h"
#include "SVSharedConfiguration.h"
#pragma endregion Includes

namespace SvSml
{
	const std::string g_shName = "MonitorListStore";

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
			m_pManagedSharedMemory = std::shared_ptr<bip::managed_shared_memory>(new bip::managed_shared_memory(bip::open_read_only, m_ShareName.c_str()));

			// get a pointer to the monitor list segment
			auto store = m_pManagedSharedMemory->find<SVMonitorListStore>(g_shName.c_str());
			//if (store.second)
			if (store.first)
			{	
				m_store = store.first;
				retVal = true;
				m_isOpen = true;
			}
			else
			{
				m_isOpen = false;
				m_store = nullptr;
			}
		}
		catch (bip::interprocess_exception & e)
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
			if (m_pManagedSharedMemory)
			{
				m_pManagedSharedMemory.reset();
			}
		}
		catch(bip::interprocess_exception& e)
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

	const SvDef::StringVector SVMonitorListReader::GetListNames() const
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
} //namespace SvSml
