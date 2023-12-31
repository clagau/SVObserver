//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVAcquisitionClass.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVValueObjectLibrary/BasicValueObjects.h"
#include "SVOResource/resource.h"
#pragma endregion Includes

class SVDeviceParamCollection;
class SVIOTriggerLoadLibraryClass;
class SVLightReference;
class SVLongValueDeviceParam;
class SVLut;

namespace SvTrig
{
class SVTriggerClass;
}

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
	explicit SVVirtualCamera(LPCSTR ObjectName) : SVObjectClass(ObjectName) {}
	SVVirtualCamera(SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCAMERAOBJECT) : SVObjectClass(pOwner, StringResourceID) {}
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

	bool RegisterCallback(ULONG_PTR pPPQ, PpqCameraCallBack pPpqCameraCallback);
	bool UnregisterCallback(ULONG_PTR pPPQ);

    SVAcquisitionClassPtr GetAcquisitionDevice() const { return m_pCurrentDevice; }
	
    HRESULT SetLightReference( SVLightReference& rArray );
	HRESULT GetLightReference( SVLightReference& rArray ) const;
	HRESULT SetLut( const SVLut& lut );
	HRESULT GetLut( SVLut& lut ) const;

	virtual HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const override;

	SvOi::ITRCImagePtr ReserveNextImageHandle( ) const;

    HRESULT GetBand(int& riBand) const;
    HRESULT GetBandSize(int& riBandSize) const;

	long BandLink() {return mlBandLink;}
	long SetBandLink(long l) {return mlBandLink = l;}

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

	void	createCameraParameters();
	HRESULT updateCameraParameters();
	HRESULT updateCameraLongParameter( LPCTSTR Name, const SVLongValueDeviceParam* pLongValueDeviceParam);
	HRESULT updateDeviceParameters(SVDeviceParamCollection& rCameraParameters);

	void SetIsColor( bool IsColor ){ m_IsColor = IsColor; };
	bool IsColor() const { return m_IsColor; };

	void setCameraID( long CameraID ) { m_CameraID = CameraID; };
	long getCameraID() const { return m_CameraID; };

	void setTempImage(const SVMatroxBuffer pImage);
	SVMatroxBuffer getTempImage();

	SvVol::BasicValueObjectPtr getCameraValue(LPCTSTR pName);

	void addNeededBuffer(uint32_t id, int neededBufferSize);
	void removeNeededBufferEntry(uint32_t id);

	bool canExternalSoftwareTrigger() const { return m_canExternalSoftwareTrigger; };

	void setAcquisitionDevice(bool fileAcquisition);

	HRESULT setParameterValue(int parameterID, _variant_t value);
#pragma endregion Public Methods

private:
#pragma region Private Methods
	void setNeededBuffer();
	bool DestroyLocal();
#pragma endregion Private Methods

#pragma region Member Variables
protected:
	long mlBandLink{ 0L };

private:
	SVAcquisitionClassPtr m_pCurrentDevice;
	SVAcquisitionClassPtr m_pMainDevice;
	SVAcquisitionClassPtr m_pFileDevice;
	SvVol::BasicValueObjects m_CameraValues;
	bool m_bFileAcquisition{ false };
	bool m_IsColor{ false };
	bool m_canExternalSoftwareTrigger{ false };
	std::string m_imageFilename;
	std::string m_imageDirectoryName;
	bool m_bImageSizeEditModeFileBased{ true };
	long m_imageLoadingMode{ 0L };
	SIZE m_imageSize{ 0L, 0L };
	long m_CameraID{ 0L };

	std::mutex m_tmpImage_mutex;
	SVMatroxBuffer m_tmpImage;
	std::unordered_map<uint32_t, int> m_neededBufferMap;
#pragma endregion Member Variables
};

inline bool isLessByName(SVVirtualCamera* lhs, SVVirtualCamera* rhs)
{
	return std::string(lhs->GetName()) < std::string(rhs->GetName());
}

} //namespace SvIe
