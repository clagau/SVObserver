//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageInfoClass
//* .File Name       : $Workfile:   SVImageInfoClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:53:46  $
//******************************************************************************

#ifndef SVIMAGEINFOCLASS_H
#define SVIMAGEINFOCLASS_H

#include "SVUtilityLibrary/SVGUID.h"
#include "SVImagePropertiesClass.h"
#include "SVImageExtentClass.h"

class SVImageClass;
class SVObjectClass;

class SVImageInfoClass
{
public:
	SVImageInfoClass();
	SVImageInfoClass( const SVImageInfoClass& p_rsvValue );
	SVImageInfoClass( const BITMAPINFOHEADER& p_rBitmapHeader );

	virtual ~SVImageInfoClass();

	const SVImageInfoClass &operator=( const SVImageInfoClass& p_rsvValue );
	const SVImageInfoClass &operator=( const BITMAPINFOHEADER& p_rBitmapHeader );

	virtual HRESULT Initialize();

	virtual SVImagePropertiesClass GetImageProperties() const;
	virtual HRESULT SetImageProperties( SVImagePropertiesClass p_svImageProperties );

	virtual SVImageExtentClass GetExtents() const;
	virtual HRESULT SetExtents( SVImageExtentClass p_svExtents );

	virtual SVExtentTranslationEnum GetTranslation();
	virtual HRESULT SetTranslation( SVExtentTranslationEnum p_eTranslation );

	virtual HRESULT GetImageProperty( SVImagePropertyEnum p_eDefinition, int &p_riValue ) const;
	virtual HRESULT GetImageProperty( SVImagePropertyEnum p_eDefinition, long &p_rlValue ) const;
	virtual HRESULT SetImageProperty( SVImagePropertyEnum p_eDefinition, long p_lValue );

	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct &p_rsvValue ) const;
	virtual HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, double p_dValue );
	virtual HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct p_svValue );

	virtual HRESULT GetImageExtentsToFit( SVImageExtentClass p_svInExtent, SVImageExtentClass &p_rsvOutExtent );

	virtual HRESULT GetOutputRectangle( RECT &p_roRect ) const;

	virtual HRESULT GetFigure( SVExtentFigureStruct &p_rsvFigure );

	long GetBufferSize();

	template< typename SV_OBJECT_TYPE >
	HRESULT GetOwnerImage( SV_OBJECT_TYPE*& p_rpObject ) const;

	HRESULT GetOwnerImage( SVObjectClass*& p_rpObject ) const;
	const SVGUID& GetOwnerImageID() const;
	void SetOwnerImage( const GUID& p_rObjectID );

	SVObjectClass* GetOwner() const;
	const SVGUID& GetOwnerID() const;
	void SetOwner( const GUID& p_rObjectID );

	HRESULT ValidateAgainstOutputSpace( const SVImageExtentClass &p_rsvValue ) const;

	bool IsEqualExcludePosition( const SVImageInfoClass &p_rsvValue ) const;
	bool IsPositionEqual( const SVImageInfoClass &p_rsvValue ) const;

	bool operator==( const SVImageInfoClass &p_rsvValue ) const;
	bool operator!=( const SVImageInfoClass &p_rsvValue ) const;

	BITMAPINFOHEADER GetBitmapInfoHeader() const;
	operator BITMAPINFOHEADER () const;

	void setDibBufferFlag(bool isDibBuffer) { m_isDibBuffer = isDibBuffer; };
	bool getDibBufferFlag() const { return m_isDibBuffer; };

private:
	SVGUID m_OwnerImageID;
	SVGUID m_OwnerObjectID;

	SVImageExtentClass m_svExtents;
	SVImagePropertiesClass m_svProperties;
	bool m_isDibBuffer;
};

#include "SVImageInfoClass.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVImageInfoClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:53:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   11 Jan 2013 14:57:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new methods to compare image information that either includes positional information or excludes positional information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   24 May 2011 15:21:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to add ability to get the Owner Object ID.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   13 Apr 2011 14:47:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   21 Mar 2011 11:36:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Image Info to contain the logic to automaticly move and resize image extents that was contained within the image class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Mar 2011 13:13:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   25 Feb 2011 11:51:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Feb 2011 13:48:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Jan 2011 09:54:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jul 2009 15:38:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Jul 2007 09:00:00   tbair
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
 *    Rev 1.0   17 Feb 2005 10:20:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moving files from SVObserver project to this project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Jul 2004 10:24:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made GetWidth and GetHeight const-correct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 10:07:50   rschock
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
 *    Rev 1.0   19 Apr 2001 14:26:20   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   New file due to header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

