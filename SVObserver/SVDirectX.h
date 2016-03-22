//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDirectX
//* .File Name       : $Workfile:   SVDirectX.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:10:24  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <ddraw.h>
#include "SVUtilityLibrary/SVBitmapInfo.h"

class SVDirectX
{
public:
	virtual ~SVDirectX();
	static SVDirectX& Instance();

	bool empty() const;
	virtual void clear();

	HRESULT Initialize();

	HRESULT RestoreAllSurfaces();

	HRESULT SetClipperHWnd( DWORD p_Flags, HWND p_Handle );

	HRESULT Blt( RECT* p_pDestRect, IDirectDrawSurface7* p_pDDSurface, RECT* p_pSourceRect, DWORD p_Flags, DDBLTFX* p_pDDBltFx );
	HRESULT GetBltStatus( DWORD p_Flags );
	HRESULT IsLost();

	static HRESULT CreatePrimarySurface();
	static HRESULT CreateSurface(long width, long height, IDirectDrawSurface7** ppDDSurface);
	static HRESULT CreateSurface(const SVBitmapInfo& p_rBitmapInfo, IDirectDrawSurface7** ppDDSurface);

	static HRESULT GetPrimarySurfaceResolution(SIZE& rSize);
	static HRESULT GetPrimarySurfacePixelDepth(int& rPixelDepth);
	static HRESULT TestCooperativeLevel();

protected:
	SVDirectX();

	IDirectDraw7* m_pDDObject;  // moved here from SVObserver.h
	IDirectDrawClipper* m_pDDClipper;  // moved here from SVObserver.h
	IDirectDrawSurface7* m_pDDDisplaySurface;  // moved here from SVObserver.h

private:
	/*
	Do not implement the following to methods.
	These definitions prevent copy and assignement operations.
	*/
	SVDirectX( const SVDirectX& p_rObject );
	const SVDirectX& operator=( const SVDirectX& p_rObject );

	void LocalClear();
};
