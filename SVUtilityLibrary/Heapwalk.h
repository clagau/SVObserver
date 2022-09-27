//*****************************************************************************
/// \copyright (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file Heapwalk.h
//*****************************************************************************
// Functions for logging heap information
//******************************************************************************

#pragma once

#ifdef LOG_HEAP_INFO
namespace SvUl
{
void startHeaplogs();
void endHeaplogs();
void logHeap(const std::string& rTitle);
} // namespace SvUl
#endif 

