//*****************************************************************************
/// \copyright COPYRIGHT (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file AcquisitionName.h
/// \brief Generates the acquisition device name
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

constexpr const char* cFileAcquisitionName(_T("File"));
constexpr const char* cMatroxGigeName(_T("Matrox_GIGE"));
constexpr const char* cSvimDigName(_T(".Dig_"));


namespace SvUl
{
inline std::string getAcquisitionName(int digitizerNumber, bool isFileAcq)
{
	std::string Result;

	if (isFileAcq)
	{
		Result = cFileAcquisitionName;
		Result += cSvimDigName + std::to_string(digitizerNumber);
	}
	else
	{
		Result = cMatroxGigeName;
		Result += cSvimDigName + std::to_string(digitizerNumber);
	}
	return Result;
}
} //namespace SvUl
