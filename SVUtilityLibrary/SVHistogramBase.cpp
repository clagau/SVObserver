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

namespace Seidenader { namespace SVUtilityLibrary {

	SVHistogramBase::SVHistogramBase():
		scale(1.0),
		m_scale(histogram::linear),
		m_axis(histogram::percent_all),
		m_colors(histogram::config),
		m_clip(histogram::no_clip),
		m_disp(histogram::normal),
		m_placement(histogram::elsewhere),
		m_disabled(0),
		m_start(0),
		m_end(256),
		m_clipCount(256),
		m_lowPeak(0),
		m_highPeak(0),
		valley_low(0),
		valley_high(255),
		valley_default(0),
		needs_repaint(true)
	{
		m_func = histogram::Scaler<histogram::linear>();
		m_values.resize(256);
		m_accumulated.resize(256);
	}

	SVHistogramBase::~SVHistogramBase()
	{
	}
}}

