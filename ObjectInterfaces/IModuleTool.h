//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface for ModuleTool
//******************************************************************************

#pragma once
#pragma region Includes
#pragma endregion Includes



#pragma region Declarations
#pragma endregion Declarations

namespace SvOi
{
///  Set the history to the moduleTool.
/// \param objectID [in]
/// \param rHistoryVector [in]
/// \returns bool return if set was successfully.
bool setHistory(uint32_t objectID, const std::vector<std::pair<time_t, std::string>>& rHistoryVector);
}