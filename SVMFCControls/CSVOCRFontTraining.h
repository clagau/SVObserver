//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRFontTraining
//* .File Name       : $Workfile:   CSVOCRFontTraining.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:12:56  $
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVMFCControls
	{
		class CSVOCRFontTraining : public CWnd
		{
		protected:
			DECLARE_DYNCREATE(CSVOCRFontTraining)
		public:
			CLSID const& GetClsid()
			{
				static CLSID const clsid
					= { 0x9aefbd69, 0xf061, 0x11d2, { 0xa7, 0xb2, 0x0, 0x10, 0x6f, 0x0, 0xc, 0x7d } };
				return clsid;
			}
			virtual BOOL Create(LPCTSTR lpszClassName,
				LPCTSTR lpszWindowName, DWORD dwStyle,
				const RECT& rect,
				CWnd* pParentWnd, UINT nID,
				CCreateContext* pContext = NULL)
			{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

			BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
				const RECT& rect, CWnd* pParentWnd, UINT nID,
				CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
			{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
			pPersist, bStorage, bstrLicKey); }

			// Attributes
		public:

			// Operations
		public:
			long DoSVOCRFontTraining(LPCTSTR PathFileSampleCharImage, LPCTSTR PathFileFontImagesAndLabels);
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

