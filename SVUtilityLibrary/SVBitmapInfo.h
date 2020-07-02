//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBitmapInfo
//* .File Name       : $Workfile:   SVBitmapInfo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:20:24  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

class SVBitmapInfo
{
public:
	typedef std::vector< RGBQUAD > SVColorTable;
	typedef std::vector< PALETTEENTRY > SVPaletteTable;

	static SVColorTable GetDefaultColorTable( int bitCount );
	static unsigned long GetColorTableSize(int bitCount, unsigned long compression );
	static unsigned long GetImageStrideInBytes( long width, int bitCount );
	static unsigned long GetImageSizeInBytes( long width, long height, int bitCount );

	SVBitmapInfo();
	SVBitmapInfo( const SVBitmapInfo& rObject );
	explicit SVBitmapInfo(const BITMAPINFO& rObject);
	SVBitmapInfo( long width, long height, int bitCount, const SVColorTable& rColorTable );

	virtual ~SVBitmapInfo();

	bool empty() const;

	void clear();

	size_t GetBitmapInfoSizeInBytes() const;
	size_t GetBitmapImageSizeInBytes() const;
	size_t GetBitmapImageStrideInBytes() const;

	long GetWidth() const;
	long GetHeight() const;
	int GetBitCount() const;
	unsigned long GetColorTableSize() const;
	RGBQUAD* GetColorTable() const;
	SVPaletteTable GetPaletteTable() const;

	BITMAPINFO* GetBitmapInfo();
	const BITMAPINFO* GetBitmapInfo() const;

	void FlipHeight();

	void Assign( const BITMAPINFO& rObject );
	void Assign( long width, long height, int bitCount, const SVColorTable& rColorTable );

	const SVBitmapInfo& operator=( const SVBitmapInfo& rObject );

	bool operator==( const SVBitmapInfo& rObject ) const;
	bool operator==( const BITMAPINFO& rObject ) const;
	bool operator==( const BITMAPINFOHEADER& rObject ) const;

	bool operator!=( const SVBitmapInfo& rObject ) const;
	bool operator!=( const BITMAPINFO& rObject ) const;
	bool operator!=( const BITMAPINFOHEADER& rObject ) const;

protected:
	size_t m_BufferSize;
	unsigned char* m_pBuffer;
};

bool operator==( const BITMAPINFO& rLhs, const SVBitmapInfo& rRhs);
bool operator==( const BITMAPINFOHEADER& rLhs, const SVBitmapInfo& rRhs);

