//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOInspectionObj
//* .File Name       : $Workfile:   SVOInspectionObj.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:04:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes

class SVOInspectionObj  
{
public:
	SVOInspectionObj();
	virtual ~SVOInspectionObj();

    CString GetInspectionLabelName() const;
    void SetInspectionLabelName(CString sLabel);

    CString GetInspectionName() const;
    void SetInspectionName(CString sInspection);

    CString GetToolsetImage() const;
    void SetToolsetImage(CString sImage);

	CString GetNewDisableMethod() const;
	void SetNewDisableMethod(CString sDisable);

	bool GetShowAuxExtent() const;
	void SetShowAuxExtent( bool ShowAuxExtent );

	long GetEnableAuxiliaryExtent() const;
	void SetEnableAuxiliaryExtent(long lEnable);

	bool HasInspectionNameChange() const;
    CString GetOrginalInspectionName() const;
    void RenameInspection(CString sNewName);
    
	bool IsNewInspection() const;
	void SetNewInspection();

	const CString& GetImportFilename() const;
	void SetImportFilename(const CString& importFilename);
	void ClearImportFilename();

	bool IsColor() const;
	void SetColor( bool Color );

private:
	CString m_sToolsetImage;
    CString m_sLabelName;
    CString m_sInspectionName;
    CString m_sOrginalName;
	CString m_sNewDisableMethod;
	CString m_ImportFilename;
	long m_lEnableAuxiliaryExtent;
	int m_iNumPanels;
	bool m_ShowAuxExtent;
    bool m_NameChanged;
    bool m_NewInspection;
	bool m_Color;
};

typedef SVSharedPtr< SVOInspectionObj > SVOInspectionObjPtr;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOInspectionObj.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:04:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Mar 2013 16:48:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetImportFilename method
 * Added SetImportFilename method
 * Added ClearImportFilename method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Jan 2006 11:19:36   Joe
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
 *    Rev 1.2   05 Aug 2003 13:50:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the saving/loading/editing of configurations to add a new property for the inspection's disable method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:05:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:38:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
