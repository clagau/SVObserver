//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file MesManHelper.cpp
/// All Rights Reserved 
//*****************************************************************************
/// File with messages used in several places in SharedMemoryLib
//******************************************************************************
#include "stdafx.h"
#include "MesManHelper.h"
#include "Definitions\StringTypeDef.h"
#include "SVUtilityLibrary\SVMetrics.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\StringHelper.h"

void MesManHelper::ThrowCreateFileMappingFailed(LPCSTR MapFileName, SvStl::SourceFileParams& FileParams, DWORD Programmcode)
{
	std::string LastError = SvUl::Format(_T("%s LastError:  %i"), MapFileName, GetLastError());
	SvDef::StringVector msgList;
	msgList.push_back(LastError);
	SvStl::MessageMgrStd MesMan(SvStl::LogOnly);
	MesMan.setMessage(SVMSG_SVO_5080_CREATEFILEMAPPINGFAILED, SvStl::Tid_Default, msgList, FileParams, Programmcode);
	MesMan.Throw();
}
void MesManHelper::ThrowMapViewOfFileFailed(LPCSTR MapFileName, SvStl::SourceFileParams& FileParams, DWORD Programmcode)
{
	std::string LastError = SvUl::Format(_T("%s LastError:  %i"), MapFileName, GetLastError());
	SvDef::StringVector msgList;
	msgList.push_back(LastError);

	SvStl::MessageMgrStd MesMan(SvStl::LogOnly);
	MesMan.setMessage(SVMSG_SVO_5081_MAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, FileParams, Programmcode);
	MesMan.Throw();
}

void MesManHelper::LogUnMapViewOfFileFailed(LPCSTR MapFileName, SvStl::SourceFileParams& FileParams, DWORD Programmcode)
{
	std::string LastError = SvUl::Format(_T("%s LastError:  %i"), MapFileName, GetLastError());
	SvDef::StringVector msgList;
	msgList.push_back(LastError);
	SvStl::MessageMgrStd MesMan(SvStl::LogOnly);
	MesMan.setMessage(SVMSG_SVO_5082_UNMAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, FileParams, Programmcode);
}