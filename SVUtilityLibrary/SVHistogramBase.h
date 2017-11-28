//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogramBase
//* .File Name       : $Workfile:   SVHistogramBase.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:47:02  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <numeric>
//Moved to precompiled header: #include <atltypes.h>
//Moved to precompiled header: #include <boost/function.hpp>

#include "SVHistogramStats.h"


namespace SvUl {
	enum
	{
		MaxIndex=255,
		VectorCount=256,
		no_clip = 257
	};

	class SVHistogramBase
	{
	public:
		SVHistogramBase();

		virtual ~SVHistogramBase();

		template<typename Iter>
		void SetPixelCounts(Iter first, Iter last);

		template<typename Canvas>
		void DrawHistogram(Canvas & canvas);

		void SetOptions(unsigned opt);

		unsigned GetOptions() const;

		bool Clip(int left, int right);

		bool SetScale(histogram::scale s);
		bool SetDisplay(histogram::disp_options d);
		void SetClient(const CRect & rect) { m_client = rect; }
		bool SetPeakDefault(int i)		{ return SetProperty(peak_default, i); }
		bool SetPeakThreshold(int i)	{ return SetProperty(peak_threshold, i); }
		bool SetValleyDefault(int i)	{ return SetProperty(valley_default, i); }
		bool SetValleyLow(int i)		{ return SetProperty(valley_low, i); }
		bool SetValleyHigh(int i)		{ return SetProperty(valley_high, i); }
		bool SetMinHeight(double d)		{ return SetProperty(peak_min_height, d, 0.0, 100.00); }
		bool SetFixedHeight(double d)	{ return SetProperty(fixed_height, d, 0.0, 100.0); }
		bool SetHeightOption(histogram::height_options opt) { return SetProperty(m_height, opt); }

		int  GetPeakDefault()		const { return peak_default; }
		int  GetPeakThreshold()		const { return peak_threshold; }
		int  GetLowClip()			const { return m_start; }
		int  GetHighClip()			const { return m_end - 1; }
		int  GetValleyDefault()		const { return valley_default; }
		int  GetValleyLow()			const { return valley_low; }
		int  GetValleyHigh()		const { return valley_high; }

		bool IsDynaHeight()			const { return m_height == histogram::dynamic; }
		double GetFixedHeight()		const { return fixed_height; }
		double GetMinHeight()		const { return peak_min_height; }

		void ForceRepaint()			{ needs_repaint = true; }
		void CalcStats();

		double mean()		const { return m_stats.mean(); }
		double variance()	const { return m_variance; }
		double stdDev()		const { return m_std_dev; }
		int max_pixel()		const { return max_px; }
		int min_pixel()		const { return min_px; }
		int pixel_num()		const { return m_stats.count_px(); }
		int bin_count()		const { return m_clipCount; }
		int low_peak()		const { return (m_values[m_lowPeak] < peak_min_height)?peak_default:m_lowPeak; }
		int high_peak()		const { return m_highPeak; }
		int valley()		const;
		int value_at(int i)	const { return m_values[i]; }

	protected:
		std::vector<int> m_values;
		std::vector<int> m_accumulated;
		int m_lowBound{ 0 };
		int m_highBound{ 0 };
		int m_start{ 0 };
		int m_end{ VectorCount };
		int m_clipCount{ VectorCount };
		int m_max{ 0 };
		int m_total{ 0 };
		int m_lowPeak{ 0 };
		int m_highPeak{ 0 };
		int m_valley{ 0 };
		double scale{ 1.0 };
		double m_ratio{ 0.0 };
		double m_factor{ 0.0 };
		histogram::scale m_scale{ histogram::linear };
		histogram::v_axis m_axis{ histogram::percent_all };
		histogram::colors m_colors{ histogram::config };
		histogram::clip_options m_clip{ histogram::no_clip };
		histogram::disp_options m_disp{ histogram::normal };
		histogram::height_options m_height{ histogram::dynamic };
		histogram::placement m_placement{ histogram::elsewhere };
		unsigned m_disabled{ 0 };
		double m_variance{ 0. };
		double m_std_dev{ 0. };
		CRect m_client;

		int min_px{ 0 };
		int max_px{ 0 };
		int peak_default{ 0 };
		int peak_threshold{ 0 };
		double peak_min_height{ 0.0 };
		double fixed_height{ 0.0 };
		int valley_low{ 0 };
		int valley_high{ MaxIndex };
		int valley_default{ 0 };

		histogram::Func m_func;

		SVHistogramStats m_stats;

		bool needs_repaint{ true };
		// convert position of the bar into its screen rectangle
		CRect Pos2Screen(int pos);
		// convert screen x-coordinate into bar position
		int Screen2Pos(int x);
		template<typename T>
		bool SetProperty(T & prop, T val, T low = 0, T high = MaxIndex);
		bool SetProperty(histogram::height_options & prop, histogram::height_options opt);
	};

	typedef SVHistogramStats Stats;

#include "SVHistogramBase.inl"

} // namespace SvUl

