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
#include <vector>
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVBitmapInfo.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:20:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Sep 2011 15:26:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added aditional methods for default bitmap information based on bitmap data elements.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2011 14:02:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jan 2011 10:41:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

