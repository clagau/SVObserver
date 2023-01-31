//*****************************************************************************
/// \copyright (c) 2016,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file MessageManagerHelper.h
//*****************************************************************************

#pragma once
#include "SVStatusLibrary/SourceFileParams.h"
#include "MessageTextEnum.h"
#include "MessageTextGenerator.h"

namespace SvStl
{

enum class Severity
{
	Success, Informational, Warning, Error,Assert
};
class  MessageManagerHelper
{
	
		
public:
	static void LogError(LPCSTR errortext, const SvStl::SourceFileParams& rFileParams, Severity sev = Severity::Error);
};


} //namespace SvStl



#define Log_Assert(a) {  if (!(a))\
					{ SvStl::MessageManagerHelper::LogError(#a, SvStl::SourceFileParams(StdMessageParams), SvStl::Severity::Assert); assert(a);}}

inline void Log_Error(SvStl::MessageTextEnum messageId, const SvDef::StringVector& additionalList = SvDef::StringVector())
{
	std::string text = SvStl::MessageTextGenerator::Instance().getText(messageId, additionalList);
	SvStl::MessageManagerHelper::LogError(text.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Severity::Error);
	return;
}
inline void Log_Info(SvStl::MessageTextEnum messageId, const SvDef::StringVector& additionalList = SvDef::StringVector())
{
	std::string text = SvStl::MessageTextGenerator::Instance().getText(messageId, additionalList);
	SvStl::MessageManagerHelper::LogError(text.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Severity::Informational);
	return;
}