//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogramStats
//* .File Name       : $Workfile:   SVHistogramStats.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:48:34  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <functional>
#pragma endregion Includes

namespace SvUl
{
	namespace histogram
	{
		enum bound_state { unknown = 0x00, shown = 0x01, active = 0x02, state_mask = 0x03 };
		enum bound_off { neither = 0x00, low = 0x10, high = 0x20, bound_mask = 0x30 }; 
		enum scale { linear = 0x00, logarithmic = 0x04, scale_mask = 0x04 };
		enum colors { plain = 0x00, config = 0x08, color_mask = 0x08 };
		enum clip_options { no_clip = 0x00, clip = 0x80, clip_mask = 0x80 };
		enum disp_options { normal = 0x000, accumulated = 0x100, disp_mask = 0x100 };
		enum grid_options { disabled = 0x000, enabled = 0x400, grid_mask = 0x400 };
		enum height_options { dynamic = 0x000, fixed = 0x800, height_mask = 0x800 };
		enum placement { elsewhere = 0x0000, threshold = 0x1000, analyzer = 0x2000, placement_mask = 0x3000 };
		enum v_axis { none = 0x00000, count = 0x10000, percent_max = 0x20000, percent_all = 0x40000, axis_mask = 0x70000 };

		template<int Scale>
		struct Scaler;

		typedef std::function<double (double)> Func;

		template<>
		struct Scaler<linear>
		{
			double operator()(double i) const
			{
				return i;
			}
		};

		template<>
		struct Scaler<logarithmic>
		{
			double operator()(double i) const
			{
				return log10(i/3+1.0);
			}
		};
	}

	class SVHistogramStats
	{

		__int64 m_total;
		__int64 m_squares;

		int m_low_peak;
		int m_low_peak_value;
		int m_high_peak;
		int m_high_peak_value;
		//int m_valley;
		int m_max_px; // max grayscale value
		int m_min_px; // min grayscale value
		int m_count_px; // pixel count
		int m_count_gs; // grayscale values count
		int m_start;
		//int m_default;
		int m_threshold;
		int m_min_peak;

	public:
		int low_peak()	const { return m_low_peak+m_start; }
		int high_peak()const { return m_high_peak+m_start; }
		int max_px()   const { return m_max_px; }
		int min_px()   const { return m_min_px; }
		int count_px() const { return m_count_px; }
		int count_gs() const { return m_count_gs; }
		int start()    const { return m_start; }
		//int max()      const { return m_max; }

		double mean() const { return m_count_px?static_cast<double>(m_total) / m_count_px:0.0; }
		double variance() const
		{
			double l_tmp = static_cast<double>(m_total); //mean() * m_count_px;
			return	m_count_px?
				(m_count_px * m_squares - l_tmp * l_tmp) /
				//-------------------------------------------------
				(static_cast<double>(m_count_px) * (m_count_px -1)):0.0;
		}

		double std_dev() const { return sqrt(variance()); }

		SVHistogramStats(int start = 0, int thr = 1): 
		m_start(start),
			m_threshold(thr),
			//m_default(def),
			m_low_peak(0),
			m_low_peak_value(0),
			m_high_peak(0),
			m_high_peak_value(0),
			m_max_px(0),
			m_min_px(0),
			m_count_px(0),
			m_count_gs(0),
			m_total(0),
			m_squares(0)
		{
		}

		SVHistogramStats & operator+=(int i)
		{ 
			m_count_px += i; // i == number of pixels with value of 'l_grayscale_val'
			__int64 l_grayscale_val = m_count_gs + m_start;
			__int64 l_tmp = l_grayscale_val * i; // number of samples * sample value
			m_total +=  l_tmp;
			m_squares += l_grayscale_val * l_tmp;
			if (i != 0)
			{
				m_max_px = m_count_gs + m_start;
				if (m_min_px == 0)
					m_min_px = m_max_px;
			}
			if (i > m_high_peak_value)
			{
				if (m_count_gs > m_high_peak + m_threshold)
				{
					m_low_peak = m_high_peak;
					m_low_peak_value = m_high_peak_value;
				}
				m_high_peak = m_count_gs;
				m_high_peak_value = i;
			}
			else if( i > m_low_peak_value && m_count_gs > m_high_peak + m_threshold)
			{
				m_low_peak = m_count_gs;
				m_low_peak_value = i;
			}
			++m_count_gs;
			return *this;
		}

		template<typename Iter>
		Iter valley(Iter first, Iter last)   const
		{
			if (first > last)
				std::swap(first, last);
			std::vector<Iter> valleys;
			valleys.reserve(last - first);
			valleys.push_back(first);
			int val = *first++;
			for (;first < last; ++first)
			{
				if (*first < val)
				{
					valleys.clear();
					valleys.push_back(first);
					val = *first;
				}
				else if (*first == val)
				{
					valleys.push_back(first);
				}
			}
			return valleys[valleys.size()/2];
		}
	};
} // namespace SvUl

namespace histogram = SvUl::histogram;
