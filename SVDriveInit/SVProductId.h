//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProductId
//* .File Name       : $Workfile:   SVProductId.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 08:33:32  $
//******************************************************************************

#ifndef SVPRODUCTID_H
#define SVPRODUCTID_H

class SVProductId
{
public:
	SVProductId();

	bool UpdateProductId() const;

	LPCTSTR GetSubElement( size_t p_Index ) const;
	HRESULT SetSubElement( size_t p_Index, LPCTSTR p_szData );

private:
	SVProductId( const SVProductId& p_rObject );

	const SVProductId& operator=( const SVProductId& p_rObject );

	bool PrepareForReseal() const;

	bool m_ResealWindows;
	CString m_PreviousProductId;
	TCHAR m_szProductId[ 5 ][ 6 ];


};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVDriveInit\SVProductId.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 08:33:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Nov 2009 09:41:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated software to fix issues with FBReseal.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Nov 2009 18:46:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  668
 * SCR Title:  Upgrade SVObserver version for Barracuda release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated drive init program to fix issues associated with new functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
