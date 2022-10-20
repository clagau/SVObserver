//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2022 by Körber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved
/// \file ArchiveMethodEnum.h
//*****************************************************************************

#pragma once
namespace SvTo
{

enum class ArchiveMode
{
	invalid			= 0,
	synchronous		= 1,
	asynchronous	= 2,
	goOffline		= 3,
};


enum class ImageFileFormat
{
	invalid = 0,
	bmp		= 1,
	png		= 2,
};

} //namespace SvTo
