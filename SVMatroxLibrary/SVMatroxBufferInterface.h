// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBufferInterface
// * .File Name       : $Workfile:   SVMatroxBufferInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 11:08:10  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxBuffer.h"
#include "SVMatroxSimpleEnums.h"
#include "SVMatroxBufferCreateExtStruct.h"
#include "SVUtilityLibrary/SVPoint.h"
#pragma endregion Includes


class SVBitmapInfo;
class SVMatroxSystem;
struct MatroxImageProps;
struct SVMatroxBufferCreateChildStruct;
struct SVMatroxBufferCreateLineStruct;
struct SVMatroxBufferCreateStruct;


/**
@SVObjectName Matrox Buffer Interface

@SVObjectOverview This class contains interface functions for managing Matrox buffers.

@SVObjectOperations There are five create functions differing by their parameters.  Each function has a SVMatroxBuffer& as its first parameter.  
The Create(SVMatroxBuffer&, SVMatroxBufferCreateLineStruct&) creates a SVMatroxBuffer using the Matrox MbufAlloc1d to create a one dimensional one-band data buffer. This is typically used for a line buffer.
The Create(SVMatroxBuffer&, LPBITMAPINFO) creates a SVMatroxBuffer using the Matrox MbufAllocColor making either a one banded or three banded image.
The Create(SVMatroxBuffer&, SVMatroxBufferCreateStruct&) creates a SVMatroxBuffer using the standard create structure. This function uses the Matrox MbufAllocColor function.
The Create(SVMatroxBuffer&, SVMatroxBufferCreateChildStruct&) creates a child buffer using the Matrox MbufChildColor2d.  
The Create(SVMatroxBuffer&, SVMatroxBufferCreateExtStructure&) creates a SVMatroxBuffer using the Matrox MbufCreateColor function.  This function would be used for non-matrox digitizers where the user is responsible for the memory rather than Matrox.
The DestroyBuffer function frees handles that were created using the create functions.
The GetCreateInfo fills the create structure from the supplied handle using the Matrox function MbufInquire. This information should be the same as when the buffer was created. The Matrox function MbufInquire is used to extract information from the handle.
The Copy functions copy either a portion of the buffer or all the buffer from one handle to another.
The CopyBuffer with two parameters uses the Matrox function MbufCopy. This function copies the specified source buffer data to the specified destination buffer. If the source and destination buffers are of different data types, MIL converts the data automatically.
The CopyBuffer with four parameters uses the Matrox function MbufCopyClip. This function copies the source buffer data to the destination buffer, starting at the specified offset. Data outside of the destination buffer is not copied (it is clipped).
The CopyBuffer with three parameters uses the Matrox function MbufCopyColor. This function copies one or all color bands of the specified source buffer to the specified destination buffer. It can also be used to insert or extract a color component from a color image. 
The Put functions put raw data into a SVMatroxBuffer. The format of the raw data is unsigned char*.
The PutBuffer function uses the Matrox function MbufPut. This function copies data from a user-supplied array to a specified SVMatroxBuffer.
The PutColor function uses the Matrox function MbufPutColor.  This function copies data from a user-supplied array to one or all bands of a specified MIL destination buffer.
The PutLine function uses the Matrox function MbufPutLine. This function reads pixel values from a user-defined array and writes them to the series of pixels, in the specified image, along the theoretical line defined by specified coordinates. The Bresenham algorithm is used to determine the theoretical line. 
The GetLine function uses the Matrox function MbufGet1d. This function copies data from a specified one-dimensional area of a SVMatroxBuffer to a user-supplied array. For multi-band buffers, this function linearly copies the data from the one-dimensional region of each band (RRR...GGG...BBB...). 
The Get function uses the Matrox function MbufInquire. This function inquires about a specified setting of a SVMatroxBuffer.
The Set function uses the Matrox function MbufControl. This function allows you to control a specified data buffer setting.
The ControlNeighborhood function uses the Matrox function MbufControlNeighborhood. This function changes the setting of an operation control type of the specified kernel buffer or structuring element buffer. The operation control type settings establish how to perform a neighborhood operation when using the specified kernel buffer or structuring element buffer.
The ClearBuffer function uses the Matrox function MbufClear.  This function clears the entire specified buffer to the specified color which is usually black.
The Import function uses the Matrox function MbufImport. This function imports data from a file into an existing or automatically allocated MIL data buffer. The function assumes that the data in the file is in the specified format.
The Export function uses the Matrox function MbufExport. This function exports a data buffer to a file, in the specified output file format.
*/
class SVMatroxBufferInterface
{
public:
	friend class SVMatroxGige;

	SVMatroxBufferInterface();

	virtual ~SVMatroxBufferInterface();

	//******* Create Functions *********
	// MbuffAlloc1d or Line
	static HRESULT Create( SVMatroxBuffer& p_rBuffer, const SVMatroxBufferCreateLineStruct& p_CreateLineStruct);
	// CreateBuffer LPBitmapInfo..
	static HRESULT Create( SVMatroxBuffer& p_rBuffer, const LPBITMAPINFO p_pBitmapInfo );
	// Mainly for Grab Buffers
	static HRESULT Create( const SVMatroxSystem& p_rSystem, SVMatroxBuffer& p_rBuffer, const SVMatroxBufferCreateStruct& p_CreateStruct);
	// MbufAllocColor - Standard buffer..
	static HRESULT Create( SVMatroxBuffer& p_rBuffer, const SVMatroxBufferCreateStruct& p_CreateStruct);
	// MbufChildColor2d - CreateChild buffer
	static HRESULT Create( SVMatroxBuffer& p_rNewBuffer, const SVMatroxBufferCreateChildStruct& p_CreateChildStruct);
	// MbufCreateColor -  Caution	 : Uses external data buffer that you must manage.
	static HRESULT Create( SVMatroxBuffer& p_rBuffer, SVMatroxBufferCreateExtStruct p_CreateColorStruct);

	static HRESULT Create( SVMatroxBuffer& p_rNewBuffer, const SVMatroxBuffer& p_CreateFrom );

	// HBitmapToNewMilHandle
	static HRESULT Create( SVMatroxBuffer& p_rNewMilId, HBITMAP& p_rHbm );
	// MilHandleToNewHBitmap
	static HRESULT Create( HBITMAP& p_rHbm, const SVMatroxBuffer& p_rFromId );

	// ****** Copy Buffer  **********
	// MbufCopy
	static HRESULT CopyBuffer( SVMatroxBuffer& p_rTo, const SVMatroxBuffer& p_rFrom );
	// MbufCopyClip
	static HRESULT CopyBuffer( SVMatroxBuffer& p_rTo, const SVMatroxBuffer& p_rFrom, long p_lXOffset, long p_lYOffset);
	// MbufCopyColor
	static HRESULT CopyBuffer( SVMatroxBuffer& p_rTo, const SVMatroxBuffer& p_rFrom, long p_lBand );

	// HBitmapToMilHandle
	static HRESULT CopyBuffer( SVMatroxBuffer& p_rMilId, HBITMAP& p_rHbm );
	static HRESULT CopyBuffer( HBITMAP& p_rsvDestDIB, SVMatroxBuffer& p_rFromId );

	///copies ImageBuffer to  string rToDIB starting with rToDIB[offset], ensures positive height if normalize_y = true   
	static HRESULT CopyBufferToFileDIB(std::string& rToDib,  SVBitmapInfo& rBitMapInfo, const SVMatroxBuffer& rFromId, bool addFileHeader = true);
	

	// ****** Information 
	static HRESULT IsParent( const SVMatroxBuffer& p_rParentBuffer, const SVMatroxBuffer& p_rChildBuffer );

	static HRESULT GetPositionPoint( POINT& rPoint, const SVMatroxBuffer& p_rBuffer );
	static HRESULT GetPositionPoint( SVPoint<long>& rPoint, const SVMatroxBuffer& p_rBuffer );

public:
	static HRESULT GetBitmapInfo( SVBitmapInfo& p_rBitmapInfo, const SVMatroxBuffer& p_rBuffer, bool* pIsMilInfo = nullptr );

	static HRESULT GetHostAddress( LPVOID p_rpHostAddress, const SVMatroxBuffer& p_rBuffer );

	static HRESULT GetDDSurfaceInterfacePtr( LPVOID& p_rpSurface, const SVMatroxBuffer& p_rBuffer );

	static HRESULT GenLutRamp( SVMatroxBuffer& p_rMilId, long StartIndex, double StartValue, long EndIndex, double EndValue );

	static HRESULT GetLine( SVMatroxBuffer& p_rBuf, __int64 p_lXStart, __int64 p_lYStart, __int64 p_lXEnd, __int64 p_lYEnd, __int64& p_rlNbrPixels, void* p_pUserArray );

	// ****** Get and Set Functions **********
	static HRESULT Get( const SVMatroxBuffer& p_rBuf, SVMatroxBufferInfoEnum p_eWhat, double& rResult );
	static HRESULT Get( const SVMatroxBuffer& p_rBuf, SVMatroxBufferInfoEnum p_eWhat, long& rResult );
	static HRESULT Get( const SVMatroxBuffer& p_rBuf, SVMatroxBufferInfoEnum p_eWhat, LONGLONG& rResult );

	// ****** Put Functions **********
	// MbufPut
	static HRESULT PutBuffer( SVMatroxBuffer& p_rTo, const unsigned char* p_pcArrayData );
	static HRESULT PutBuffer( SVMatroxBuffer& p_rTo, const long* p_plArrayData );
	// MbufPutColor
	static HRESULT PutColor( SVMatroxBuffer& p_rTo, const unsigned char* p_pArrayData );

	// MbufPut1d
	static HRESULT PutLine( SVMatroxBuffer& p_rTo, long p_lCount, const unsigned char* p_pArrayData);
	// MbufGet1d

	static HRESULT Set(const SVMatroxBuffer& p_rBuf, SVMatroxBufferInfoEnum p_eWhat, const double p_rdValue);
	static HRESULT Set( const SVMatroxBuffer& p_rBuf, SVMatroxBufferInfoEnum p_eWhat, const long long p_rlValue );

	// ****** Miscellaneous *********
	static HRESULT ControlNeighborhood( SVMatroxBuffer& p_rBuf, SVMatroxBufferInfoEnum p_eWhat, const long p_lValue);

	static HRESULT ClearBuffer( SVMatroxBuffer& p_rBuffer, double p_dColor );

	// ***** Inport / Export *********
	static HRESULT Import(SVMatroxBuffer& p_rBuf, const std::string& rFileName, SVMatroxFileTypeEnum p_eFileType, bool p_bRestore= false );
	static HRESULT Export(const SVMatroxBuffer& p_rBuf, const std::string& rFileName, SVMatroxFileTypeEnum p_eFileType );

	//************************************
	//! Get  the Dimension  from the Imagefile 
	//! \param rFileName [in]
	//! \param Width [out]
	//! \param Height [out]
	//! \returns S_OK if no error occur 
	//************************************
	static HRESULT GetImageSize(const std::string& rFileName, long &rWidth, long &rHeight);

	static bool IsChildBuffer(const SVMatroxBuffer& p_rBuffer);
	static bool IsColorBandBuffer(const SVMatroxBuffer& p_rBuffer);
	static bool IsColorBuffer(const SVMatroxBuffer& p_rBuffer);

	static HRESULT InquireBufferProperties(const SVMatroxBuffer& p_rBuffer, MatroxImageProps& rImageProps );
	static HRESULT CreateBuffer(SVMatroxBuffer& p_rBuffer, MatroxImageProps& rImageProps , void *Memory);


protected:
	static HRESULT CopyBuffer( SVMatroxBuffer& p_rTo, __int64 p_From );
	static HRESULT CopyBuffer( __int64 p_To, const SVMatroxBuffer& p_rFrom );

	static void createImageBufferPtr(SVMatroxBuffer& rBuffer, __int64 MatroxID, const std::string& rCreatorName);

private:
	static HRESULT GetBitmapInfo( LPBITMAPINFO& p_rpBitmapInfo, const SVMatroxBuffer& p_rBuffer );
	static __int64 Convert2MatroxType( SVMatroxBufferAttributeEnum p_eType );
	static long Convert2MatroxType   ( SVMatroxBufferTypeEnum p_eType );
	static long Convert2MatroxType   ( SVMatroxBufferInfoEnum p_eType );
};

