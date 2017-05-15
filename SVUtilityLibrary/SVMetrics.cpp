//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMetrics
//* .File Name       : $Workfile:   SVMetrics.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jun 2014 10:26:14  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <limits>
#include "SVMetrics.h"
#pragma endregion Includes

namespace SvUl
{
	SVMetrics::SVMetrics()
	{
		Clear();
	}

	void SVMetrics::Clear()
	{
		minTime = std::numeric_limits<double>::max();
		maxTime = 0.0;
		avgTime = 0.0;
		lastTime = 0.0;
	}

	void SVMetrics::Update(double elapsed)
	{
		bool bAvg = true;

		lastTime = elapsed;

		if (elapsed > maxTime) { maxTime = elapsed; }

		if (minTime == std::numeric_limits<double>::max())
		{
			minTime = elapsed;
			bAvg = false;
		}
		else if (elapsed < minTime)
		{
			minTime = elapsed;
		}

		avgTime += elapsed;

		if (bAvg) { avgTime /= 2.0; }
	}
} // namespace SvUl
