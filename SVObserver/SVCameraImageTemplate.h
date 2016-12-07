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

#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVImageObjectClass.h"
#pragma endregion Includes

class SVInspectionProcess;
class SVVirtualCamera;

class SVCameraImageTemplate : public SVImageClass 
{
public:
	SVCameraImageTemplate( LPCSTR ObjectName );
	SVCameraImageTemplate( SVObjectClass* POwner, int StringResourceID );

	virtual ~SVCameraImageTemplate();

	HRESULT UpdateCameraImage( LPCTSTR p_szCameraName );
	HRESULT UpdateCameraImage( const SVGUID& p_CameraID );

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStruct ) override;
	virtual BOOL SetImageDepth( long lDepth ) override;

	virtual HRESULT ResetObject() override;

	GUID GetDigitizerID();
	SVVirtualCamera* GetCamera() const;
	SVString GetCameraName() const;

	HRESULT DisconnectBuffers();
	HRESULT ReconnectBuffers();

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const override;
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;

	virtual void Persist(SVObjectWriter& rWriter) override;

	BOOL ResetImageIndex();

	virtual SVImageIndexStruct GetSourceImageIndex( SVDataManagerHandle* pHandle, const SVGuidSVCameraInfoStructMap& rGuidCameraMap ) override;
	virtual BOOL GetImageHandleIndex( SVImageIndexStruct& rlIndex ) const override;

	HRESULT RestoreMainImage( SVInspectionProcess* p_psvInspection );

protected:
	virtual BOOL CreateBuffers( const SVImageInfoClass& p_rImageInfo, SVImageObjectClassPtr p_ImageArrayPtr ) = 0;

	virtual BOOL DestroyImage() override;

	virtual HRESULT GetImageIndex( SVDataManagerHandle& p_rHandle, const SVImageIndexStruct& rIndex ) const override;

	SVImageObjectClassPtr GetCameraBufferArrayPtr() const;

	virtual HRESULT RegisterAsSubObject() override;
	virtual HRESULT UnregisterAsSubObject() override;

	HRESULT RebuildCameraImage();

	SVVirtualCamera* mpCamera;
	SVGUID digitizerObjectID;
	SVImageIndexStruct m_CurrentIndex;
};

typedef std::set< SVCameraImageTemplate* > SVCameraImagePtrSet;

