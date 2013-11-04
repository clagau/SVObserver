//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolExtentClass
//* .File Name       : $Workfile:   SVToolExtentClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:40:18  $
//******************************************************************************

#include "SVUtilityLibrary/SVUtilityGlobals.h"

inline HRESULT SVToolExtentClass::Initialize()
{
	HRESULT l_svOk = S_OK;

	m_psvTool = NULL;
	m_psvToolImage = NULL;

	m_eImageType = SVImageTypeUnknown;

	m_eTranslation = SVExtentTranslationUnknown;
	m_eShape = SVExtentShapeUnknown;

	KeepPrevError( l_svOk, m_svProperties.Initialize() );

	m_bAlwaysUpdate = true;

	m_psvSelectedImage = NULL;

	KeepPrevError( l_svOk, m_svRootOffsetData.Initialize() );
	KeepPrevError( l_svOk, m_svSelectedOffsetData.Initialize() );

	return l_svOk;
}

inline SVToolClass* SVToolExtentClass::GetTool() const
{
	return m_psvTool;
}

inline void SVToolExtentClass::SetTool( SVToolClass *p_psvTool )
{ 
	m_psvTool = p_psvTool;
}

inline SVImageClass* SVToolExtentClass::GetToolImage() const
{
	return m_psvToolImage;
}

inline void SVToolExtentClass::SetToolImage( SVImageClass *p_psvToolImage )
{ 
	if( m_psvToolImage != p_psvToolImage )
	{
		m_psvToolImage = p_psvToolImage;

		m_svRootOffsetData.Initialize();
		m_svSelectedOffsetData.Initialize();
	}
}

inline SVImageTypeEnum SVToolExtentClass::GetImageType() const
{
	return m_eImageType;
}

inline void SVToolExtentClass::SetImageType( SVImageTypeEnum p_eImageType )
{
	m_eImageType = p_eImageType;
}

inline SVImageClass* SVToolExtentClass::GetSelectedImage() const
{
	return m_psvSelectedImage;
}

inline void SVToolExtentClass::SetSelectedImage( SVImageClass* p_psvSelectedImage )
{
	if( m_psvSelectedImage != p_psvSelectedImage )
	{
		m_psvSelectedImage = p_psvSelectedImage;

		m_svSelectedOffsetData.Initialize();
	}
}

inline bool SVToolExtentClass::GetAlwaysUpdate() const
{
	return m_bAlwaysUpdate;
}

inline void SVToolExtentClass::SetAlwaysUpdate( bool p_bAlwaysUpdate )
{
	m_bAlwaysUpdate = p_bAlwaysUpdate;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolExtentClass.inl_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:40:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   11 Mar 2011 14:33:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Oct 2010 14:16:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   28 May 2010 14:30:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with includes and make them consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Dec 2009 13:20:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Jan 2006 17:36:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Sep 2005 12:07:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix bug with un-initialized HResult that caused Auxilliary Extents to be wrong.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Sep 2005 12:18:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New File to support Auxilliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
