//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShareControlHandler
//* .File Name       : $Workfile:   SVShareControlHandler.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   02 Oct 2014 09:05:54  $
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "SVShareControlHandler.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	SVShareControlHandler::SVShareControlHandler()
	: m_shareName("ShareControl")
	, m_count(0)
	, m_filterChangeCount(0)
	, m_ctrl(nullptr)
	{
		SVSharedConfiguration::Log("ControlHandler::Constructor");
		const std::string segmentName = SVSharedConfiguration::GetControlShareName();
		try
		{
			SVSharedConfiguration::EnsureShareDirectoryExists();
			shm = managed_shared_memory_shared_ptr(new boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create, segmentName.c_str(), 4096));
			m_ctrl = shm->find<SVShareControl>(m_shareName.c_str()).first;
			if (!m_ctrl)
			{
				SVShareControlAllocator salloc = shm->get_allocator<SVShareControl>();
				shm->construct<SVShareControl>(m_shareName.c_str())(salloc);
				m_ctrl = shm->find<SVShareControl>(m_shareName.c_str()).first;
			}
		}
		catch (const boost::interprocess::interprocess_exception & e)
		{
			SVSharedConfiguration::Log(e.what());
			Release();
		}
	}

	SVShareControlHandler::~SVShareControlHandler()
	{
		SVSharedConfiguration::Log("ControlHandler::Destructor");
		Release();
	}

	void SVShareControlHandler::Release()
	{
		SVSharedConfiguration::Log("ControlHandler::Release");
		if (nullptr != shm.get())
		{
			if (m_ctrl)
			{
				shm->destroy_ptr(m_ctrl);
				m_ctrl = nullptr;
			}
			//@TODO[MEC][7.40][28.09.2016]  destroying shared memory must be more sophisticated 
			// with this code problems occurs because the other partner do not recognize the vanishing of shared memory
			/* 
			shm.reset();
			try
			{
			const std::string segmentName = SVSharedConfiguration::GetControlShareName();
			boost::interprocess::shared_memory_object::remove(segmentName.c_str());
			}
			catch (boost::interprocess::interprocess_exception& e)
			{
			SVSharedConfiguration::Log(e.what());
			}*/
		}
	}

	bool SVShareControlHandler::IsCreated() const
	{
		SVSharedConfiguration::Log("ControlHandler::IsCreated");
		return (nullptr != shm.get() && m_ctrl);
	}

	void SVShareControlHandler::SetReady()
	{ 
		SVSharedConfiguration::Log("ControlHandler::SetReady"); 
		m_ctrl->ready = ++m_count; 
		m_ctrl->ack = 0; 
	} 

	void SVShareControlHandler::ClearReady()
	{
		SVSharedConfiguration::Log("ControlHandler::ClearReady");
		m_ctrl->ready = 0;
		m_ctrl->ack = 0;
	}

	bool SVShareControlHandler::IsReady() const 
	{
		SVSharedConfiguration::Log("ControlHandler::IsReady");
		return (IsCreated() && m_ctrl->ready) ? true : false;
	}

	bool SVShareControlHandler::ReadyChanged(long previousValue) const
	{
		SVSharedConfiguration::Log("ControlHandler::ReadyChanged");
		return m_ctrl->ready != previousValue;
	}

	void SVShareControlHandler::SetAck()
	{
		SVSharedConfiguration::Log("ControlHandler::SetAck");
		m_ctrl->ack = 1;
	}

	bool SVShareControlHandler::GotAck() const
	{
		SVSharedConfiguration::Log("ControlHandler::GotAck");
		return m_ctrl->ack ? true : false;
	}

	long SVShareControlHandler::GetReadyCount() const
	{
		long count = 0;
		if (m_ctrl)
		{
			count = m_ctrl->ready;
		}
		return count;
	}

	void SVShareControlHandler::TickSVOHeartbeat()
	{
		m_ctrl->svo_heartbeat = GetTickCount();
	}

	void SVShareControlHandler::TickRRSHeartbeat()
	{
		m_ctrl->rrs_heartbeat = GetTickCount();
	}

	timestamp SVShareControlHandler::GetSVOHeartbeat() const
	{
		return m_ctrl->svo_heartbeat;
	}

	timestamp SVShareControlHandler::GetRRSHeartbeat() const
	{
		return m_ctrl->rrs_heartbeat;
	}

	void SVShareControlHandler::SetProductFilterChanged()
	{
		SVSharedConfiguration::Log("ControlHandler::SetProductFilterChanged");
		m_ctrl->productFilterChanged = ++m_filterChangeCount;
	}

	bool SVShareControlHandler::ProductFilterChanged(long previousValue) const
	{
		SVSharedConfiguration::Log("ControlHandler::ProductFilterChanged");
		return m_ctrl->productFilterChanged != previousValue;
	}

	long SVShareControlHandler::GetProductFilterChangeCount() const 
	{
		long count = 0;
		if (m_ctrl)
		{
			count = m_ctrl->productFilterChanged;
		}
		return count;
	}

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/
