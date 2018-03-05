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

#pragma region Declarations
enum ZoomEnum
{
	ZoomSmallest,
	ZoomSmall,
	ZoomNormal,
	ZoomLarge,
	ZoomLargest,
	ZoomPlus,
	ZoomMinus,
	ZoomFitAll,
	ZoomFitWidth,
	ZoomFitHeight,
	ZoomValue,
};
#pragma endregion Declarations

class ZoomHelper
{
#pragma region Constructor
public:
	ZoomHelper();
	virtual ~ZoomHelper();

	void Init();
	void Exit();
#pragma endregion Constructor

#pragma region Public Methods
public:
	bool SetZoomType(ZoomEnum ZoomType, unsigned int ZoomIndex=0);

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
	// Parameter ZoomType defines which side should fit
	// Parameter: CSize ImageSize
	// Parameter: CSize ViewSize
	// Returns:  bool true if successful
	//************************************
	bool CalculateZoomFit(ZoomEnum ZoomType, CSize ImageSize, CSize ViewSize);

	//************************************
	// Method:    IsFit
	// Description:  True if zoom is set to fit
	// Returns:   bool
	//************************************
	bool IsFit() const { return m_bIsFit; }

	//************************************
	// Method:    IsOne
	// Description:  True if zoom is set to 1.0 (100%)
	// Returns:   bool
	//************************************
	bool IsOne() const { return m_bIsOne; } 

	//************************************
	// Method:    CloneSettings
	// Description:  Copies all values from
	// Source into this class.
	// Parameter: const ZoomHelper & Source
	// Returns:   void
	//************************************
	void CloneSettings(const ZoomHelper &Source);

	void Clear();

	double GetZoomMin() const {return m_Zoom_Min; }
	double GetZoomMax() const { return m_Zoom_Max; }
	
	unsigned int GetScaleIndex() const { return m_ScaleIndex; }
	unsigned int GetScaleCount() const { return m_cScaleCount; }
	HGDIOBJ GetFont() const { return m_hFont[m_ScaleIndex]; }

	//************************************
	// Method:    GetZoomstep
	// Description:  Calculate the zoom factor value, which is
	// increased or decreased when the ZoomPlus or ZoomMinus method is called.
	// Returns:   double
	//************************************
	double GetZoomStep() const;
#pragma endregion Public Methods

#pragma region Private Methods
protected:
	//************************************
	// Method:    FindScaleIndex
	// Description:  Calculate the index which corresponds to the zoom factor
	// Parameter: void
	// Returns:   unsigned int
	//************************************
	unsigned int FindScaleIndex();

	//************************************
	// Method:    ZoomChanged
	// Description:  Called when Zoom factor is changed.
	// This method exists to be overridden by derived classes.
	// Returns:   void
	//************************************
	void ZoomChanged();
#pragma endregion Private Methods

#pragma region Member variables
protected:
	double m_Zoom{1.0};
	double m_ZoomFit{1.0};
	double m_Zoom_Min;
	double m_Zoom_Max;

	bool m_bIsOne{false};
	bool m_bIsFit{false};

	unsigned int m_ScaleIndex;
	static int m_InstanceCount;
	static const unsigned int m_cScaleCount = 31;
	static double m_ScaleFactor[m_cScaleCount];
	static HGDIOBJ m_hFont[m_cScaleCount];
 #pragma endregion Member variables
};

