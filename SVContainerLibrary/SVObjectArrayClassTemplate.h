//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectArrayClassTemplate
//* .File Name       : $Workfile:   SVObjectArrayClassTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:27:02  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

namespace SvCl
{
	typedef std::vector<POINT> SVObjectPointArrayClass;
	typedef std::vector<BOOL> SVObjectBoolArrayClass;
	typedef std::vector<BYTE> SVObjectByteArrayClass;
	typedef std::vector<char> SVObjectCharArrayClass;
	typedef std::vector<long> SVObjectLongArrayClass;
	typedef std::vector<double> SVObjectDoubleArrayClass;
	typedef std::vector<DWORD> SVObjectDWordArrayClass;
	typedef std::vector<__int64> SVObjectInt64ArrayClass;
	typedef std::vector<_variant_t> SVObjectVariantArrayClass;
	typedef std::vector<std::string> SVObjectStdStringArrayClass;
} //namespace SvCl
