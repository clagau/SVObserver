//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to solve Global Constant value conflicts
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVOResource/resource.h"
#include "GridCtrlLibrary\GridCtrl.h"
#include "Definitions/GlobalConstantData.h"
#pragma endregion Includes

namespace SvOg
{
	class GlobalConstantConflictDlg : public CDialog
	{
	#pragma region Declarations
	private:
		enum { IDD = IDD_GLOBAL_CONSTANT_CONFLICT };

		DECLARE_DYNAMIC(GlobalConstantConflictDlg)
	#pragma endregion Declarations

	#pragma region Constructor
	public:
		GlobalConstantConflictDlg( SvDef::GlobalConflictPairVector& rGlobalPairs, CWnd* pParent = nullptr );
		virtual ~GlobalConstantConflictDlg();
	#pragma endregion Constructor

	#pragma region Protected Methods
	protected:
		DECLARE_MESSAGE_MAP()

		virtual void DoDataExchange(CDataExchange* pDX) override;
		afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/);
		virtual BOOL OnInitDialog() override;

		void LoadGlobalData();
		void setGridGlobalData(const SvDef::GlobalConstantData& rGlobalData, int RowIndex, int& rColumnIndex );
		void convertValueToString( const _variant_t& rValue, std::string& rText,  std::string& rType );
	#pragma endregion Protected Methods

	#pragma region Member variables
	private:
		SvGcl::CGridCtrl m_Grid;						//The grid displaying the conflicting values
		SvDef::GlobalConflictPairVector& m_rGlobalPairs;  //This is the conflict Global Data as data pair
	#pragma endregion Member variables
	};
} //namespace SvOg
