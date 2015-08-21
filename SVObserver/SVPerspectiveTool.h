//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPerspectiveToolClass
//* .File Name       : $Workfile:   SVPerspectiveTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   09 Dec 2014 09:47:26  $
//******************************************************************************

#ifndef SVPERSPECTIVETOOLCLASS_H
#define SVPERSPECTIVETOOLCLASS_H

#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVImageClass.h"
#include "SVTool.h"
#include "SVValueObjectImpl.h"

const CString PERSPECTIVE_WARP_TYPE_HORIZONTAL = _T("Horizontal Warp");
const CString PERSPECTIVE_WARP_TYPE_VERTICAL = _T("Vertical Warp");

class SVPerspectiveToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVPerspectiveToolClass );

public:
	SVPerspectiveToolClass( BOOL p_bCreateDefaultTaskList = FALSE, 
		                    SVObjectClass *p_pOwner = NULL, 
												int p_iStringResourceID = IDS_CLASSNAME_SVPERSPECTIVETOOL );
	virtual ~SVPerspectiveToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct *p_pCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual BOOL OnValidate();

	SVImageClass *GetInputImage();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );
	virtual SVTaskObjectClass* GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );
	virtual bool DoesObjectHaveExtents() const override;
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );

	enum WarpType
	{
		WarpTypeInvalid = 0,
		WarpTypeHorizontal = 1,
		WarpTypeVertical = 2,
	};


protected:
	HRESULT UpdateOutputImageExtents();
	virtual BOOL onRun( SVRunStatusClass &p_rRunStatus );
//	virtual DWORD_PTR processMessage( DWORD p_dwMessageID, DWORD_PTR p_dwMessageValue, DWORD_PTR p_dwMessageContext );

private:
	void LocalInitialize();

	HRESULT LocalDestroy();

	HRESULT CreateLUT();
	HRESULT DestroyLUT();
	
	SVInObjectInfoStruct m_InputImageObjectInfo;

	SVImageClass m_OutputImage;
	SVEnumerateValueObjectClass m_svWarpType;
	SVDoubleValueObjectClass m_svXOffset;
	SVDoubleValueObjectClass m_svYOffset;
	SVEnumerateValueObjectClass m_svInterpolationMode;

	SVMatroxBuffer m_LutX;
	SVMatroxBuffer m_LutY;
private:
	// String value object for Source Image Names
	SVStringValueObjectClass m_svSourceImageNames;
	HRESULT CollectInputImageNames( SVRunStatusClass& RRunStatus );

public:
	virtual HRESULT GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames );

};

#endif // _SVPERSPECTIVETOOLCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPerspectiveTool.h_v  $
 * 
 *    Rev 1.2   09 Dec 2014 09:47:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed OperatorMovable from class definition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 May 2014 13:47:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revsied processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:21:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   03 Aug 2012 11:12:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OperatorMovable base to check for operator move context menu option
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Jul 2012 14:19:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   21 Mar 2011 12:13:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   11 Mar 2011 14:31:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Feb 2011 15:09:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Jan 2011 11:50:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Mar 2008 12:57:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added functions to support source image names
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Jul 2007 15:44:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Aug 2005 07:59:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jul 2005 15:47:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  501
 * SCR Title:  Expose Cylindrical Warp and Perspective Warp Tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Interpolation mode to PerspectiveWarpTool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   09 Mar 2005 13:46:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check-in new files for the new perspective tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
