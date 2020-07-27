//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/07/22,2020/07/22 by Seidenader Maschinenbau GmbH. All Rights Reserved 
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
	};

}