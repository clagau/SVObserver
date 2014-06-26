//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ZoomHelper
//* .File Name       : $Workfile:   ZoomHelper.h  $
//* .Description     : this Class encapsulates Zoom functionality
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:28:54  $
//******************************************************************************

#pragma once

class ZoomHelper
{
#pragma region Constructor
public:
	ZoomHelper();
#pragma endregion Constructor

#pragma region Public Methods
public:
	//************************************
	// Method:    GetZoom
	// Description: return Zoomfactor
	// Returns:   double
	//************************************
	double GetZoom() const;

	//************************************
	// Method:    SetZoom
	// Description:  Set the zoom facor
	// if changeMax is true maxvalues are adapted to value, else value is adapted to maxvalues
	// Parameter: double zoom
	// Parameter: bool changeMax
	// Returns:   void
	//************************************
	void SetZoom(double zoom, bool changeMax = false);

	//************************************
	// Method:    SetToFit
	// Description:  set the zoom value to  m_ZoomFit,
	// the maximum zoom value where the whole image is visible
	// Returns:   double
	//************************************
	double SetToFit();

	//************************************
	// Method:    SetToOne
	// Description:  The zoom value is set to 1.0
	// Returns:   double
	//************************************
	double SetToOne();

	//************************************
	// Method:    ZoomPlus
	// Description:  Increase the Zoomfactor dependent on the Zoomfactor
	// Returns:   double
	//************************************
	double ZoomPlus();

	//************************************
	// Method:    ZoomMinus
	// Description:  Decrease the Zoom factor dependent  on the Zoom factor 
	// Returns:   double
	//************************************
	double ZoomMinus();

	//************************************
	// Method:    CalculateZoomFit
	// Description:   calculate m_ZoomFit,
	// the maximum zoom value where the whole image is visible
	// Parameter: CSize ImageSize
	// Parameter: CSize ViewSize
	// Returns:   double
	//************************************
	double CalculateZoomFit(CSize ImageSize, CSize ViewSize);

	//************************************
	// Method:    IsFit
	// Description:  True if zoom is set to fit
	// Returns:   bool
	//************************************
	bool IsFit() const;

	//************************************
	// Method:    IsOne
	// Description:  True if zoom is set to 1.0 
	// Returns:   bool
	//************************************
	bool IsOne() const;

	//************************************
	// Method:    CloneSettings
	// Description:  copies all values of this class
	// Parameter: const ZoomHelper & Source
	// Returns:   void
	//************************************
	void CloneSettings(const ZoomHelper &Source);

	//************************************
	// Method:    Clear
	// Description:  Initialize Values
	// Returns:   void
	//************************************
	void Clear();

	//************************************
	// Method:    GetZoomMin
	// Description:  Return the smallest zoomvalue
	// Returns:   double
	//************************************
	double GetZoomMin() const;

	//************************************
	// Method:    GetZoomMax
	// Description:  Return the biggest zoom value
	// Returns:   double
	//************************************
	double GetZoomMax() const;

	//************************************
	// Method:    GetZoomStepLarge
	// Description:   return ZoomStep large 
	// Returns:   double
	//************************************
	double GetZoomStepLarge() const;
	//************************************
	// Method:    GetZoomStepSmall
	// Description: return ZoomStep Small 
	// Returns:   double
	//************************************
	double GetZoomStepSmall() const;
	
#pragma endregion Public Methods

#pragma region Member variables
private:
	double m_Zoom;
	double m_Zoom_Min;
	double m_Zoom_Max;
	double m_ZoomFit;
	bool m_bIsOne;
	bool m_bIsFit;
	double m_Default_Min;
	double m_Default_Max;
	double m_ZoomStepLarge;
	double m_ZoomStepSmall;
#pragma endregion Member variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\ZoomHelper.h_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:28:54   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */