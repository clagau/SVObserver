//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVirtualCamera
//* .File Name       : $Workfile:   SVVirtualCamera.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   17 Jul 2014 20:51:54  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVInfoStructs.h"
#include "SVContainerLibrary/SVMap.h"
#include "SVContainerLibrary/SVSet.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVImageLibrary/SVLut.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "TriggerHandling/SVTriggerRelayClass.h"
#include "TriggerHandling/SVFileAcquisitionInitiator.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVAcquisitionClass.h"
#include "SVValueObjectLibrary/BasicValueObjects.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#pragma endregion Includes

#pragma region Declarations
typedef SVVector< SVVirtualCamera* > SVVirtualCameraArray;
typedef SVSet< SVVirtualCamera* > SVVirtualCameraSet;
typedef std::set< SVVirtualCamera* > SVVirtualCameraPtrSet;
typedef SVSet< SVAcquisitionClassPtr > SVAcquisitionClassSet;
typedef SVMap< CString, SVVirtualCamera* > SVVirtualCameraMap;

class SVORequestClass;
class SVDeviceParamCollection;
#pragma endregion Declarations


class SVVirtualCamera : public SVObjectClass
{
public:
#pragma region Constructor
	SVVirtualCamera( LPCSTR ObjectName );
	SVVirtualCamera( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCAMERAOBJECT );
	virtual ~SVVirtualCamera();
#pragma endregion Constructor

#pragma region Public Methods
	virtual BOOL GetImageInfo( SVImageInfoClass *pImageInfo );

	virtual long GetImageDepth() const;

	//************************************
	//! This method is used to synchronize parameters mirrored in the object manager
	//! \param pSender <in> pointer from the object which sent the notification
	//! \param Type <in> the refresh type pre or post
	//! \param S_OK on success
	//************************************
	virtual HRESULT RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type );

	BOOL Create( LPCTSTR p_szDeviceName );
	BOOL Destroy();

	// Runtime Functions
	BOOL CanGoOnline() const;
	BOOL GoOnline();
	BOOL GoOffline();

	BOOL RegisterFinishProcess( void *pOwner, LPSVFINISHPROC pFunc );
	BOOL UnregisterFinishProcess( void *pOwner, LPSVFINISHPROC pFunc );

    SVAcquisitionClassPtr GetAcquisitionDevice() const { return mpsvDevice; }
	
    HRESULT SetLightReference( SVLightReference& rArray );
	HRESULT GetLightReference( SVLightReference& rArray ) const;
	HRESULT SetLut( const SVLut& lut );
	HRESULT GetLut( SVLut& lut ) const;

	HRESULT GetSourceImageIndex( SVDataManagerHandle& p_rHandle, const SVProductInfoStruct& p_rProduct ) const;
	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;

	HRESULT ReserveNextImageHandleIndex( SVDataManagerHandle& p_rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType = SV_ACQUISITION ) const;
	virtual BOOL ReserveImageHandleIndex( const SVDataManagerHandle& p_rDMIndexHandle ) const;

	virtual BOOL CopyValue( const SVDataManagerHandle& p_From, const SVDataManagerHandle& p_To );

	void DumpDMInfo( LPCTSTR p_szName ) const;

	SVAcquisitionClassPtr mpsvDevice;
	SVCallbackClassPtrQueue m_CallbackList;

    HRESULT GetBand(int& riBand) const;
    HRESULT GetBandSize(int& riBandSize) const;

	inline long BandLink() {return mlBandLink;}
	inline long SetBandLink(long l) {return mlBandLink = l;}

	bool IsFileAcquisition() const;
	void SetFileAcquisitionMode(bool bFileAcquisition);

	LPCTSTR GetImageFilename() const;
	void SetImageFilename(const SVString& filename);

	LPCTSTR GetImageDirectoryName() const;
	void SetImageDirectoryName(const SVString& directoryName);

	long GetFileLoadingMode() const;
	void SetFileLoadingMode(long mode);

	bool IsFileImageSizeEditModeFileBased() const;
	void SetFileImageSizeEditModeFileBased(bool bFileBsed);

	const SIZE& GetFileImageSize() const;
	void SetFileImageSize(const SIZE& size);

	long GetFileImageWidth() const;
	void SetFileImageWidth(long width);

	long GetFileImageHeight() const;
	void SetFileImageHeight(long height);

	HRESULT RegisterTriggerRelay(SVIOTriggerLoadLibraryClass* triggerDLL, unsigned long ulIndex);
	HRESULT UnregisterTriggerRelay();

	void	createCameraParameters();
	HRESULT updateCameraParameters();
	HRESULT updateCameraLongParameter( LPCTSTR Name, const SVLongValueDeviceParam* pLongValueDeviceParam);
	HRESULT updateDeviceParameters(SVDeviceParamCollection& rCameraParameters);

	inline void SetIsColor( bool IsColor ){ m_IsColor = IsColor; };
	inline bool IsColor()const { return m_IsColor; };

	inline void setCameraID( long CameraID ) { m_CameraID = CameraID; };
	inline long getCameraID() const { return m_CameraID; };
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	static HRESULT CALLBACK SVImageCallback( void *pvOwner, void *pvCaller, void *pvResponse );

	virtual void FinishProcess( SVODataResponseClass *pResponse );
#pragma endregion Protected Methods

private:
#pragma region Private Methods
	BOOL DestroyLocal();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	long mlBandLink;
	CStdioFile m_LogFile;

private:
	BasicValueObjects m_CameraValues;
	bool m_bFileAcquisition;
	bool m_IsColor;
	SVString m_imageFilename;
	SVString m_imageDirectoryName;
	bool m_bImageSizeEditModeFileBased;
	long m_imageLoadingMode;
	SIZE m_imageSize;
	SvTh::SVTriggerRelayClass<SvTh::SVFileAcquisitionInitiator> m_triggerRelay;
	long m_CameraID;
#pragma endregion Member Variables
};

