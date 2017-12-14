//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ShareControl.h
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#pragma once
#include "SVSharedMemoryLibrary\SharedMemReader.h"
#include "LastResponseData.h"
class CShareControl
{
public:
	CShareControl(CLastResponseData* pResponseData);
	~CShareControl();
	bool EventHandler(DWORD event);
	
	SvSml::SharedMemReader   m_MemReader;
	CLastResponseData*		 m_pResponseData = nullptr;
};


