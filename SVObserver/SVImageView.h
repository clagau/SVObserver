//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageView
//* .File Name       : $Workfile:   SVImageView.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   24 Sep 2013 16:18:34  $
//******************************************************************************

#ifndef SVIMAGEVIEW_H
#define SVIMAGEVIEW_H

#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVRunControlLibrary/SVImageIndexStruct.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVSystemLibrary/SVThreadWait.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVUtilityLibrary/SVWinHandle.h"
#include "SVInfoStructs.h"

struct IDirectDrawSurface7;

class SVToolClass;
class SVImageViewScroll;
class SVIPDoc;
class SVTaskObjectClass;
struct SVCameraInfoStruct;
class SVObjectWriter;

class SVImageViewClass : public CView
{
	DECLARE_DYNCREATE( SVImageViewClass )

public:
	SVImageViewClass();
	virtual ~SVImageViewClass();

	virtual HRESULT UpdateToolExtents( SVExtentLocationPropertyEnum p_svLocation, double p_x, double p_y );
	virtual HRESULT UpdateToolExtents( SVExtentLocationPropertyEnum p_svLocation, double p_angle );
	virtual HRESULT UpdateToolExtents( const SVImageExtentClass& p_rExtents );
	virtual HRESULT UpdateToolExtentsToFit( const SVImageExtentClass& p_rExtents );
	virtual HRESULT GetToolExtents(SVImageExtentClass& p_svToolExtents);

	SVIPDoc* GetIPDoc() const;

	void AttachToImage( const SVGUID& p_rImageId );
	void AttachToImage( LPCTSTR p_imageName );
	void DetachFromImage();

	const SVGUID& GetImageID() const;

	SVImageClass* GetImage();
	SVTaskObjectClass* GetSelectedObject();

	void GetImageRect( CRect &p_rect );

	BOOL GetParameters(SVObjectWriter& rWriter);

	typedef SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters( SVTreeType& p_tree, SVTreeType::SVBranchHandle p_parent );

	BOOL CheckParameters( SVTreeType& p_tree, SVTreeType::SVBranchHandle p_parent );

	HRESULT RecreateImageSurface();

	void ShowExtremeLUT( bool p_show = true );
	
	void TransformToViewSpace( CPoint& p_point );
	void TransformFromViewSpace( CPoint& p_point );

	BOOL GetObjectAtPoint( POINT p_point );

	//{{AFX_MSG(SVImageViewClass)
	afx_msg void OnLButtonDown( UINT p_nFlags, CPoint p_point );
	afx_msg void OnMouseMove( UINT p_nFlags, CPoint p_point );
	afx_msg void OnLButtonUp( UINT p_nFlags, CPoint p_point );
	afx_msg void OnLButtonDblClk( UINT p_nFlags, CPoint p_point );
	afx_msg void OnRButtonDblClk( UINT p_nFlags, CPoint p_point );
	afx_msg void OnCaptureChanged( CWnd* p_pWnd );
	afx_msg void OnNcMouseMove( UINT p_hitTest, CPoint p_point );
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu( CWnd* p_pWnd, CPoint p_point );
	afx_msg BOOL OnEraseBkgnd( CDC* p_pDC );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL( SVImageViewClass )
	public:
	virtual BOOL Create( LPCTSTR p_className, LPCTSTR p_windowName, DWORD p_style, const RECT& p_rect, CWnd* p_pParentWnd, UINT p_NID, CCreateContext* p_pContext = NULL );
	virtual void OnInitialUpdate();
	virtual BOOL OnCommand( WPARAM p_wParam, LPARAM p_lParam );
	protected:
	virtual void OnDraw( CDC* p_pDC );
	virtual void OnUpdate( CView* p_pSender, LPARAM p_lHint, CObject* p_pHint );
	//}}AFX_VIRTUAL

protected:
	void Initialize();

	bool GetScrollPosition( CPoint& p_point );
	bool SetScrollPosition( CPoint& p_point );

	bool SetScaleIndex( unsigned long p_scaleIndex );
	bool SetImageRect( CRect& p_rect );

	SVImageClass* GetImageByName( CString& p_imageName ) const;

	HRESULT ShouldDraw( const SVExtentMultiLineStruct& p_rMultiLine );
	void DrawOverlay( SVDrawContext* PDrawContext, const SVExtentMultiLineStruct& p_rMultiLine );
	void UpdateOverlays( HDC p_hDC, long p_X, long p_Y );

	void SelectDisplayImage();

	void ReleaseImageSurface();

	HICON GetObjectCursor( POINT p_point );
	HICON GetObjectCursor( SVExtentLocationPropertyEnum p_svLocation, POINT p_point);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext& p_dc ) const;
#endif

	SVBitmapInfo GetBitmapInfo() const;
	const unsigned char* GetBitmapBits() const;

	HRESULT UpdateImageSurfaces( const SVBitmapInfo& p_rBitmapInfo );
	HRESULT CopyBitsToSurface( const CRect& p_rSourceRect, const SVBitmapInfo& p_rBitmapInfo, const unsigned char* p_pBitmapBits );
/* Obsolete - Replaced due to performance reasons
	HRESULT UpdateScaledSurfaceWithExtremeLUT();
*/
	HRESULT BlitToScaledSurface( CRect& p_rSourceRect, CRect& p_rDestRect );
	HRESULT BlitToPrimarySurface( CRect& p_rDestRect );
	HRESULT RecreateLostSurface();

	HRESULT GetRectInfo( CRect& p_rSourceRect, CRect& p_rDestRect );

	HRESULT UpdateBufferFromIPDoc();
	HRESULT UpdateSurface();
	HRESULT DisplaySurface();
	HRESULT NotifyIPDocDisplayComplete();

	SVByteVector m_ImageDIB;
	SVExtentMultiLineStructCArray m_OverlayData;

	unsigned long m_scaleIndex;
	unsigned long m_scaleCount;
	double m_scaleFactor[ 31 ];
	HGDIOBJ m_hFont[ 31 ];

	SVGUID m_ImageId;
	CString m_imageName;

	IDirectDrawSurface7* m_pDDImageSurface;
	IDirectDrawSurface7* m_pDDScaledImageSurface;

	bool m_showExtremeLUT;

	// JMS - Extent Objects

	SVTaskObjectClass* m_psvObject;

	SVExtentLocationPropertyEnum m_svLocation;

	SVExtentLocationPropertyEnum m_svMousePickLocation;

	// JMS - Extent Objects

	BOOL m_isPicked;

	BOOL m_mouseIsOverTool;
	CPoint m_mousePoint;
	CPoint m_lastMouseMovePoint;

	HBRUSH m_hWindowBackgroundColor;
	SVWinHandle<HICON> m_hActionIcon;

	int m_sourceImageWidth;
	int m_sourceImageHeight;
	int m_sourceBitCount;

	CRect m_LastRect;

	SVThreadWait m_ThreadWait;

};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	//	SVIMAGEVIEW_H

//** EOF **

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVImageView.h_v  $
 * 
 *    Rev 1.1   24 Sep 2013 16:18:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  860
 * SCR Title:  Resize Icons Should Change Based on a Tool's Rotation
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed GetObjectCursor methods to return HICON.
 *   Added m_hActionIcon.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:56:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.55   04 Oct 2012 11:25:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced the CTreeCtrl with MaterialsTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.54   28 Sep 2012 14:49:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated saving operation to use the object writer instead of the XML tree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.53   25 Sep 2012 15:19:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new tree type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.52   09 Aug 2012 16:05:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed performance issues when update image view when showing extreme LUT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.51   07 Aug 2012 16:59:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed problem with overlay offset based on scroll position.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.50   07 Aug 2012 16:11:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated views to reduce flicker.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.49   02 Jul 2012 16:58:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.48   04 Oct 2011 18:53:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated handshaking between IPDoc and ImageView to fix freezing of the ImageView.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.47   03 Oct 2011 10:04:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated the handshaking between the display object thread and the image view draw functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.46   16 Sep 2011 16:00:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects to use new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.45   10 Feb 2011 14:55:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44   05 Nov 2010 14:54:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove the un-necessary IP View Control from the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.43   01 Jun 2010 10:43:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   12 Feb 2010 12:55:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  687
 * SCR Title:  Large Images not displayed correctly in SVObserver
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Additional DirectDraw Surface for Stretch Blitting.
 * Added ReleaseSurface method.
 * Revised Blitting to stretch blit to offscreen surface rather than Primarry surface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.41   16 Dec 2009 11:00:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   21 Sep 2009 13:19:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  662
 * SCR Title:  Add Display Change Support to SVImageView
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added RecreateSurface method.
 * Removed OnDisplayChange handler.
 * Removed member variable m_surfaceLost.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   03 Sep 2009 10:07:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   10 Feb 2009 10:41:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised UpdateImage method to return status code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   15 Dec 2008 11:50:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed DirectDraw include (now in SVDirectX.h)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   16 Oct 2008 15:36:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed OnTimer (empty function)
 * Removed SetPickType (unimplemented function)
 * General formatting clean up
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   12 Sep 2008 11:55:12   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated to use DirectDraw to display images.
 * Added CreateImageSurface function
 * Added OnDisplayChange function
 * Moved m_pDDObject, m_pDDClipper and m_pDDDisplaySurface to SVObserver.h
 * Removed unnecessary includes
 * Removed MIL display parameters:  m_svDisplayBuffer and m_svResourceBuffer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   30 May 2008 13:27:54   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed all DirectX changes since 5.00 Beta 1.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   29 May 2008 16:13:08   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added member variable m_svImageInfo
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   20 May 2008 08:23:52   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added CreateImageSurface function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   15 May 2008 15:07:30   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Restored Matrox display member variables from 5.00 beta 2
 * Removed unnecessary includes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   14 Apr 2008 10:11:38   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed MIL display parameters:  m_svDisplayBuffer and m_svResourceBuffer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   10 Apr 2008 07:49:56   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   * Updated to use DirectDraw to display images.
 * * Created OnDisplayChange function to handle changes to display settings.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   10 Apr 2008 07:44:08   BWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  617
 * SCR Title:  Use DirectX instead of Matrox to display images
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   * Updated to use DirectDraw to display images.
 * * Created OnDisplayChange function to handle changes to display settings.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   30 Oct 2007 15:25:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic to manage GDI resource.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   24 Jul 2007 13:31:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   22 Jun 2007 10:40:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   21 Jun 2007 13:05:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   02 Nov 2005 09:07:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented published result images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   31 Aug 2005 08:38:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update source code call to handle new refresh methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   24 Aug 2005 09:39:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update logic to make all Create, Update and Destroy calls to the display buffers use the Message Pump.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   22 Aug 2005 10:14:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   10 Aug 2005 12:31:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   18 Jul 2005 12:19:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  497
 * SCR Title:  Remove Freeze Set from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed bFreeze variable from SVImageView class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   27 Jun 2005 09:37:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  497
 * SCR Title:  Remove Freeze Set from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed the freeze image, freeze methods and freeze variables from image view.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   17 Feb 2005 14:28:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   12 Jul 2004 10:26:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed from BOOL to HRESULT in BuildImageWindow, BuildImageWindowFromImageName, and CloseImageWindow
 * made SetDisplayImage protected
 * added #include "SVErrorException.h"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   24 Dec 2003 14:33:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  387
 * SCR Title:  Change display logic to delay updating while the inspection has a backlog
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed the display to not update tool figures while the display is off and to use the PaintDC instead of the window's DC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   07 May 2003 13:16:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  361
 * SCR Title:  SVObserver's display may become incorrect when running at very high speeds
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified display logic so that image view displaying a SourceImage will look into the ProductInfoStruct to find the source image index for the camera from which they are derived instead of assuming it is the first camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   22 Apr 2003 10:24:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   06 Mar 2003 13:17:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  328
 * SCR Title:  Upgrade SVObserver to MIL 7.1
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated to work with MIL 7.1
 * MvgaDispXXX no longer works. Create a copy of the DIB instead.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   05 Feb 2003 17:29:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Made modifications to the display logic to display the current source and result images indexes, respectively. This should make it work correctly regardless of whether it is online or performing RunOnce/Regression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   30 Jan 2003 10:47:44   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated FreezePanel, SetupWindow, DrawImageOnOverlay, ShowExtremeLUT, GetDisplayImageHandle, Refresh, SelectDisplayImage, GetViewSize, CloseImageWindow, BuildImageWindowFromImageName, BuildImageWindow  and constructor methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   10 Jan 2003 14:03:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to how the images are displayed while running and while doing a RunOnce operation to use the image that is specified in the SVProductInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   19 Nov 2002 13:27:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 *        Added GetParameters and SetParameters functions
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   04 Apr 2002 10:02:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code syntax to allow rational rose to reverse engineer properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   26 Apr 2000 19:08:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added viewPickPoint to keep mouse pick point in View Space.
 * Added include for SVAdjustImageLinePosition.h
 * Revised OnCommand to set viewPickPoint to zeroes.
 * Revised OnCommand to display new adjustment dialog for the Angular Profile tool.
 * Revised OnMouseMove to calculate deltas in view space as well as transformed space.
 * Revised OnMouseMove to send both pickpoints for the 
 * following messages:
 * SVM_MOVE,SVM_R_SIZE,SVM_B_SIZE,SVM_L_SIZE,
 * SVM_T_SIZE,SVM_RB_SIZE,SVM_LB_SIZE,
 * SVM_LT_SIZE,SVM_RT_SIZE
 * REvised getObjectAtPoint to set viewPickPoint.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   07 Mar 2000 11:33:20   Jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  114
 * SCR Title:  Integrate Single Pixel Move Context Menu.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Added
 * 
 *   SVImageViewClass::UnNormalize () to convert from "real" coordinates to view coordinates.
 * 
 *   SVImageViewClass::SetPickType () to give access to protected member.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   21 Feb 2000 15:10:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added getObjectCursor and getObjectAtPoint methods.
 * Moved code from onMouseMove to getObjectCursor.
 * Moved code from onLButtonDown to getObjectAtPoint.
 * Revised getObjectCursor and getObjectAtPoint to use relative extents.
 * Revised OnLButtonDblClk to check if view initialized.
 * Revised OnRButtonDblClk to check if view initialized.
 * Revised OnLButtonDown to check if view initialized.
 * Revised OnMouseMove to check if view initialized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   Feb 21 2000 10:32:02   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  109
 * SCR Title:  Freeze on Reject for v3.1
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced new Freeze on reject capbility.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
