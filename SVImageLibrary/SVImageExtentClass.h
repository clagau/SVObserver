//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageExtentClass
//* .File Name       : $Workfile:   SVImageExtentClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:53:38  $
//******************************************************************************

#ifndef SVIMAGEEXTENTCLASS_H
#define SVIMAGEEXTENTCLASS_H

#include <map>
#include <vector>
#include "SVExtentDimensionsClass.h"
#include "SVExtentEnums.h"
#include "SVExtentFigureStruct.h"
#include "SVExtentOffsetStruct.h"
#include "SVExtentPointStruct.h"
#include "SVExtentPositionClass.h"
#include "SVExtentLineStruct.h"
#include "SVExtentMultiLineStruct.h"

extern double SVGetRadius( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svPoint );
extern double SVGetRotationAngle( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svPoint );

extern SVExtentPointStruct SVRotatePoint( SVExtentPointStruct p_svCenter, double p_dRadius, double p_dAngle );
extern SVExtentPointStruct SVRotatePoint( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svStart, double p_dAngle );

extern double SVGetFlippedRotationAngle( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svPoint );
extern SVExtentPointStruct SVRotateAndFlipPoint( SVExtentPointStruct p_svCenter, double p_dRadius, double p_dAngle );
extern SVExtentPointStruct SVRotateAndFlipPoint( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svStart, double p_dAngle );

typedef std::vector<SVExtentPropertyEnum> SVExtentPropertyListType;
typedef std::map< SVExtentPropertyEnum, CString > SVExtentPropertyMapType;

class SVImageExtentClass
{
public:
	SVImageExtentClass();
	SVImageExtentClass( const SVImageExtentClass &p_rsvValue );
	virtual ~SVImageExtentClass();

	const SVImageExtentClass &operator=( const SVImageExtentClass &p_rsvValue );

	virtual HRESULT Initialize();

	virtual SVExtentTranslationEnum GetTranslation() const;
	virtual HRESULT SetTranslation( SVExtentTranslationEnum p_eTranslation );

	virtual const SVExtentDimensionsClass& GetDimensions() const;
	virtual HRESULT SetDimensions( SVExtentDimensionsClass p_svDimensions );

	virtual const SVExtentPositionClass &GetPosition() const;
	virtual HRESULT SetPosition( SVExtentPositionClass p_svPosition );

	virtual HRESULT ClearOldProperties();

	virtual HRESULT DisableExtentProperty( SVExtentPropertyEnum p_eProperty );

	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const;
	virtual HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct &p_rsvValue ) const;
	virtual HRESULT GetExtentPropertyList( SVExtentPropertyEnum p_eWhichProperties, SVExtentPropertyListType& p_rPropertyList ) const;
	virtual HRESULT GetExtentPropertyList( SVExtentPropertyEnum p_eWhichProperties, SVExtentPropertyMapType& p_rPropertyMap ) const;
	static CString GetExtentPropertyName( SVExtentPropertyEnum p_eProperty );

	virtual HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, double p_dValue );
	virtual HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct p_svValue );

	virtual HRESULT UpdateData();

	virtual HRESULT GetFigure( SVExtentFigureStruct &p_rsvFigure ) const;

	virtual SVExtentLocationPropertyEnum GetLocationPropertyAt( SVExtentPointStruct p_svPoint );

	virtual HRESULT Update( SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd );
	virtual HRESULT UpdateFromOutputSpace( SVExtentLocationPropertyEnum p_eLocation, long p_lX, long p_lY );

	/// GetRectangle  
	///  Retrieves the rect values for the input/parent image that is 
	///  referenced by the current Extents.
	virtual HRESULT GetRectangle( RECT &p_roRect ) const;

	/// GetOutputRectangle  
	///  Retrieves the rect values for the output image that is referenced by 
	///  the current Extents.  The translation value is used in determining 
	///  these dimensions.
	virtual HRESULT GetOutputRectangle( RECT &p_roRect ) const;
	
	/// GetLogicalRectangle  
	///  Retrieves the rect values based on the ROI of the current extents.
	virtual HRESULT GetLogicalRectangle( RECT &p_roRect ) const;

	virtual HRESULT TranslateToOutputSpace( SVExtentPointStruct p_svValue, SVExtentPointStruct &p_rsvResult );

	virtual HRESULT TranslateFromOutputSpace( SVExtentPointStruct p_svValue, SVExtentPointStruct &p_rsvResult ) const;
	virtual HRESULT TranslateFromOutputSpace( SVExtentFigureStruct p_svValue, SVExtentFigureStruct &p_rsvResult ) const;
	virtual HRESULT TranslateFromOutputSpace( SVExtentLineStruct p_svValue, SVExtentLineStruct &p_rsvResult ) const;
	virtual HRESULT TranslateFromOutputSpace( SVExtentMultiLineStruct p_svValue, SVExtentMultiLineStruct &p_rsvResult ) const;

	virtual HRESULT TranslateLineFromOutputSpace( SVExtentPointStruct p_svRadiusPoint, SVExtentPointStruct p_svRotatePoint, double p_dLineAngle, SVExtentPointStruct p_svValue, SVExtentPointStruct &p_rsvResult ) const;

	virtual HRESULT ValidateAgainstOutputSpace( const SVImageExtentClass &p_rsvValue ) const;

	virtual HRESULT GetTitlePoint(POINT &p_ptTitlePoint) const;
	virtual HRESULT GetTitlePoint(SVExtentPointStruct &p_svTitlePoint) const;

	bool IsEqualExcludePosition( const SVImageExtentClass &p_rsvValue ) const;
	bool IsPositionEqual( const SVImageExtentClass &p_rsvValue ) const;

	bool operator==( const SVImageExtentClass &p_rsvValue ) const;
	bool operator!=( const SVImageExtentClass &p_rsvValue ) const;

	virtual HRESULT UpdateSourceOffset( SVExtentOffsetStruct& p_rsvOffsetData );

protected:
	virtual HRESULT UpdateLine( SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd );
	virtual HRESULT UpdatePolar( SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd );
	virtual HRESULT UpdatePolarFromOutputSpace( SVExtentLocationPropertyEnum p_eLocation, long p_dX, long p_dY );
	virtual HRESULT UpdateHorizontalPerspective( SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd );
	virtual HRESULT UpdateVerticalPerspective( SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd );

	virtual HRESULT ClearOutputData();

	virtual HRESULT BuildOutputDimensions();
	virtual HRESULT BuildFigure();

private:
	/// TranslateToLocalSpace
	///	 Translate the point to be relative (local space).  Usually this relates 
	///  to the ROI of the parent image.
	HRESULT TranslateToLocalSpace(const SVExtentPointStruct& rValue, SVExtentPointStruct& rResult);
	// Input Attributes
	SVExtentTranslationEnum m_eTranslation;
	SVExtentPositionClass m_svPosition;
	SVExtentDimensionsClass m_svDimensions;

	// Output Attributes
	SVExtentFigureStruct m_svFigure;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageLibrary\SVImageExtentClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:53:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   11 Jan 2013 14:57:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new methods to compare image information that either includes positional information or excludes positional information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Jul 2012 15:44:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Get Title Point to a const method since it does not changes any internal attributes and allows it to be called by a const object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Jul 2012 08:22:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated attribute names and updated methods to const to make variables more clear.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Mar 2011 13:13:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   10 Feb 2011 13:48:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   28 Oct 2010 13:32:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   15 Dec 2009 11:34:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Jan 2006 16:49:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVImageExtentClass to use new SVExtentOffsetStruct for managing the extent offset data. Fixed check in problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2006 16:40:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVImageExtentClass to use new SVExtentOffsetStruct for managing the extent offset data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Sep 2005 10:36:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified UpdateSourceOffset to change bOffsetValid flag depending upon translation.  Also added case for warp translations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   01 Sep 2005 12:21:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added UpdateSourceOffset(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jun 2005 13:38:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   using new SVExtentTranslationFigureShift translation type and new method GetTitlePoint
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Mar 2005 09:03:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated Line data to include draw angle to remove rounding issues with drawing straight lines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Mar 2005 14:01:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  478
 * SCR Title:  Add a new Perspective Warp Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support the new Perspective Warp Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Mar 2005 09:40:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   modified public functions returning HRESULT to use SVHRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:04:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First Iteration of File
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
