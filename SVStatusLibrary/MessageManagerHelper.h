//*****************************************************************************
/// \copyright (c) 2016,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file MessageManagerHelper.h
//*****************************************************************************

#pragma once
#include "SVStatusLibrary/SourceFileParams.h"
#include "MessageTextEnum.h"
#include <source_location>
#include "Definitions/StringTypeDef.h"

namespace SvStl
{

enum class Severity
{
	Success, Informational, Warning, Error, Assert
};
class  MessageManagerHelper
{
public:
	static void LogError(LPCSTR errortext, const SvStl::SourceFileParams& rFileParams, Severity sev);
	static void  LogError(SvStl::MessageTextEnum messageId, const SvDef::StringVector& additionalList, Severity sev, const std::source_location location);
};


} //namespace SvStl

inline void Log_Error(SvStl::MessageTextEnum messageId, const SvDef::StringVector& additionalList = SvDef::StringVector(), const std::source_location location = std::source_location::current())
{
	SvStl::MessageManagerHelper::LogError(messageId, additionalList, SvStl::Severity::Error, location);
}
inline void Log_Info(SvStl::MessageTextEnum messageId, const SvDef::StringVector& additionalList = SvDef::StringVector(), const std::source_location location = std::source_location::current())
{
	SvStl::MessageManagerHelper::LogError(messageId, additionalList, SvStl::Severity::Informational, location);
}


#define Log_Assert(a) {  if (!(a))\
					{ SvStl::MessageManagerHelper::LogError(#a, SvStl::SourceFileParams(StdMessageParams), SvStl::Severity::Assert); assert(a);}}

