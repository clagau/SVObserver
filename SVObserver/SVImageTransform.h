//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageTransform
//* .File Name       : $Workfile:   SVImageTransform.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 12:44:00  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVIMAGETRANSFORM_H
#define SVIMAGETRANSFORM_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "SVTransform.h"
#include "SVImageClass.h"

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************


class SVImageTransformClass : public SVTransformClass
{
	SV_DECLARE_CLASS( SVImageTransformClass )

#pragma region Constructor
public:
	SVImageTransformClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVIMAGETRANSFORM );
	virtual ~SVImageTransformClass();
#pragma endregion

#pragma region Public Methods
public:
#pragma region virtual
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	virtual BOOL OnValidate();
	virtual HRESULT ResetObject( );
#pragma endregion

	SVImageClass* getInputImage();
	SVImageClass* getOutputImage();
#pragma endregion

#pragma region Protected Methods
protected:
	virtual BOOL onRun( SVRunStatusClass& runStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	virtual HRESULT UpdateTransformData( long p_lIndex );
#pragma endregion

#pragma region Private Methods
private:
	HRESULT CollectInputImageNames( SVRunStatusClass& runStatus );
#pragma endregion

#pragma region Member variables
protected:
	// Embedded Object: ( Outputs )
	SVImageClass				m_outputImageObject;
	SVBoolValueObjectClass		m_useExtentsOnly;

	// Inputs
	SVInObjectInfoStruct		m_inputImageObjectInfo;

	SVDoubleValueObjectClass	m_extentWidth;
	SVDoubleValueObjectClass	m_extentHeight;

	SVDoubleValueObjectClass	m_extentDisplacementX;
	SVDoubleValueObjectClass	m_extentDisplacementY;
	
	SVDoubleValueObjectClass	m_extentSourceX;
	SVDoubleValueObjectClass	m_extentSourceY;

	SVDoubleValueObjectClass	m_extentRotationAngle;
	SVEnumerateValueObjectClass m_interpolationMode;
#pragma region
};

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVIMAGETRANSFORM_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVImageTransform.h_v  $
 * 
 *    Rev 1.3   15 May 2014 12:44:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 10:48:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jan 2014 16:43:36   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  879
 * SCR Title:  Add interpolation mode to transformation tool
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed code to conform with guidelines.
 * Added m_interpolationMode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:56:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   06 Mar 2008 15:33:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Code to collect source Image Names for Image Tools.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   13 Sep 2005 09:52:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the method to correctly caculate the extent data and pass it correctly to the cached source/aux data and to the main extent information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Aug 2005 15:56:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new value objects to handle the translation extents and hooked then to the tool extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Feb 2005 14:27:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jan 2004 09:56:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update header information to make getInputImage method public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 10:18:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jan 2003 10:33:52   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated processMessage, onRun and CreateObject methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:36:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Class for Image Transformations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/