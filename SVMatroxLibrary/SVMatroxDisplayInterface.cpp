// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDisplayInterface
// * .File Name       : $Workfile:   SVMatroxDisplayInterface.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:12:48  $
// ******************************************************************************

#include "stdafx.h"
#include <assert.h>
#include "SVMatroxDisplayInterface.h"
#include "SVMatroxBufferTemplate.h"
#include "SVMatroxImageBuffer.h"
#include "SVMatroxImagingLibrary.h"  // has MIL includes
#include "SVMatroxResourceMonitor.h"

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
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Create( SVMatroxDisplay& p_rDispId)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );

#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == SVMEE_STATUS_OK )
		{
			MIL_ID l_NewId = MdispAlloc(M_DEFAULT_HOST,
				M_DEFAULT,		// M_AUTOMATIC new doc does not mention anything about this.
				M_DISPLAY_SETUP,// M_DISPLAY_SETUP is a string "M_DEFAULT"
				M_WINDOWED,		// M_WINDOWED | M_GDI_OVERLAY
				M_NULL);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			if( l_Code == SVMEE_STATUS_OK )
			{
				SVMatroxResourceMonitor::InsertIdentifier( SVDisplayID, l_NewId );

				if( p_rDispId.empty() )
				{
					p_rDispId.m_DisplayIdentifier = l_NewId;
				}
				else
				{
					l_Code = Destroy( p_rDispId );
					p_rDispId.m_DisplayIdentifier = l_NewId;
				}
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Allocate Display 

@SVOperationDescription This function Creates a SVMatroxDisplayBuffer.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Create( SVMatroxDisplayBuffer& p_rDispBufId, SVMatroxBufferCreateStruct p_CreateStruct )
{
	SVStatusCode l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		__int64 l_lAttrib =   M_IMAGE | M_PROC | M_DISP | M_DIB;

		if( p_CreateStruct.m_eType > 0 )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MIL_ID l_NewBuf = M_NULL;
				if( p_CreateStruct.m_lSizeBand == 3 )
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

				if( l_Code == SVMEE_STATUS_OK )
				{
					SVMatroxResourceMonitor::InsertIdentifier( SVDisplayBufferID, l_NewBuf );

					p_rDispBufId.m_DisplayBufferPtr = new SVMatroxImageBuffer( l_NewBuf, "SVMatroxDisplayInterface::Create-BufferCreate" );
				}
			}
		}
		else
		{
			l_Code = SVMEE_INVALID_PARAMETER;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Create - SVMatroxBuffer 

@SVOperationDescription This function Creates a SVMatroxDisplayBuffer from an existing SVMatroxBuffer.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Create( SVMatroxDisplayBuffer& p_rNewBuffer, const SVMatroxBuffer& p_CreateFrom )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
	SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

	l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

	if( l_Code == SVMEE_STATUS_OK )
	{
		SVMatroxBufferInterface l_BufIntf;
		SVMatroxBuffer l_Buf;
		l_Code = l_BufIntf.Create( l_Buf, p_CreateFrom );
		if( l_Code == SVMEE_STATUS_OK )
		{
			SVMatroxResourceMonitor::InsertIdentifier( SVDisplayBufferID, l_Buf.GetIdentifier() );

			p_rNewBuffer.m_DisplayBufferPtr = l_Buf.m_BufferPtr;
		}
	}
	return l_Code;
}


/**
@SVOperationName Destroy - SVMatroxDisplay

@SVOperationDescription This function destroys a SVMatroxDisplay.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Destroy( SVMatroxDisplay& p_rDispId)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rDispId.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MdispFree( p_rDispId.m_DisplayIdentifier );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
				{
					SVMatroxResourceMonitor::EraseIdentifier( SVDisplayID, p_rDispId.m_DisplayIdentifier );

					p_rDispId.m_DisplayIdentifier = M_NULL;
				}
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;

}

/**
@SVOperationName Destroy - SVMatroxDisplayBuffer

@SVOperationDescription This function destroys a SVMatrox Display Buffer.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Destroy( SVMatroxDisplayBuffer& p_rDispBufId)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rDispBufId.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				SVMatroxResourceMonitor::EraseIdentifier( SVDisplayBufferID, p_rDispBufId.GetIdentifier() );

				p_rDispBufId.clear();
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}



/**
@SVOperationName Convert2MatroxType

@SVOperationDescription This function converts the SVObserver type to a Matrox type.

*/
long SVMatroxDisplayInterface::Convert2MatroxType( SVDisplayCntrlEnum p_eDisp) 
{
	long l_lMatroxType = M_UNINITIALIZED;
	switch( p_eDisp )
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
		case SVDispWindowScrollbar:
		{
			l_lMatroxType = M_WINDOW_SCROLLBAR;
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
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Get( const SVMatroxDisplay& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, double& p_rdValue) 
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType( p_eDispFlag );
		if( l_lMatroxType != M_UNINITIALIZED )
		{
			if( !p_rDispId.empty() )
			{
				double l_dValue;
				MdispInquire( p_rDispId.m_DisplayIdentifier, l_lMatroxType, &l_dValue );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Get - long

@SVOperationDescription This function inquires about a specified display settings and puts the results in a long.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Get( const SVMatroxDisplay& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, long& p_rlData) 
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType( p_eDispFlag );
		if( l_lMatroxType != M_UNINITIALIZED )
		{
			if( !p_rDispId.empty() )
			{
				long l_lValue;
				if(( p_eDispFlag & SVDispDouble) == SVDispDouble)
				{	// if this parameter is expecting a double then we better use a double.
					double l_dTmp = 0;
					MdispInquire( p_rDispId.m_DisplayIdentifier, l_lMatroxType, &l_dTmp );
					l_lValue = static_cast<long>(l_dTmp);
				}
				else
				{
					MdispInquire( p_rDispId.m_DisplayIdentifier, l_lMatroxType, &l_lValue );
				}
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
				{
					p_rlData = l_lValue;
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


/**
@SVOperationName Get - SVMatroxBuffer

@SVOperationDescription This function gets the SVMatroxBuffer associated with the specified SVMatroxDisplay 

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Get( const SVMatroxDisplay& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, SVMatroxBuffer& p_rBuffer) 
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( p_eDispFlag == SVDispWindowOvrBufID || p_eDispFlag == SVDispSelected )
		{
			if( !p_rDispId.empty() )
			{
				long l_lValue;
				MdispInquire( p_rDispId.m_DisplayIdentifier, M_SELECTED, &l_lValue );
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
				{
					p_rBuffer.m_BufferPtr = new SVMatroxBufferTemplate( l_lValue, "SVMatroxDisplayInterface::Get" );
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Destroy

@SVOperationDescription This function frees the specified SVMatroxDisplay.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Set( const SVMatroxDisplay& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, const double& p_dValue)
{
	return Set(p_rDispId, p_eDispFlag, static_cast<long>( p_dValue ));
}

/**
@SVOperationName Set

@SVOperationDescription This function allows you to control the specified MIL display settings.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Set( const SVMatroxDisplay& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, const long& p_lValue)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		long l_lMatroxType = Convert2MatroxType( p_eDispFlag );
		if( l_lMatroxType != M_UNINITIALIZED )
		{
			if( !p_rDispId.empty() )
			{
				MdispControl( p_rDispId.m_DisplayIdentifier, l_lMatroxType, p_lValue );
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName GetBitmapInfo

@SVOperationDescription Gets a pointer to the bitmapinfo of the display.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::GetBitmapInfo( LPBITMAPINFO& p_rpBitmapInfo, 
																			 const SVMatroxDisplayBuffer& p_rBuffer )
{
	SVStatusCode l_Code;
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rBuffer.empty() )
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
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}



/**
@SVOperationName GetHostAddress

@SVOperationDescription Gets the host address for the specified display.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::GetHostAddress( LPVOID p_rpHostAddress, 
																			  const SVMatroxDisplayBuffer& p_rBuffer )
{
	SVStatusCode l_Code;

#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rBuffer.empty() )
		{
			MIL_ID l_NewBuf = MbufInquire(p_rBuffer.GetIdentifier(),
				M_HOST_ADDRESS,
				p_rpHostAddress);

			l_Code = SVMatroxApplicationInterface::GetLastStatus();

		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Display Lut

@SVOperationDescription This function associates a LUT buffer with the specified display.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Lut( const SVMatroxDisplay& p_rDispId, const SVMatroxBuffer& p_rLutBufId)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rDispId.empty() )
		{
			MdispLut( p_rDispId.m_DisplayIdentifier, p_rLutBufId.GetIdentifier() );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Clear Lut Display

@SVOperationDescription This function dis-associates or clears any LUT that was associated with the specified display

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::LutClear( const SVMatroxDisplay& p_rDispId)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rDispId.empty() )
		{
			MdispLut( p_rDispId.m_DisplayIdentifier, M_DEFAULT );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Display Pan

@SVOperationDescription This function associates pan and scroll values with the specified display.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Pan( const SVMatroxDisplay& p_rDispId, const double& p_dXOffset, const double& p_dYOffset)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rDispId.empty() )
		{
			MdispPan( p_rDispId.m_DisplayIdentifier, p_dXOffset, p_dYOffset );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Display Select

@SVOperationDescription This function outputs the content of the specified image buffer to the specified MIL display. 

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Select( const SVMatroxDisplay& p_rDispId, const SVMatroxBuffer& p_rImageId)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rDispId.empty() )
		{
			MdispSelect( p_rDispId.m_DisplayIdentifier, p_rImageId.GetIdentifier() );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Display Select Window

@SVOperationDescription This function displays the specified image buffer contents in the specified user window, using the specified MIL display.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::SelectWindow( const SVMatroxDisplay& p_rDispId, const SVMatroxDisplayBuffer& p_rImageId, const HWND& p_hClientWindowHandle )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rDispId.empty() )
		{
			MdispSelectWindow( p_rDispId.m_DisplayIdentifier, p_rImageId.GetIdentifier(), p_hClientWindowHandle );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

/**
@SVOperationName Display Deselect Window

@SVOperationDescription This function deselects any image that was selected to the specified MIL display.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Deselect( const SVMatroxDisplay& p_rDispId)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rDispId.empty() )
		{
			MdispSelect( p_rDispId.m_DisplayIdentifier, M_NULL );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}


/**
@SVOperationName Display Zoom

@SVOperationDescription This function associates a zoom factor with the specified display.

*/
SVMatroxDisplayInterface::SVStatusCode SVMatroxDisplayInterface::Zoom( const SVMatroxDisplay& p_rDispId, const double& p_dXFactor, const double& p_dYFactor )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif

	{
		if( !p_rDispId.empty() )
		{
			MdispZoom( p_rDispId.m_DisplayIdentifier, p_dXFactor, p_dYFactor );
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	assert( l_Code == SVMEE_STATUS_OK );
	return l_Code;
}



// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxDisplayInterface.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:12:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Sep 2011 14:29:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Martox Library to include a new resource management system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Jan 2011 10:36:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Jun 2008 10:24:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SVDisplayCntrlEnum to include double flag and logic to read a double value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 May 2008 11:27:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug in DisplayInterface Get function 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:20:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
