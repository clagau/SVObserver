//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to edit or add a Global Constant value 
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comutil.h>
#include "SVOResource\resource.h"
#include "Definitions/GlobalConstantData.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvOg
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
		GlobalConstantDlg( SvDef::GlobalConstantData& rData, CWnd* pParent = nullptr );

		virtual ~GlobalConstantDlg();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		//************************************
		//! Method to set a list of existing names already used
		//! \param rNames [in] reference to list of names
		//************************************
		void setExistingNames( const SvDef::StringVector& rNames );
	#pragma endregion Public Methods

	#pragma region Private Methods
	private:
		DECLARE_MESSAGE_MAP()

		virtual void DoDataExchange(CDataExchange* pDX) override;
		virtual BOOL OnInitDialog() override;
		virtual void OnOK() override;
		
		void DDV_GlobalConstantValue( CDataExchange* pDX, SvDef::GlobalConstantData::DataTypeEnum Type );
		void DDV_GlobalName( CDataExchange* pDX, const std::string& rName );
	#pragma region Private Methods

	#pragma region Member variables
	private:
		SvDef::GlobalConstantData& m_rData;					//Reference to the global constant data
		SvDef::StringVector		m_ExistingNames;			//List of existing global constant names
		std::string				m_Branch;					//The branch in which the global resides 
		CString					m_Name;						//The global constant name
		CComboBox				m_Type;						//The global constant type
		CString					m_Description;				//The global constant description
		CString					m_Value;					//The string value of the constant
	#pragma endregion Member variables
	};
} //namespace SvOg
