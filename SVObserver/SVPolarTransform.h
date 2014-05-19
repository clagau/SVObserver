//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPolarTransform
//* .File Name       : $Workfile:   SVPolarTransform.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:56:12  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVPOLARTRANSFORM_H
#define SVPOLARTRANSFORM_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVTaskObject.h"

class SVToolClass;

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************



class SVPolarTransformClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVPolarTransformClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVPolarTransformClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVPOLARTRANSFORM );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVPolarTransformClass();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL OnValidate();

protected:
	SVDoubleValueObjectClass* getInputCenterXResult();
	SVDoubleValueObjectClass* getInputCenterYResult();

	SVDoubleValueObjectClass* getInputStartRadiusResult();
	SVDoubleValueObjectClass* getInputEndRadiusResult();

	SVDoubleValueObjectClass* getInputStartAngleResult();
	SVDoubleValueObjectClass* getInputEndAngleResult();

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	// Inputs
	SVInObjectInfoStruct inputCenterXResult;
	SVInObjectInfoStruct inputCenterYResult;
	
	SVInObjectInfoStruct inputStartRadiusResult;
	SVInObjectInfoStruct inputEndRadiusResult;

	SVInObjectInfoStruct inputStartAngleResult;
	SVInObjectInfoStruct inputEndAngleResult;


	// Embedded Objects (outputs)
	//SVBoolValueObjectClass performPolarTransformation;
};


//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVPOLARTRANSFORM_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPolarTransform.h_v  $
 * 
 *    Rev 1.2   15 May 2014 11:56:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed processMessage signature to use DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:00:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:30:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Feb 2005 15:19:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 13:52:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jan 2003 14:06:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the class to inherit from SVExtentHolder instead of SVTaskObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Nov 2002 10:44:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed reference to System
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 01 2000 12:07:12   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   NEW Module: Provides base class for polar transformation.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/