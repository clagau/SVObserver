//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once
namespace SvTo
{

enum SVArchiveMethodEnum
{
	SVArchiveInvalidMethod = 0,
	SVArchiveSynchronous   = 1,
	SVArchiveAsynchronous  = 2,
	SVArchiveGoOffline     = 3,
};

bool memoryNeedsToBeConsidered(SVArchiveMethodEnum mode);

} //namespace SvTo
