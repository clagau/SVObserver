//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogramBase
//* .File Name       : $Workfile:   SVHistogramBase.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVHistogramBase.h"
#pragma endregion Includes

namespace SvUl
{
// convert position of the bar into its screen rectangle
RECT SVHistogramBase::Pos2Screen(int pos)
{
	RECT result(m_client);
	result.left = static_cast<int>((pos-m_start) * m_ratio);
	result.right = static_cast<int>((1+pos-m_start) * m_ratio);
	return result;
}

// convert screen x-coordinate into bar position
int SVHistogramBase::Screen2Pos(int x)
{
	x = ( x > 0) ? x : 0;

	int width = std::abs(m_client.right-m_client.left);
	x = x > width ? width : x;

	return static_cast<int> (x / m_ratio) + m_start;
}

bool SVHistogramBase::SetDisplay(histogram::disp_options d)
{
	if (m_disp == d)
	{
		return false;
	}
	m_disp = d;
	return true;
}

bool SVHistogramBase::SetScale(histogram::scale s)
{
	if (m_scale == s)
		return false;
	m_scale = s;
	switch (s)
	{
	case histogram::linear:
		m_func = histogram::Scaler<histogram::linear>();
		needs_repaint = true;
		return true;
	case histogram::logarithmic:
		m_func = histogram::Scaler<histogram::logarithmic>();
		needs_repaint = true;
		return true;
	default:
		m_func = histogram::Scaler<histogram::linear>();
		m_scale = histogram::linear;
		return false;
	}
}

#pragma warning (push)
#pragma warning (disable : 5054)
unsigned int SVHistogramBase::GetOptions() const
{
	return static_cast<unsigned int >(m_scale | m_axis | m_colors | m_clip | m_disp | m_placement);
}
#pragma warning (pop)

bool SVHistogramBase::Clip(int left, int right)
{
	if (left == no_clip)
		left = m_start;
	if (right == no_clip)
		right = m_end - 1;
	if (0 > left || left > right || right > MaxIndex || (m_start == left && m_end == right + 1))
		return false;
	m_start = left;
	m_end = right + 1;
	m_clipCount = m_end - m_start;
	return true;
}

void SVHistogramBase::CalcStats()
{
	Stats l_acc(m_start, peak_threshold);
	for (int i = m_start; i < m_end; ++i)
		l_acc += m_values[i];
	m_stats = l_acc;
	m_variance = m_stats.variance();
	m_std_dev = sqrt(m_variance);
	min_px = m_stats.min_px();
	max_px = m_stats.max_px();
	m_highPeak = m_stats.high_peak();
	m_lowPeak = m_stats.low_peak();
}

void SVHistogramBase::SetPixelCounts(std::vector<long>::const_iterator first, std::vector<long>::const_iterator last)
{
	assert(last - first == VectorCount);
	std::vector<int>::iterator dest = m_values.begin(), dest2 = m_accumulated.begin();
	m_max = m_total = 0;
	for (std::vector<long>::const_iterator it = first; it != last; ++it, ++dest, ++dest2)
	{
		if (*it > m_max) m_max = *it;
		m_total += *it;
		*dest = *it;
		*dest2 = m_total;
	}
	if (!m_total)
		m_total = 1;
	needs_repaint = true;
}

void SVHistogramBase::SetOptions(unsigned opt)
{
	m_colors = static_cast<histogram::colors>(opt & histogram::color_mask);
	m_scale  = static_cast<histogram::scale>(opt & histogram::scale_mask);
	m_axis   = static_cast<histogram::v_axis>(opt & histogram::axis_mask);
	m_clip   = static_cast<histogram::clip_options>(opt & histogram::clip_mask);
	m_disp   = static_cast<histogram::disp_options>(opt & histogram::disp_mask);
	m_placement = static_cast<histogram::placement>(opt & histogram::placement_mask);
}

int SVHistogramBase::valley() const
{
	int l_valley = static_cast<int>(m_stats.valley(&m_values[m_highPeak], &m_values[m_lowPeak]) - &m_values[0]);
	return (valley_low <= l_valley && l_valley <= valley_high)?l_valley:valley_default;
}

void SVHistogramBase::DrawHistogram(drawFunctor draw)
{
	int width = std::abs(m_client.right - m_client.left);
	m_ratio = (static_cast<double>(width) / m_clipCount) + .0001;
	if (m_height == histogram::fixed)
	{
		m_factor = (m_disp == histogram::normal)?fixed_height/100.0:1.0;
		scale = 1.0;
	}
	else
	{
		m_factor = static_cast<double>(m_max)/m_total;
	}
	double l_max = (m_disp == histogram::normal)?((m_height == histogram::fixed)?m_factor*m_total:m_max):m_total;
	int bottom = std::abs(m_client.top - m_client.bottom);
	std::vector<int>& l_values = (m_disp == histogram::normal) ? m_values : m_accumulated;
	for (int i = m_start; i < m_end; ++i)
	{
		double val = m_func(100.0*l_values[i]/l_max);
		int height = static_cast<int>(scale * ((val * bottom) / m_func(100.0) ));

		int top = (bottom - height < 0) ? 0 : bottom - height;

		if (top < bottom)
		{
			RECT l_bar = Pos2Screen(i);
			l_bar.top = top;
			draw(l_bar, m_colors ? i : 0);
		}
	}
}

bool SVHistogramBase::SetProperty(histogram::height_options & prop, histogram::height_options opt)
{
	if (prop != opt)
	{
		prop = opt;
		return true;
	}
	return false;
}

} // namespace SvUl
