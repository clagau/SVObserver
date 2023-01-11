//*****************************************************************************
/// \copyright (c) 2016,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file MessageManagerHelper.h
//*****************************************************************************

#pragma once
#include "SVStatusLibrary/SourceFileParams.h"

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
#define Log_Error(text) SvStl::MessageManagerHelper::LogError(text, SvStl::SourceFileParams(StdMessageParams),SvStl::Severity::Error);
#define Log_Info(text) SvStl::MessageManagerHelper::LogError(text, SvStl::SourceFileParams(StdMessageParams),SvStl::Severity::Informational);
#define Log_Assert(a) {  if (!(a))\
					{ SvStl::MessageManagerHelper::LogError(#a, SvStl::SourceFileParams(StdMessageParams), SvStl::Severity::Assert); assert(a);}}
