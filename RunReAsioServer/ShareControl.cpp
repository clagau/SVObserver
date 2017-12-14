//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ShareControl.cpp
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#include  "StdAfx.h"
#include "ShareControl.h"
#include "SVSharedMemoryLibrary\ShareEvents.h"
#include "LastResponseData.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "Definitions\StringTypeDef.h"
CShareControl::CShareControl(CLastResponseData* pResponseData)
{
	m_pResponseData = pResponseData;
}
CShareControl::~CShareControl()
{
	
}

bool CShareControl::EventHandler(DWORD event)
{
	bool res(false);
	switch (event)
	{
	case SvSml::ShareEvents::Change:
		try
		{
			Sleep(SvSml::ShareEvents::Delay_Before_ClearShare);
			m_MemReader.Clear();
			res = true;
		}
		catch (std::exception& rExp)
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			SvDef::StringVector msgList;
			msgList.push_back(rExp.what());
			Exception.setMessage(SVMSG_RRS_2_STD_EXCEPTION, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
		break;
	case SvSml::ShareEvents::Ready:
	{
		try
		{
			DWORD version = SvSml::ShareEvents::GetInstance().GetReadyCounter();
			m_MemReader.Reload(version);
			if (m_pResponseData)
				m_pResponseData->ClearHeld();
			res = true;
		}
		catch (std::exception& rExp)
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			SvDef::StringVector msgList;
			msgList.push_back(rExp.what());
			Exception.setMessage(SVMSG_RRS_2_STD_EXCEPTION, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
		break;
	}
	default:
		break;
	}
	return res;
}
