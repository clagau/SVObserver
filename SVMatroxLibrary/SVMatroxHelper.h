//*****************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// This file contains helper function for this library
//*****************************************************************************
#pragma once

#include "SVMatroxIdentifierEnum.h"

/// Destroy an matrox handle with a defined method.
/// \param rId [in] ID of the handle.
/// \param pFreeFunc [in] function pointer to the matrox free function (e.g. MblobFree).
/// \param identifierType [in] Identifer to remove the entry from the debug class SVMatroxResourceMonitor.
/// \returns HRESULT
HRESULT DestroyMatroxId(__int64& rId, void(*pFreeFunc)(MIL_ID), SVMatroxIdentifierEnum identifierType);