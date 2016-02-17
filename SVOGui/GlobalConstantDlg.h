//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to edit or add a Global Constant value 
//******************************************************************************
#pragma once

#pragma region Includes
#include <comutil.h>
#include "SVOResource\resource.h"
#include "SVUtilityLibrary\SVString.h"
#include "ObjectInterfaces\DataStructures.h"
#pragma endregion Includes

namespace Seidenader { namespace SVOGui
{
	class GlobalConstantDlg : public CDialog
	{
	#pragma region Declarations
	public:
		enum { IDD = IDD_GLOBAL_CONSTANT };

		DECLARE_DYNAMIC(GlobalConstantDlg)

	#pragma endregion Declarations
	
	#pragma region Constructor
	public:
		//************************************
		//! The class constructor
		//! \param rData [in] reference to the Global Constant data
		//! \param pParent [in] the parent window for the dialog
		//************************************
		GlobalConstantDlg( SvOi::GlobalConstantData& rData, CWnd* pParent = nullptr );

		virtual ~GlobalConstantDlg();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		//************************************
		//! Method to set a list of existing names already used
		//! \param rNames [in] reference to list of names
		//************************************
		void setExistingNames( const SVStringArray& rNames );
	#pragma endregion Public Methods

	#pragma region Private Methods
	private:
		DECLARE_MESSAGE_MAP()

		virtual void DoDataExchange(CDataExchange* pDX);
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		
		void DDV_GlobalConstantValue( CDataExchange* pDX, SvOi::GlobalConstantData::DataTypeEnum Type );
		void DDV_GlobalName( CDataExchange* pDX, const SVString& rName );
	#pragma region Private Methods

	#pragma region Member variables
	private:
		SvOi::GlobalConstantData& m_rData;					//Reference to the global constant data
		SVStringArray			m_ExistingNames;			//List of existing global constant names
		CString					m_Branch;					//The branch in which the global resides 
		CString					m_Name;						//The global constant name
		CComboBox				m_Type;						//The global constant type
		CString					m_Description;				//The global constant description
		CString					m_Value;					//The string value of the constant
	#pragma endregion Member variables
	};
} /* namespace SVOGui */ } /* namespace Seidenader */

namespace SvOg = Seidenader::SVOGui;
