//*****************************************************************************
/// \copyright (c) 2018,2018 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes

void MesManHelper::ThrowCreateFileMappingFailed(LPCSTR MapFileName, const SvStl::SourceFileParams& rFileParams)
{
	std::string LastError = std::format(_T("{} LastError:  {}"), MapFileName, GetLastError());
	SvDef::StringVector msgList;
	msgList.push_back(LastError);
	SvStl::MessageManager MesMan(SvStl::MsgType::Log);
	MesMan.setMessage(SVMSG_SVO_5080_CREATEFILEMAPPINGFAILED, SvStl::Tid_Default, msgList, rFileParams);
	MesMan.Throw();
}
void MesManHelper::ThrowMapViewOfFileFailed(LPCSTR MapFileName, const SvStl::SourceFileParams& rFileParams)
{
	std::string LastError = std::format(_T("{} LastError:  {}"), MapFileName, GetLastError());
	SvDef::StringVector msgList;
	msgList.push_back(LastError);

	SvStl::MessageManager MesMan(SvStl::MsgType::Log);
	MesMan.setMessage(SVMSG_SVO_5081_MAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, rFileParams);
	MesMan.Throw();
}

void MesManHelper::LogUnMapViewOfFileFailed(LPCSTR MapFileName, const SvStl::SourceFileParams& rFileParams)
{
	std::string LastError = std::format(_T("{} LastError:  {}"), MapFileName, GetLastError());
	SvDef::StringVector msgList;
	msgList.push_back(LastError);
	SvStl::MessageManager MesMan(SvStl::MsgType::Log);
	MesMan.setMessage(SVMSG_SVO_5082_UNMAPVIEWOFFileFAILED, SvStl::Tid_Default, msgList, rFileParams);
}