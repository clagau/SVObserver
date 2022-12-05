//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2022 by Körber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved
/// \file ArchiveMethodEnum.h
//*****************************************************************************

#pragma once
namespace SvDef
{

enum class ArchiveMode
{
	invalid			= 0,
	synchronous		= 1,
	asynchronous	= 2,
	goOffline		= 3,
};

inline bool memoryNeedsToBeConsidered(ArchiveMode mode)
{
	return mode == ArchiveMode::goOffline || mode == ArchiveMode::asynchronous;
}

} //namespace SvDef
