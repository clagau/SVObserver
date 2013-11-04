//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRotateReference
//* .File Name       : $Workfile:   SVRotateReference.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:01:56  $
//******************************************************************************

#ifndef SVROTATEREFERENCE_H
#define SVROTATEREFERENCE_H

#include "SVTaskObject.h"
#include "SVImageClass.h"

class SVToolClass;

class SVRotateReferenceClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVRotateReferenceClass )

public:
	SVRotateReferenceClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVROTATEREFERENCE );
	virtual ~SVRotateReferenceClass();
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	virtual BOOL OnValidate();

	SVImageClass*					getInputImage();
	SVImageClass*					getOutputImage();

protected:
	SVBoolValueObjectClass*			getInputXEnabled();
	SVDoubleValueObjectClass*		getInputXResult();
	SVBoolValueObjectClass*			getInputYEnabled();
	SVDoubleValueObjectClass*		getInputYResult();
	SVBoolValueObjectClass*			getInputBetaEnabled();
	SVDoubleValueObjectClass*		getInputBetaResult();
	SVDoubleValueObjectClass*		getLearnedX();
	SVDoubleValueObjectClass*		getLearnedY();
	SVDoubleValueObjectClass*		getLearnedBeta();


	virtual BOOL	onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD	processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext );

	SVInObjectInfoStruct		inputXEnabled;
	SVInObjectInfoStruct		inputXResult;
	SVInObjectInfoStruct		inputYEnabled;
	SVInObjectInfoStruct		inputYResult;
	SVInObjectInfoStruct		inputBetaEnabled;
	SVInObjectInfoStruct		inputBetaResult;
	SVInObjectInfoStruct		inputImageObjectInfo;

	// Enbedded Objects:
	SVImageClass				outputImageObject;
	SVDoubleValueObjectClass    learnedX;
	SVDoubleValueObjectClass    learnedY;
	SVDoubleValueObjectClass    learnedBeta;

};


//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVROTATEREFERENCE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRotateReference.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:01:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   25 Jul 2012 14:48:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   17 Feb 2005 15:23:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to implement the new create / reset methodology and new extents.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   19 Jan 2004 10:18:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update header information to make getInputImage method public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   22 Apr 2003 15:12:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   31 Jan 2003 11:26:48   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to fix a check-in problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   30 Jan 2003 16:26:16   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated processMessage, onRun, onUpdateFigure and CreateObject methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   10 Jan 2003 15:54:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function VerifyImageForOverlay to determine which object has overlays on which image.
 * Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   20 Nov 2002 13:15:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   25 Feb 2000 16:10:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised handler for SVM_GETFIRSTMOVEABLE/RESIZEABLE.
 * Revised onDraw to call pImage->GetTransform() to get contextual transformation.
 * Removed getTranslationContext and getRotationContext
 * methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   21 Feb 2000 14:58:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added getTranslationContext method.
 * Added getRotationContext method.
 * Revised CreateObject() to set display context image.
 * Revised processMessage handling of Move/Size messages
 * to use SVDrawContext
 * Revised onDraw to call checkDrawAllowed().
 * Revised onUpdateFigure to use relative extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   16 Feb 2000 17:05:12   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Modified Draw functions to use the DrawContext structure
 * and removed unused methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:37:48   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 14 1999 15:12:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provide rotate (move) capability.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
