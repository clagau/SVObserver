//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file Eventlog.h
/// All Rights Reserved 
//*****************************************************************************
// Notification Handler for status changes
//******************************************************************************

#pragma once


namespace SvRc
{
void SVLog(HRESULT hr, LPCTSTR file, long line);
void SVLog(LPCTSTR msg, LPCTSTR file, long line);
} //namespace SvRc
