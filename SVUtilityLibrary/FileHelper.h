//*****************************************************************************
/// \copyright COPYRIGHT (c) 2022,2022 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file FileHelper.h
/// \brief 
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvUl
{
std::vector<std::string> getFileList(LPCTSTR pPath, LPCTSTR pExtension, bool recursive);
} //namespace SvUl
