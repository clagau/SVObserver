
#include "SVBitmapInfo.h"
#include "SVMatroxBuffer.h"


template <typename T>
class SVPoint
{
public:

	SVPoint()
	{
	}
	explicit SVPoint(const POINT& rRhs) { *this = rRhs; }
	SVPoint(const SVPoint& rRhs) = default;
	SVPoint(SVPoint&& Rhs) = default;

	SVPoint(T x, T y) { m_x = x; m_y = y; }
	explicit SVPoint(const _variant_t& rVariant) { *this = rVariant; }

	virtual ~SVPoint() = default;

	std::string toString() const
	{
		return std::string();
	}

	SVPoint& operator=(const POINT& rRhs)
	{
		m_x = static_cast<T>(rRhs.x);
		m_y = static_cast<T>(rRhs.y);

		return (*this);
	}

	SVPoint& operator=(const SVPoint& rRhs) = default;

	SVPoint& operator=(const _variant_t& rRhs)
	{
		m_x = {};
		m_y = {};

		return (*this);
	}

	bool operator==(const SVPoint& rRhs) const
	{
		return (m_x == rRhs.m_x && m_y == rRhs.m_y);
	}

	bool operator!=(const SVPoint& rRhs) const
	{
		return (m_x != rRhs.m_x || m_y != rRhs.m_y);
	}

	friend SVPoint operator-(const SVPoint& rLhs, const SVPoint& rRhs)
	{
		SVPoint point {rLhs};
		point.m_x -= rRhs.m_x;
		point.m_y -= rRhs.m_y;
		return point;
	}

	friend SVPoint operator+(const SVPoint& rLhs, const SVPoint& rRhs)
	{
		SVPoint point {rLhs};
		point.m_x += rRhs.m_x;
		point.m_y += rRhs.m_y;
		return point;
	}

	const SVPoint& operator-=(const SVPoint& rRhs)
	{
		*this = *this - rRhs;

		return *this;
	}

	const SVPoint& operator+=(const SVPoint& rRhs)
	{
		*this = *this + rRhs;
		return *this;
	}

	friend SVPoint operator/(const SVPoint& rPoint, double constant)
	{
		return SVPoint {static_cast<T> (rPoint.m_x / constant), static_cast<T> (rPoint.m_y / constant)};
	}

	explicit operator POINT() const
	{
		return POINT {static_cast<long> (m_x), static_cast<long> (m_y)};
	}

	explicit operator _variant_t () const
	{
		return _variant_t.SetString(toString());
	}

	void clear()
	{
		m_x = {};
		m_y = {};
	}

	POINT getPoint() const { return static_cast<POINT> (*this); }

	T m_x {};
	T m_y {};
};

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
