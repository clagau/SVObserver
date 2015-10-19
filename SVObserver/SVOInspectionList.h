//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOInspectionList
//* .File Name       : $Workfile:   SVOInspectionList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:04:18  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVContainerLibrary\SVList.h"
#include "SVOInspectionObj.h"
#pragma endregion Includes

class SVOInspectionList  
{
public:
	typedef SVList< SVOInspectionObjPtr > SVInspectionList;
	typedef SVInspectionList::iterator iterator;
	typedef SVInspectionList::const_iterator const_iterator;

	SVOInspectionList();
	virtual ~SVOInspectionList();

	BOOL AddInspectionToList(CString sInspectLabel, CString sInspectName, bool NewInspection);
	BOOL SetToolsetImage(CString sInspectLabel, CString sImage);
	BOOL SetColor(CString sInspectLabel, bool Color);
	BOOL SetNewDisableMethod(CString sInspectLabel, CString sDisable);
	BOOL SetEnableAuxiliaryExtent(CString sInspectLabel, long sEnable);
	BOOL SetShowAuxExtent(CString sInspectLabel, bool p_bShow);
	BOOL ReNameInspection(CString sInspectLabel, CString sNewInspectName);
	BOOL RemoveInspectionFromList(CString sInspectLabel);

	CString GetInspectionName(CString sInspectLabel);
	CString GetInspectionLabel(CString sInspectName);
	SVOInspectionObjPtr GetInspectionByName(CString sInspectLabel);
	SVOInspectionObjPtr GetInspectionByPosition(int iPos);
	BOOL IsInspectionInList(CString sInspectLabel) const;
	BOOL IsInspectionNameInList(CString sInspectName) const;
	BOOL IsInspectionInList(CString sInspectLabel, CString sInspectName) const;

	void ResetContent();
	int GetInspectionListCount() const;

private:
	SVInspectionList m_InspectionList;

	iterator FindInspectionPosition(CString sInspectLabel);
	iterator FindInspectionPositionFromName(CString sInspectName);


};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOInspectionList.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:04:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Nov 2010 16:25:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Sep 2008 16:36:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetInspectionListCount to be const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Jan 2006 11:19:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to hide Aux Extent property when a gage tool is in the toolset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Oct 2005 09:40:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support adding a flag to the Inspection that will enable Auxiliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Aug 2003 13:50:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the saving/loading/editing of configurations to add a new property for the inspection's disable method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:05:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:38:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
