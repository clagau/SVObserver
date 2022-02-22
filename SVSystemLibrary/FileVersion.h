//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/07/22,2020/07/22 by Körber Pharma Inspection GmbH. All Rights Reserved 
/// \file FileVersion.h
/// DESCRIPTION
//******************************************************************************

#pragma once

namespace SvSyl
{


	class FileVersion
	{
	public:
		static bool getFileInfo(VS_FIXEDFILEINFO& rFileInfo, LPCTSTR Filename);
		static bool isEqual(const VS_FIXEDFILEINFO& rFileInfoA, const VS_FIXEDFILEINFO& rFileInfoB);
		static bool  isEqual(const std::string& rFilePathA, const std::string& rFilePathB);
	};

}