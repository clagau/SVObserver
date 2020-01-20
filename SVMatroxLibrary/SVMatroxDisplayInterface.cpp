// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDisplayInterface
// * .File Name       : $Workfile:   SVMatroxDisplayInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   03 Jun 2014 13:43:08  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVMatroxDisplayInterface.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxBufferTemplate.h"
#include "SVMatroxErrorEnum.h"
#include "SVMatroxHelper.h"
#include "SVMatroxImageBuffer.h"
#include "SVMatroxResourceMonitor.h"
#pragma endregion Includes

/**
@SVOperationName Default Constructor

@SVOperationDescription

*/
SVMatroxDisplayInterface::SVMatroxDisplayInterface()
{
}

/**
@SVOperationName Destructor

@SVOperationDescription

*/
SVMatroxDisplayInterface::~SVMatroxDisplayInterface()
{
}

/**
@SVOperationName Create

@SVOperationDescription This function Creates a SVMatroxDisplay.

*/
HRESULT SVMatroxDisplayInterface::CreateDisplay(__int64& p_rDispId)
{
	HRESULT l_Code(S_OK);

#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_ID l_NewId = MdispAlloc(M_DEFAULT_HOST,
			M_DEFAULT,		// M_AUTOMATIC new doc does not mention anything about this.
			MIL_TEXT("M_DEFAULT"),
			M_WINDOWED,		// M_WINDOWED | M_GDI_OVERLAY
			M_NULL);

		l_Code = SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
		{
			SVMatroxResourceMonitor::InsertIdentifier(SVDisplayID, l_NewId);
			l_Code = DestroyDisplay(p_rDispId);
			p_rDispId = l_NewId;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Allocate Display

@SVOperationDescription This function Creates a SVMatroxBuffer.

*/
HRESULT SVMatroxDisplayInterface::Create(SVMatroxBuffer& p_rDispBufId, SVMatroxBufferCreateStruct p_CreateStruct)
{
	HRESULT l_Code {S_OK};
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		__int64 l_lAttrib = M_IMAGE | M_PROC | M_DISP | M_DIB;

		if (p_CreateStruct.m_eType > 0)
		{

			MIL_ID l_NewBuf = M_NULL;
			if (p_CreateStruct.m_lSizeBand == 3)
			{
				l_NewBuf = MbufAllocColor(M_DEFAULT_HOST,
					p_CreateStruct.m_lSizeBand,
					p_CreateStruct.m_lSizeX,
					p_CreateStruct.m_lSizeY,
					p_CreateStruct.m_eType,
					l_lAttrib,
					M_NULL);
			}
			else
			{
				l_NewBuf = MbufAlloc2d(M_DEFAULT_HOST,
					p_CreateStruct.m_lSizeX,
					p_CreateStruct.m_lSizeY,
					p_CreateStruct.m_eType,
					l_lAttrib,
					M_NULL);
			}
			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if (l_Code == S_OK)
			{
				SVMatroxResourceMonitor::InsertIdentifier(SVDisplayBufferID, l_NewBuf);

				p_rDispBufId.m_BufferPtr = SVMatroxBufferPtr {new SVMatroxImageBuffer(l_NewBuf, "SVMatroxDisplayInterface::Create-BufferCreate")};
			}

		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBuffer

@SVOperationDescription This function Creates a SVMatroxBuffer from an existing SVMatroxBuffer.

*/
HRESULT SVMatroxDisplayInterface::Create(SVMatroxBuffer& p_rNewBuffer, const SVMatroxBuffer& p_CreateFrom)
{
	HRESULT l_Code(S_OK);

	SVMatroxBufferInterface l_BufIntf;
	SVMatroxBuffer l_Buf;
	l_Code = l_BufIntf.Create(l_Buf, p_CreateFrom);
	if (l_Code == S_OK)
	{
		SVMatroxResourceMonitor::InsertIdentifier(SVDisplayBufferID, l_Buf.GetIdentifier());

		p_rNewBuffer.m_BufferPtr = l_Buf.m_BufferPtr;
	}

	return l_Code;
}


/**
@SVOperationName Destroy - SVMatroxDisplay

@SVOperationDescription This function destroys a SVMatroxDisplay.

*/
HRESULT SVMatroxDisplayInterface::DestroyDisplay(__int64& rDispId)
{
	return DestroyMatroxId(rDispId, MdispFree, SVDisplayID);
}

/**
@SVOperationName Destroy - SVMatroxBuffer

@SVOperationDescription This function destroys a SVMatrox Display Buffer.

*/
HRESULT SVMatroxDisplayInterface::Destroy(SVMatroxBuffer& p_rDispBufId)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rDispBufId.empty())
		{

			SVMatroxResourceMonitor::EraseIdentifier(SVDisplayBufferID, p_rDispBufId.GetIdentifier());
			p_rDispBufId.clear();
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}



/**
@SVOperationName Convert2MatroxType

@SVOperationDescription This function converts the SVObserver type to a Matrox type.

*/
long SVMatroxDisplayInterface::Convert2MatroxType(SVDisplayCntrlEnum p_eDisp)
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch (p_eDisp)
	{
		case SVDispUpdate:
		{
			l_lMatroxType = M_UPDATE;
			break;
		}
		case SVDispWindowMove:
		{
			l_lMatroxType = M_WINDOW_MOVE;
			break;
		}
		case SVDispWindowResize:
		{
			l_lMatroxType = M_WINDOW_RESIZE;
			break;
		}
		case SVDispWindowZoom:
		{
			l_lMatroxType = M_WINDOW_ZOOM;
			break;
		}
		case SVDispSelected:
		{
			l_lMatroxType = M_SELECTED;
			break;
		}
		case SVDispZoomFactorX:
		{
			l_lMatroxType = M_ZOOM_FACTOR_X;
			break;
		}
		case SVDispZoomFactorY:
		{
			l_lMatroxType = M_ZOOM_FACTOR_Y;
			break;
		}
		case SVDispPanX:
		{
			l_lMatroxType = M_PAN_OFFSET_X;
			break;
		}
		case SVDispPanY:
		{
			l_lMatroxType = M_PAN_OFFSET_Y;
			break;
		}
		case SVDispWindowOvrWrite:
		{
			l_lMatroxType = M_OVERLAY;
			break;
		}
		case SVDispWindowOvrBufID:
		{
			l_lMatroxType = M_OVERLAY_ID;
			break;
		}
		case SVDispKeyColor:
		{
			l_lMatroxType = M_TRANSPARENT_COLOR;
			break;
		}
	}
	return l_lMatroxType;
}


/**
@SVOperationName Get - double&

@SVOperationDescription This function inquires about a specified display settings and puts the results in a double.

*/
HRESULT SVMatroxDisplayInterface::Get(const __int64& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, double& p_rdValue)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType(p_eDispFlag);
		if (l_lMatroxType != M_UNINITIALIZED)
		{
			if (M_NULL != p_rDispId)
			{
				double l_dValue;
				MdispInquire(p_rDispId, l_lMatroxType, &l_dValue);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if (l_Code == S_OK)
				{
					p_rdValue = l_dValue;
				}
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Get - long

@SVOperationDescription This function inquires about a specified display settings and puts the results in a long.

*/
HRESULT SVMatroxDisplayInterface::Get(const __int64& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, long& p_rlData)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType(p_eDispFlag);
		if (l_lMatroxType != M_UNINITIALIZED)
		{
			if (M_NULL != p_rDispId)
			{
				MIL_INT l_lValue;
				if ((p_eDispFlag & SVDispDouble) == SVDispDouble)
				{	// if this parameter is expecting a double then we better use a double.
					double l_dTmp = 0;
					MdispInquire(p_rDispId, l_lMatroxType, &l_dTmp);
					l_lValue = static_cast<long>(l_dTmp);
				}
				else
				{
					MdispInquire(p_rDispId, l_lMatroxType, &l_lValue);
				}
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if (l_Code == S_OK)
				{
					p_rlData = static_cast<long>(l_lValue);
				}
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}


/**
@SVOperationName Destroy

@SVOperationDescription This function frees the specified SVMatroxDisplay.

*/
HRESULT SVMatroxDisplayInterface::Set(const __int64& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, const double& p_dValue)
{
	return Set(p_rDispId, p_eDispFlag, static_cast<long>(p_dValue));
}

/**
@SVOperationName Set

@SVOperationDescription This function allows you to control the specified MIL display settings.

*/
HRESULT SVMatroxDisplayInterface::Set(const __int64& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, const long& p_lValue)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType(p_eDispFlag);
		if (l_lMatroxType != M_UNINITIALIZED)
		{
			if (M_NULL != p_rDispId)
			{
				MdispControl(p_rDispId, l_lMatroxType, p_lValue);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_INVALID_HANDLE;
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch (...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName GetBitmapInfo

@SVOperationDescription Gets a pointer to the bitmapinfo of the display.

*/
HRESULT SVMatroxDisplayInterface::GetBitmapInfo(LPBITMAPINFO& p_rpBitmapInfo,
	const SVMatroxBuffer& p_rBuffer)
{
	HRESULT l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rBuffer.empty())
		{
			MbufInquire(p_rBuffer.GetIdentifier(),
				M_BITMAPINFO,
				&p_rpBitmapInfo);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

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
	assert(l_Code == S_OK);
	return l_Code;
}



/**
@SVOperationName GetHostAddress

@SVOperationDescription Gets the host address for the specified display.

*/
HRESULT SVMatroxDisplayInterface::GetHostAddress(LPVOID p_rpHostAddress, const SVMatroxBuffer& p_rBuffer)
{
	HRESULT l_Code;

#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (!p_rBuffer.empty())
		{
			MbufInquire(p_rBuffer.GetIdentifier(), M_HOST_ADDRESS, p_rpHostAddress);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
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
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Display Lut

@SVOperationDescription This function associates a LUT buffer with the specified display.

*/
HRESULT SVMatroxDisplayInterface::Lut(const __int64& p_rDispId, const SVMatroxBuffer& p_rLutBufId)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (M_NULL != p_rDispId)
		{
			MdispLut(p_rDispId, p_rLutBufId.GetIdentifier());
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
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
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Display Pan

@SVOperationDescription This function associates pan and scroll values with the specified display.

*/
HRESULT SVMatroxDisplayInterface::Pan(const __int64& p_rDispId, const double& p_dXOffset, const double& p_dYOffset)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (M_NULL != p_rDispId)
		{
			MdispPan(p_rDispId, p_dXOffset, p_dYOffset);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
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
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Display Select

@SVOperationDescription This function outputs the content of the specified image buffer to the specified MIL display.

*/
HRESULT SVMatroxDisplayInterface::Select(const __int64& p_rDispId, const SVMatroxBuffer& p_rImageId)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (M_NULL != p_rDispId)
		{
			MdispSelect(p_rDispId, p_rImageId.GetIdentifier());
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
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
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Display Select Window

@SVOperationDescription This function displays the specified image buffer contents in the specified user window, using the specified MIL display.

*/
HRESULT SVMatroxDisplayInterface::SelectWindow(const __int64& p_rDispId, const SVMatroxBuffer& p_rImageId, const HWND& p_hClientWindowHandle)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (M_NULL != p_rDispId)
		{
			MdispSelectWindow(p_rDispId, p_rImageId.GetIdentifier(), p_hClientWindowHandle);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
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
	assert(l_Code == S_OK);
	return l_Code;
}

/**
@SVOperationName Display Deselect Window

@SVOperationDescription This function deselects any image that was selected to the specified MIL display.

*/
HRESULT SVMatroxDisplayInterface::Deselect(const __int64& p_rDispId)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (M_NULL != p_rDispId)
		{
			MdispSelect(p_rDispId, M_NULL);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
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
	assert(l_Code == S_OK);
	return l_Code;
}


/**
@SVOperationName Display Zoom

@SVOperationDescription This function associates a zoom factor with the specified display.

*/
HRESULT SVMatroxDisplayInterface::Zoom(const __int64& p_rDispId, const double& p_dXFactor, const double& p_dYFactor)
{
	HRESULT l_Code(S_OK);
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if (M_NULL != p_rDispId)
		{
			MdispZoom(p_rDispId, p_dXFactor, p_dYFactor);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
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
	assert(l_Code == S_OK);
	return l_Code;
}



