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
#include "SVExtentDimensionsClass.h"
#include "Definitions/SVExtentEnums.h"
#include "SVExtentFigureStruct.h"
#include "SVExtentOffsetStruct.h"
#include "SVExtentPointStruct.h"
#include "SVExtentPositionClass.h"
#include "SVExtentLineStruct.h"
#include "SVExtentMultiLineStruct.h"

#pragma endregion Includes

extern double SVGetRadius( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svPoint );
extern double SVGetRotationAngle( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svPoint );

extern SVExtentPointStruct SVRotatePoint( SVExtentPointStruct p_svCenter, double p_dRadius, double p_dAngle );
extern SVExtentPointStruct SVRotatePoint( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svStart, double p_dAngle );

extern double SVGetFlippedRotationAngle( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svPoint );
extern SVExtentPointStruct SVRotateAndFlipPoint( SVExtentPointStruct p_svCenter, double p_dRadius, double p_dAngle );
extern SVExtentPointStruct SVRotateAndFlipPoint( SVExtentPointStruct p_svCenter, SVExtentPointStruct p_svStart, double p_dAngle );

typedef std::vector<SvDef::SVExtentPropertyEnum> SVExtentPropertyListType;
typedef std::map< SvDef::SVExtentPropertyEnum, std::string > SVExtentPropertyMapType;

class SVImageExtentClass
{
public:
	SVImageExtentClass();
	SVImageExtentClass( const SVImageExtentClass &p_rsvValue );
	~SVImageExtentClass();

	const SVImageExtentClass &operator=( const SVImageExtentClass &p_rsvValue );

	HRESULT Initialize();

	SvDef::SVExtentTranslationEnum GetTranslation() const;
	HRESULT SetTranslation( SvDef::SVExtentTranslationEnum p_eTranslation );

	const SVExtentDimensionsClass& GetDimensions() const;
	HRESULT SetDimensions( SVExtentDimensionsClass p_svDimensions );

	const SVExtentPositionClass &GetPosition() const;
	HRESULT SetPosition( SVExtentPositionClass p_svPosition );

	HRESULT DisableExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty );

	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const;
	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const;
	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const;
	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, SVExtentPointStruct &p_rsvValue ) const;
	HRESULT GetExtentPropertyList( SvDef::SVExtentPropertyEnum p_eWhichProperties, SVExtentPropertyListType& p_rPropertyList ) const;
	HRESULT GetExtentPropertyList( SvDef::SVExtentPropertyEnum p_eWhichProperties, SVExtentPropertyMapType& p_rPropertyMap ) const;
	static std::string GetExtentPropertyName( SvDef::SVExtentPropertyEnum p_eProperty );

	HRESULT SetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, double p_dValue );
	HRESULT SetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, SVExtentPointStruct p_svValue );

	HRESULT UpdateData();

	HRESULT GetFigure( SVExtentFigureStruct &p_rsvFigure ) const;

	SvDef::SVExtentLocationPropertyEnum GetLocationPropertyAt( SVExtentPointStruct p_svPoint );

	HRESULT Update( SvDef::SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd );
	HRESULT UpdateFromOutputSpace( SvDef::SVExtentLocationPropertyEnum p_eLocation, long p_lX, long p_lY );

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

	HRESULT TranslateToOutputSpace( SVExtentPointStruct p_svValue, SVExtentPointStruct &p_rsvResult );

	HRESULT TranslateFromOutputSpace( SVExtentPointStruct p_svValue, SVExtentPointStruct &p_rsvResult ) const;
	HRESULT TranslateFromOutputSpace( SVExtentFigureStruct p_svValue, SVExtentFigureStruct &p_rsvResult ) const;
	HRESULT TranslateFromOutputSpace( SVExtentLineStruct p_svValue, SVExtentLineStruct &p_rsvResult ) const;
	HRESULT TranslateFromOutputSpace( SVExtentMultiLineStruct p_svValue, SVExtentMultiLineStruct &p_rsvResult ) const;

	HRESULT TranslateLineFromOutputSpace( SVExtentPointStruct p_svRadiusPoint, SVExtentPointStruct p_svRotatePoint, double p_dLineAngle, SVExtentPointStruct p_svValue, SVExtentPointStruct &p_rsvResult ) const;

	HRESULT ValidateAgainstOutputSpace( const SVImageExtentClass &p_rsvValue ) const;

	HRESULT GetTitlePoint(POINT &p_ptTitlePoint) const;
	HRESULT GetTitlePoint(SVExtentPointStruct &p_svTitlePoint) const;

	bool IsEqualExcludePosition( const SVImageExtentClass &p_rsvValue ) const;
	bool IsPositionEqual( const SVImageExtentClass &p_rsvValue ) const;

	bool operator==( const SVImageExtentClass &p_rsvValue ) const;
	bool operator!=( const SVImageExtentClass &p_rsvValue ) const;

	HRESULT UpdateSourceOffset( SVExtentOffsetStruct& p_rsvOffsetData );

protected:
	HRESULT UpdateLine( SvDef::SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd );
	HRESULT UpdatePolar( SvDef::SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd );
	HRESULT UpdatePolarFromOutputSpace( SvDef::SVExtentLocationPropertyEnum p_eLocation, long p_dX, long p_dY );
	HRESULT UpdateHorizontalPerspective( SvDef::SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd );
	HRESULT UpdateVerticalPerspective( SvDef::SVExtentLocationPropertyEnum p_eLocation, SVExtentPointStruct p_svStart, SVExtentPointStruct p_svEnd );

	HRESULT ClearOutputData();

	HRESULT BuildOutputDimensions();
	HRESULT BuildFigure();

private:
	/// TranslateToLocalSpace
	///	 Translate the point to be relative (local space).  Usually this relates 
	///  to the ROI of the parent image.
	HRESULT TranslateToLocalSpace(const SVExtentPointStruct& rValue, SVExtentPointStruct& rResult);
	// Input Attributes
	SvDef::SVExtentTranslationEnum m_eTranslation;
	SVExtentPositionClass m_svPosition;
	SVExtentDimensionsClass m_svDimensions;

	// Output Attributes
	SVExtentFigureStruct m_svFigure;
};

