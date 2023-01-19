//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : HistogramAnalyzer
//* .File Name       : $Workfile:   SVHistogramAnalyzerSetup.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:46:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "AnalyzerOperators/HistogramAnalyzer.h"
#include "SVHistogramAnalyzerSetup.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSetupDialogManager.h"
#include "SVStatusLibrary/MessageManager.h"
#include "InspectionCommands/CommandExternalHelper.h"
#pragma endregion Includes

BEGIN_MESSAGE_MAP(SVSquare, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void SVSquare::OnPaint()
{
	CPaintDC dc(this);
	CRect l_rect;
	GetClientRect(&l_rect);
	CBrush l_brush(m_color);
	CBrush * l_tmp = dc.SelectObject(&l_brush);
	dc.FillRect(&l_rect, &l_brush);
	dc.SelectObject(l_tmp);
}

BOOL SVSquare::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetResourceHandle();

	if (!(::GetClassInfo(hInst, _T("SVCuteSquare"), &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style = CS_DBLCLKS | CS_PARENTDC | CS_BYTEALIGNCLIENT;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInst;
		wndcls.hIcon = nullptr;
		wndcls.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground = (HBRUSH)(COLOR_GRADIENTACTIVECAPTION);
		wndcls.lpszMenuName = nullptr;
		wndcls.lpszClassName = _T("SVCuteSquare");

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

#pragma warning (push)
#pragma warning (disable : 5054)
SVHistogramAnalyzerSetup::SVHistogramAnalyzerSetup(CWnd* pParent /*=nullptr*/)
	: CDialog(SVHistogramAnalyzerSetup::IDD, pParent), m_pAnalyzer(nullptr)
{
	unsigned int opt = m_histogram.GetOptions();
	opt &= ~(histogram::color_mask | histogram::clip_mask | histogram::placement_mask);
	m_histogram.SetOptions(opt | histogram::config | histogram::clip | histogram::analyzer);
}
#pragma warning (pop)

SVHistogramAnalyzerSetup::~SVHistogramAnalyzerSetup()
{
}

void SVHistogramAnalyzerSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HISTOGRAM, m_histogram);
	DDX_Control(pDX, IDC_FIRSTPEAKCOLOR, m_firstSquare);
	DDX_Control(pDX, IDC_SECONDPEAKCOLOR, m_secondSquare);
	DDX_Control(pDX, IDC_VALLEYCOLOR, m_valleySquare);
}


BEGIN_MESSAGE_MAP(SVHistogramAnalyzerSetup, CDialog)
	ON_BN_CLICKED(IDC_RADIOLIN, &SVHistogramAnalyzerSetup::OnBnClickedRadiolin)
	ON_BN_CLICKED(IDC_RADIOLOG, &SVHistogramAnalyzerSetup::OnBnClickedRadiolog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_RESETBUTTON, &SVHistogramAnalyzerSetup::OnBnClickedResetbutton)
	ON_BN_CLICKED(IDC_ACCUMULATE, &SVHistogramAnalyzerSetup::OnBnClickedAccumulate)
	ON_EN_CHANGE(IDC_DIST_THRESHOLD, &SVHistogramAnalyzerSetup::OnEnChangeDistThreshold)
	ON_EN_CHANGE(IDC_DEFAULT_PEAK, &SVHistogramAnalyzerSetup::OnEnChangeDefaultPeak)
	ON_EN_CHANGE(IDC_HEIGHT_THRESHOLD, &SVHistogramAnalyzerSetup::OnEnChangeHeightThreshold)
	ON_EN_KILLFOCUS(IDC_DIST_THRESHOLD, &SVHistogramAnalyzerSetup::OnEnKillfocusDistThreshold)
	ON_EN_KILLFOCUS(IDC_HEIGHT_THRESHOLD, &SVHistogramAnalyzerSetup::OnEnKillfocusHeightThreshold)
	ON_EN_KILLFOCUS(IDC_LOWCLIP, &SVHistogramAnalyzerSetup::OnEnKillfocusLowclip)
	ON_EN_KILLFOCUS(IDC_HIGHCLIP, &SVHistogramAnalyzerSetup::OnEnKillfocusHighclip)
	ON_EN_KILLFOCUS(IDC_DEFAULT_PEAK, &SVHistogramAnalyzerSetup::OnEnKillfocusDefaultPeak)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_LOWCLIP, &SVHistogramAnalyzerSetup::OnEnChangeLowclip)
	ON_EN_CHANGE(IDC_HIGHCLIP, &SVHistogramAnalyzerSetup::OnEnChangeHighclip)
	ON_EN_CHANGE(IDC_FIXEDEDIT, &SVHistogramAnalyzerSetup::OnEnChangeFixededit)
	ON_EN_KILLFOCUS(IDC_FIXEDEDIT, &SVHistogramAnalyzerSetup::OnEnKillfocusFixededit)
	ON_EN_CHANGE(IDC_LOW_VALLEY, &SVHistogramAnalyzerSetup::OnEnChangeLowValley)
	ON_EN_KILLFOCUS(IDC_LOW_VALLEY, &SVHistogramAnalyzerSetup::OnEnKillfocusLowValley)
	ON_EN_CHANGE(IDC_HIGH_VALLEY, &SVHistogramAnalyzerSetup::OnEnChangeHighValley)
	ON_EN_KILLFOCUS(IDC_HIGH_VALLEY, &SVHistogramAnalyzerSetup::OnEnKillfocusHighValley)
	ON_EN_CHANGE(IDC_DEFAULT_VALLEY, &SVHistogramAnalyzerSetup::OnEnChangeDefaultValley)
	ON_EN_KILLFOCUS(IDC_DEFAULT_VALLEY, &SVHistogramAnalyzerSetup::OnEnKillfocusDefaultValley)
	ON_BN_CLICKED(IDC_RADIODYNAMIC, &SVHistogramAnalyzerSetup::OnBnClickedRadiodynamic)
	ON_BN_CLICKED(IDC_RADIOFIXED, &SVHistogramAnalyzerSetup::OnBnClickedRadiofixed)
	ON_BN_CLICKED(IDC_VALLEY_RANGE, &SVHistogramAnalyzerSetup::OnBnClickedValleyRange)
	ON_BN_CLICKED(IDC_SECONDPEAK_RANGE, &SVHistogramAnalyzerSetup::OnBnClickedSecondpeakRange)
	ON_BN_CLICKED(IDC_FIRSTPEAK_RANGE, &SVHistogramAnalyzerSetup::OnBnClickedFirstpeakRange)
END_MESSAGE_MAP()


HRESULT SVHistogramAnalyzerSetup::SetInspectionData()
{
	return SvCmd::RunOnceSynchronous(m_pAnalyzer->GetInspection()->getObjectId());
}

int SVHistogramAnalyzerSetup::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	const std::vector<long>& resultVector = m_pAnalyzer->GetHistogram();
	m_histogram.SetPixelCounts(resultVector.begin(), resultVector.end());
	m_firstSquare.SetColor(SvDef::DarkGreen);
	m_secondSquare.SetColor(SvDef::Crimson);
	m_valleySquare.SetColor(SvDef::Plum);

	return 0;
}

void SVHistogramAnalyzerSetup::OnBnClickedRadiolin()
{
	if (m_histogram.SetScale(histogram::linear))
	{
		m_histogram.Invalidate(0);
	}
}

void SVHistogramAnalyzerSetup::OnBnClickedRadiolog()
{
	if (m_histogram.SetScale(histogram::logarithmic))
	{
		m_histogram.Invalidate(0);
	}
}

void SVHistogramAnalyzerSetup::OnBnClickedResetbutton()
{
	SetDlgItemInt(IDC_LOWCLIP, c_zero, FALSE);
	SetDlgItemInt(IDC_HIGHCLIP, c_255, FALSE);
}

void SVHistogramAnalyzerSetup::OnBnClickedAccumulate()
{
	HWND hWndCtrl;
	GetDlgItem(IDC_ACCUMULATE, &hWndCtrl);
	bool l_accumulate = ::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0L) > 0;

	if (m_histogram.SetDisplay(l_accumulate ? histogram::accumulated : histogram::normal))
	{
		m_histogram.ForceRepaint();
		m_histogram.Invalidate(0);
	}
}

void SVHistogramAnalyzerSetup::OnBnClickedRadiodynamic()
{
	CWnd * hFx = GetDlgItem(IDC_FIXEDEDIT);
	if (hFx)
		hFx->EnableWindow(FALSE);
	m_histogram.SetHeightOption(histogram::dynamic);
	Refresh();
}

void SVHistogramAnalyzerSetup::OnBnClickedRadiofixed()
{
	CWnd * hFx = GetDlgItem(IDC_FIXEDEDIT);
	if (hFx)
		hFx->EnableWindow(TRUE);
	m_histogram.SetHeightOption(histogram::fixed);
	Refresh();
}

inline void SVHistogramAnalyzerSetup::SetResultRange(SvPb::EmbeddedIdEnum embeddedID)
{
	SvOi::IObjectClass* pAnalyzerResult = m_pAnalyzer->GetResultObject(embeddedID);
	auto* pInspection = m_pAnalyzer->GetInspection();
	if (pAnalyzerResult && pInspection)
	{
		SVSetupDialogManager::Instance().SetupDialog(pAnalyzerResult->GetClassID(), pInspection->getObjectId(), pAnalyzerResult->getObjectId(), this);
	}
	else
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams));
	}
}

void SVHistogramAnalyzerSetup::OnBnClickedValleyRange()
{
	SetResultRange(m_pAnalyzer->msvValley.GetEmbeddedID());
}

void SVHistogramAnalyzerSetup::OnBnClickedSecondpeakRange()
{
	SetResultRange(m_pAnalyzer->msvLowPeak.GetEmbeddedID());
}

void SVHistogramAnalyzerSetup::OnBnClickedFirstpeakRange()
{
	SetResultRange(m_pAnalyzer->msvHighPeak.GetEmbeddedID());
}

void SVHistogramAnalyzerSetup::OnEnChangeDistThreshold()
{
	BOOL l_ok = false;
	int l_threshold = static_cast<int>(GetDlgItemInt(IDC_DIST_THRESHOLD, &l_ok, FALSE));
	if (l_ok && m_histogram.SetPeakThreshold(l_threshold))
	{
		Refresh();
	}
}

void SVHistogramAnalyzerSetup::OnEnChangeDefaultPeak()
{
	BOOL l_ok = false;
	int l_defaultPeak = static_cast<int>(GetDlgItemInt(IDC_DEFAULT_PEAK, &l_ok, FALSE));
	if (l_ok && m_histogram.SetPeakDefault(l_defaultPeak))
	{
		Refresh();
	}
}

void SVHistogramAnalyzerSetup::OnEnChangeHeightThreshold()
{
	BOOL l_ok = false;
	TCHAR buff[32];
	l_ok = GetDlgItemText(IDC_HEIGHT_THRESHOLD, buff, 31);
	if (l_ok)
	{
		double l_threshold = _tstof(buff);
		if (m_histogram.SetMinHeight(l_threshold))
			Refresh();
	}
}

void SVHistogramAnalyzerSetup::OnEnChangeLowclip()
{
	BOOL l_ok = false;
	int l_low = static_cast<int>(GetDlgItemInt(IDC_LOWCLIP, &l_ok, FALSE));
	if (l_ok && m_histogram.Clip(l_low, SvUl::no_clip))
	{
		Refresh();
	}
}

void SVHistogramAnalyzerSetup::OnEnChangeHighclip()
{
	BOOL l_ok = false;
	int l_high = static_cast<int>(GetDlgItemInt(IDC_HIGHCLIP, &l_ok, FALSE));
	if (l_ok && m_histogram.Clip(SvUl::no_clip, l_high))
	{
		Refresh();
	}
}

void SVHistogramAnalyzerSetup::OnEnChangeFixededit()
{
	BOOL l_ok = false;
	TCHAR buff[32];
	l_ok = GetDlgItemText(IDC_FIXEDEDIT, buff, 31);
	if (l_ok)
	{
		double l_threshold = _tstof(buff);
		if (m_histogram.SetFixedHeight(l_threshold))
			Refresh();
	}
}

void SVHistogramAnalyzerSetup::OnEnChangeLowValley()
{
	BOOL l_ok = false;
	int l_tmp = static_cast<int>(GetDlgItemInt(IDC_LOW_VALLEY, &l_ok, FALSE));
	if (l_ok && m_histogram.SetValleyLow(l_tmp))
	{
		Refresh();
	}
}

void SVHistogramAnalyzerSetup::OnEnChangeHighValley()
{
	BOOL l_ok = false;
	int l_tmp = static_cast<int>(GetDlgItemInt(IDC_HIGH_VALLEY, &l_ok, FALSE));
	if (l_ok && m_histogram.SetValleyHigh(l_tmp))
	{
		Refresh();
	}
}

void SVHistogramAnalyzerSetup::OnEnChangeDefaultValley()
{
	BOOL l_ok = false;
	int l_tmp = static_cast<int>(GetDlgItemInt(IDC_DEFAULT_VALLEY, &l_ok, FALSE));
	if (l_ok && m_histogram.SetValleyDefault(l_tmp))
	{
		Refresh();
	}
}

void SVHistogramAnalyzerSetup::OnEnKillfocusDistThreshold()
{
	SetDlgItemInt(IDC_DIST_THRESHOLD, m_histogram.GetPeakThreshold(), FALSE);
}

void SVHistogramAnalyzerSetup::OnEnKillfocusHeightThreshold()
{
	std::string Text = std::format(_T("{:3.2f}"), m_histogram.GetMinHeight());
	SetDlgItemText(IDC_HEIGHT_THRESHOLD, Text.c_str());
}

void SVHistogramAnalyzerSetup::OnEnKillfocusLowclip()
{
	SetDlgItemInt(IDC_LOWCLIP, m_histogram.GetLowClip(), FALSE);
}

void SVHistogramAnalyzerSetup::OnEnKillfocusHighclip()
{
	SetDlgItemInt(IDC_HIGHCLIP, m_histogram.GetHighClip(), FALSE);
}

void SVHistogramAnalyzerSetup::OnEnKillfocusDefaultPeak()
{
	SetDlgItemInt(IDC_DEFAULT_PEAK, m_histogram.GetPeakDefault(), FALSE);
}

void SVHistogramAnalyzerSetup::OnEnKillfocusFixededit()
{
	std::string Text = std::format(_T("{:3.2f}"), m_histogram.GetFixedHeight());
	SetDlgItemText(IDC_FIXEDEDIT, Text.c_str());
}

void SVHistogramAnalyzerSetup::OnEnKillfocusLowValley()
{
	SetDlgItemInt(IDC_LOW_VALLEY, m_histogram.GetValleyLow(), FALSE);
}

void SVHistogramAnalyzerSetup::OnEnKillfocusHighValley()
{
	SetDlgItemInt(IDC_HIGH_VALLEY, m_histogram.GetValleyHigh(), FALSE);
}

void SVHistogramAnalyzerSetup::OnEnKillfocusDefaultValley()
{
	SetDlgItemInt(IDC_DEFAULT_VALLEY, m_histogram.GetValleyDefault(), FALSE);
}

void SVHistogramAnalyzerSetup::OnClose()
{
	CDialog::OnClose();
}

void SVHistogramAnalyzerSetup::Refresh()
{
	m_histogram.CalcStats();
	m_histogram.ForceRepaint();
	std::string Text;
	Text = std::format(_T("{:3.2f}"), m_histogram.mean());
	SetDlgItemText(IDC_MEAN, Text.c_str());
	Text = std::format(_T("{:3.2f}"), m_histogram.stdDev());
	SetDlgItemText(IDC_STDDEV, Text.c_str());
	Text = std::format(_T("{:d}"), m_histogram.min_pixel());
	SetDlgItemText(IDC_MIN, Text.c_str());
	Text = std::format(_T("{:d}"), m_histogram.max_pixel());
	SetDlgItemText(IDC_MAX, Text.c_str());
	Text = std::format(_T("{:d}"), m_histogram.pixel_num());
	SetDlgItemText(IDC_COUNT, Text.c_str());
	Text = std::format(_T("{:d}"), m_histogram.bin_count());
	SetDlgItemText(IDC_GRAYS, Text.c_str());
	Text = std::format(_T("{:d}"), m_histogram.low_peak());
	SetDlgItemText(IDC_LOWPEAK, Text.c_str());
	Text = std::format(_T("{:d}"), m_histogram.high_peak());
	SetDlgItemText(IDC_HIGHPEAK, Text.c_str());
	Text = std::format(_T("{:d}"), m_histogram.valley());
	SetDlgItemText(IDC_VALLEY, Text.c_str());
	Invalidate(0);
}


BOOL SVHistogramAnalyzerSetup::OnInitDialog()
{
	CDialog::OnInitDialog();
	Refresh();
	HWND hWndCtrl = ::GetDlgItem(m_hWnd, IDC_ACCUMULATE);
	int l_chk = m_histogram.GetDisplay();
	::SendMessage(hWndCtrl, BM_SETCHECK, l_chk, 0L);

	l_chk = (m_histogram.GetScale() == histogram::linear) ? IDC_RADIOLIN : IDC_RADIOLOG;
	CheckRadioButton(IDC_RADIOLOG, IDC_RADIOLIN, l_chk);

	SetDlgItemInt(IDC_HIGHCLIP, m_histogram.GetHighClip(), FALSE);
	SetDlgItemInt(IDC_LOWCLIP, m_histogram.GetLowClip(), FALSE);
	SetDlgItemInt(IDC_DIST_THRESHOLD, m_histogram.GetPeakThreshold(), FALSE);
	SetDlgItemInt(IDC_DEFAULT_PEAK, m_histogram.GetPeakDefault(), FALSE);
	SetDlgItemInt(IDC_LOW_VALLEY, m_histogram.GetValleyLow(), FALSE);
	SetDlgItemInt(IDC_HIGH_VALLEY, m_histogram.GetValleyHigh(), FALSE);
	SetDlgItemInt(IDC_DEFAULT_VALLEY, m_histogram.GetValleyDefault(), FALSE);

	std::string Text = std::format(_T("{:3.2f}"), m_histogram.GetMinHeight());
	SetDlgItemText(IDC_HEIGHT_THRESHOLD, Text.c_str());
	l_chk = (m_histogram.GetHeightOption() == histogram::dynamic) ? IDC_RADIODYNAMIC : IDC_RADIOFIXED;
	CheckRadioButton(IDC_RADIODYNAMIC, IDC_RADIOFIXED, l_chk);
	Text = std::format(_T("{:3.2f}"), m_histogram.GetFixedHeight());
	SetDlgItemText(IDC_FIXEDEDIT, Text.c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
}

