//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProductFilterEnum
//* .File Name       : $Workfile:   SVProductFilterEnum.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   28 Aug 2014 18:33:50  $
//******************************************************************************
#pragma once

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	enum SVProductFilterEnum
	{
		LastInspectedFilter = 1,
		LastRejectFilter = 2
	};

} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;
