//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageInfoClass
//* .File Name       : $Workfile:   SVImageInfoClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:53:46  $
//******************************************************************************

#pragma once

#include "SVImagePropertiesClass.h"
#include "SVImageExtentClass.h"
#include "Definitions/ObjectDefines.h"

class SVObjectClass;
namespace SvIe
{
class SVImageClass;
}

class SVImageInfoClass
{
public:
	SVImageInfoClass();
	SVImageInfoClass( const SVImageInfoClass& rRhs );
	explicit SVImageInfoClass( const BITMAPINFOHEADER& rBitmapHeader );

	~SVImageInfoClass();

	SVImageInfoClass &operator=( const SVImageInfoClass& rRhs);
	const SVImageInfoClass &operator=( const BITMAPINFOHEADER& rBitmapHeader );

	HRESULT Initialize();

	SVImagePropertiesClass GetImageProperties() const;
	void SetImageProperties( const SVImagePropertiesClass& rImageProperties );

	const SVImageExtentClass& GetExtents() const;
	HRESULT SetExtents(const SVImageExtentClass& rExtents );

	SvPb::SVExtentTranslationEnum GetTranslation();
	HRESULT SetTranslation( SvPb::SVExtentTranslationEnum p_eTranslation );

	HRESULT GetImageProperty( SvDef::SVImagePropertyEnum p_eDefinition, int &p_riValue ) const;
	HRESULT GetImageProperty( SvDef::SVImagePropertyEnum p_eDefinition, long &p_rlValue ) const;
	HRESULT SetImageProperty( SvDef::SVImagePropertyEnum p_eDefinition, long p_lValue );

	HRESULT GetExtentProperty( SvPb::SVExtentPropertyEnum p_eProperty, long &p_rlValue ) const;
	HRESULT GetExtentProperty( SvPb::SVExtentPropertyEnum p_eProperty, double &p_rdValue ) const;
	HRESULT GetExtentProperty( SvPb::SVExtentPropertyEnum p_eProperty, POINT &p_roValue ) const;
	HRESULT GetExtentProperty( SvPb::SVExtentPropertyEnum p_eProperty, SVPoint<double> &rValue ) const;
	HRESULT SetExtentProperty( SvPb::SVExtentPropertyEnum p_eProperty, double p_dValue );
	HRESULT SetExtentProperty( SvPb::SVExtentPropertyEnum p_eProperty, const SVPoint<double>& rValue );

	void getExtentProperties(::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter >& rExtentProperties) const
	{

		 return m_svExtents.getExtentProperties(rExtentProperties);
	}

	HRESULT GetImageExtentsToFit(SVImageExtentClass inExtent, SVImageExtentClass &rOutExtent);

	HRESULT GetOutputRectangle( RECT& rRect ) const;

	const SVExtentFigureStruct& GetFigure() const;

	long GetBufferSize();

	SvIe::SVImageClass* GetOwnerImage() const;
	void SetOwnerImage(uint32_t objectID );

	SVObjectClass* GetOwner() const;
	uint32_t GetOwnerID() const;
	void SetOwner(uint32_t objectID);

	HRESULT ValidateAgainstOutputSpace( const SVImageExtentClass &p_rsvValue ) const;

	bool operator==( const SVImageInfoClass &p_rsvValue ) const;
	bool operator!=( const SVImageInfoClass &p_rsvValue ) const;

	BITMAPINFOHEADER GetBitmapInfoHeader() const;
	operator BITMAPINFOHEADER () const;

	void setDibBufferFlag(bool isDibBuffer) { m_isDibBuffer = isDibBuffer; };
	bool getDibBufferFlag() const { return m_isDibBuffer; };
	long getOutputWidth() const;
	long getOutputHeight() const;
	long getBandNumber() const;
	long  getPixelDepth() const;

	void setTransfermatrix(const std::vector<double>& rMatrix);

private:
	uint32_t m_OwnerImageID{ SvDef::InvalidObjectId };
	uint32_t m_OwnerObjectID{ SvDef::InvalidObjectId };

	SVImageExtentClass m_svExtents;
	SVImagePropertiesClass m_svProperties;
	bool m_isDibBuffer;
};

