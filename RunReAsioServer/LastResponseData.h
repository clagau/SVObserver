//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file LastResponseData.h
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************

#pragma once

#include "SVSharedMemoryLibrary\ShareEvents.h"
#include "SVSharedMemoryLibrary\MLProduct.h"
class CLastResponseData
{
public:
	CLastResponseData(){}
	~CLastResponseData(){}
	
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


