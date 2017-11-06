//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageInfoClass
//* .File Name       : $Workfile:   SVImageInfoClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:53:46  $
//******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVGUID.h"
#include "SVImagePropertiesClass.h"
#include "SVImageExtentClass.h"

class SVImageClass;
class SVObjectClass;

class SVImageInfoClass
{
public:
	SVImageInfoClass();
	SVImageInfoClass( const SVImageInfoClass& p_rsvValue );
	SVImageInfoClass( const BITMAPINFOHEADER& p_rBitmapHeader );

	~SVImageInfoClass();

	const SVImageInfoClass &operator=( const SVImageInfoClass& p_rsvValue );
	const SVImageInfoClass &operator=( const BITMAPINFOHEADER& p_rBitmapHeader );

	HRESULT Initialize();

	SVImagePropertiesClass GetImageProperties() const;
	HRESULT SetImageProperties( SVImagePropertiesClass p_svImageProperties );

	SVImageExtentClass GetExtents() const;
	HRESULT SetExtents( SVImageExtentClass p_svExtents );

	SVExtentTranslationEnum GetTranslation();
	HRESULT SetTranslation( SVExtentTranslationEnum p_eTranslation );

	HRESULT GetImageProperty( SvDef::SVImagePropertyEnum p_eDefinition, int &p_riValue ) const;
	HRESULT GetImageProperty( SvDef::SVImagePropertyEnum p_eDefinition, long &p_rlValue ) const;
	HRESULT SetImageProperty( SvDef::SVImagePropertyEnum p_eDefinition, long p_lValue );

	HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const;
	HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const;
	HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const;
	HRESULT GetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct &p_rsvValue ) const;
	HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, double p_dValue );
	HRESULT SetExtentProperty( SVExtentPropertyEnum p_eProperty, SVExtentPointStruct p_svValue );

	HRESULT GetImageExtentsToFit( SVImageExtentClass p_svInExtent, SVImageExtentClass &p_rsvOutExtent );

	HRESULT GetOutputRectangle( RECT &p_roRect ) const;

	HRESULT GetFigure( SVExtentFigureStruct &p_rsvFigure );

	long GetBufferSize();

	template< typename SV_OBJECT_TYPE >
	HRESULT GetOwnerImage( SV_OBJECT_TYPE*& p_rpObject ) const;

	HRESULT GetOwnerImage( SVObjectClass*& p_rpObject ) const;
	const SVGUID& GetOwnerImageID() const;
	void SetOwnerImage( const GUID& p_rObjectID );

	SVObjectClass* GetOwner() const;
	const SVGUID& GetOwnerID() const;
	void SetOwner( const GUID& p_rObjectID );

	HRESULT ValidateAgainstOutputSpace( const SVImageExtentClass &p_rsvValue ) const;

	bool IsEqualExcludePosition( const SVImageInfoClass &p_rsvValue ) const;
	bool IsPositionEqual( const SVImageInfoClass &p_rsvValue ) const;

	bool operator==( const SVImageInfoClass &p_rsvValue ) const;
	bool operator!=( const SVImageInfoClass &p_rsvValue ) const;

	BITMAPINFOHEADER GetBitmapInfoHeader() const;
	operator BITMAPINFOHEADER () const;

	void setDibBufferFlag(bool isDibBuffer) { m_isDibBuffer = isDibBuffer; };
	bool getDibBufferFlag() const { return m_isDibBuffer; };

private:
	SVGUID m_OwnerImageID;
	SVGUID m_OwnerObjectID;

	SVImageExtentClass m_svExtents;
	SVImagePropertiesClass m_svProperties;
	bool m_isDibBuffer;
};

#include "SVImageInfoClass.inl"

