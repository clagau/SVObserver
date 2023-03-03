//****************************************************************************
/// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file Digitizer.h
/// \brief Digitizer structures required to be able to load SVMatroxGige.dll
//******************************************************************************

/***Note this file has copies of structure used in SVObserver***/
#include "SVMatroxBuffer.h"
#include "SVUtilityLibrary/SVPoint.h"
#include "SVUtilityLibrary/SVBitmapInfo.h"

class SVImageBufferHandleInterface
{
public:

	bool empty() const { return false; }
	void clear() {};
	
	SVPoint<long> GetPositionPoint() const { return SVPoint<long>{0L, 0L}; }
	SVBitmapInfo GetBitmapInfo() const { return SVBitmapInfo {}; }
	unsigned char* GetBufferAddress() const { return nullptr; }

	const SVMatroxBuffer& GetBuffer() const { return m_MatroxBuffer; }
	SVMatroxBuffer& GetBuffer() { return m_MatroxBuffer; }

protected:
	SVImageBufferHandleInterface() {};

private:
	// Do not implement
	SVImageBufferHandleInterface(const SVImageBufferHandleInterface& p_rObject) = delete;

	// Do not implement
	const SVImageBufferHandleInterface& operator=(const SVImageBufferHandleInterface& p_rObject) = delete;

	SVMatroxBuffer m_MatroxBuffer;
};

typedef std::shared_ptr< SVImageBufferHandleInterface > SVImageBufferHandlePtr;


class ITRCImage
{
public:

	/// Return if image is only read-only. If only read only, do not use it for changing mil function.
	/// \returns bool
	bool isReadOnly() const { return false; }

	/// Get Handle to the image.
	/// \returns SvOi::SVImageBufferHandlePtr
	SVImageBufferHandlePtr getHandle() const { return SVImageBufferHandlePtr(); }

	/// Get the buffer position. (Needed for internal use)
	/// \returns int
	int getBufferPos() const { return  0; }

	/// Return if not empty image buffer available. 
	/// \returns bool
	bool isEmpty() const { return true; }

	/// Check if this class still valid (after a reset of the TRC, the class become invalid)
	/// \returns bool
	bool isValid() const { return true; }
};

typedef std::shared_ptr<ITRCImage> ITRCImagePtr;

enum CameraDataEnum : int
{
	ChunkFrameID = 0,
	ChunkTimeStamp,
	ChunkLineStatusAll,
	CameraDataNumber
};


struct CameraInfo
{
	CameraInfo() = default;

	double m_startFrameTimestamp {0.0};
	double m_endFrameTimestamp {0.0};

	ITRCImagePtr m_pImage {nullptr};

	std::array<_variant_t, CameraDataEnum::CameraDataNumber> m_cameraData;
};


class SVAcquisitionBufferInterface
{
public:
	virtual ~SVAcquisitionBufferInterface() = default;

	virtual unsigned long GetBufferWidth() const = 0;
	virtual unsigned long GetBufferHeight() const = 0;
	virtual int GetBufferFormat() const = 0;

	virtual ITRCImagePtr GetNextBuffer() = 0;
	virtual HRESULT UpdateWithCompletedBuffer(CameraInfo&& cameraInfo) = 0;
	virtual void setNeededBuffers(int neededBuffers) = 0;
};

typedef void (WINAPI* ImageCallback)(CameraInfo&& cameraInfo);

class SVAcquisitionBuffer : public SVAcquisitionBufferInterface
{
public:
	unsigned long GetBufferWidth() const override { return 640; }
	unsigned long GetBufferHeight() const override { return 480; }
	int GetBufferFormat() const override { return 0; }

	ITRCImagePtr GetNextBuffer() override
	{
		return ITRCImagePtr();
	}

	HRESULT UpdateWithCompletedBuffer(CameraInfo&& cameraInfo) override
	{
		if (nullptr != m_pCallBack)
		{
			m_pCallBack(std::move(cameraInfo));
		}
		return S_OK;
	}
	void setNeededBuffers(int ) override {}

	ImageCallback m_pCallBack {nullptr};
};
#pragma once
