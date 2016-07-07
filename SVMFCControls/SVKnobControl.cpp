//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVKnobControl
//* .File Name       : $Workfile:   SVKnobControl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:46:14  $
//******************************************************************************

#include "stdafx.h"
#include "SVOResource\resource.h"
#include "SVKnobControl.h"

namespace Seidenader
{
	namespace SVMFCControls
	{
		IMPLEMENT_DYNAMIC(SVKnobControl, CStatic)

			SVKnobControl::SVKnobControl(): 
		CStatic(),
			m_dragging(false),
			m_min(min_val()),
			m_max(max_val()),
			m_angle(3.0)
		{
			clicks = 0;
			RegisterWindowClass();
		}

		SVKnobControl::~SVKnobControl()
		{
		}


		BEGIN_MESSAGE_MAP(SVKnobControl, CStatic)
			ON_WM_LBUTTONDOWN()
			ON_WM_LBUTTONUP()
			ON_WM_MOUSEMOVE()
			ON_WM_PAINT()
			ON_WM_ERASEBKGND()
		END_MESSAGE_MAP()


		BOOL SVKnobControl::RegisterWindowClass()
		{
			WNDCLASS wndcls;
			HINSTANCE hInst = AfxGetResourceHandle();

			if (!(::GetClassInfo(hInst, SVKNOB_CLASSNAME, &wndcls)))
			{
				// otherwise we need to register a new class
				wndcls.style            = CS_DBLCLKS | CS_PARENTDC | CS_BYTEALIGNCLIENT;
				wndcls.lpfnWndProc      = ::DefWindowProc;
				wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
				wndcls.hInstance        = hInst;
				wndcls.hIcon            = nullptr;
				wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
				wndcls.hbrBackground    = (HBRUSH) (COLOR_GRADIENTACTIVECAPTION);
				wndcls.lpszMenuName     = nullptr;
				wndcls.lpszClassName    = SVKNOB_CLASSNAME;

				if (!AfxRegisterClass(&wndcls))
				{
					AfxThrowResourceException();
					return FALSE;
				}
			}

			return TRUE;
		}
		void SVKnobControl::DoDataExchange(CDataExchange* pDX)
		{
		}

		// SVKnobControl message handlers

		void SVKnobControl::OnLButtonDown(UINT nFlags, CPoint point)
		{
			CRect l_rect = dotRect();
			l_rect.InflateRect(1, 1);
			if (l_rect.PtInRect(point))
			{
				SetCapture();
				m_dragging = true;
				l_rect = knob.rect;
				l_rect.InflateRect(-6, -6);
				l_rect.bottom = m_maxy;
				ClientToScreen(&l_rect);
				if (!ClipCursor(&l_rect))
				{
					DWORD er = GetLastError();
					LPTSTR msg = nullptr;
					FormatMessage(
						FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						nullptr,
						er,
						0,
						msg,
						32,
						nullptr
						);
					OutputDebugString(msg);
					LocalFree(msg);
				}
			}
		}

		void SVKnobControl::OnLButtonUp(UINT nFlags, CPoint point)
		{
			ClipCursor(nullptr);
			m_dragging = false;
			ReleaseCapture();
		}

		void SVKnobControl::OnMouseMove(UINT nFlags, CPoint point)
		{
			if (m_dragging)
			{
				CPoint pt = CPoint(m_center.x - point.x, m_center.y - point.y);
				if (pt.x == 0 && pt.y == 0)
				{
					pt.x =1;
				}
				double l_angle = xy2angle(pt);
				double l_value = angle2val(l_angle);
				CString msg;
				
				if (l_value < m_min)
				{
					l_value = m_min;
					l_angle = val2angle(l_value);
				}
				else if (l_value > m_max)
				{
					if (m_value < m_min + (m_max-m_min)/2.0)
					{
						l_value = m_min;
					}
					else
					{
						l_value = m_max;
					}
					l_angle = val2angle(l_value);
				}
				m_dotPoint = angle2xy(knob.center, l_angle, m_radius);
				pt = m_dotPoint + CSize(4,4);
				ClientToScreen(&pt);
				ClientToScreen(&point);
				if (fabs(m_value - l_value) > .001)
				{
					m_value = l_value;
					m_angle = l_angle;
					GetParent()->PostMessage(WM_TRIGGER_CHANGE, (WPARAM)int_value(m_value), 0);
					Invalidate(FALSE);
				}
			}
		}

		void SVKnobControl::OnPaint()
		{
			CPaintDC dc(this); // device context for painting
			CRect client;
			GetClientRect(client);
			if (nullptr == m_image.GetSafeHandle())
			{
				m_image.CreateCompatibleBitmap(&dc, client.Width(), client.Height());
			}
			if (nullptr == m_screen.GetSafeHandle())
			{
				m_screen.CreateCompatibleBitmap(&dc, client.Width(), client.Height());
			}
			CBitmap * t_screen = dc.SelectObject(&m_screen);
			CBitmap * t_image = m_dc.SelectObject(&m_image);
			m_dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
			m_dc.FillSolidRect(client, GetSysColor(COLOR_BTNFACE));
			TransparentBlt(m_dc, knob, CPoint(0,0));
			DrawTicks(m_dc);
			m_dotPoint = angle2xy(knob.center - CSize(5,5), m_angle, m_radius);
			CPen pen(PS_SOLID, 1, RGB(182, 28, 39));
			CPen * t_pen = m_dc.SelectObject(&pen);
			CBrush brush(&(dot.bitmap.face));
			CBrush * t_brush = m_dc.SelectObject(&brush);
			CRect rect = dot.rect;
			rect.MoveToXY(m_dotPoint);
			m_dc.Ellipse(rect);

			dc.BitBlt(0, 0, knob.rect.Width(), knob.rect.Height(), &m_dc, 0, 0, SRCCOPY);
			dc.SelectObject(t_screen);
			m_dc.SelectObject(t_brush);
			m_dc.SelectObject(t_pen);
			m_dc.SelectObject(t_image);
		}

		void SVKnobControl::DrawTicks(CDC & dc)
		{
			CRect rect;
			GetClientRect(&rect);
			CFont font;
			font.CreateFont(
				10,                        // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_NORMAL,                 // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				_T("Lucida Console"));     // lpszFacename
			CFont * tmp_font = dc.SelectObject(&font);
			dc.TextOut(2, rect.Height() - 10, "min");
			dc.TextOut(rect.Width() - 18, rect.Height() - 10, "max");
			dc.SelectObject(tmp_font);
			double pi_6 = pi()/6.0;
			CPen pen(PS_SOLID, 1, RGB(0,18,29));
			CPen * t_pen = dc.SelectObject(&pen);
			CBrush brush(RGB(0, 10, 20));
			CBrush * t_brush = dc.SelectObject(&brush);
			for (double a = 0.0; a < pihalf(); a += pi_6)
			{
				Tick(dc, pitwo() - a, false);
				Tick(dc, pi() + a, false);
			}
			Tick(dc, pi_6, true);
			Tick(dc, 5.0*pi_6, true);
			Tick(dc, 3.0*pihalf(), true);
			dc.SelectObject(t_brush);
			dc.SelectObject(t_pen);
		}

		void SVKnobControl::Tick(CDC & dc, double angle, bool bold)
		{
			int radius = m_radius + 18 + static_cast<int>(bold);
			int off = 1 + static_cast<int>(bold);
			CPoint pt = angle2xy(m_center, angle, radius) + CSize(-off--, -off);
			off = bold?5:3;
			CRect rect = CRect(pt, CSize(off, off));
			dc.Ellipse(&rect);
		}

		BOOL SVKnobControl::OnEraseBkgnd(CDC* pDC)
		{
			return TRUE;
		}

		void SVKnobControl::CreateBitmaps()
		{
			HDC hdc = ::GetDC(nullptr);
			CDC dc;
			dc.Attach(hdc);
			m_dc.CreateCompatibleDC(&dc);
			knob.LoadBitmaps(IDB_KNOB_FACE, IDB_KNOB_MASK, dc);
			dot.LoadBitmaps(IDB_SVDOT_FACE, IDB_SVDOT_MASK, dc);
			::ReleaseDC(nullptr, dc.Detach());
			m_radius = knob.rect.Width()/2 - 2;
			m_center = knob.center;
			m_radius -= dot.rect.Width() * 2;
			CPoint pt = val2xy(m_min);
			m_maxy = pt.y;
			pt = val2xy(m_max);
			if (m_maxy < pt.y)
				m_maxy = pt.y;
			++m_maxy;
		}

		bool SVKnobControl::TransparentBlt(CDC & dest, sv::hold & src, CPoint pt)
		{
			dest.SetBkMode(TRANSPARENT);
			CRect rect = src.rect;
			rect.MoveToXY(pt);
			BitBlt(dest, rect, src.mdc.mask, CPoint(0,0), SRCAND);
			BitBlt(dest, rect, src.mdc.face, CPoint(0,0), SRCPAINT);
			return true;
		}

		int SVKnobControl::SetValue(int value)
		{
			int ret = value;
			m_value = int2val(value);
			if (m_value < m_min)
			{
				m_value = m_min;
				ret = int_value(m_value);
			}
			if (m_value > m_max)
			{
				m_value = m_max;
				ret = int_value(m_value);
			}
			m_angle = val2angle(m_value);
			CString msg;
			msg.Format("angle: %.4f, value: %.4f, int: %d\n", m_angle, m_value, value);
			OutputDebugString(msg);
			Invalidate();
			return ret;
		}
	} //SVMFCControls
} //Seidenader

