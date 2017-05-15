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

#pragma region Includes
//Moved to precompiled header: #include <set>
#include "ObjectInterfaces/SVImageEnums.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVToolExtentPropertiesClass.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

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
	};

	SVToolExtentClass();
	~SVToolExtentClass();

	inline HRESULT Initialize();

	inline SVToolClass* GetTool() const;
	inline void SetTool( SVToolClass* p_psvTool );

	inline SVImageClass* GetToolImage() const;
	inline void SetToolImage( SVImageClass* p_psvToolImage );

	inline SvOi::SVImageTypeEnum GetImageType() const;
	inline void SetImageType( SvOi::SVImageTypeEnum p_eImageType );

	inline SVImageClass* GetSelectedImage() const;
	inline void SetSelectedImage( SVImageClass* p_psvSelectedImage );

	inline bool GetAlwaysUpdate() const;
	inline void SetAlwaysUpdate( bool p_bAlwaysUpdate );

	HRESULT ValidExtentAgainstParentImage( const SVImageExtentClass& p_rImageExtent ) const;
	HRESULT UpdateExtentToParentExtents( unsigned long p_ulIndex, SVImageExtentClass& p_rNewExtent  );
	HRESULT UpdateExtentAgainstParentImage( unsigned long p_ulIndex, const SVImageExtentClass& p_rImageExtent );
	HRESULT GetParentExtent( SVImageExtentClass& p_rParent ) const;

	HRESULT UpdateImageWithExtent( unsigned long p_ulIndex, SVToolExtentTypeEnum p_ToolExtentType );

	SVExtentTranslationEnum GetTranslation();
	HRESULT SetTranslation( SVExtentTranslationEnum p_eTranslation );
	HRESULT SetTranslation( SVExtentTranslationEnum p_eTranslation, long p_DataIndex );

	HRESULT GetExtentShape( SVExtentPropertyEnum p_eProperty, SVExtentShapeEnum &p_reValue ) const;

	HRESULT GetExtentObject( SVExtentPropertyEnum p_eProperty, SvOi::IValueObject*& rpValueObject ) const;
	HRESULT SetExtentObject( SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject );

	HRESULT GetExtentValue( SVExtentPropertyEnum p_eProperty, _variant_t& rValue ) const;
	HRESULT SetExtentValue( SVExtentPropertyEnum p_eProperty, long p_DataIndex, const _variant_t& rValue );

	HRESULT GetExtentPropertyInfo( SVExtentPropertyEnum p_eProperty, SVExtentPropertyInfoStruct& p_rInfo ) const;
	HRESULT SetExtentPropertyInfo( SVExtentPropertyEnum p_eProperty, const SVExtentPropertyInfoStruct& p_rInfo );

	
//- GetImageExtent -----------------------------------------------------------
//- There appears to be little direct connection between the SVToolExtentClass
//- and the SVImageExtentClass.  This function appears to attempt to translate  
//- between the two.  Translating and copying from the SVToolExtentClass based 
//- structure into the SVImageExtentClass based structure. -------------------
	HRESULT GetImageExtent( SVImageExtentClass &p_rsvImageExtent )const ;

	HRESULT GetImageExtent( unsigned long p_ulIndex, SVImageExtentClass &p_rsvImageExtent ) const;
	HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );
	HRESULT GetFilteredImageExtentPropertyList( SVExtentPropertyListType& p_rPropertyList );

	// ******* Begin Source Extent Data
	// *
	// * These are local run operation variables
	// * Do not use these variables unless for specific source extent uses
	// *
	// *******

	HRESULT GetRootOffsetData( SVExtentOffsetStruct& p_rsvOffsetData );
	HRESULT GetSelectedOffsetData( SVExtentOffsetStruct& p_rsvOffsetData );
	HRESULT UpdateOffsetDataToImage( SVExtentOffsetStruct& p_rsvOffsetData, SVImageClass* p_svToolImage );
	HRESULT UpdateOffsetData( bool p_bForceUpdate );
	HRESULT UpdateOffsetData( bool p_bForceUpdate, SVImageClass* p_svToolImage );
	HRESULT TranslatePointToSource( SVExtentPointStruct p_svIn, SVExtentPointStruct& p_rsvOut );
	HRESULT TranslatePositionPointToSource( SVExtentPointStruct& p_rsvOut );
	SVString GetAuxiliaryDrawTypeString() const;

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
	SvOi::SVImageTypeEnum m_eImageType;

	SVExtentTranslationEnum m_eTranslation;
	SVExtentShapeEnum m_eShape;

	SVToolExtentPropertiesClass m_svProperties;

	static SVTranslationFilterSet m_LinearToolTranslations;
};

#include "SVToolExtentClass.inl"

