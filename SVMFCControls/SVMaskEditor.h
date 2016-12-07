#pragma once

namespace Seidenader
{
	namespace SVMFCControls
	{
		class CSVMaskEditor : public CWnd
		{
		protected:
			DECLARE_DYNCREATE(CSVMaskEditor)
		public:
			CLSID const& GetClsid()
			{
				static CLSID const clsid
					= { 0xc87c9b74, 0xe6dc, 0x11d2, { 0xa7, 0x7b, 0x0, 0x10, 0x6f, 0x0, 0xc, 0x7d } };
				return clsid;
			}
			virtual BOOL Create(LPCTSTR lpszClassName,
				LPCTSTR lpszWindowName, DWORD dwStyle,
				const RECT& rect,
				CWnd* pParentWnd, UINT nID,
				CCreateContext* pContext = nullptr) override
			{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

			BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
				const RECT& rect, CWnd* pParentWnd, UINT nID,
				CFile* pPersist = nullptr, BOOL bStorage = FALSE,
				BSTR bstrLicKey = nullptr)
			{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
			pPersist, bStorage, bstrLicKey); }

			// Attributes
		public:
			LPUNKNOWN GetImageDIBHandle();
			void SetImageDIBHandle(LPUNKNOWN);

			long GetMaskOperator();
			void SetMaskOperator(long);

			LPUNKNOWN GetMaskData();
			void SetMaskData(LPUNKNOWN);

			// Operations
		public:
			void Refresh();
			long DoEditMaskModal();
			void AboutBox();
		};
	} //SVMFCControls
} //Seidenader

namespace SvMc = Seidenader::SVMFCControls;
