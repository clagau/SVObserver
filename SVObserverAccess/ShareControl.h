//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ShareControl.h
/// All Rights Reserved 
//*****************************************************************************
/// ShareControl implements the connection to the shared memory 
//******************************************************************************
#pragma once
#include "SVSharedMemoryLibrary\SharedMemReader.h"
#include "LastResponseData.h"
//! ShareControl implements the connection to the shared memory 
class ShareControl
{
public:
	ShareControl(LastResponseData* pResponseData);
	~ShareControl();
	bool EventHandler(DWORD event);
	
	SvSml::SharedMemReader   m_MemReader;
	LastResponseData*		 m_pResponseData = nullptr;
};


