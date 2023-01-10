//*****************************************************************************
/// \copyright (c) 2016,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file MessageManagerHelper.h
//*****************************************************************************

#pragma once
#include "SVStatusLibrary/SourceFileParams.h"

namespace SvStl
{


class  MessageManagerHelper
{

public:
	static void LogError(LPCSTR errortext, const SvStl::SourceFileParams& rFileParams, int level = 0);
};


} //namespace SvStl
#define Log_Error(text) SvStl::MessageManagerHelper::LogError(text, SvStl::SourceFileParams(StdMessageParams),0);
#define Log_Info(text) SvStl::MessageManagerHelper::LogError(text, SvStl::SourceFileParams(StdMessageParams),2);
#define Log_Assert(a) {  if (!(a))\
					{ SvStl::MessageManagerHelper::LogError(#a, SvStl::SourceFileParams(StdMessageParams), 3); assert(a);}}
