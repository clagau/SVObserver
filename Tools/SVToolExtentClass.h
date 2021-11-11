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
#include "SVToolExtentPropertiesClass.h"
#include "SVImageLibrary/SVExtentOffsetStruct.h"
#include "SVImageLibrary/SVImageExtentClass.h"
#pragma endregion Includes

namespace SvIe
{
class SVImageClass;
}

namespace SvOi
{
class IValueObject;
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

	SVToolExtentClass();
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

	SVImageExtentClass& getImageExtent() {return m_ImageExtent;}
	const SVImageExtentClass& getImageExtent() const { return m_ImageExtent; }

	HRESULT ValidExtentAgainstParentImage( const SVImageExtentClass& rImageExtent ) const;
	HRESULT UpdateExtentToParentExtents( SVImageExtentClass& rNewExtent  );
	HRESULT UpdateExtentAgainstParentImage( const SVImageExtentClass& rImageExtent );
	HRESULT GetParentExtent( SVImageExtentClass& rParent ) const;

	HRESULT UpdateImageWithExtent( SVToolExtentTypeEnum ToolExtentType );

	SvPb::SVExtentTranslationEnum GetTranslation() const;
	void SetTranslation( SvPb::SVExtentTranslationEnum eTranslation );
	HRESULT SetLinearTranslation(SvPb::SVExtentTranslationEnum eTranslation);

	HRESULT GetExtentObject( SvPb::SVExtentPropertyEnum extentProperty, SvOi::IValueObject*& rpValueObject ) const;
	void SetExtentObject( SvPb::SVExtentPropertyEnum extentProperty, SvOi::IValueObject* pValueObject );

	HRESULT GetExtentValue(SvPb::SVExtentPropertyEnum extentProperty, _variant_t& rValue) const;
	HRESULT SetExtentValue(SvPb::SVExtentPropertyEnum extentProperty, const _variant_t& rValue);

	HRESULT GetExtentPropertyInfo( SvPb::SVExtentPropertyEnum extentProperty, SvIe::SVExtentPropertyInfoStruct& rInfo ) const;
	HRESULT SetExtentPropertyInfo( SvPb::SVExtentPropertyEnum extentProperty, const SvIe::SVExtentPropertyInfoStruct& rInfo );

	void getExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties) const;

//- updateImageExtent -----------------------------------------------------------
//- There appears to be little direct connection between the SVToolExtentClass
//- and the SVImageExtentClass.  This function appears to attempt to translate  
//- between the two.  Translating and copying from the SVToolExtentClass based 
//- structure into the SVImageExtentClass based structure. -------------------
	HRESULT updateImageExtent(bool init);

	HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent);

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

	HRESULT ensureValidScaleFactorIfDirectValue(const SVImageExtentClass& rImageExtent, SvPb::SVExtentPropertyEnum extentProperty);

private:
	bool isDirectValue(SvPb::SVExtentPropertyEnum extentProperty) const;

	typedef std::set< SvPb::SVExtentTranslationEnum > SVTranslationFilterSet;

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

	SvPb::SVExtentTranslationEnum m_eTranslation;
	SvDef::SVExtentShapeEnum m_eShape;

	SVToolExtentPropertiesClass m_Properties;

	

	SVImageExtentClass m_ImageExtent;
	static SVTranslationFilterSet m_LinearToolTranslations;

	mutable bool m_CircularReference = false;					//! Use this flag during UpdateOffsetDataToImage to make sure no circular references are present
	
};

} //namespace SvIe
