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
#pragma endregion Includes

extern double SVGetRadius(const SVPoint<double>& rCenter, const SVPoint<double>& rPoint);
extern double SVGetRotationAngle(const SVPoint<double>& rCenter, const SVPoint<double>& rPoint);

extern SVPoint<double> SVRotatePoint(const SVPoint<double>& rCenter, double dRadius, double dAngle );
extern SVPoint<double> SVRotatePoint(const SVPoint<double>& rCenter, const SVPoint<double>& rStartPoint, double dAngle );

extern double SVGetFlippedRotationAngle(const SVPoint<double>& rCenter, const SVPoint<double>& rPoint);
extern SVPoint<double> SVRotateAndFlipPoint(const SVPoint<double>& rCenter, double dRadius, double dAngle);
extern SVPoint<double> SVRotateAndFlipPoint(const SVPoint<double>& rCenter, const SVPoint<double>& rStartPoint, double dAngle);

typedef std::vector<SvDef::SVExtentPropertyEnum> SVExtentPropertyVector;
typedef std::map<SvDef::SVExtentPropertyEnum, std::string> SVExtentPropertyStringMap;
typedef std::map<SvDef::SVExtentPropertyEnum, double> SVExtentPropertyValueMap;
typedef std::map<SvDef::SVExtentPropertyEnum, std::pair<SvDef::SVExtentPropertyEnum, SvDef::SVExtentPropertyEnum>> SVExtentPropertyPointMap;

class SVImageExtentClass
{
public:
	SVImageExtentClass();
	SVImageExtentClass(const SVImageExtentClass& rRhs);
	~SVImageExtentClass();

	const SVImageExtentClass &operator=(const SVImageExtentClass &rRhs);

	void Initialize();

	SvDef::SVExtentTranslationEnum GetTranslation() const;
	void SetTranslation( SvDef::SVExtentTranslationEnum p_eTranslation );

	void DisableExtentProperty( SvDef::SVExtentPropertyEnum eProperty );

	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum eProperty, long &rValue ) const;
	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum eProperty, double &rValue ) const;
	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum eProperty, POINT &rValue ) const;
	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum eProperty, SVPoint<double>& rValue ) const;
	SVExtentPropertyStringMap GetExtentPropertyMap(SvDef::SVExtentPropertyEnum eWhichProperties) const;
	SVExtentPropertyVector GetExtentPropertyVector(SvDef::SVExtentPropertyEnum eWhichProperties) const;

	HRESULT SetExtentProperty(SvDef::SVExtentPropertyEnum eProperty, double dValue, bool clearOutputData = true);
	HRESULT SetExtentProperty(SvDef::SVExtentPropertyEnum eProperty, const SVPoint<double>& rValue, bool clearOutputData = true);

	HRESULT UpdateData();

	bool hasFigure() const;
	const SVExtentFigureStruct& GetFigure() const { return m_figure; }

	SvDef::SVExtentLocationPropertyEnum GetLocationPropertyAt(const SVPoint<double>& rPoint) const;

	HRESULT Update( SvDef::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStartPoint, const SVPoint<double>& rEndPoint);
	HRESULT UpdateFromOutputSpace( SvDef::SVExtentLocationPropertyEnum eLocation, long lX, long lY );

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

private:
	HRESULT UpdateLine( SvDef::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd );
	HRESULT UpdatePolar( SvDef::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd );
	HRESULT UpdatePolarFromOutputSpace(SvDef::SVExtentLocationPropertyEnum eLocation, long p_dX, long p_dY );
	HRESULT UpdateHorizontalPerspective(SvDef::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd);
	HRESULT UpdateVerticalPerspective(SvDef::SVExtentLocationPropertyEnum eLocation, const SVPoint<double>& rStart, const SVPoint<double>& rEnd);

	void ClearOutputData();

	HRESULT BuildOutputDimensions();
	HRESULT BuildFigure();

	HRESULT setProperty(SvDef::SVExtentPropertyEnum eProperty, double dValue);

	bool isEnabled(SvDef::SVExtentPropertyEnum eProperty) const;

	/// TranslateToLocalSpace
	///	 Translate the point to be relative (local space).  Usually this relates 
	///  to the ROI of the parent image.
	HRESULT TranslateToLocalSpace(SVPoint<double> value, SVPoint<double>& rResult);
	// Input Attributes
	SvDef::SVExtentTranslationEnum m_eTranslation;
	// Output Attributes
	SVExtentFigureStruct m_figure;

	unsigned long m_properties{0UL};

	SVExtentPropertyValueMap m_extentValues;
};

