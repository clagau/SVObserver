//*****************************************************************************
/// \copyright (c) 2018,2018 by Igor Pavlov
/// \file IProgress.h
/// All Rights Reserved 
/// Author           : Igor Pavlov
/// License          : Public Domain License
/// From             : http://www.7-zip.de/sdk.html
//*****************************************************************************
/// 7z progress interface
//******************************************************************************

#pragma once
#include "7zTypes.h"
#include "IDecl.h"

#define INTERFACE_IProgress(x) \
  STDMETHOD(SetTotal)(UInt64 total) x; \
  STDMETHOD(SetCompleted)(const UInt64 *completeValue) x; \

DECL_INTERFACE(IProgress, 0, 5)
{
  INTERFACE_IProgress(PURE)
};
