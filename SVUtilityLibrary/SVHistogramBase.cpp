//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogramBase
//* .File Name       : $Workfile:   SVHistogramBase.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:46:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVHistogramBase.h"

namespace SvUl
{

	SVHistogramBase::SVHistogramBase()
	{
		m_func = histogram::Scaler<histogram::linear>();
		m_values.resize(VectorCount);
		m_accumulated.resize(VectorCount);
	}

	SVHistogramBase::~SVHistogramBase()
	{
	}
} // namespace SvUl

