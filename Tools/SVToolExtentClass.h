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
#include "Definitions/SVImageEnums.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#include "SVToolExtentPropertiesClass.h"
#pragma endregion Includes

namespace SvIe
{
class SVImageClass;
}

namespace SvTo
{

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

	SVToolExtentClass(SVImageExtentClass& rImageExtent);
	~SVToolExtentClass();

	HRESULT Initialize();

	SvTo::SVToolClass* GetTool() const { return m_pTool; }
	void SetTool(SvTo::SVToolClass* pTool ) { m_pTool = pTool; }

	SvIe::SVImageClass* GetToolImage() const { return m_pToolImage; }
	void SetToolImage(SvIe::SVImageClass* pToolImage);

	SvIe::SVImageClass* GetSelectedImage() const { return m_pSelectedImage; }
	void SetSelectedImage(SvIe::SVImageClass* pSelectedImage);

	bool GetAlwaysUpdate() const { return m_bAlwaysUpdate; }
	void SetAlwaysUpdate( bool bAlwaysUpdate ) { m_bAlwaysUpdate = bAlwaysUpdate; }

	HRESULT ValidExtentAgainstParentImage( const SVImageExtentClass& p_rImageExtent ) const;
	HRESULT UpdateExtentToParentExtents( SVImageExtentClass& p_rNewExtent  );
	HRESULT UpdateExtentAgainstParentImage( const SVImageExtentClass& p_rImageExtent );
	HRESULT GetParentExtent( SVImageExtentClass& p_rParent ) const;

	HRESULT UpdateImageWithExtent( SVToolExtentTypeEnum p_ToolExtentType );

	SvDef::SVExtentTranslationEnum GetTranslation();
	void SetTranslation( SvDef::SVExtentTranslationEnum eTranslation );
	HRESULT SetLinearTranslation(SvDef::SVExtentTranslationEnum eTranslation);

	HRESULT GetExtentObject( SvDef::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject*& rpValueObject ) const;
	void SetExtentObject( SvDef::SVExtentPropertyEnum p_eProperty, SvOi::IValueObject* pValueObject );

	HRESULT GetExtentValue( SvDef::SVExtentPropertyEnum p_eProperty, _variant_t& rValue ) const;
	HRESULT SetExtentValue( SvDef::SVExtentPropertyEnum p_eProperty, const _variant_t& rValue );

	HRESULT GetExtentPropertyInfo( SvDef::SVExtentPropertyEnum p_eProperty, SvIe::SVExtentPropertyInfoStruct& p_rInfo ) const;
	HRESULT SetExtentPropertyInfo( SvDef::SVExtentPropertyEnum p_eProperty, const SvIe::SVExtentPropertyInfoStruct& p_rInfo );

	
//- updateImageExtent -----------------------------------------------------------
//- There appears to be little direct connection between the SVToolExtentClass
//- and the SVImageExtentClass.  This function appears to attempt to translate  
//- between the two.  Translating and copying from the SVToolExtentClass based 
//- structure into the SVImageExtentClass based structure. -------------------
	HRESULT updateImageExtent();

	HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent );
	HRESULT GetFilteredImageExtentPropertyList( SVExtentPropertyVector& p_rPropertyList );

	// ******* Begin Source Extent Data
	// *
	// * These are local run operation variables
	// * Do not use these variables unless for specific source extent uses
	// *
	// *******

	HRESULT GetRootOffsetData( SVExtentOffsetStruct& rOffsetData );
	HRESULT GetSelectedOffsetData( SVExtentOffsetStruct& rOffsetData );
	HRESULT UpdateOffsetDataToImage( SVExtentOffsetStruct& rOffsetData, SvIe::SVImageClass* pToolImage );
	HRESULT UpdateOffsetData( bool bForceUpdate );
	HRESULT UpdateOffsetData( bool bForceUpdate, SvIe::SVImageClass* pToolImage );
	HRESULT TranslatePointToSource(SVPoint<double> inPoint, SVPoint<double>& rOutPoint );
	std::string GetAuxiliaryDrawTypeString() const;

	// ******* End Source Extent Data

private:
	typedef std::set< SvDef::SVExtentTranslationEnum > SVTranslationFilterSet;

	// ******* Begin Source Extent Data
	// *
	// * These are local run operation variables
	// * Do not use these variables unless for specific source extent uses
	// *
	// *******

	bool m_bAlwaysUpdate;

	SvIe::SVImageClass* m_pSelectedImage;
	SvTo::SVToolClass* m_pTool;

	SVExtentOffsetStruct m_svRootOffsetData;
	SVExtentOffsetStruct m_svSelectedOffsetData;

	// ******* End Source Extent Data

	SvIe::SVImageClass* m_pToolImage;

	SvDef::SVExtentTranslationEnum m_eTranslation;
	SvDef::SVExtentShapeEnum m_eShape;

	SVToolExtentPropertiesClass m_Properties;

	SVImageExtentClass& m_rImageExtent;			//NOTE! This accesses the variable m_imageExtent directly in SVTaskObject

	static SVTranslationFilterSet m_LinearToolTranslations;
};

} //namespace SvIe
