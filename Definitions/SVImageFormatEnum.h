//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SvDef::SVImageFormatEnum
//* .File Name       : $Workfile:   SVImageFormatEnum.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:11:24  $
//******************************************************************************

#pragma once

namespace SvDef
{

	enum SVImageFormatEnum           // Matrox Image Format Definitions
	{
		SVImageFormatUnknown = 0,
		SVImageFormatMono8 = 10,  // default format
		SVImageFormatBGR888 = 30,  // M_BGR24
		SVImageFormatBGR888X = 35,  // M_BGR32
	};
} //namespace SvDef