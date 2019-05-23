//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file MesManHelper.h
/// All Rights Reserved 
//*****************************************************************************
/// File with messages used in several places in SharedMemoryLib
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvStl
{
struct SourceFileParams;
}

class MesManHelper
{
public:
	static void ThrowCreateFileMappingFailed(LPCSTR MapfileName, SvStl::SourceFileParams& FileParams, DWORD Programmcode);
	static void ThrowMapViewOfFileFailed(LPCSTR MapfileName, SvStl::SourceFileParams& FileParams, DWORD Programmcode);
	static void LogUnMapViewOfFileFailed(LPCSTR MapfileName, SvStl::SourceFileParams& FileParams, DWORD Programmcode);
};

