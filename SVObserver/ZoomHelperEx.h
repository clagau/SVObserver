//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .File Name       : $Workfile:   ZoomHelperEx.h  $
//* .Description     : Extends the zoom helper class with the zoom functionality from SVImageView class.
//					The class encapsulates an array from zoom factors with an array of fonts.
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 13:23:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOMFCLibrary\ZoomHelper.h"
#pragma endregion Includes

#pragma region Declarations
enum EZoomMode
{
	EZOOM_SMALLEST,
	EZOOM_SMALL,
	EZOOM_NORMAL,
	EZOOM_LARGE,
	EZOOM_LARGEST,
	EZOOM_IN,
	EZOOM_OUT,
	EZOOM_VALUE
};
#pragma endregion Declarations

class ZoomHelperEx : public ZoomHelper
{
#pragma region Constructor
public:
	ZoomHelperEx();
	~ZoomHelperEx();
#pragma endregion Constructor
	//************************************
	// Method:    Init
	// Description:  Initialize the static variables create fonts.
	// Returns:   void
	//************************************
	void Init();

	//************************************
	// Method:    Exit
	// Description: Clean up when last instance is destroyed
	// Returns:   void
	//************************************
	void Exit();

#pragma region Public Methods
public:
	//************************************
	// Method:    ChangeScaleIndex
	// Description:  Set the Zoom based on a value of type EZoomMode (which corresponds to 
	//		the SVImageView context menu preset zoom sizes) or an arbitrary value.
	// Parameter: EZoomMode e
	// Parameter: unsigned int
	// Returns:   bool
	//************************************
	bool ChangeScaleIndex(EZoomMode e, unsigned int = 0);

	//************************************
	// Method:    GetScaleIndex
	// Description: return the scale index
	// Returns:   unsigned int
	//************************************
	unsigned int GetScaleIndex() const;

	//************************************
	// Method:    GetScaleCount
	// Description:  the numbers of scale indices
	// Returns:   unsigned int
	//************************************
	unsigned int GetScaleCount() const;

	//************************************
	// Method:    GetFont
	// Description:  return the font which corresponds to the zoom factor
	// Returns:   HGDIOBJ
	//************************************
	HGDIOBJ GetFont()const;
#pragma endregion Public Methods

#pragma region Protected Methods
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
	// Description:  function is called when zoom factor is changed 
	// Parameter: void
	// Returns:   void
	//************************************
	virtual void ZoomChanged() override;
#pragma endregion Protected Methods

#pragma region Member variables
private:
	unsigned int m_scaleIndex;
	static int s_InstanceCount;
	static const unsigned int s_scaleCount = 31;
	static double m_scaleFactor[ s_scaleCount ];
	static HGDIOBJ m_hFont[ s_scaleCount ];
#pragma endregion Member variables
};

