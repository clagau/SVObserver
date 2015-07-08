//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolExtentClass
//* .File Name       : $Workfile:   SVToolExtentClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 08:21:22  $
//******************************************************************************

#ifndef _SVTOOLEXTENTCLASS_H
#define _SVTOOLEXTENTCLASS_H

#include <set>
#include "ObjectInterfaces/SVImageEnums.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVToolExtentPropertiesClass.h"

class SVImageClass;
class SVToolClass;

class SVToolExtentClass
{
public:
	enum SVToolExtentTypeEnum
	{
		SVUnknownToolExtent        = 0,
		SVToolExtent               = 1,
		SVTransformationToolExtent = 2,
		SVColorToolExtent          = 3,
		SVRebuildOnResizeToolExtent = 4,
	};

	SVToolExtentClass();
	virtual ~SVToolExtentClass();

	inline HRESULT Initialize();

	inline SVToolClass* GetTool() const;
	inline void SetTool( SVToolClass* p_psvTool );

	inline SVImageClass* GetToolImage() const;
	inline void SetToolImage( SVImageClass* p_psvToolImage );

	inline SVImageTypeEnum GetImageType() const;
	inline void SetImageType( SVImageTypeEnum p_eImageType );

	inline SVImageClass* GetSelectedImage() const;
	inline void SetSelectedImage( SVImageClass* p_psvSelectedImage );

	inline bool GetAlwaysUpdate() const;
	inline void SetAlwaysUpdate( bool p_bAlwaysUpdate );

	HRESULT ValidExtentAgainstParentImage( const SVImageExtentClass& p_rImageExtent ) const;
	HRESULT UpdateExtentToParentExtents( unsigned long p_ulIndex, SVImageExtentClass& p_rNewExtent  );
	HRESULT UpdateExtentAgainstParentImage( unsigned long p_ulIndex, const SVImageExtentClass& p_rImageExtent );
	HRESULT GetParentExtent( SVImageExtentClass& p_rParent ) const;

	HRESULT UpdateImageWithExtent( unsigned long p_ulIndex, SVToolExtentTypeEnum p_ToolExtentType );

	virtual SVExtentTranslationEnum GetTranslation();
	virtual HRESULT SetTranslation( SVExtentTranslationEnum p_eTranslation );
	virtual HRESULT SetTranslation( SVExtentTranslationEnum p_eTranslation, long p_DataIndex );

	virtual HRESULT GetExtentShape( SVExtentPropertyEnum p_eProperty, SVExtentShapeEnum &p_reValue ) const;

	virtual HRESULT GetExtentObject( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *&p_rpsvValue ) const;
	virtual HRESULT SetExtentObject( SVExtentPropertyEnum p_eProperty, SVValueObjectClass *p_psvValue );

	virtual HRESULT GetExtentValue( SVExtentPropertyEnum p_eProperty, VARIANT& p_rValue ) const;
	virtual HRESULT SetExtentValue( SVExtentPropertyEnum p_eProperty, long p_DataIndex, const _variant_t& p_rValue );

	virtual HRESULT GetExtentPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const;
	virtual HRESULT SetExtentPropertyInfo( SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo );

	
//- GetImageExtent -----------------------------------------------------------
//- There appears to be little direct connection between the SVToolExtentClass
//- and the SVImageExtentClass.  This function appears to attempt to translate  
//- between the two.  Translating and copying from the SVToolExtentClass based 
//- structure into the SVImageExtentClass based structure. -------------------
	virtual HRESULT GetImageExtent( SVImageExtentClass &p_rsvImageExtent )const ;

	virtual HRESULT GetImageExtent( unsigned long p_ulIndex, SVImageExtentClass &p_rsvImageExtent ) const;
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );
	virtual HRESULT GetFilteredImageExtentPropertyList( SVExtentPropertyListType& p_rPropertyList );

	// ******* Begin Source Extent Data
	// *
	// * These are local run operation variables
	// * Do not use these variables unless for specific source extent uses
	// *
	// *******

	virtual HRESULT GetRootOffsetData( SVExtentOffsetStruct& p_rsvOffsetData );
	virtual HRESULT GetSelectedOffsetData( SVExtentOffsetStruct& p_rsvOffsetData );
	virtual HRESULT UpdateOffsetDataToImage( SVExtentOffsetStruct& p_rsvOffsetData, SVImageClass* p_svToolImage );
	virtual HRESULT UpdateOffsetData( bool p_bForceUpdate );
	virtual HRESULT UpdateOffsetData( bool p_bForceUpdate, SVImageClass* p_svToolImage );
	virtual HRESULT TranslatePointToSource( SVExtentPointStruct p_svIn, SVExtentPointStruct& p_rsvOut );
	virtual HRESULT TranslatePositionPointToSource( SVExtentPointStruct& p_rsvOut );
	virtual HRESULT GetAuxilliaryDrawTypeString( CString& p_strDrawType );

	// ******* End Source Extent Data

protected:

	// ******* Begin Source Extent Data
	// *
	// * These are local run operation variables
	// * Do not use these variables unless for specific source extent uses
	// *
	// *******

	bool m_bAlwaysUpdate;

	SVImageClass* m_psvSelectedImage;

	SVExtentOffsetStruct m_svRootOffsetData;
	SVExtentOffsetStruct m_svSelectedOffsetData;

	// ******* End Source Extent Data

private:
	typedef std::set< SVExtentTranslationEnum > SVTranslationFilterSet;

	SVToolClass* m_psvTool;

	SVImageClass* m_psvToolImage;

	// This value does not appear to get used.
	SVImageTypeEnum m_eImageType;

	SVExtentTranslationEnum m_eTranslation;
	SVExtentShapeEnum m_eShape;

	SVToolExtentPropertiesClass m_svProperties;

	static SVTranslationFilterSet m_LinearToolTranslations;

};

#include "SVToolExtentClass.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolExtentClass.h_v  $
 * 
 *    Rev 1.1   07 May 2013 08:21:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   19 Apr 2013 09:14:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added UpdateExtentToParentExtents.
 * Added GetParentExtent.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Apr 2013 16:35:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  821
 * SCR Title:  Fix issue with Linear Tool when Enabling Rotation
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added Linear tool Extent Translation funtioanlity to Tool Extent Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:42:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   11 Jan 2013 15:04:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  807
 * SCR Title:  Allow Shift Tool to use Source Image Extents
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new extent functionality that does not rebuild the image buffers on a positional change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Mar 2011 16:28:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Mar 2011 13:59:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   14 Mar 2011 08:07:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to perform a partial validation of extents to prevent crashing or causing load failure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   11 Mar 2011 14:33:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
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
 *    Rev 1.1   01 Sep 2005 12:04:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New functions to support Auxilliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Feb 2005 12:11:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding new files to implement new extent functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
