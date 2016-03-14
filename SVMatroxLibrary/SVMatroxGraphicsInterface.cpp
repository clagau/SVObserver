//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGraphicsInterface
//* .File Name       : $Workfile:   SVMatroxGraphicsInterface.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:17:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxGraphicsInterface.h"
#include "SVMatroxEnums.h"
#include "SVMatroxImagingLibrary.h"
#include "SVMatroxResourceMonitor.h"

SVMatroxGraphicsInterface::SVMatroxGraphicsInterface()
{
}

SVMatroxGraphicsInterface::~SVMatroxGraphicsInterface()
{
}

double SVMatroxGraphicsInterface::CreateRGB888( unsigned char p_Red, unsigned char p_Green, unsigned char p_Blue )
{
	return M_RGB888( p_Red, p_Green, p_Blue );
}

SVMatroxGraphicsInterface::SVStatusCode SVMatroxGraphicsInterface::Clear( SVMatroxGraphics& p_rGraphicsID, SVMatroxBuffer& p_rBuffer )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rGraphicsID.empty() && !p_rBuffer.empty() )
		{
			MgraClear( p_rGraphicsID.m_GraphicsId, p_rBuffer.GetIdentifier() );

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
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

SVMatroxGraphicsInterface::SVStatusCode SVMatroxGraphicsInterface::RectangleFill( SVMatroxGraphics& p_rGraphicsID, SVMatroxBuffer& p_rBuffer, const RECT& p_rRectangle )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rGraphicsID.empty() && !p_rBuffer.empty() )
		{
			MgraRectFill( p_rGraphicsID.m_GraphicsId, p_rBuffer.GetIdentifier(),
				p_rRectangle.left, p_rRectangle.top, 
				p_rRectangle.right, p_rRectangle.bottom );

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
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

SVMatroxGraphicsInterface::SVStatusCode SVMatroxGraphicsInterface::Color( SVMatroxGraphics& p_rGraphicsID, double p_Color )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rGraphicsID.empty() )
		{
			MgraColor( p_rGraphicsID.m_GraphicsId, p_Color );

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
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

SVMatroxGraphicsInterface::SVStatusCode SVMatroxGraphicsInterface::BackColor( SVMatroxGraphics& p_rGraphicsID, double p_Color )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rGraphicsID.empty() )
		{
			MgraBackColor( p_rGraphicsID.m_GraphicsId, p_Color );

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
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

SVMatroxGraphicsInterface::SVStatusCode SVMatroxGraphicsInterface::Create( SVMatroxGraphics& p_rGraphicsID )
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
			MIL_ID l_NewID = M_NULL;

			l_NewID = MgraAlloc( M_DEFAULT_HOST, M_NULL );

			// Check For errors
			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if( l_Code == SVMEE_STATUS_OK )
			{// Free handle if already filled.
				SVMatroxResourceMonitor::InsertIdentifier( SVGraphicsID, l_NewID );

				if( !p_rGraphicsID.empty() )	
				{
					Destroy( p_rGraphicsID );
				}
				// Set new handle
				p_rGraphicsID.m_GraphicsId = l_NewID;
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
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

SVMatroxGraphicsInterface::SVStatusCode SVMatroxGraphicsInterface::Destroy( SVMatroxGraphics& p_rGraphicsID )
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( !p_rGraphicsID.empty() )
		{
			SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

			l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

			if( l_Code == SVMEE_STATUS_OK )
			{
				MgraFree( p_rGraphicsID.m_GraphicsId );

				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				if( l_Code == SVMEE_STATUS_OK )
				{
					SVMatroxResourceMonitor::EraseIdentifier( SVGraphicsID, p_rGraphicsID.m_GraphicsId );

					p_rGraphicsID.m_GraphicsId = M_NULL;
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
	assert(l_Code == SVMEE_STATUS_OK );
	return l_Code;
}

