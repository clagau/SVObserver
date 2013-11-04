//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageEnums
//* .File Name       : $Workfile:   SVImageEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:53:32  $
//******************************************************************************

#ifndef _SVIMAGEENUMS_H
#define _SVIMAGEENUMS_H

#include "SVHBitmapUtilitiesLibrary\SVImageFormatEnum.h"

enum SVImageTypeEnum
{
	SVImageTypeUnknown     = 0,
	SVImageTypeFixed       = 1,
	SVImageTypeIndependent = 2,
	SVImageTypeDependent   = 3,
	SVImageTypePhysical    = 4,
	SVImageTypeLogical     = 5,
	SVImageTypeVirtual     = 6,
	SVImageTypeMain        = 7,
	SVImageTypeRGBMain     = 8,
};


enum SVImagePropertyEnum
{
	SVImagePropertyAll = 0x0000001F,
	SVImagePropertyFormat = 0x00000001,
	SVImagePropertyPixelDepth = 0x00000002,
	SVImagePropertyBandNumber = 0x00000004,
	SVImagePropertyBandLink = 0x00000008,
	SVImagePropertyResetOwner = 0x00000010,
};

/*
// Original Coreco Light Reference values
#define CORACQ_PRM_VIC_FIRST			0x00000800
#define CORACQ_PRM_VIC( n, size)		( (((n) + CORACQ_PRM_VIC_FIRST) << 16) | (size))
#define CORACQ_PRM_BRIGHTNESS						CORACQ_PRM_VIC( 0x00000003, 4)
#define CORACQ_PRM_BRIGHTNESS_RED				CORACQ_PRM_VIC( 0x00000004, 4)
#define CORACQ_PRM_BRIGHTNESS_GREEN				CORACQ_PRM_VIC( 0x00000005, 4)
#define CORACQ_PRM_BRIGHTNESS_BLUE 				CORACQ_PRM_VIC( 0x00000006, 4)
#define CORACQ_PRM_CONTRAST						CORACQ_PRM_VIC( 0x00000007, 4)
#define CORACQ_PRM_CONTRAST_RED	  				CORACQ_PRM_VIC( 0x00000008, 4)
#define CORACQ_PRM_CONTRAST_GREEN  				CORACQ_PRM_VIC( 0x00000009, 4)
#define CORACQ_PRM_CONTRAST_BLUE	  				CORACQ_PRM_VIC( 0x0000000a, 4)
*/
enum SVLightReferenceTypeEnum
{
	SVLightReferenceTypeUnknown         = 0,
	SVLightReferenceTypeBrightness      = 10,
	SVLightReferenceTypeBrightnessRed   = 11,
	SVLightReferenceTypeBrightnessGreen = 12,
	SVLightReferenceTypeBrightnessBlue  = 13,
	SVLightReferenceTypeContrast        = 20,
	SVLightReferenceTypeContrastRed     = 21,
	SVLightReferenceTypeContrastGreen   = 22,
	SVLightReferenceTypeContrastBlue    = 23,

	SVCorLightReferenceTypeBrightness      = 0x08030004,
	SVCorLightReferenceTypeBrightnessRed   = 0x08040004,
	SVCorLightReferenceTypeBrightnessGreen = 0x08050004,
	SVCorLightReferenceTypeBrightnessBlue  = 0x08060004,
	SVCorLightReferenceTypeContrast        = 0x08070004,
	SVCorLightReferenceTypeContrastRed     = 0x08080004,
	SVCorLightReferenceTypeContrastGreen   = 0x08090004,
	SVCorLightReferenceTypeContrastBlue    = 0x080A0004,

};

enum SVDigitizerParameterEnum
{
	SVDigitizerParameterUnknown = 0,
	SVDigitizerParameterBrightness = 100,
	SVDigitizerParameterBrightnessMax = 101,
	SVDigitizerParameterBrightnessMin = 102,
	SVDigitizerParameterBrightnessStep = 103,
	SVDigitizerParameterBrightnessRed = 110,
	SVDigitizerParameterBrightnessRedMax = 111,
	SVDigitizerParameterBrightnessRedMin = 112,
	SVDigitizerParameterBrightnessRedStep = 113,
	SVDigitizerParameterBrightnessGreen = 120,
	SVDigitizerParameterBrightnessGreenMax = 121,
	SVDigitizerParameterBrightnessGreenMin = 122,
	SVDigitizerParameterBrightnessGreenStep = 123,
	SVDigitizerParameterBrightnessBlue = 130,
	SVDigitizerParameterBrightnessBlueMax = 131,
	SVDigitizerParameterBrightnessBlueMin = 132,
	SVDigitizerParameterBrightnessBlueStep = 133,
	SVDigitizerParameterContrast = 140,
	SVDigitizerParameterContrastMax = 141,
	SVDigitizerParameterContrastMin = 142,
	SVDigitizerParameterContrastStep = 143,
	SVDigitizerParameterContrastRed = 150,
	SVDigitizerParameterContrastRedMax = 151,
	SVDigitizerParameterContrastRedMin = 152,
	SVDigitizerParameterContrastRedStep = 153,
	SVDigitizerParameterContrastGreen = 160,
	SVDigitizerParameterContrastGreenMax = 161,
	SVDigitizerParameterContrastGreenMin = 162,
	SVDigitizerParameterContrastGreenStep = 163,
	SVDigitizerParameterContrastBlue = 170,
	SVDigitizerParameterContrastBlueMax = 171,
	SVDigitizerParameterContrastBlueMin = 172,
	SVDigitizerParameterContrastBlueStep = 173,
	SVDigitizerParameterLUT = 180,
	SVDigitizerParameterLUTFormat = 181,
	SVDigitizerParameterLUTBandSize = 182,
	SVDigitizerParameterLUTBandCount = 183,
};

enum SVDigitizerParameterTypeEnum
{
	SVDigitizerParameterTypeUnknown = 0,
	SVDigitizerParameterTypeAnyValue = 10,
	SVDigitizerParameterTypeNumericValue = 11,
	SVDigitizerParameterTypeRangedValue = 12,
	SVDigitizerParameterTypeEnum = 20,
	SVDigitizerParameterTypeEnumWithValue = 21,
	SVDigitizerParameterTypeLUT = 30,
	SVDigitizerParameterTypeDCamRegister = 40,
	SVDigitizerParameterTypeDCamFeature = 41,
};

enum SVDigitizerParameterDCamFeatureControlEnum
{
	SVDigitizerParameterDCamFeatureControlOff = 0,
	SVDigitizerParameterDCamFeatureControlAuto = 1,
	SVDigitizerParameterDCamFeatureControlManual = 2,
	SVDigitizerParameterDCamFeatureControlOnePushAction = 3,
};

#endif   // _SVIMAGEENUMS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVImageEnums.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:53:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   13 Apr 2011 19:10:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated image types to include a new fixed image type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   13 Apr 2011 14:51:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   05 Apr 2011 08:59:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new image type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Feb 2011 16:23:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to move data elements to appropriate objects to minimize tight object coupling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Jul 2007 09:03:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved ImageFormatEnum to SVHBitmapUtilities
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Nov 2005 15:19:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include the new parameter functions for the DLL and new image formats for the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Feb 2005 14:15:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new enums.  SVImageTypeEnum and SVImagePropertyEnum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Oct 2003 14:38:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added a new enum for digitizer parameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 13:57:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added image objects to new image interface library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
