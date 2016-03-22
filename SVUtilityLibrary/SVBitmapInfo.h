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

	static SVColorTable GetDefaultColorTable( unsigned short p_BitCount );
	static unsigned long GetColorTableSize( unsigned short p_BitCount, unsigned long p_Compression );
	static unsigned long GetImageStrideInBytes( long p_Width, unsigned short p_BitCount );
	static unsigned long GetImageSizeInBytes( long p_Width, long p_Height, unsigned short p_BitCount );

	SVBitmapInfo();
	SVBitmapInfo( const SVBitmapInfo& p_rObject );
	SVBitmapInfo( const BITMAPINFO& p_rObject );
	SVBitmapInfo( long p_Width, long p_Height, unsigned short p_BitCount, const SVColorTable& p_rColorTable );

	virtual ~SVBitmapInfo();

	bool empty() const;

	void clear();

	size_t GetBitmapInfoSizeInBytes() const;
	size_t GetBitmapImageSizeInBytes() const;
	size_t GetBitmapImageStrideInBytes() const;

	long GetWidth() const;
	long GetHeight() const;
	unsigned short GetBitCount() const;
	unsigned long GetColorTableSize() const;
	RGBQUAD* GetColorTable() const;
	SVPaletteTable GetPaletteTable() const;

	BITMAPINFO* GetBitmapInfo();
	const BITMAPINFO* GetBitmapInfo() const;

	void FlipHeight();

	void Assign( const BITMAPINFO& p_rObject );
	void Assign( long p_Width, long p_Height, unsigned short p_BitCount, const SVColorTable& p_rColorTable );

	const SVBitmapInfo& operator=( const SVBitmapInfo& p_rObject );

	bool operator==( const SVBitmapInfo& p_rObject ) const;
	bool operator==( const BITMAPINFO& p_rObject ) const;
	bool operator==( const BITMAPINFOHEADER& p_rObject ) const;

	bool operator!=( const SVBitmapInfo& p_rObject ) const;
	bool operator!=( const BITMAPINFO& p_rObject ) const;
	bool operator!=( const BITMAPINFOHEADER& p_rObject ) const;

protected:
	size_t m_BufferSize;
	unsigned char* m_pBuffer;
};

bool operator==( const BITMAPINFO& p_rLeft, const SVBitmapInfo& p_rRight );
bool operator==( const BITMAPINFOHEADER& p_rLeft, const SVBitmapInfo& p_rRight );

