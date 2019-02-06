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

namespace SvIe
{
class SVImageClass;
}

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
	void SetImageProperties( const SVImagePropertiesClass& rImageProperties );

	const SVImageExtentClass& GetExtents() const;
	HRESULT SetExtents(const SVImageExtentClass& rExtents );

	SvDef::SVExtentTranslationEnum GetTranslation();
	HRESULT SetTranslation( SvDef::SVExtentTranslationEnum p_eTranslation );

	HRESULT GetImageProperty( SvDef::SVImagePropertyEnum p_eDefinition, int &p_riValue ) const;
	HRESULT GetImageProperty( SvDef::SVImagePropertyEnum p_eDefinition, long &p_rlValue ) const;
	HRESULT SetImageProperty( SvDef::SVImagePropertyEnum p_eDefinition, long p_lValue );

	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const;
	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const;
	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const;
	HRESULT GetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, SVPoint<double> &rValue ) const;
	HRESULT SetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, double p_dValue );
	HRESULT SetExtentProperty( SvDef::SVExtentPropertyEnum p_eProperty, const SVPoint<double>& rValue );

	HRESULT GetImageExtentsToFit(SVImageExtentClass inExtent, SVImageExtentClass &rOutExtent);

	HRESULT GetOutputRectangle( RECT& rRect ) const;

	const SVExtentFigureStruct& GetFigure() const;

	long GetBufferSize();

	template< typename SV_OBJECT_TYPE >
	HRESULT GetOwnerImage( SV_OBJECT_TYPE*& p_rpObject ) const;

	HRESULT GetOwnerImage( SVObjectClass*& p_rpObject ) const;
	void SetOwnerImage( const GUID& p_rObjectID );

	SVObjectClass* GetOwner() const;
	const SVGUID& GetOwnerID() const;
	void SetOwner( const GUID& p_rObjectID );

	HRESULT ValidateAgainstOutputSpace( const SVImageExtentClass &p_rsvValue ) const;

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

