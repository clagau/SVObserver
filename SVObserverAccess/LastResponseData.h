//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file LastResponseData.h
/// All Rights Reserved 
//*****************************************************************************
///CLastResponseData holds the data of the last response 
//******************************************************************************

#pragma once

#include "SVSharedMemoryLibrary\ShareEvents.h"
#include "SVSharedMemoryLibrary\MLProduct.h"
#include <mutex>
//!LastResponseData holds the data of the last response 
struct LastResponseData
{
public:
	LastResponseData(){}
	~LastResponseData(){}
	
	void ClearHeld()
	{
		{
			std::lock_guard<std::mutex> guard(m_ProtectLastFailstatus);
			m_LastFailstatus.clear();
		}
		{
			std::lock_guard<std::mutex> guard(m_ProtectLastReject);
			m_LastReject.clear();;
		}
		{
			std::lock_guard<std::mutex> guard(m_ProtectLastProduct);
			m_LastProduct.clear();
		}
	}
	std::mutex m_ProtectLastProduct;
	std::mutex m_ProtectLastReject;
	std::mutex m_ProtectLastFailstatus;
	
	std::map<std::string, SvSml::pProd> m_LastProduct;
	std::map<std::string, SvSml::pProd> m_LastReject;
	std::map<std::string, std::unique_ptr<SvSml::vecpProd>> m_LastFailstatus;
};


