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
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "SVImageLibrary/SVLightReference.h"
#include "SVImageLibrary/SVLut.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "TriggerInformation/SVTriggerObject.h"
#include "TriggerInformation/SVTriggerRelayClass.h"
#include "TriggerInformation/SVFileAcquisitionInitiator.h"
#include "SVAcquisitionClass.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVValueObjectLibrary/BasicValueObjects.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "SVOResource/resource.h"
#pragma endregion Includes

class SVDeviceParamCollection;

namespace SvIe
{
class SVVirtualCamera;

#pragma region Declarations
typedef std::vector<SVVirtualCamera*> SVVirtualCameraPtrVector;
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
	bool GetImageInfo( SVImageInfoClass *pImageInfo );

	//************************************
	//! This method is used to synchronize parameters mirrored in the object manager
	//! \param pSender <in> pointer from the object which sent the notification
	//! \param Type <in> the refresh type pre or post
	//! \param S_OK on success
	//************************************
	virtual HRESULT RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type ) override;

	bool Create( LPCTSTR p_szDeviceName );
	bool Destroy();

	// Runtime Functions
	bool CanGoOnline() const;
	bool GoOnline();
	bool GoOffline();

	bool RegisterFinishProcess( void *pOwner, LPSVFINISHPROC pFunc );
	bool UnregisterFinishProcess( void *pOwner, LPSVFINISHPROC pFunc );

    SVAcquisitionClassPtr GetAcquisitionDevice() const { return mpsvDevice; }
	
    HRESULT SetLightReference( SVLightReference& rArray );
	HRESULT GetLightReference( SVLightReference& rArray ) const;
	HRESULT SetLut( const SVLut& lut );
	HRESULT GetLut( SVLut& lut ) const;

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;

	SvTrc::IImagePtr ReserveNextImageHandle( ) const;

	SVAcquisitionClassPtr mpsvDevice;
	SVCallbackClassPtrQueue m_CallbackList;

    HRESULT GetBand(int& riBand) const;
    HRESULT GetBandSize(int& riBandSize) const;

	inline long BandLink() {return mlBandLink;}
	inline long SetBandLink(long l) {return mlBandLink = l;}

	bool IsFileAcquisition() const;
	void SetFileAcquisitionMode(bool bFileAcquisition);

	LPCTSTR GetImageFilename() const;
	void SetImageFilename(const std::string& filename);

	LPCTSTR GetImageDirectoryName() const;
	void SetImageDirectoryName(const std::string& directoryName);

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

	void setTempImage(const SVMatroxBuffer pImage);
	SVMatroxBuffer getTempImage();
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	static HRESULT CALLBACK SVImageCallback( void *pvOwner, void *pvCaller, void *pvResponse );

	void FinishProcess( SVODataResponseClass *pResponse );
#pragma endregion Protected Methods

private:
#pragma region Private Methods
	bool DestroyLocal();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	long mlBandLink;

private:
	SvVol::BasicValueObjects m_CameraValues;
	bool m_bFileAcquisition;
	bool m_IsColor;
	std::string m_imageFilename;
	std::string m_imageDirectoryName;
	bool m_bImageSizeEditModeFileBased;
	long m_imageLoadingMode;
	SIZE m_imageSize;
	SvTi::SVTriggerRelayClass<SvTi::SVFileAcquisitionInitiator> m_triggerRelay;
	long m_CameraID;

	std::mutex m_tmpImage_mutex;
	SVMatroxBuffer m_tmpImage;
#pragma endregion Member Variables
};

inline bool isLessByName(SVVirtualCamera* lhs, SVVirtualCamera* rhs)
{
	return std::string(lhs->GetName()) < std::string(rhs->GetName());
}

} //namespace SvIe
