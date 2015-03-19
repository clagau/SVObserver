//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathTool
//* .File Name       : $Workfile:   SVMathTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:29:58  $
//******************************************************************************

#ifndef SVMATHTOOL_H
#define SVMATHTOOL_H

#include "SVTool.h"

class SVMathEquationClass;

class SVMathToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVMathToolClass );

public:
	SVMathToolClass( BOOL BCreateDefaultTaskList=FALSE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVMATHTOOL );

	virtual ~SVMathToolClass();

	BOOL CreateObject(SVObjectLevelCreateStruct* PCreateStruct );

	virtual BOOL OnValidate();

protected:
	virtual void init(void);
	void HideRangeIndirectValueObjects();
	virtual DWORD_PTR processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext);

	SVMathEquationClass* pMathEquation;
};

#endif
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMathTool.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:29:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   25 Jul 2012 14:31:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   02 Jul 2012 17:19:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   17 Feb 2005 14:48:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   22 Apr 2003 11:16:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   30 Jan 2003 15:02:00   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   20 Nov 2002 08:32:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   21 Feb 2000 10:07:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed isMoveable and isFreeMoveable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:08:06   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Sep 1999 11:14:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Obsolete methods.
 * Added result object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   11 Sep 1999 19:58:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVMathEquationClass as a friend.
 * Added mathFigure.
 * Removed obsolete code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Sep 10 1999 18:07:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Removed obsolete constructors and Serialize().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Sep 1999 11:50:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Run logic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
