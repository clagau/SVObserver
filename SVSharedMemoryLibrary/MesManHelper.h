//*****************************************************************************
/// \copyright (c) 2018,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	static void ThrowCreateFileMappingFailed(LPCSTR MapfileName, const SvStl::SourceFileParams& rFileParams, DWORD Programmcode);
	static void ThrowMapViewOfFileFailed(LPCSTR MapfileName, const SvStl::SourceFileParams& rFileParams, DWORD Programmcode);
	static void LogUnMapViewOfFileFailed(LPCSTR MapfileName, const SvStl::SourceFileParams& rFileParams, DWORD Programmcode);
};

