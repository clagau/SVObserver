//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
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

	//innitialize and recalculate the imageExtent if necessary 
	HRESULT updateImageExtent(bool init);

	HRESULT SetImageExtentAndProperty( const SVImageExtentClass& rImageExtent);

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
	//What is at the cursor 
	SvPb::SVExtentLocationPropertyEnum GetLocationPropertyAt(const SVPoint<double>& rPoint) const;
	//drag and drop some ROI boundary
	HRESULT UpdateDraggingROI( SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStartPoint, const SVPoint<double>& rEndPoint);

	//ROI was changed 
	HRESULT UpdateFromOutputSpace(SvPb::SVExtentLocationPropertyEnum eLocation, long lX, long lY);

	//Where is the point in the output
	HRESULT TranslateToOutputSpace(SVPoint<double> value, SVPoint<double> &rResult) const;


	HRESULT GetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, long& rValue) const
	{
		return m_ImageExtent.GetExtentProperty(eProperty, rValue);
	}
	HRESULT GetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, double& rValue) const
	{
		return m_ImageExtent.GetExtentProperty(eProperty, rValue);
	}
	HRESULT GetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, POINT& rValue) const
	{
		return m_ImageExtent.GetExtentProperty(eProperty, rValue);
	}
	HRESULT GetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, SVPoint<double>& rValue) const
	{
		return m_ImageExtent.GetExtentProperty(eProperty, rValue);
	}

	HRESULT SetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, double dValue, bool clearOutputData = true)
	{
		return m_ImageExtent.SetExtentProperty(eProperty, dValue, clearOutputData);
	}
	HRESULT SetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, const SVPoint<double>& rValue, bool clearOutputData = true)
	{
		return m_ImageExtent.SetExtentProperty(eProperty, rValue, clearOutputData);
	}


	
	HRESULT TranslateFromOutputSpace(SVPoint<double> value, SVPoint<double>& rResult) const
	{
		return m_ImageExtent.TranslateFromOutputSpace(value, rResult);
	}
	HRESULT TranslateFromOutputSpace(SVExtentFigureStruct value, SVExtentFigureStruct& rResult) const
	{
		return m_ImageExtent.TranslateFromOutputSpace(value, rResult);
	}
	HRESULT TranslateFromOutputSpace(SVExtentLineStruct value, SVExtentLineStruct& rResult) const
	{
		return m_ImageExtent.TranslateFromOutputSpace(value, rResult);
	}
	HRESULT TranslateFromOutputSpace(SVExtentMultiLineStruct value, SVExtentMultiLineStruct& rResult) const
	{
		return m_ImageExtent.TranslateFromOutputSpace( value, rResult);
	}
	HRESULT GetOutputRectangle(RECT& p_roRect) const
	{
		return m_ImageExtent.GetOutputRectangle(p_roRect);
	}


private:
	HRESULT TranslateToLocalSpace(SVPoint<double> value, SVPoint<double>& rResult);
	HRESULT UpdateLine(SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd);
	HRESULT UpdatePolar(SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd);
	HRESULT UpdatePolarFromOutputSpace(SvPb::SVExtentLocationPropertyEnum eLocation, long p_dX, long p_dY);
	HRESULT UpdateHorizontalPerspective(SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd);
	HRESULT UpdateVerticalPerspective(SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd);

	bool isDirectValue(SvPb::SVExtentPropertyEnum extentProperty) const;

	typedef std::set< SvPb::SVExtentTranslationEnum > SVTranslationFilterSet;

	// ******* Begin Source Extent Data
	// *
	// * These are local run operation variables
	// * Do not use these variables unless for specific source extent uses
	// *
  // *******
	
	bool m_bAlwaysUpdate {true};

	SvIe::SVImageClass* m_pSelectedImage {nullptr}; //auxtoolimage
	SvTo::SVToolClass* m_pTool {nullptr};
	

	SVExtentOffsetStruct m_svRootOffsetData;
	SVExtentOffsetStruct m_svSelectedOffsetData;

	// ******* End Source Extent Data

	//OUTPUT IMAGE
	SvIe::SVImageClass* m_pToolImage {nullptr};

	SvPb::SVExtentTranslationEnum m_eTranslation {SvPb::SVExtentTranslationUnknown};
	SvDef::SVExtentShapeEnum m_eShape {SvDef::SVExtentShapeUnknown};

	//@TODO[MEC][10.30][17.11.2022]  CHECK if  m_Properties is necessary additional to m_Imageextent
	SVToolExtentPropertiesClass m_Properties;

	

	SVImageExtentClass m_ImageExtent;

	mutable bool m_CircularReference {false};					//! Use this flag during UpdateOffsetDataToImage to make sure no circular references are present
	
};

} //namespace SvIe
