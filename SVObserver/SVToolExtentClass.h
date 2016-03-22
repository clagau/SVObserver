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

#pragma once

//Moved to precompiled header: #include <set>
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
	virtual HRESULT GetAuxiliaryDrawTypeString( CString& p_strDrawType );

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

