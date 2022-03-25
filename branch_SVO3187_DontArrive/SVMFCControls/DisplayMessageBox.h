//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// .Module Name     : Display message dialog
/// .File Name       : $Workfile:   DisplayMessageBox.h  $
/// .Description	 : This class is used to display Information, Warning and Error
///					   messages and is based on the Windows Message box
/// ----------------------------------------------------------------------------
/// .Current Version : $Revision:   1.0  $
/// .Check In Date   : $Date:   18 Feb 2015 02:56:02  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource\resource.h"
#pragma endregion Includes

namespace SvMc
{
#pragma region Declarations
	const int MaxButtons = 3;
#pragma endregion Declarations

	class DisplayMessageBox : public CDialog
	{
		//Structure used to describe the buttons in the display message box
		struct ButtonInfo
		{
			ButtonInfo() :
				m_Result(0)
				, m_Show(SW_SHOW)
				, m_Focus(false)
			{
			}
			CString m_Label;							//The label of the button
			UINT m_Result;								//The result returned when this button is clicked
			UINT m_Show;								//Either SW_SHOW or SW_HIDE to display or hide the button
			bool m_Focus;								//True if this button is to have the focus
		};

		enum { IDD = IDD_DISPLAY_MSG_BOX };

	public:
#pragma region Constructor
		//************************************
		//! The class constructor
		//! \param pParent [in] the parent window for the dialog message box
		//************************************
		DisplayMessageBox(CWnd* pParent = nullptr);

		//************************************
		//! The class constructor with message text and details
		//! \param pParent [in] pointer to the parent window
		//! \param Message [in] the message to be displayed
		//! \param MessageDetails [in] the message details to be displayed
		//! \param Type [in] is the type as used in the Windows MessageBox
		//************************************
		DisplayMessageBox(CWnd* pParent, LPCTSTR rMessage, LPCTSTR rMessageDetails, UINT Type);

		//************************************
		//! The class destructor
		//************************************
		virtual ~DisplayMessageBox();
#pragma endregion Constructor

	public:
#pragma region Public Methods
		//************************************
		//! The static method to show the message box 
		//! \param hParent [in] handle to the parent window
		//! \param Message [in] the message to be displayed
		//! \param MessageDetails [in] the message details to be displayed
		//! \param Type [in] is the type as used in the Windows MessageBox
		//! \returns the result of the message box or IDCANCEL if not displayed
		//************************************
		static INT_PTR showDialog(HWND hParent, LPCTSTR Message, LPCTSTR MessageDetails, UINT Type);
#pragma endregion Public Methods

	private:
#pragma region Private Methods
		DECLARE_MESSAGE_MAP()

		virtual void DoDataExchange(CDataExchange* pDX) override;
		virtual BOOL OnInitDialog() override;
		afx_msg void OnPaint();
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		afx_msg void OnBnClickedButton(UINT nID);
		afx_msg void OnBnClickedShowHideDetails();

		//************************************
		//! Initializes the dialog boxes buttons and icon
		//************************************
		void Initialize();

		//************************************
		//! Initializes the dialog boxes buttons
		//************************************
		void initializeButtons();

		//************************************
		//! Initializes the dialog box icon
		//************************************
		void initializeIcon();

		//************************************
		//! Calling the method toggles between showing and hiding the message details
		//************************************
		void ShowHideMessageDetails();

		//************************************
		//! Sets the font used by the display message dialog
		//************************************
		void SetDisplayMessageBoxFont();
#pragma region Private Methods

	private:
#pragma region Member variables
		CFont m_DlgFont;								//The dialog font which uses the same font as the Windows message box
		CString m_Message;								//The main message to display
		CString m_MessageDetails;						//The message details
		CStatic m_MessageBoxIcon;						//The message box icon which uses the same icons as the message box
		CStatic m_MessageCtrl;							//Main message control
		CButton m_ShowHideDetails;						//The show hide details button
		CStatic m_MessageDetailsCtrl;					//The message details control
		CStatic m_DetailsBorder;						//The message details border
		CButton m_Buttons[MaxButtons];					//The button controls
		ButtonInfo m_ButtonInfos[MaxButtons];			//The button information structure list which describes the buttons
		CBrush m_WhiteBrush;							//A white brush used to paint the background
		HICON m_Icon;									//The handle of the icon which is displayed
		bool m_ShowDetails;								//Flag which determines if the message details is shown
		UINT m_Type;									//The message box type
		static CString m_Caption;								//Caption of the dialog with the SVObserver version
#pragma endregion Member variables
	};
} //namespace SvLib
