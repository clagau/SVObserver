//*****************************************************************************
/// \copyright (c) 2017,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SVMatroxHelper.h
//*****************************************************************************
/// This file contains helper function for this library
//*****************************************************************************
#pragma once

#include "SVMatroxIdentifierEnum.h"
#include "Definitions/SVMatroxSimpleEnums.h"

/// Destroys a matrox handle with a defined method.
/// \param rId [in] ID of the handle.
/// \param pFreeFunc [in] function pointer to the matrox free function (e.g. MblobFree).
/// \param identifierType [in] Identifer to remove the entry from the debug class SVMatroxResourceMonitor.
/// \returns HRESULT
HRESULT DestroyMatroxId(__int64& rId, void(*pFreeFunc)(MIL_ID), SVMatroxIdentifierEnum identifierType);

[[nodiscard]] ImageFileFormat inferMilImageFileFormat(const TCHAR* pFilepath);
[[nodiscard]] ImageFileFormat inferMilImageFileFormat(const std::string& filenameOrPath);
MIL_INT64 getMilImageFormatCode(ImageFileFormat fileFormat);
std::string imageFileNameExtension(ImageFileFormat fileFormat);



