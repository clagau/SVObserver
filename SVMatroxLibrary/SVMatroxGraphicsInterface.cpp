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

#pragma region Includes
#include "stdafx.h"
#include "SVMatroxGraphicsInterface.h"
#include "SVMatroxApplicationInterface.h"
#include "SVMatroxErrorEnum.h"
#include "SVMatroxHelper.h"
#include "SVMatroxResourceMonitor.h"
#pragma endregion Includes

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

HRESULT SVMatroxGraphicsInterface::Clear(__int64& p_rGraphicsID, SVMatroxBuffer& p_rBuffer )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( M_NULL !=p_rGraphicsID && !p_rBuffer.empty() )
		{
			MgraClear( p_rGraphicsID, p_rBuffer.GetIdentifier() );

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
	assert(l_Code == S_OK );
	return l_Code;
}

HRESULT SVMatroxGraphicsInterface::RectangleFill(__int64& p_rGraphicsID, SVMatroxBuffer& p_rBuffer, const RECT& p_rRectangle )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( M_NULL != p_rGraphicsID && !p_rBuffer.empty() )
		{
			MgraRectFill( p_rGraphicsID, p_rBuffer.GetIdentifier(),
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
	assert(l_Code == S_OK );
	return l_Code;
}

HRESULT SVMatroxGraphicsInterface::Color(__int64& p_rGraphicsID, double p_Color )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( M_NULL != p_rGraphicsID )
		{
			MgraColor( p_rGraphicsID, p_Color );

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
	assert(l_Code == S_OK );
	return l_Code;
}

HRESULT SVMatroxGraphicsInterface::BackColor(__int64& p_rGraphicsID, double p_Color )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( M_NULL != p_rGraphicsID )
		{
			MgraBackColor( p_rGraphicsID, p_Color );

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
	assert(l_Code == S_OK );
	return l_Code;
}

HRESULT SVMatroxGraphicsInterface::Create(__int64& p_rGraphicsID )
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVMatroxResourceMonitor::SVAutoLock l_AutoLock;

		l_Code = SVMatroxResourceMonitor::GetAutoLock( l_AutoLock );

		if( l_Code == S_OK )
		{
			MIL_ID l_NewID = M_NULL;

			l_NewID = MgraAlloc( M_DEFAULT_HOST, M_NULL );

			// Check For errors
			l_Code = SVMatroxApplicationInterface::GetLastStatus();

			if( l_Code == S_OK )
			{// Free handle if already filled.
				SVMatroxResourceMonitor::InsertIdentifier( SVGraphicsID, l_NewID );

				Destroy( p_rGraphicsID );
				// Set new handle
				p_rGraphicsID = l_NewID;
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
	assert(l_Code == S_OK );
	return l_Code;
}

HRESULT SVMatroxGraphicsInterface::Destroy(__int64& rGraphicsID )
{
	return DestroyMatroxId(rGraphicsID, MgraFree, SVGraphicsID);
}

