//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBuffer
//* .File Name       : $Workfile:   SVImageBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:28  $
//******************************************************************************

#ifndef SVIMAGEBUFFER_H
#define SVIMAGEBUFFER_H

#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVImageLibrary/SVExtentPointStruct.h"

struct SVOverlayFigureStruct
{
	SVOverlayFigureStruct()
	{
		lFigureSize = 0;
		pFigurePoints = NULL;
		oFigureColor = 0;
	}

	SVOverlayFigureStruct(const SVExtentLineStruct& p_ExtentLineStruct)
	{
		lFigureSize = 0;
		pFigurePoints = NULL;
		*this = p_ExtentLineStruct;
	}

	const SVOverlayFigureStruct& operator = (const SVExtentLineStruct& p_ExtentLineStruct)
	{
		Clear();

		oFigureColor = p_ExtentLineStruct.m_dwColor;

		lFigureSize = static_cast< long >( p_ExtentLineStruct.m_svPointArray.GetSize() );

		pFigurePoints = new POINT[lFigureSize];
		for ( int i = 0; i < lFigureSize; i++ )
		{
			SVExtentPointStruct svExtentPtStruct = p_ExtentLineStruct.m_svPointArray.GetAt(i);
			pFigurePoints[i] = svExtentPtStruct.operator POINT();
		}
		
		return *this;
	}

	~SVOverlayFigureStruct()
	{
		Clear();
	}

	void Clear()
	{
		lFigureSize = 0;

		if( pFigurePoints != NULL )
		{
			delete [] pFigurePoints;
			pFigurePoints = NULL;
		}
	}

	COLORREF oFigureColor;
	long lFigureSize;
	POINT *pFigurePoints;
};// end SVOverlayFigureStruct

struct SVOverlayStruct
{
	SVOverlayStruct()
	{
		lFigureSize = 0;
		pOverlays = NULL;
		oTextColor = 0;
	}
	
	SVOverlayStruct(const SVExtentMultiLineStruct& p_MultiLineStruct)
	{
		lFigureSize = 0;
		pOverlays = NULL;
		*this = p_MultiLineStruct;
	}

	~SVOverlayStruct()
	{
		Clear();
	}

	void Clear()
	{
		for( int i = 0; i < lFigureSize; i++ )
		{
			pOverlays[i].Clear();
		}

		lFigureSize = 0;

		if( pOverlays != NULL )
		{
			delete [] pOverlays;
			pOverlays = NULL;
		}
	}

	const SVOverlayStruct& operator = ( const SVExtentMultiLineStruct& p_rStruct )
	{
		Clear();

		oTextColor = p_rStruct.m_Color;
		oTextPoint = p_rStruct.m_StringPoint.operator POINT();
		strText = p_rStruct.m_csString;
		
		lFigureSize = static_cast< long >( p_rStruct.m_svLineArray.GetSize() );

		pOverlays = new SVOverlayFigureStruct[lFigureSize];

		for ( int i = 0; i < lFigureSize; i++ )
		{
			pOverlays[i] = p_rStruct.m_svLineArray.GetAt(i);
			pOverlays[i].oFigureColor = p_rStruct.m_Color;
		}

		return *this;
	}

	COLORREF oTextColor;
	POINT oTextPoint;
	CString strText;

	long lFigureSize;
	SVOverlayFigureStruct *pOverlays;
};// end SVOverlayStruct

class SVImageOverlayClass
{
public:
	// Construction section
	SVImageOverlayClass();
	SVImageOverlayClass( const SVImageOverlayClass& rhs );
	SVImageOverlayClass(const SVExtentMultiLineStructCArray& p_MultiLineArrayStruct);
	~SVImageOverlayClass();

	const SVImageOverlayClass& operator = ( const SVImageOverlayClass& rhs );
	const SVImageOverlayClass& operator = ( const SVExtentMultiLineStructCArray& p_rMultiLineArrayStruct );

	// General and header section
	long GetBufferSize() const;
	BOOL GetBufferSize( long &lSize ) const;
	BOOL GetBuffer( BYTE *&pBuffer );

	// Overlay section
	BOOL GetOverlayCount( long &lCount ) const;
	BOOL GetOverlayFigureAt( long &lIndex, SVOverlayStruct &oOverlay );

	// General and header section
	BOOL SetBufferSize( long lSize, bool bClearBuffer = true );
	//BOOL SetBuffer( BYTE *&pBuffer, long &lSize );

	// Overlay section
	BOOL SetOverlayCount( long &lCount );
	BOOL SetOverlayFigureAt( long &lIndex, SVOverlayStruct &oOverlay );

	BOOL CalculateSize( long &lSize );

	void Clear();

private:
	void Init();

	long m_lBufferSize;
	BYTE *m_pBuffer;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVImageBuffer.h_v  $
 * 
 *    Rev 1.1   01 Oct 2013 14:19:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:49:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   04 Oct 2012 11:10:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated construction functionality to include additional attribute initialization.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Jul 2012 08:57:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated attribute names in overlay structures.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Dec 2009 11:00:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   20 Mar 2006 13:57:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  547
 * SCR Title:  Fix Image overlay data
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed overlays issue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   17 Jan 2006 10:00:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renamed SVImageBufferClass to SVImageOverlayClass
 * implemented correct copy semantics
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Aug 2005 12:31:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 14:25:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 10:07:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jan 2003 10:12:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified SVImageBufferClass to only deal with overlays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
