// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearToolClass.h
// * .File Name       : $Workfile:   SVLinearToolClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   09 Dec 2014 09:47:26  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVTool.h"
#include "SVImageClass.h"
#pragma endregion Includes

#pragma region Declarations
const LPCSTR g_strOrientationEnums = _T( "Horizontal=0,Vertical=90" );
#pragma endregion Declarations

class SVLinearToolClass : public SVToolClass
, public AllowResizeToParent
{
	SV_DECLARE_CLASS( SVLinearToolClass );

public:
#pragma region Constructor
	SVLinearToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVLINEARTOOL );
	virtual ~SVLinearToolClass();
#pragma endregion Constructor

#pragma region Public Methods
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual HRESULT SetImageExtentToParent( unsigned long p_ulIndex );
	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );
	virtual HRESULT DoesObjectHaveExtents() const;

	CString GetProfileOrientation();
	BOOL GetRotation();

	//************************************
	//! return true if autosizing is disabled because rotation is turned on 
	//! \returns bool
	//************************************
	bool IsAutoSizeDisabled();
	BOOL IsToolRotated();

	virtual HRESULT GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames );
#pragma endregion Public Methods

	//Embedded Object : Profile Orientation
	SVEnumerateValueObjectClass m_voProfileOrientation; // @WARNING:  bad practice making members public
	SVBoolValueObjectClass m_voUseProfileRotation; // @WARNING:  bad practice making members public
	SVStringValueObjectClass m_svSourceImageNames; // @WARNING:  bad practice making members public

protected:
#pragma region Protected Methods
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
#pragma endregion Protected Methods

	SVDoubleValueObjectClass m_svRotationAngle;
	SVDoubleValueObjectClass m_svRotationPointX;
	SVDoubleValueObjectClass m_svRotationPointY;

private:
#pragma region Private Methods
	virtual void init();
#pragma endregion Private Methods
};

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLinearToolClass.h_v  $
 * 
 *    Rev 1.3   09 Dec 2014 09:47:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed OperatorMovable from class definition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2014 13:12:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 11:15:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:04:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Aug 2012 11:12:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OperatorMovable base to check for operator move context menu option
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Jul 2012 09:01:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Mar 2011 12:11:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 Mar 2008 16:05:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Code to collect source Image Names for Image Tools.
 * and
 * Added Attributes to support Image Definition Lists.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Aug 2005 16:07:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2005 10:49:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Adding new files for the new linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/