//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CDSVPictureDisplay
//* .File Name       : $Workfile:   CDSVPictureDisplay.h  $
//* .Description     : Declaration of ActiveX Control wrapper class(es) created by Microsoft Visual C++
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Jul 2014 03:58:52  $
//******************************************************************************

#pragma once

// Mit Microsoft Visual C++ generierte IDispatch-Wrapperklasse(n)

class CDSVPictureDisplay : public CWnd
{
protected:
	DECLARE_DYNCREATE(CDSVPictureDisplay)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x8174991A, 0xA5F5, 0x4CB3, { 0x85, 0xE3, 0x3B, 0x31, 0xFA, 0xEF, 0x23, 0x14 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR , LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext*) override
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = nullptr, BOOL bStorage = FALSE,
				BSTR bstrLicKey = nullptr)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// Attribute
public:
enum
{
    TopButtons = 0,
    RightButtons = 1,
    LeftButtons = 2,
    BottomButtons = 3,
    HideButtons = 4
}_AppearanceTypes;
enum
{
    TopLeft = 0,
    BottomRight = 1
}_JustifyTypes;
enum
{
    RectangleROI = 0,
    LineROI = 1,
    TextROI = 2,
    ArrowROI = 3,
    EllipseROI = 4,
    DoughnutROI = 5,
    TrapezoidROI = 6,
    GraphROI = 7,
	MarkerROI = 8,
    InvalidROIType = 9,
	TriangleROI = 10,
	PolyPointsROI = 11,
	SegmentROI = 12,
}_ROIType;
enum AllowType
{
    AllowNone = 0,
    AllowMove = 1,
    AllowResize = 2,
    AllowResizeAndMove = 3,
    AllowDelete = 4,
    AllowAll = 7
}_AllowType;
enum
{
    VerticalAxisTop = 0,
    VerticalAxisBottom = 1,
    HorizontalAxisLeft = 2,
    HorizontalAxisRight = 3,
    InvalidROISubtype = 4
}ROISubType_Trapezoid;
enum 
{	
	ImageArea_NoScale			= 0,
	ViewArea_Fit				= 1,
	ViewArea_ScalePerParameter	= 2,
	ImageArea_ScalePerParameter	= 3,
} 	ROISubType_Graph;
enum 
{
	SubType_Points = 0,
	SubType_Polyline = 1,
	SubType_Polygon = 2
} 	ROISubType_PolyPoints;
enum
{
	VerticalMarker		= 0,
	HorizontalMarker	= 1,
} ROISubType_Marker;
enum
{
    None = 0,
    LeftToRight = 1,
    RightToLeft = 2,
    TopToBottom = 3,
    BottomToTop = 4
}_SVOrientation;
enum
{
    P_Type = 2,
    P_X1 = 3,
    P_Y1 = 4,
    P_X2 = 5,
    P_Y2 = 6,
    P_Text = 7,
    P_Orientation = 8,
    P_ARRAY_XY = 9,
    P_AllowEdit = 10,
	P_X_Min=11,
	P_X_Max=12,
	P_Y_Min=13,
	P_Y_Max=14,
	P_Is_Flip_Vertical=15,
	P_Is_Flip_Horizontal=16,
	P_Min=17,
	P_Max=18,
	P_VALUE=19,
    P_Offset = 20,
    P_SideThickness = 21,
    P_TopThickness = 22,
    P_SubType = 23,
    P_Color = 24,
    P_SelectedColor = 25,
	P_SubType_X	= 26,
	P_SubType_Y	= 27,
	P_X3 = 28,
	P_Y3 = 29,
	P_StartAngle = 30,
	P_StopAngle = 31,
}_ParameterType;

public:
	void Refresh()
	{
		InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
	}
	
	SCODE AddTab(LPCTSTR text, long * pHandle)
	{
		SCODE result;
		static BYTE parms[] = VTS_BSTR VTS_PI4 ;
		InvokeHelper(0x5, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, text, pHandle);
		return result;
	}
	SCODE RemoveTab(long lTab)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, lTab);
		return result;
	}
	SCODE SelectTab(long lTab)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, lTab);
		return result;
	}
	SCODE ShowTab(long lTab, long bShow)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, lTab, bShow);
		return result;
	}
	SCODE RenameTab(long lTab, LPCTSTR Name)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_BSTR ;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, lTab, Name);
		return result;
	}
	SCODE SetZoom(long p_lTab, float p_fZoom)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_R4 ;
		InvokeHelper(0xa, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, p_lTab, p_fZoom);
		return result;
	}
	SCODE GetZoom(long p_lTab, float * pFloat)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_PR4 ;
		InvokeHelper(0xb, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, p_lTab, pFloat);
		return result;
	}
	SCODE SetPicture(long p_lTab, LPDISPATCH p_Picture, unsigned long Color)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_DISPATCH VTS_UI4 ;
		InvokeHelper(0xc, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, p_lTab, p_Picture, Color);
		return result;
	}
	SCODE SetPictureWithROI(long p_lTab, LPDISPATCH p_Picture, unsigned long Color, LPDISPATCH p_RoiList)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_DISPATCH VTS_UI4 VTS_DISPATCH ;
		InvokeHelper(0xd, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, p_lTab, p_Picture, Color, p_RoiList);
		return result;
	}
	SCODE AddOverlay(long lTab, VARIANT * ParameterList, VARIANT * ParameterValues, long * pHandle)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_PVARIANT VTS_PVARIANT VTS_PI4 ;
		InvokeHelper(0x27, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, lTab, ParameterList, ParameterValues, pHandle);
		return result;
	}
	SCODE AddOverlayToGroup(long lGroup, long lTab, VARIANT * pParameterList, VARIANT * pParameterValue, long * pHandle)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT VTS_PI4 ;
		InvokeHelper(0x25, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, lGroup, lTab, pParameterList, pParameterValue, pHandle);
		return result;
	}
	SCODE EditOverlay(long lTab, long lHandle, VARIANT * pParameterList, VARIANT * pParameterValue)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x26, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, lTab, lHandle, pParameterList, pParameterValue);
		return result;
	}
	SCODE RemoveOverlay(long lTab, long Handle)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x1b, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, lTab, Handle);
		return result;
	}
	SCODE RemoveAllOverlays(long lTab)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1c, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, lTab);
		return result;
	}
	SCODE SetEditAllow(long lTab, long lHandle, long l_AllowEdit)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1d, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, lTab, lHandle, l_AllowEdit);
		return result;
	}
	SCODE SelectObject(long p_lTab, long p_lHandle)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x1e, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, p_lTab, p_lHandle);
		return result;
	}
	SCODE EnableSaveButton(long p_lTab, long p_lEnable)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x1f, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, p_lTab, p_lEnable);
		return result;
	}
	SCODE GetViewRect(long lTab, long * X1, long * Y1, long * X2, long * Y2)
	{
		SCODE result;
		static BYTE parms[] = VTS_I4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x20, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, lTab, X1, Y1, X2, Y2);
		return result;
	}
	SCODE GetSelectedTab(long * pTab)
	{
		SCODE result;
		static BYTE parms[] = VTS_PI4 ;
		InvokeHelper(0x22, DISPATCH_METHOD, VT_ERROR, (void*)&result, parms, pTab);
		return result;
	}

// Properties
//

long GetButtonAppearance()
{
	long result;
	GetProperty(0x1, VT_I4, (void*)&result);
	return result;
}
void SetButtonAppearance(long propVal)
{
	SetProperty(0x1, VT_I4, propVal);
}
long GetButtonSpacing()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}
void SetButtonSpacing(long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}
long GetButtonJustify()
{
	long result;
	GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}
void SetButtonJustify(long propVal)
{
	SetProperty(0x3, VT_I4, propVal);
}
long GetScrollBarWidth()
{
	long result;
	GetProperty(0x4, VT_I4, (void*)&result);
	return result;
}
void SetScrollBarWidth(long propVal)
{
	SetProperty(0x4, VT_I4, propVal);
}
BOOL GetIsClearOverlayByNewImage()
{
	BOOL result;
	GetProperty(0x21, VT_BOOL, (void*)&result);
	return result;
}
void SetIsClearOverlayByNewImage(BOOL propVal)
{
	SetProperty(0x21, VT_BOOL, propVal);
}
BOOL GetSimultaneousZoom()
{
	BOOL result;
	GetProperty(0x23, VT_BOOL, (void*)&result);
	return result;
}
void SetSimultaneousZoom(BOOL propVal)
{
	SetProperty(0x23, VT_BOOL, propVal);
}
BOOL GetBoundaryCheck()
{
	BOOL result;
	GetProperty(0x28, VT_BOOL, (void*)&result);
	return result;
}
void SetBoundaryCheck(BOOL propVal)
{
	SetProperty(0x28, VT_BOOL, propVal);
}
BOOL GetZoomSlider()
{
	BOOL result;
	GetProperty(0x29, VT_BOOL, (void*)&result);
	return result;
}
void SetZoomSlider(BOOL propVal)
{
	SetProperty(0x29, VT_BOOL, propVal);
}
BOOL GetHideScrollBar()
{
	BOOL result;
	GetProperty(0x2a, VT_BOOL, (void*)&result);
	return result;
}
void SetHideScrollBar(BOOL propVal)
{
	SetProperty(0x2a, VT_BOOL, propVal);
}
BOOL GetHideTabControl()
{
	BOOL result;
	GetProperty(0x2b, VT_BOOL, (void*)&result);
	return result;
}
void SetHideTabControl(BOOL propVal)
{
	SetProperty(0x2b, VT_BOOL, propVal);
}


};


