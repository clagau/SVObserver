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

#ifndef SVDIRECTX_H
#define SVDIRECTX_H

#include <ddraw.h>
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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDirectX.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:10:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Sep 2011 15:53:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated display fucntionality based on new resource management methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Sep 2009 13:14:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  662
 * SCR Title:  Add Display Change Support to SVImageView
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added CreatePrimarySurface method.
 * Added CreateSurface method.
 * Added GetPrimarySurfaceResolution method.
 * Added GetPrimarySurfacePixelDepth method.
 * Added TestCooperativeLevel method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jan 2009 13:49:26   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Replaced InitializeObject and InitializeSurface with Initialize
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Dec 2008 13:02:42   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial version of the file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
