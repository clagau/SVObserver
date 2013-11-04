//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageInfoClass
//* .File Name       : $Workfile:   SVImageInfoClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:53:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageInfoClass.h"

#include "SVHBitmapUtilitiesLibrary/SVHBitmapUtilities.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SVImageInfoClass::SVImageInfoClass()
{
	Initialize();
}

SVImageInfoClass::SVImageInfoClass( const SVImageInfoClass &p_rsvValue )
{
	Initialize();

	*this = p_rsvValue;
}

SVImageInfoClass::SVImageInfoClass( const BITMAPINFOHEADER& p_rBitmapHeader )
{
	*this = p_rBitmapHeader;
}

SVImageInfoClass::~SVImageInfoClass()
{
	Initialize();
}

const SVImageInfoClass &SVImageInfoClass::operator=( const SVImageInfoClass &p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		m_OwnerImageID = p_rsvValue.m_OwnerImageID;
		m_OwnerObjectID = p_rsvValue.m_OwnerObjectID;
		m_svExtents = p_rsvValue.m_svExtents;
		m_svProperties = p_rsvValue.m_svProperties;

		m_svExtents.UpdateData();
	}
	return ( *this );
}


// And how 
const SVImageInfoClass &SVImageInfoClass::operator=( const BITMAPINFOHEADER& p_rBitmapHeader )
{
	HRESULT l_hr = 0;
	Initialize();

	int l_iFormat = SVImageFormatUnknown;
	if( p_rBitmapHeader.biBitCount == 8 )
	{
		l_iFormat = SVImageFormatMono8;
	}
	else
	if( p_rBitmapHeader.biBitCount == 24 )
	{
		l_iFormat = SVImageFormatRGB888;
	}
	else
	if( p_rBitmapHeader.biBitCount == 32 )
	{
		l_iFormat = SVImageFormatRGB8888;
	}
	SetImageProperty( SVImagePropertyFormat, l_iFormat );

	SetImageProperty( SVImagePropertyPixelDepth, p_rBitmapHeader.biBitCount );
	SetImageProperty( SVImagePropertyBandNumber, 1 );
	SetImageProperty( SVImagePropertyBandLink, 0 );

	SetExtentProperty( SVExtentPropertyHeight, abs(p_rBitmapHeader.biHeight) );
	SetExtentProperty( SVExtentPropertyWidth, p_rBitmapHeader.biWidth );

	if( p_rBitmapHeader.biBitCount == 24 )
	{
		SetImageProperty( SVImagePropertyPixelDepth, 8 );
		SetImageProperty( SVImagePropertyBandNumber, 3 );
	}

	return *this;
}

HRESULT SVImageInfoClass::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_OwnerImageID.clear();

	m_OwnerObjectID.clear();

	if ( m_svExtents.Initialize() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svProperties.Initialize() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svExtents.SetTranslation( SVExtentTranslationShift ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svExtents.SetExtentProperty( SVExtentPropertyPositionPoint, 10.0 ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svExtents.SetExtentProperty( SVExtentPropertyRectangle, 100.0 ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svExtents.SetExtentProperty( SVExtentPropertyRotationAngle, 0.0 ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_svExtents.SetExtentProperty( SVExtentPropertyTranslationOffset, 0.0 ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( SetImageProperty( SVImagePropertyFormat, SVImageFormatMono8 ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( SetImageProperty( SVImagePropertyPixelDepth, 8 ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( SetImageProperty( SVImagePropertyBandNumber, 1 ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( SetImageProperty( SVImagePropertyBandLink, 0 ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( SetImageProperty( SVImagePropertyResetOwner, FALSE ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

SVImagePropertiesClass SVImageInfoClass::GetImageProperties() const
{
	return m_svProperties;
}

HRESULT SVImageInfoClass::SetImageProperties( SVImagePropertiesClass p_svImageProperties )
{
	m_svProperties = p_svImageProperties;

	return S_OK;
}

SVImageExtentClass SVImageInfoClass::GetExtents() const
{
	return m_svExtents;
}

HRESULT SVImageInfoClass::SetExtents( SVImageExtentClass p_svExtents )
{
	m_svExtents = p_svExtents;

	HRESULT l_hrOk = m_svExtents.UpdateData();

	return l_hrOk;
}

SVExtentTranslationEnum SVImageInfoClass::GetTranslation()
{
	return m_svExtents.GetTranslation();
}

HRESULT SVImageInfoClass::SetTranslation( SVExtentTranslationEnum p_eTranslation )
{
	HRESULT l_hrOk = m_svExtents.SetTranslation( p_eTranslation );

	if ( l_hrOk == S_OK )
	{
		l_hrOk = m_svExtents.UpdateData();
	}

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetImageProperty( SVImagePropertyEnum p_eProperty, int &p_riValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svProperties.GetImageProperty( p_eProperty, p_riValue );

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetImageProperty( SVImagePropertyEnum p_eProperty, long &p_rlValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svProperties.GetImageProperty( p_eProperty, p_rlValue );

	return l_hrOk;
}

HRESULT SVImageInfoClass::SetImageProperty( SVImagePropertyEnum p_eProperty, long p_lValue )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svProperties.SetImageProperty( p_eProperty, p_lValue );

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	double l_dValue = 0.0;

	l_hrOk = GetExtentProperty( p_eProperty, l_dValue );

	if ( l_hrOk == S_OK )
	{
		p_rlValue = static_cast<long>(l_dValue);
	}

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svExtents.GetExtentProperty( p_eProperty, p_rdValue );

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svExtents.GetExtentProperty( p_eProperty, p_roValue );

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct &p_rsvValue ) const
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svExtents.GetExtentProperty( p_eProperty, p_rsvValue );

	return l_hrOk;
}

HRESULT SVImageInfoClass::SetExtentProperty( SVExtentPropertyEnum p_eProperty, double p_dValue )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svExtents.SetExtentProperty( p_eProperty, p_dValue );

	if ( l_hrOk == S_OK )
	{
		l_hrOk = m_svExtents.UpdateData();
	}

	return l_hrOk;
}

HRESULT SVImageInfoClass::SetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct p_svValue )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svExtents.SetExtentProperty( p_eProperty, p_svValue );

	if ( l_hrOk == S_OK )
	{
		l_hrOk = m_svExtents.UpdateData();
	}

	return l_hrOk;
}

HRESULT SVImageInfoClass::GetImageExtentsToFit( SVImageExtentClass p_svInExtent, SVImageExtentClass &p_rsvOutExtent )
{
	HRESULT l_hrOk = S_FALSE;

	RECT l_oNewRect;
	RECT l_oParentRect;

	SVExtentPointStruct l_svPositionPoint;

	if( GetOutputRectangle( l_oParentRect ) == S_OK &&
			p_svInExtent.GetRectangle( l_oNewRect ) == S_OK &&
			p_svInExtent.GetExtentProperty( SVExtentPropertyPositionPoint, l_svPositionPoint ) == S_OK )
	{
		long l_lWidth = l_oNewRect.right - l_oNewRect.left;
		long l_lHeight = l_oNewRect.bottom - l_oNewRect.top;

		if( l_oParentRect.right - l_oParentRect.left < l_lWidth )
		{
			l_lWidth = l_oParentRect.right - l_oParentRect.left;
		}

		if( l_oParentRect.bottom - l_oParentRect.top < l_lHeight )
		{
			l_lHeight = l_oParentRect.bottom - l_oParentRect.top;
		}

		if( l_oNewRect.left < 0 || ( l_oParentRect.right - l_oParentRect.left < l_oNewRect.right - l_oNewRect.left ) )
		{
			l_svPositionPoint.m_dPositionX = 0.0;
		}
		else if( l_oParentRect.right < l_oNewRect.right )
		{
			l_svPositionPoint.m_dPositionX -= l_oNewRect.right - l_oParentRect.right;
		}

		if( l_oNewRect.top < 0  || ( l_oParentRect.bottom - l_oParentRect.top < l_oNewRect.bottom - l_oNewRect.top ) )
		{
			l_svPositionPoint.m_dPositionY = 0.0;
		}
		else if( l_oParentRect.bottom < l_oNewRect.bottom )
		{
			l_svPositionPoint.m_dPositionY -= l_oNewRect.bottom - l_oParentRect.bottom;
		}

		l_hrOk = p_svInExtent.SetExtentProperty( SVExtentPropertyPositionPoint, l_svPositionPoint );

		if( l_hrOk == S_OK )
		{
			l_hrOk = p_svInExtent.SetExtentProperty( SVExtentPropertyWidth, l_lWidth );
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = p_svInExtent.SetExtentProperty( SVExtentPropertyHeight, l_lHeight );
		}

		if( l_hrOk == S_OK )
		{
			p_rsvOutExtent = p_svInExtent;
		}
	}
	
	return l_hrOk;
}

HRESULT SVImageInfoClass::GetOutputRectangle( RECT &p_roRect ) const
{
	return m_svExtents.GetOutputRectangle( p_roRect );
}

HRESULT SVImageInfoClass::GetFigure( SVExtentFigureStruct &p_rsvFigure )
{
	return m_svExtents.GetFigure( p_rsvFigure );
}

HRESULT SVImageInfoClass::GetOwnerImage( SVObjectClass*& p_rpObject ) const
{
	HRESULT l_Status = S_OK;

	p_rpObject = SVObjectManagerClass::Instance().GetObject( m_OwnerImageID );

	if( p_rpObject == NULL )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

const SVGUID& SVImageInfoClass::GetOwnerImageID() const
{
	return m_OwnerImageID;
}

void SVImageInfoClass::SetOwnerImage( const GUID& p_rObjectID )
{
	m_OwnerImageID = p_rObjectID;
}

SVObjectClass* SVImageInfoClass::GetOwner() const
{
	SVObjectClass* l_pObject = NULL;

	l_pObject = SVObjectManagerClass::Instance().GetObject( m_OwnerObjectID );

	return l_pObject;
}

const SVGUID& SVImageInfoClass::GetOwnerID() const
{
	return m_OwnerObjectID;
}

void SVImageInfoClass::SetOwner( const GUID& p_rObjectID )
{
	m_OwnerObjectID = p_rObjectID;
}

HRESULT SVImageInfoClass::ValidateAgainstOutputSpace( const SVImageExtentClass &p_rsvValue ) const
{
	return m_svExtents.ValidateAgainstOutputSpace( p_rsvValue );
}

bool SVImageInfoClass::IsEqualExcludePosition( const SVImageInfoClass &p_rsvValue ) const
{
	bool l_bOk = true;
	
	l_bOk = l_bOk && m_OwnerImageID == p_rsvValue.m_OwnerImageID;
	l_bOk = l_bOk && m_OwnerObjectID == p_rsvValue.m_OwnerObjectID;
	l_bOk = l_bOk && m_svExtents.IsEqualExcludePosition( p_rsvValue.m_svExtents );
	l_bOk = l_bOk && m_svProperties == p_rsvValue.m_svProperties;

	return l_bOk;
}

bool SVImageInfoClass::IsPositionEqual( const SVImageInfoClass &p_rsvValue ) const
{
	return m_svExtents.IsPositionEqual( p_rsvValue.m_svExtents );
}

bool SVImageInfoClass::operator==( const SVImageInfoClass &p_rsvValue ) const
{
	bool l_bOk = true;
	
	l_bOk = l_bOk && m_OwnerImageID == p_rsvValue.m_OwnerImageID;
	l_bOk = l_bOk && m_OwnerObjectID == p_rsvValue.m_OwnerObjectID;
	l_bOk = l_bOk && m_svExtents == p_rsvValue.m_svExtents;
	l_bOk = l_bOk && m_svProperties == p_rsvValue.m_svProperties;

	return l_bOk;
}

bool SVImageInfoClass::operator!=( const SVImageInfoClass &p_rsvValue ) const
{
	return ! ( *this == p_rsvValue );
}

long SVImageInfoClass::GetBufferSize()
{
	long lDepth = 1;
	CRect rect;
	m_svExtents.GetOutputRectangle(rect);
	long lHeight = rect.Height();
	long lWidth = rect.Width();
	long lFormat;
	m_svProperties.GetImageProperty( SVImagePropertyFormat, lFormat );

	switch ( lFormat )
	{
		case SVImageFormatMono8:  // Mono
		{
			break;
		}
		case SVImageFormatRGB888:   // RGB
		{
			lDepth = 3;
			break;
		}
		case SVImageFormatRGB8888:  // RGB
		{
			lDepth = 4;
			break;
		}
	}
	return lHeight * lWidth * lDepth;
}

BITMAPINFOHEADER SVImageInfoClass::GetBitmapInfoHeader() const
{
	HRESULT l_hr = S_OK;
	BITMAPINFOHEADER l_info;
	int l_iPixelDepth=0;
	long l_iWidth = 0;
	long l_iHeight = 0;
	int l_iFormat = 0;
	int l_iBands = 0;

	l_hr = GetImageProperty( SVImagePropertyPixelDepth, l_iPixelDepth );
	l_hr = m_svExtents.GetExtentProperty( SVExtentPropertyOutputWidth, l_iWidth );
	l_hr = m_svExtents.GetExtentProperty( SVExtentPropertyOutputHeight, l_iHeight );
	l_hr = GetImageProperty( SVImagePropertyFormat, l_iFormat );
	l_hr = GetImageProperty( SVImagePropertyBandNumber, l_iBands );


	l_info.biWidth = l_iWidth;
	l_info.biHeight = l_iHeight;
	l_info.biBitCount = l_iPixelDepth;

	WORD wClrBits = 8;
	if ( l_iFormat == SVImageFormatRGB888 )
		wClrBits = 24;
	else if ( l_iFormat == SVImageFormatRGB8888 )
		wClrBits = 32;

	l_info.biSize = sizeof(BITMAPINFOHEADER); 
	l_info.biPlanes = 1;
	l_info.biBitCount = wClrBits; 
	if (wClrBits < 24)
	{
		l_info.biClrUsed = (1<<wClrBits); 
	}
	
	// If the bitmap is not compressed, set the BI_RGB flag. 
	l_info.biCompression = BI_RGB;
	
	// Compute the number of bytes in the array of color 
	// indices and store the result in biSizeImage. 
	// For Windows NT, the width must be DWORD aligned unless 
	// the bitmap is RLE compressed.
	l_info.biSizeImage = ((abs(l_info.biWidth) * wClrBits +31) & ~31) /8 * abs(l_info.biHeight); 

	// 0 indicates that all colors are important.
	l_info.biClrImportant = 0;
	
	return l_info;
}

SVImageInfoClass::operator BITMAPINFOHEADER () const
{
	return GetBitmapInfoHeader();
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVImageInfoClass.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:53:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   11 Jan 2013 14:57:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new methods to compare image information that either includes positional information or excludes positional information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   17 Aug 2012 16:13:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent caculations to correct for parent image shrinking smaller than child image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   12 Oct 2011 14:28:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues with extent data for transformation tool and polar unwrap tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   24 May 2011 15:21:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to add ability to get the Owner Object ID.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   13 Apr 2011 14:47:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   21 Mar 2011 11:36:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Image Info to contain the logic to automaticly move and resize image extents that was contained within the image class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Mar 2011 13:13:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   25 Feb 2011 11:51:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   10 Feb 2011 13:48:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   27 Jan 2011 09:54:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Dec 2010 15:41:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Dec 2009 11:34:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Jul 2009 15:38:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 Jul 2007 09:00:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Operator=( BITMAPINFOHEADER )
 * Added Constructor( BITMAPINFOHEADER )
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Jun 2007 11:11:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Nov 2005 15:18:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include the new parameter functions for the DLL and new image formats for the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Jun 2005 10:02:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added self assignment check in operator =
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 May 2005 12:11:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetBufferSize
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Feb 2005 10:20:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moving files from SVObserver project to this project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   12 Jul 2004 10:24:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made GetWidth and GetHeight const-correct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Aug 2003 12:59:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added equality check in operator =
 * cleaned up tabs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 10:07:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jan 2003 16:31:24   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed global function SVFillImageInfo from files.  Updated all methods to remove type casting warning messages.  Added image type and derived flag to class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Apr 2001 14:25:28   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   New file due to header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
