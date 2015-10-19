//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOInspectionObj
//* .File Name       : $Workfile:   SVOInspectionObj.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:04:30  $
//******************************************************************************

#include "stdafx.h"
#include "SVOInspectionObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVOInspectionObj::SVOInspectionObj() :
	m_NameChanged( false )
,	m_NewInspection( false )
,	m_sNewDisableMethod( _T( "Method 1" ) )
,	m_Color( false )
,	m_ShowAuxExtent( true )
{
}

SVOInspectionObj::~SVOInspectionObj()
{
}

CString SVOInspectionObj::GetInspectionLabelName() const
{
    return m_sLabelName;
}

void SVOInspectionObj::SetInspectionLabelName(CString sLabel)
{
    m_sLabelName = sLabel;
}

CString SVOInspectionObj::GetInspectionName() const
{
    return m_sInspectionName;
}

void SVOInspectionObj::SetInspectionName(CString sInspection)
{
    m_sInspectionName = sInspection;
}

CString SVOInspectionObj::GetToolsetImage() const
{
    return m_sToolsetImage; 
}

void SVOInspectionObj::SetToolsetImage(CString sImage)
{
    m_sToolsetImage = sImage;
} 

CString SVOInspectionObj::GetNewDisableMethod() const
{
    return m_sNewDisableMethod;
}

void SVOInspectionObj::SetNewDisableMethod(CString sDisable)
{
    m_sNewDisableMethod = sDisable;
} 

bool SVOInspectionObj::GetShowAuxExtent() const
{
	return m_ShowAuxExtent;
}

void SVOInspectionObj::SetShowAuxExtent( bool ShowAuxExtent )
{
	 m_ShowAuxExtent = ShowAuxExtent;
}

long SVOInspectionObj::GetEnableAuxiliaryExtent() const
{
    return m_lEnableAuxiliaryExtent; 
}

void SVOInspectionObj::SetEnableAuxiliaryExtent( long  lEnable)
{
    m_lEnableAuxiliaryExtent = lEnable;
} 

bool SVOInspectionObj::HasInspectionNameChange() const
{
    return m_NameChanged;
}

CString SVOInspectionObj::GetOrginalInspectionName() const
{
    return m_sOrginalName;
}

void SVOInspectionObj::RenameInspection(CString sNewName)
{
    if (!m_NameChanged)
    {
        //only change once...
        m_NameChanged = true;
        m_sOrginalName = m_sInspectionName;
    }
    m_sInspectionName = sNewName;
}

bool SVOInspectionObj::IsNewInspection() const
{
	return m_NewInspection;
}

void SVOInspectionObj::SetNewInspection()
{
    m_NewInspection = true;
}

const CString& SVOInspectionObj::GetImportFilename() const
{
	return m_ImportFilename;
}

void SVOInspectionObj::SetImportFilename(const CString& filename)
{
	m_ImportFilename = filename;
}

void SVOInspectionObj::ClearImportFilename()
{
	m_ImportFilename.Empty();
}

bool SVOInspectionObj::IsColor() const
{
	return m_Color;
}

void SVOInspectionObj::SetColor( bool Color )
{
	m_Color = Color;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOInspectionObj.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:04:30   bWalter
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
 *    Rev 1.1   22 Apr 2003 13:05:54   rschock
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
