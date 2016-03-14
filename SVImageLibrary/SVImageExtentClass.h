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

