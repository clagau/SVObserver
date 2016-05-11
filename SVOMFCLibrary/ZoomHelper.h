//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ZoomHelper
//* .File Name       : $Workfile:   ZoomHelper.h  $
//* .Description     : this Class encapsulates Zoom functionality
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   13 Aug 2014 17:19:56  $
//******************************************************************************

#pragma once

class ZoomHelper
{
#pragma region Constructor
public:
	ZoomHelper();
	virtual ~ZoomHelper();
#pragma endregion Constructor

#pragma region Public Methods
public:
	//************************************
	// Method:    GetZoom
	// Description: returns Zoomfactor
	// Returns:   double
	//************************************
	double GetZoom() const;

	//************************************
	// Method:    SetZoom
	// Description:  Set the current zoom factor
	// Parameter: double zoom
	// Returns:   void
	//************************************
	void SetZoom(double zoom);

	//************************************
	// Method:    ExtendMinMax
	// Description: Extend min-max interval, if necessary, so that zoom lies between min and max.
	// Parameter: double zoom
	// Returns:   void
	//************************************
	void ExtendMinMax(double zoom);

	//************************************
	// Method:    SetToFit
	// Description:  Set the zoom value to m_ZoomFit,
	// the maximum zoom value where the whole image is visible.
	// Returns:   new Zoom factor
	//************************************
	double SetToFit();

	//************************************
	// Method:    SetToOne
	// Description:  The zoom value is set to 1.0 (100%).
	//************************************
	void  SetToOne();

	//************************************
	// Method:    ZoomPlus
	// Description:  Increase the ZoomFactor
	// by an amount dependent on the ZoomFactor.
	// Returns:   double
	//************************************
	double ZoomPlus();

	//************************************
	// Method:    ZoomMinus
	// Description:  Decrease the Zoom factor
	// by an amount dependent on the Zoom factor.
	// Returns:   double
	//************************************
	double ZoomMinus();

	//************************************
	// Method:    CalculateZoomFit
	// Description:   Calculate m_ZoomFit,
	// the maximum zoom value where the whole image is visible.
	// Parameter: CSize ImageSize
	// Parameter: CSize ViewSize
	// Returns:  bool true if successful
	//************************************
	bool CalculateZoomFit(CSize ImageSize, CSize ViewSize);

	//************************************
	// Method:    IsFit
	// Description:  True if zoom is set to fit
	// Returns:   bool
	//************************************
	bool IsFit() const;

	//************************************
	// Method:    IsOne
	// Description:  True if zoom is set to 1.0 (100%)
	// Returns:   bool
	//************************************
	bool IsOne() const;

	//************************************
	// Method:    CloneSettings
	// Description:  Copies all values from
	// Source into this class.
	// Parameter: const ZoomHelper & Source
	// Returns:   void
	//************************************
	void CloneSettings(const ZoomHelper &Source);

	//************************************
	// Method:    Clear
	// Description:  Reset to initial values
	// Returns:   void
	//************************************
	void Clear();

	//************************************
	// Method:    GetZoomMin
	// Description:  Return the smallest zoom value.
	// Returns:   double
	//************************************
	double GetZoomMin() const;

	//************************************
	// Method:    GetZoomMax
	// Description:  Return the biggest zoom value.
	// Returns:   double
	//************************************
	double GetZoomMax() const;

	//************************************
	// Method:    GetZoomstep
	// Description:  Calculate the zoom factor value, which is
	// increased or decreased when the ZoomPlus or ZoomMinus method is called.
	// Returns:   double
	//************************************
	double GetZoomStep() const;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	//************************************
	// Method:    ZoomChanged
	// Description:  Called when Zoom factor is changed.
	// This method exists to be overridden by derived classes.
	// Returns:   void
	//************************************
	virtual void ZoomChanged();
#pragma endregion Protected Methods

#pragma region Member variables
protected:
	double m_Zoom;
	double m_Zoom_Min;
	double m_Zoom_Max;
	double m_ZoomFit;

	bool m_bIsOne;
	bool m_bIsFit;

	const double m_Default_Min;
	const double m_Default_Max;
	const double m_ZoomStepLarge;
	const double m_ZoomStepSmall;
#pragma endregion Member variables
};

