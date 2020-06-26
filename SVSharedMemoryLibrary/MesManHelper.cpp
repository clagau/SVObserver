//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file MesManHelper.cpp
/// All Rights Reserved 
//*****************************************************************************
/// File with messages used in several places in SharedMemoryLib
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "MesManHelper.h"
#include "Definitions/StringTypeDef.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SourceFileParams.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

void MesManHelper::ThrowCreateFileMappingFailed(LPCSTR MapFileName, const SvStl::SourceFileParams& rFileParams, DWORD Programmcode)
{
	std::string LastError = SvUl::Format(_T("%s LastError:  %i"), MapFileName, GetLastError());
	SvDef::StringVector msgList;
	msgList.push_back(LastError);
	SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log);
	MesMan.setMessage(SVMSG_SVO_5080_CREATEFILEMAPPINGFAILED, SvStl::Tid_Default, msgList, rFileParams, Programmcode);
	MesMan.Throw();
}
void MesManHelper::ThrowMapViewOfFileFailed(LPCSTR MapFileName, const SvStl::SourceFileParams& rFileParams, DWORD Programmcode)
{
	std::string LastError = SvUl::Format(_T("%s LastError:  %i"), MapFileName, GetLastError());
	SvDef::StringVector msgList;
	msgList.push_back(LastError);

	SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log);
	MesMan.setMessage(SVMSG_SVO_5081_MAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, rFileParams, Programmcode);
	MesMan.Throw();
}

void MesManHelper::LogUnMapViewOfFileFailed(LPCSTR MapFileName, const SvStl::SourceFileParams& rFileParams, DWORD Programmcode)
{
	std::string LastError = SvUl::Format(_T("%s LastError:  %i"), MapFileName, GetLastError());
	SvDef::StringVector msgList;
	msgList.push_back(LastError);
	SvStl::MessageMgrStd MesMan(SvStl::MsgType::Log);
	MesMan.setMessage(SVMSG_SVO_5082_UNMAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, rFileParams, Programmcode);
}