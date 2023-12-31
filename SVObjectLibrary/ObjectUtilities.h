//*****************************************************************************
/// \copyright (c) 2022,2022 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
/// \file ObjectUtilities.h
//*****************************************************************************
/// This file contains free utility functions for object classes.
//******************************************************************************
#pragma once

namespace SvOl
{
std::string renameFormulaTextAfterRenameObject(const std::string& rFormula, const SVObjectClass& rRenamedObject, const std::string& rOldName);
}
