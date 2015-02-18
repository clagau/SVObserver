//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
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

namespace Seidenader
{
	namespace SVOMFCLibrary
	{
		#pragma region Declarations
		const int MaxButtons = 3;
		#pragma endregion Declarations

		class DisplayMessageBox : public CDialog
		{
			//Structure used to describe the buttons in the display message box
			struct ButtonInfo
			{
				ButtonInfo():
				 m_Result( 0 )
				,m_Show( SW_SHOW )
				{
				}
				CString m_Label;							//The label of the button
				UINT m_Result;								//The result returned when this button is clicked
				UINT m_Show;								//Either SW_SHOW or SW_HIDE to display or hide the button
			};

			enum { IDD = IDD_DISPLAY_MSG_BOX };

			DECLARE_DYNAMIC(DisplayMessageBox)
		
		public:
		#pragma region Constructor
			//************************************
			//! The class constructor
			//! \param pParent [in] the parent window for the dialog message box
			//************************************
			DisplayMessageBox( CWnd* pParent = nullptr );

			//************************************
			//! The class constructor with message text and details
			//! \param pParent [in] the parent window for the dialog message box
			//! \param rMessage <in> Reference to the main message to display
			//! \param rDisplayMessage <in> Reference to the detailed message
			//************************************
			DisplayMessageBox( CWnd* pParent, CString& rMessage, CString& rMessageDetails, UINT Type );

			//************************************
			//! The class destructor
			//************************************
			virtual ~DisplayMessageBox();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			//************************************
			//! The static method to show the message box 
			//! \param ErrorCode [in] is the unique error number
			//! \param Type [in] is the type as used in the Windows MessageBox
			//! \returns the result of the message box or IDCANCEL if not displayed
			//************************************
			static INT_PTR showDialog( CWnd* pParent, CString& rMessage, CString& rMessageDetails, UINT Type );
		#pragma endregion Public Methods

		private:
		#pragma region Private Methods
			DECLARE_MESSAGE_MAP()

			virtual void DoDataExchange(CDataExchange* pDX);
			virtual BOOL OnInitDialog();
			afx_msg void OnPaint();
			afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
			afx_msg void OnBnClickedButton( UINT nID );
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
		#pragma endregion Member variables
		};
	}
}

namespace SvOml = Seidenader::SVOMFCLibrary;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVOMFCLibrary\DisplayMessageBox.h_v  $
 * 
 *    Rev 1.0   18 Feb 2015 02:56:02   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  984
 * SCR Title:  Exception Display Class with Exception Manager Template SVO-524
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */