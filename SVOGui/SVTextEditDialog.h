//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTextEditDialog
//* .File Name       : $Workfile:   SVTextEditDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:36:26  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#pragma endregion Includes

namespace SvOg
{
	class SVTextEditDialog : public CDialog
	{
		enum { IDD = IDD_TEXT_EDIT };
	#pragma region Constructor
	public:
		//************************************
		//! The class constructor
		//! \param Text [in] the text to be edited
		//! \param Title [in] the window Title
		//! \param pParent [in] the parent window for the dialog
		//************************************
		explicit SVTextEditDialog(LPCTSTR Text, CWnd* pParent = nullptr);
		virtual ~SVTextEditDialog();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		//************************************
		//! Method to get the text that has been edited
		//! \return reference to the text
		//************************************
		const CString& getText() { return m_Text; };
	
	#pragma endregion Public Methods

	#pragma region Private Methods
	private:
		DECLARE_MESSAGE_MAP()

		virtual void DoDataExchange(CDataExchange* pDX) override;
		virtual BOOL OnInitDialog() override;
	#pragma region Private Methods

	#pragma region Member variables
	private:
		CString			m_Text;					//The text to be edited
	#pragma endregion Member variables
	};
} //namespace SvOg
