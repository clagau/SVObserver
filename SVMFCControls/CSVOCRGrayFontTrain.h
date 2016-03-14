//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRGrayFontTrain
//* .File Name       : $Workfile:   CSVOCRGrayFontTrain.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:13:10  $
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVMFCControls
	{
		class CSVOCRGrayFontTrain : public CWnd
		{
		protected:
			DECLARE_DYNCREATE(CSVOCRGrayFontTrain)
		public:
			CLSID const& GetClsid()
			{
				static CLSID const clsid
					= { 0x29BF4CF3, 0x27AF, 0x11d4, { 0xA8, 0xBA, 0x00, 0x10, 0x6F, 0x03, 0x09, 0xAB } };
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
			BOOL SetLVLastCallBackErrorCode(long FAR* Code);
			long DoSVOCRGrayFontTrain(LPCTSTR PathFileSampleCharImage, LPCTSTR PathFileFontImagesAndLabels);
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;

