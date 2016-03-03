//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogramBase
//* .File Name       : $Workfile:   SVHistogramBase.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:30  $
//******************************************************************************

// convert position of the bar into its screen rectangle
inline CRect SVHistogramBase::Pos2Screen(int pos)
{
	CRect retval(m_client);
	retval.left = static_cast<int>((pos-m_start) * m_ratio);
	retval.right = static_cast<int>((1+pos-m_start) * m_ratio);
	return retval;
}

// convert screen x-coordinate into bar position
inline int SVHistogramBase::Screen2Pos(int x)
{
	if ( x < 0) x = 0;
	if ( x > m_client.Width()) x = m_client.Width();
	return static_cast<int>(x / m_ratio) + m_start;
}

inline bool SVHistogramBase::SetDisplay(histogram::disp_options d)
{
	if (m_disp == d) 
		return false;
	m_disp = d;
	return true;
}

inline bool SVHistogramBase::SetScale(histogram::scale s)
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

inline unsigned SVHistogramBase::GetOptions() const
{
	return m_scale | m_axis | m_colors | m_clip | m_disp | m_placement;
}

inline bool SVHistogramBase::Clip(int left, int right)
{
	if (left == no_clip)
		left = m_start;
	if (right == no_clip)
		right = m_end - 1;
	if (0 > left || left > right || right > 255 || (m_start == left && m_end == right + 1))
		return false;
	m_start = left;
	m_end = right + 1;
	m_clipCount = m_end - m_start;
	return true;
}

inline void SVHistogramBase::CalcStats()
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

template<typename Iter>
inline void SVHistogramBase::SetPixelCounts(Iter first, Iter last)
{
	assert(last - first == 256);
	std::vector<int>::iterator dest = m_values.begin(), dest2 = m_accumulated.begin();
	m_max = m_total = 0;
	for (Iter it = first; it != last; ++it, ++dest, ++dest2)
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

inline void SVHistogramBase::SetOptions(unsigned opt)
{
	m_colors = static_cast<histogram::colors>(opt & histogram::color_mask);
	m_scale  = static_cast<histogram::scale>(opt & histogram::scale_mask);
	m_axis   = static_cast<histogram::v_axis>(opt & histogram::axis_mask);
	m_clip   = static_cast<histogram::clip_options>(opt & histogram::clip_mask);
	m_disp   = static_cast<histogram::disp_options>(opt & histogram::disp_mask);
	m_placement = static_cast<histogram::placement>(opt & histogram::placement_mask);
}

inline int SVHistogramBase::valley() const
{
	int l_valley = static_cast<int>(m_stats.valley(&m_values[m_highPeak], &m_values[m_lowPeak]) - &m_values[0]);
	return (valley_low <= l_valley && l_valley <= valley_high)?l_valley:valley_default;
}

template<typename Canvas>
inline void SVHistogramBase::DrawHistogram(Canvas & canvas)
{
	m_ratio = (static_cast<double>(m_client.Width()) / m_clipCount) + .0001;
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
	int l_bottom = m_client.Height();
	std::vector<int> & l_values = (m_disp == histogram::normal)?m_values:m_accumulated;
	for (int i = m_start; i < m_end; ++i)
	{
		double val = m_func(100.0*l_values[i]/l_max);
		int l_height = static_cast<int>(scale * ((val * l_bottom) / m_func(100.0) ));

		int l_top = l_bottom - l_height;
		if (l_top < 0)
			l_top = 0;
		if (l_top < l_bottom)
		{
			CRect l_bar = Pos2Screen(i);
			l_bar.top = l_top;
			canvas.Draw(l_bar, m_colors?i:0);
		}
	}
}

template<typename T>
inline bool SVHistogramBase::SetProperty(T & prop, T val, T low, T high) 
{ 
	if (prop == val) return false; 
	prop = (val < low)?low:(val>high)?high:val; 
	return true; 
}

inline bool SVHistogramBase::SetProperty(histogram::height_options & prop, histogram::height_options opt)
{
	if (prop != opt)
	{
		prop = opt;
		return true;
	}
	return false;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVHistogramBase.inl_v  $
 * 
 *    Rev 1.1   01 Oct 2013 14:19:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:45:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jan 2011 11:12:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
