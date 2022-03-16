//******************************************************************************
/// \copyright (c) 2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ArchiveDataAsynchron.cpp
/// All Rights Reserved
//******************************************************************************

#include "stdafx.h"
#include "ArchiveDataAsynchron.h"
#include "SVStatusLibrary/MessageManager.h"
#include <vcruntime_exception.h>
#include "SVMessage/SVMessage.h"

namespace SvTo
{

const size_t ArchiveDataAsynchron::MAXQUEUESIZE = 50000;


void ArchiveDataAsynchron::ArchiveData::writeToFile()
{
	if (m_pfileArchive && m_pfileArchive->is_open())
	{
		try
		{
			m_pfileArchive->write(m_entry.c_str(), m_entry.size());
		}
		catch (std::exception& e)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			SvDef::StringVector msgList;
			msgList.push_back(e.what());
			Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorWrite, msgList, SvStl::SourceFileParams(StdMessageParams));
		}

	}
}
ArchiveDataAsynchron& ArchiveDataAsynchron::Instance()
{
	static ArchiveDataAsynchron inst;
	return inst;
}
ArchiveDataAsynchron::~ArchiveDataAsynchron()
{
	GoOffline();
}
ArchiveDataAsynchron::ArchiveDataAsynchron()
{
	;
}

bool ArchiveDataAsynchron::GoOnline()
{
	m_Finish_Thread = false;
	if (m_thread.get_id() == std::thread().get_id())
	{
		m_ArchiveData_queue.clear();
		m_thread = std::thread([this](){ThreadFunction(); });
	}
	return true;
}
bool ArchiveDataAsynchron::GoOffline()
{
	m_Finish_Thread = true;
	m_data_cond.notify_one();
	if (m_thread.joinable())
	{
		m_thread.join();
	}
	return true;

}

bool ArchiveDataAsynchron::QueueData(const ArchiveData& data)
{
	std::lock_guard<std::mutex> lk(m_mutex);
	if (m_ArchiveData_queue.size() > MAXQUEUESIZE)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorArchiveQueueFull,  SvStl::SourceFileParams(StdMessageParams));
		return false;
	}
	
	m_ArchiveData_queue.push_back(data);
	m_data_cond.notify_one();
	return true;
}

bool ArchiveDataAsynchron::QueueData(ArchiveData&& data)
{
	std::lock_guard<std::mutex> lk(m_mutex);
	if (m_ArchiveData_queue.size() > MAXQUEUESIZE)
	{
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
	
		Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorArchiveQueueFull, SvStl::SourceFileParams(StdMessageParams));
		return false;
	}

	
	m_ArchiveData_queue.push_back(std::move(data));
	m_data_cond.notify_one();
	return true;
}

void ArchiveDataAsynchron::ThreadFunction()
{
	while (true)
	{
		ArchiveData data;
		bool newdata = false;
		{
			std::unique_lock<std::mutex> lk(m_mutex);
			m_data_cond.wait(lk, [this]{return !m_ArchiveData_queue.empty() || m_Finish_Thread; });
			
			if (!m_ArchiveData_queue.empty())
			{
				newdata = true;
				data = m_ArchiveData_queue.front();
				m_ArchiveData_queue.pop_front();
			}
			lk.unlock();
		}

		if (newdata)
		{
			data.writeToFile();
		}
		while (true)
		{
			{
				std::lock_guard<std::mutex> lkg(m_mutex);
				if (!m_ArchiveData_queue.empty())
				{
					data = m_ArchiveData_queue.front();
					m_ArchiveData_queue.pop_front();
				}
				else
				{
					break;
				}
			}
			data.writeToFile();
		}
		if (m_Finish_Thread == true)
		{
			break;
		}

	}
	return;
}
}