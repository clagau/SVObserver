//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTransform
//* .File Name       : $Workfile:   SVTransform.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:26:46  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVTRANSFORM_H
#define SVTRANSFORM_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "SVTaskObject.h"

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************



class SVTransformClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVTransformClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVTransformClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVTRANSFORM );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVTransformClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL OnValidate();

protected:
	SVDoubleValueObjectClass* getInputTranslationXResult();
	SVDoubleValueObjectClass* getInputTranslationYResult();
	SVDoubleValueObjectClass* getInputRotationXResult();
	SVDoubleValueObjectClass* getInputRotationYResult();
	SVDoubleValueObjectClass* getInputRotationAngleResult();

	virtual DWORD processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext );

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Inputs
	SVInObjectInfoStruct inputTranslationXResult;
	SVInObjectInfoStruct inputTranslationYResult;
	
	SVInObjectInfoStruct inputRotationXResult;
	SVInObjectInfoStruct inputRotationYResult;
	SVInObjectInfoStruct inputRotationAngleResult;

	// Embedded Objects (outputs)
	SVBoolValueObjectClass performTranslation;
	SVBoolValueObjectClass performRotation;

	SVDoubleValueObjectClass    learnedTranslationX;
	SVDoubleValueObjectClass    learnedTranslationY;
	SVDoubleValueObjectClass    learnedRotationX;
	SVDoubleValueObjectClass    learnedRotationY;
	SVDoubleValueObjectClass    learnedRotationAngle;
};

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVTRANSFORM_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTransform.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 12:26:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Dec 2012 12:17:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed code associated with SCR 798
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Nov 2012 13:40:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  798
 * SCR Title:  Change Transformation Tool to have an option for translation only
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new value object for the new translation mode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 14:04:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes to support new Create Object / Reset Methodology
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 17:07:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Nov 2002 14:46:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed SystemObjectInfoStruct from CreateObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:35:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New Class for Transformations (base class).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
