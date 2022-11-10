// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxErrorEnum.h"
#include "Definitions/SVMatroxSimpleEnums.h"
#include "SVFileSystemLibrary/ImageFileFormats.h"
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
The PutLine function uses the Matrox function MbufPutLine. This function reads pixel values from a user-defined array and writes them to the series of pixels, in the specified image, along the theoretical line defined by specified coordinates. The Bresenham algorithm is used to determine the theoretical line. 
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

	SVMatroxBufferInterface() = default;

	virtual ~SVMatroxBufferInterface() = default;

	//******* Create Functions *********
	// MbuffAlloc1d or Line
	static HRESULT Create( SVMatroxBuffer& rBuffer, const SVMatroxBufferCreateLineStruct& CreateLineStruct); //!< creates a Matrox buffer using a SVMatroxBufferCreateLineStruct.
	// Mainly for Grab Buffers
	static HRESULT Create( const SVMatroxSystem& rSystem, SVMatroxBuffer& rBuffer, const SVMatroxBufferCreateStruct& CreateStruct); //!< creates a Matrox buffer using a SVMatroxSystem and a SVMatroxBufferCreateStruct
	// MbufAllocColor - Standard buffer..
	static HRESULT Create( SVMatroxBuffer& rBuffer, const SVMatroxBufferCreateStruct& CreateStruct); //!< creates a Matrox buffer using a SVMatroxBufferCreateStruct.
	// MbufChildColor2d - CreateChild buffer
	static HRESULT Create( SVMatroxBuffer& rNewBuffer, const SVMatroxBufferCreateChildStruct& CreateChildStruct);//!< creates a Matrox child buffer using a SVMatroxBufferCreateChildStruct.
	// MbufCreateColor -  Caution	 : Uses external data buffer that you must manage.
	static HRESULT Create( SVMatroxBuffer& rBuffer, SVMatroxBufferCreateExtStruct CreateColorStruct); //!< creates a Matrox buffer using a SVMatroxBufferCreateExtStruct. This type is used for interfacing to other libraries such as Intek.

	static HRESULT Create( SVMatroxBuffer& rNewBuffer, const SVMatroxBuffer& CreateFrom, bool addDibFlag = false ); //!< creates a Matrox buffer using a SVMatroxBuffer.

	// HBitmapToNewMilHandle
	static HRESULT Create( SVMatroxBuffer& rNewMilId, HBITMAP& rHbm ); //!< creates a SVMatroxBuffer from a HBitmap.

	static HRESULT createToHBitmap(SVMatroxBuffer& rNewMilId, const HBITMAP& rHbm);

	// ****** Copy Buffer  **********
	// MbufCopy
	static HRESULT CopyBuffer(const SVMatroxBuffer& rTo, const SVMatroxBuffer& rFrom ); //!< copies the data from a SVMatroxBuffer to a SVMatroxBuffer.
	// MbufCopyClip
	static HRESULT CopyBuffer(const SVMatroxBuffer& rTo, const SVMatroxBuffer& rFrom, long lXOffset, long lYOffset); //!< copies the data from a SVMatroxBuffer to a SVMatroxBuffer with x and y offsets.  If the destination is not large enough to put the source with its offsets then it is clipped.
	// MbufCopyColor
	static HRESULT CopyBuffer(const SVMatroxBuffer& rTo, const SVMatroxBuffer& rFrom, long lBand ); //!< copies the data from a SVMatroxBuffer to a SVMatroxBuffer with x and y offsets.  If the destination is not large enough to put the source with its offsets then it is clipped.

	// HBitmapToMilHandle
	static HRESULT CopyBuffer(const  SVMatroxBuffer& rMilId, HBITMAP& rHbm ); //!< copies the data from a HBitmap to a SVMatroxBuffer.

	///copies ImageBuffer to  string rToDIB starting with rToDIB[offset], ensures positive height if normalize_y = true   
	static HRESULT CopyBufferToFileDIB(std::string& rToDib,  SVBitmapInfo& rBitMapInfo, const SVMatroxBuffer& rFromId, bool addFileHeader = true);
	

	// ****** Information 
	static HRESULT IsParent( const SVMatroxBuffer& rParentBuffer, const SVMatroxBuffer& rChildBuffer );

	static HRESULT GetPositionPoint( POINT& rPoint, const SVMatroxBuffer& rBuffer );
	static HRESULT GetPositionPoint( SVPoint<long>& rPoint, const SVMatroxBuffer& rBuffer );

public:
	static HRESULT GetBitmapInfo( SVBitmapInfo& rBitmapInfo, const SVMatroxBuffer& rBuffer, bool* pIsMilInfo = nullptr );

	template <typename T>
	static HRESULT GetHostAddress(T** rpHostAddress, const SVMatroxBuffer& rBuffer)
	{
		HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
		try
#endif
		{
			if (!rBuffer.empty())
			{
				MbufInquire(rBuffer.GetIdentifier(), M_HOST_ADDRESS, rpHostAddress);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				assert(rpHostAddress);
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
#ifdef USE_TRY_BLOCKS
		catch (...)
		{
			l_Code = SVMEE_MATROX_THREW_EXCEPTION;
			SVMatroxApplicationInterface::LogMatroxException();
		}
#endif
		assert(S_OK == l_Code);
		return l_Code;
	}

	// ****** Get and Set Functions **********
	static HRESULT Get( const SVMatroxBuffer& rBuf, SVMatroxBufferInfoEnum eWhat, double& rResult ); //!< uses MbufInquire to get information about a SVMatroxBuffer and stores it in a double.
	static HRESULT Get( const SVMatroxBuffer& rBuf, SVMatroxBufferInfoEnum eWhat, long& rResult );
	static HRESULT Get( const SVMatroxBuffer& rBuf, SVMatroxBufferInfoEnum eWhat, LONGLONG& rResult ); //!< uses MbufInquire to get information about a SVMatroxBuffer and stores it in a LONGLONG.

	// ****** Put Functions **********
	// MbufPut
	static HRESULT PutBuffer(const SVMatroxBuffer& rTo, const unsigned char* pcArrayData ); //!< copies the data from a user array to a MatroxBuffer.
	static HRESULT PutBuffer(const SVMatroxBuffer& rTo, const long* plArrayData ); //!< copies the data from a user array to a MatroxBuffer.
	
	// MbufPut1d
	static HRESULT PutLine(const SVMatroxBuffer& rTo, long lCount, const unsigned char* pArrayData); //<! copies the data from a user array to a MatroxBuffer.
	// MbufGet1d

	static HRESULT Set(const SVMatroxBuffer& rBuf, SVMatroxBufferInfoEnum eWhat, const double rdValue); //<! allows you to control a specified data buffer settings with a supplied double.
	static HRESULT Set( const SVMatroxBuffer& rBuf, SVMatroxBufferInfoEnum eWhat, const long long rlValue ); //1< allows you to control a specified data buffer settings with a supplied long.

	// ****** Miscellaneous *********
	static HRESULT ControlNeighborhood(const SVMatroxBuffer& rBuf, SVMatroxBufferInfoEnum eWhat, const long lValue); //!< changes the setting of an operation control type of the specified kernel buffer or structuring element buffer.

	static HRESULT ClearBuffer(const SVMatroxBuffer& rBuffer, double dColor ); //!< clears the entire specified buffer to the specified color.

	// ***** Inport / Export *********
	static HRESULT Import(SVMatroxBuffer& rBuf, const std::string& rFileName, ImageFileFormat fileFormat, bool bRestore= false ); //!< imports data from a file into an existing or automatically allocated MIL data buffer.
	static HRESULT Export(const SVMatroxBuffer& rBuf, const std::string& rFileName, ImageFileFormat fileFormat ); //!< exports a data buffer to a file, in the specified output file format.

	//************************************
	//! Get  the Dimension  from the Imagefile 
	//! \param rFileName [in]
	//! \param Width [out]
	//! \param Height [out]
	//! \returns S_OK if no error occur 
	//************************************
	static HRESULT GetImageSize(const std::string& rFileName, long &rWidth, long &rHeight);

	static HRESULT InquireBufferProperties(const SVMatroxBuffer& rBuffer, MatroxImageProps& rImageProps );
	static HRESULT CreateBuffer(SVMatroxBuffer& rBuffer, MatroxImageProps& rImageProps , void *Memory);

	static HRESULT CopyBuffer(const SVMatroxBuffer& rTo, __int64 From );
	static HRESULT CopyBuffer( __int64 To, const SVMatroxBuffer& rFrom );
protected:
	static void createImageBufferPtr(SVMatroxBuffer& rBuffer, __int64 MatroxID, const std::string& rCreatorName);

private:
	static HRESULT GetBitmapInfo( LPBITMAPINFO& rpBitmapInfo, const SVMatroxBuffer& rBuffer ); //!< returns a pointer (LPBITMAPINFO) to the header of the DIB associated with the MIL buffer (if any) or NULL.
	static __int64 Convert2MatroxType( SVMatroxBufferAttributeEnum eType ); //!< converts the SVMatroxBufferTypeEnum to a Matrox Type.
	static long Convert2MatroxType   ( SVMatroxBufferTypeEnum eType ); //!< converts the SVMatroxBufferTypeEnum to a Matrox Type.
	static long Convert2MatroxType   ( SVMatroxBufferInfoEnum eType ); //!< converts the SVMatroxBufferTypeEnum to a Matrox Type.
};

