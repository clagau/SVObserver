//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShareControlHandler
//* .File Name       : $Workfile:   SVShareControlHandler.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   28 Aug 2014 18:39:46  $
//******************************************************************************
#include "StdAfx.h"
#include "SVShareControlHandler.h"

using namespace SeidenaderVision;

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
	if (shm.get() != nullptr)
	{
		if (m_ctrl)
		{
			shm->destroy_ptr(m_ctrl);
			m_ctrl = nullptr;
		}
		shm.reset();
		try
		{
			const std::string segmentName = SVSharedConfiguration::GetControlShareName();
			boost::interprocess::shared_memory_object::remove(segmentName.c_str());
		}
		catch (boost::interprocess::interprocess_exception& e)
		{
			SVSharedConfiguration::Log(e.what());
		}
	}
}

bool SVShareControlHandler::IsCreated() const
{
	SVSharedConfiguration::Log("ControlHandler::IsCreated");
	return (shm.get() != nullptr && m_ctrl);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSharedMemoryLibrary\SVShareControlHandler.cpp_v  $
 * 
 *    Rev 1.3   28 Aug 2014 18:39:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added variable and accessor methods for filterChanged. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Aug 2014 12:10:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised Release method to remove the shared memory file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Aug 2014 17:35:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised constructor to call SVSharedConfiguration::GetControlShareName method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Aug 2014 17:07:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/