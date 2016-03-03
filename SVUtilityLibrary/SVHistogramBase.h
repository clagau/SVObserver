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

#include <vector>
#include <numeric>
#include <atltypes.h>  // needed for CRect

#include <boost/function.hpp>

#include "SVHistogramStats.h"


namespace Seidenader { namespace SVUtilityLibrary {
	enum {no_clip = 257};

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
		int m_lowBound, m_highBound;
		int m_start, m_end, m_clipCount;
		int m_max;
		int m_total;
		int m_lowPeak;
		int m_highPeak;
		int m_valley;
		double scale;
		double m_ratio;
		double m_factor;
		histogram::scale m_scale;
		histogram::v_axis m_axis;
		histogram::colors m_colors;
		histogram::clip_options m_clip;
		histogram::disp_options m_disp;
		histogram::height_options m_height;
		histogram::placement m_placement;
		unsigned m_disabled;
		double m_variance;
		double m_std_dev;
		CRect m_client;

		int min_px;
		int max_px;
		int peak_default;
		int peak_threshold;
		double peak_min_height;
		double fixed_height;
		int valley_low;
		int valley_high;
		int valley_default;

		histogram::Func m_func;

		SVHistogramStats m_stats;

		bool needs_repaint;
		// convert position of the bar into its screen rectangle
		CRect Pos2Screen(int pos);
		// convert screen x-coordinate into bar position
		int Screen2Pos(int x);
		template<typename T>
		bool SetProperty(T & prop, T val, T low = 0, T high = 255); 
		bool SetProperty(histogram::height_options & prop, histogram::height_options opt);
	};

	typedef SVHistogramStats Stats;

#include "SVHistogramBase.inl"

} }

namespace SvUl = Seidenader::SVUtilityLibrary;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVHistogramBase.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:47:02   bWalter
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