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
//!LastResponseData holds the data of the last response 
struct LastResponseData
{
public:
	LastResponseData(){}
	~LastResponseData(){}
	
	void ClearHeld()
	{
		m_LastFailstatus.clear();
		m_LastReject.clear();;
		m_LastProduct.clear();
	}
	std::map<std::string, SvSml::productPointer> m_LastProduct;
	std::map<std::string, SvSml::productPointer> m_LastReject;
	std::map<std::string, SvSml::FailstatusPointer> m_LastFailstatus;
};


