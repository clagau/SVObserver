//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file MesManHelper.h
/// All Rights Reserved 
//*****************************************************************************
/// File with messages used in several places in SharedMemoryLib
//******************************************************************************
#pragma region Includes
#include "SVStatusLibrary\SourceFileParams.h"
#pragma endregion Includes
#pragma once
class MesManHelper
{
public:
	static void ThrowCreateFileMappingFailed(LPCSTR MapfileName, SvStl::SourceFileParams& FileParams, DWORD Programmcode);
	static void ThrowMapViewOfFileFailed(LPCSTR MapfileName, SvStl::SourceFileParams& FileParams, DWORD Programmcode);
	static void LogUnMapViewOfFileFailed(LPCSTR MapfileName, SvStl::SourceFileParams& FileParams, DWORD Programmcode);
};

