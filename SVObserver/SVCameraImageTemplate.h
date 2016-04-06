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

#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVImageClass.h"
#include "SVImageObjectClass.h"
#pragma endregion Includes

class SVInspectionProcess;
class SVVirtualCamera;

struct SVProductInfoStruct;

class SVCameraImageTemplate : public SVImageClass 
{
public:
	SVCameraImageTemplate( LPCSTR ObjectName );
	SVCameraImageTemplate( SVObjectClass* POwner, int StringResourceID );

	virtual ~SVCameraImageTemplate();

	virtual HRESULT UpdateCameraImage( LPCTSTR p_szCameraName );
	virtual HRESULT UpdateCameraImage( const SVGUID& p_CameraID );

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStruct );
	virtual BOOL SetImageDepth( long lDepth );

	virtual HRESULT ResetObject();

	virtual GUID GetDigitizerID();
	virtual SVVirtualCamera* GetCamera() const;
	virtual SVString GetCameraName() const;

	virtual HRESULT DisconnectBuffers();
	virtual HRESULT ReconnectBuffers();

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );

	virtual void Persist(SVObjectWriter& rWriter);

	virtual BOOL ResetImageIndex();

	virtual SVImageIndexStruct GetSourceImageIndex( SVProductInfoStruct* pProduct );
	virtual BOOL GetImageHandleIndex( SVImageIndexStruct& rlIndex ) const;

	virtual HRESULT RestoreMainImage( SVInspectionProcess* p_psvInspection );

protected:
	virtual BOOL CreateBuffers( const SVImageInfoClass& p_rImageInfo, SVImageObjectClassPtr p_ImageArrayPtr ) = 0;

	virtual BOOL DestroyImage();

	virtual HRESULT GetImageIndex( SVDataManagerHandle& p_rHandle, const SVImageIndexStruct& rIndex ) const;

	virtual SVImageObjectClassPtr GetCameraBufferArrayPtr() const;

	virtual HRESULT RegisterAsSubObject();
	virtual HRESULT UnregisterAsSubObject();

	virtual HRESULT RebuildCameraImage();

	SVVirtualCamera* mpCamera;
	SVGUID digitizerObjectID;
	SVImageIndexStruct m_CurrentIndex;
};

typedef std::set< SVCameraImageTemplate* > SVCameraImagePtrSet;

