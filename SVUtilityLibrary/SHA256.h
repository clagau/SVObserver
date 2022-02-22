//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/10/15,2020/10/15 by Körber Pharma Inspection GmbH. All Rights Reserved 
/// \file HashUtil.h
///  calculate hashcode for the file
/// //******************************************************************************

#pragma once


namespace SvUl
{
	std::string SHA256(const char* const path, double* pDuration=nullptr );

}