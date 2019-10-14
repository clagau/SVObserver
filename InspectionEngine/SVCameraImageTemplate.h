//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraImageTemplate
//* .File Name       : $Workfile:   SVCameraImageTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:49:34  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <set>
#include "SVUtilityLibrary/SVGUID.h"
#include "InspectionEngine/SVImageClass.h"
#pragma endregion Includes

namespace SvIe
{

class SVInspectionProcess;
class SVVirtualCamera;

class SVCameraImageTemplate : public SVImageClass 
{
public:
	explicit SVCameraImageTemplate( LPCSTR ObjectName );
	SVCameraImageTemplate( SVObjectClass* POwner, int StringResourceID );

	virtual ~SVCameraImageTemplate();

	HRESULT UpdateCameraImage( LPCTSTR p_szCameraName );
	HRESULT UpdateCameraImage( const SVGUID& p_CameraID );

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SVVirtualCamera* GetCamera() const;
	std::string GetCameraName() const;

	HRESULT DisconnectBuffers();
	HRESULT ReconnectBuffers();

	virtual HRESULT GetObjectValue( const std::string& rValueName, _variant_t& rValue ) const override;
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;

	virtual void Persist(SvOi::IObjectWriter& rWriter) override;

protected:
	virtual bool CreateBuffers(const SVImageInfoClass& p_rImageInfo) = 0;

	virtual bool DestroyImage() override;

	HRESULT RebuildCameraImage();

	SVVirtualCamera* m_pCamera;
	SVGUID m_digitizerObjectID;
};

typedef std::set< SVCameraImageTemplate* > SVCameraImagePtrSet;

} //namespace SvIe
