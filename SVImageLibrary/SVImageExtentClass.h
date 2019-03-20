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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "Definitions/SVExtentEnums.h"
#include "SVExtentFigureStruct.h"
#include "SVExtentOffsetStruct.h"
#include "SVExtentLineStruct.h"
#include "SVExtentMultiLineStruct.h"
#include "SVUtilityLibrary/SVPoint.h"
#include "SVProtobuf/SVO-Enum.h"
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

extern double SVGetRadius(const SVPoint<double>& rCenter, const SVPoint<double>& rPoint);
extern double SVGetRotationAngle(const SVPoint<double>& rCenter, const SVPoint<double>& rPoint);

extern SVPoint<double> SVRotatePoint(const SVPoint<double>& rCenter, double dRadius, double dAngle );
extern SVPoint<double> SVRotatePoint(const SVPoint<double>& rCenter, const SVPoint<double>& rStartPoint, double dAngle );

extern double SVGetFlippedRotationAngle(const SVPoint<double>& rCenter, const SVPoint<double>& rPoint);
extern SVPoint<double> SVRotateAndFlipPoint(const SVPoint<double>& rCenter, double dRadius, double dAngle);
extern SVPoint<double> SVRotateAndFlipPoint(const SVPoint<double>& rCenter, const SVPoint<double>& rStartPoint, double dAngle);

typedef std::map<SvPb::SVExtentPropertyEnum, std::string> SVExtentPropertyStringMap;
typedef std::map<SvPb::SVExtentPropertyEnum, double> SVExtentPropertyValueMap;
typedef std::map<SvPb::SVExtentPropertyEnum, std::pair<SvPb::SVExtentPropertyEnum, SvPb::SVExtentPropertyEnum>> SVExtentPropertyPointMap;

class SVImageExtentClass
{
public:
	SVImageExtentClass();
	SVImageExtentClass(const SVImageExtentClass& rRhs);
	~SVImageExtentClass();

	const SVImageExtentClass &operator=(const SVImageExtentClass &rRhs);

	void Initialize();

	SvPb::SVExtentTranslationEnum GetTranslation() const;
	void SetTranslation( SvPb::SVExtentTranslationEnum p_eTranslation );

	void DisableExtentProperty( SvPb::SVExtentPropertyEnum eProperty );

	HRESULT GetExtentProperty( SvPb::SVExtentPropertyEnum eProperty, long &rValue ) const;
	HRESULT GetExtentProperty( SvPb::SVExtentPropertyEnum eProperty, double &rValue ) const;
	HRESULT GetExtentProperty( SvPb::SVExtentPropertyEnum eProperty, POINT &rValue ) const;
	HRESULT GetExtentProperty( SvPb::SVExtentPropertyEnum eProperty, SVPoint<double>& rValue ) const;

	HRESULT SetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, double dValue, bool clearOutputData = true);
	HRESULT SetExtentProperty(SvPb::SVExtentPropertyEnum eProperty, const SVPoint<double>& rValue, bool clearOutputData = true);

	HRESULT UpdateData();

	bool hasFigure() const;
	const SVExtentFigureStruct& GetFigure() const { return m_figure; }

	SvPb::SVExtentLocationPropertyEnum GetLocationPropertyAt(const SVPoint<double>& rPoint) const;

	HRESULT Update( SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStartPoint, const SVPoint<double>& rEndPoint);
	HRESULT UpdateFromOutputSpace( SvPb::SVExtentLocationPropertyEnum eLocation, long lX, long lY );

	/// GetRectangle  
	///  Retrieves the rect values for the input/parent image that is 
	///  referenced by the current Extents.
	HRESULT GetRectangle( RECT &p_roRect ) const;

	/// GetOutputRectangle  
	///  Retrieves the rect values for the output image that is referenced by 
	///  the current Extents.  The translation value is used in determining 
	///  these dimensions.
	HRESULT GetOutputRectangle( RECT &p_roRect ) const;
	
	/// GetLogicalRectangle  
	///  Retrieves the rect values based on the ROI of the current extents.
	HRESULT GetLogicalRectangle( RECT &p_roRect ) const;

	HRESULT TranslateToOutputSpace(SVPoint<double> value, SVPoint<double> &rResult) const;

	HRESULT TranslateFromOutputSpace(SVPoint<double> value, SVPoint<double> &rResult) const;
	HRESULT TranslateFromOutputSpace( SVExtentFigureStruct p_svValue, SVExtentFigureStruct &p_rsvResult ) const;
	HRESULT TranslateFromOutputSpace( SVExtentLineStruct p_svValue, SVExtentLineStruct &p_rsvResult ) const;
	HRESULT TranslateFromOutputSpace( SVExtentMultiLineStruct p_svValue, SVExtentMultiLineStruct &p_rsvResult ) const;

	HRESULT TranslateLineFromOutputSpace(const SVPoint<double>& rRadiusPoint, const SVPoint<double>& rRotatePoint, double p_dLineAngle, const SVPoint<double>& rValue, SVPoint<double> &rResult) const;

	HRESULT ValidateAgainstOutputSpace( const SVImageExtentClass &p_rsvValue ) const;

	HRESULT GetTitlePoint(POINT& rTitlePoint) const;
	HRESULT GetTitlePoint(SVPoint<double>& rTitlePoint) const;

	bool operator==(const SVImageExtentClass& rRhs) const;
	bool operator!=(const SVImageExtentClass& rRhs) const;

	HRESULT UpdateSourceOffset( SVExtentOffsetStruct& rOffsetData ) const;

	/// Set the image extent data depending of an image file.
	/// \param pFileName [in] Filename of the image.
	/// \returns HRESULT S_OK on success.
	HRESULT SetDataFromFile(LPCTSTR pFileName);

	void getExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties) const;

private:
	HRESULT UpdateLine( SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd );
	HRESULT UpdatePolar( SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd );
	HRESULT UpdatePolarFromOutputSpace(SvPb::SVExtentLocationPropertyEnum eLocation, long p_dX, long p_dY );
	HRESULT UpdateHorizontalPerspective(SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd);
	HRESULT UpdateVerticalPerspective(SvPb::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd);

	void ClearOutputData();

	HRESULT BuildOutputDimensions();
	HRESULT BuildFigure();

	HRESULT setProperty(SvPb::SVExtentPropertyEnum eProperty, double dValue);

	bool isEnabled(SvPb::SVExtentPropertyEnum eProperty) const;

	/// TranslateToLocalSpace
	///	 Translate the point to be relative (local space).  Usually this relates 
	///  to the ROI of the parent image.
	HRESULT TranslateToLocalSpace(SVPoint<double> value, SVPoint<double>& rResult);
	// Input Attributes
	SvPb::SVExtentTranslationEnum m_eTranslation;
	// Output Attributes
	SVExtentFigureStruct m_figure;

	unsigned long m_properties{0UL};

	SVExtentPropertyValueMap m_extentValues;
};

