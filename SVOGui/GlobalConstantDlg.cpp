//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to edit or add a Global Constant value 
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <comutil.h>
//Moved to precompiled header: #include <regex>
#include "GlobalConstantDlg.h"
#include "TextDefinesSvOg.h"
#include "SVTextEditDialog.h"
#include "SVObjectLibrary/SVObjectNameInfo.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#pragma endregion Includes

namespace SvOg
{
	#pragma region Declarations
	#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif

	BEGIN_MESSAGE_MAP(GlobalConstantDlg, CDialog)
	END_MESSAGE_MAP()

	IMPLEMENT_DYNAMIC(GlobalConstantDlg, CDialog)
	#pragma endregion Declarations

	#pragma region Constructor
	GlobalConstantDlg::GlobalConstantDlg( SvDef::GlobalConstantData& rData, CWnd* pParent /*nullptr*/ )
	  : CDialog(GlobalConstantDlg::IDD, pParent)
	, m_rData( rData )
	, m_Branch( SVString(SvOl::FqnGlobal) + _T(".") )
	{

	}

	GlobalConstantDlg::~GlobalConstantDlg()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	void GlobalConstantDlg::setExistingNames( const SVStringVector& rNames )
	{
		m_ExistingNames.clear();

		m_ExistingNames = rNames;
	}
	#pragma endregion Public Methods

	#pragma region Private Methods
	void GlobalConstantDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Text(pDX, IDC_GLOBAL_NAME, m_Name);

		DDV_GlobalName( pDX, SVString(m_Branch + m_Name.GetString()) );
		DDX_Control(pDX, IDC_GLOBAL_TYPE, m_Type);
		DDX_Text(pDX, IDC_GLOBAL_VALUE, m_Value);
		int CurrentSelection = m_Type.GetCurSel();
		if( -1 != CurrentSelection && SvDef::GlobalConstantData::DataTypeMax >  CurrentSelection )
		{
			SvDef::GlobalConstantData::DataTypeEnum CurrentType( static_cast<SvDef::GlobalConstantData::DataTypeEnum> (CurrentSelection) );
			DDV_GlobalConstantValue( pDX, CurrentType );
		}
		DDX_Text(pDX, IDC_GLOBAL_DESCRIPTION, m_Description);
	}

	BOOL GlobalConstantDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();

		for(int i=0; i < SvDef::GlobalConstantData::DataTypeMax; i++ )
		{
			m_Type.AddString( GlobalConstantTypes[i] );
		}

		//Set default type as Decimal
		if( SvDef::GlobalConstantData::DecimalType < m_Type.GetCount() )
		{
			m_Type.SetCurSel( SvDef::GlobalConstantData::DecimalType );
		}

		if( !m_rData.m_DottedName.empty() )
		{
			SVObjectNameInfo ParseName;

			ParseName.ParseObjectName( m_rData.m_DottedName );
			m_Name = ParseName.m_NameArray[ParseName.m_NameArray.size() - 1].c_str();
			ParseName.RemoveBottomName();
			m_Branch = ParseName.GetObjectArrayName() + _T(".");
		}

		switch( m_rData.m_Value.vt )
		{
		case VT_R8:
			{
				m_Value.Format( _T("%.06f"), m_rData.m_Value.dblVal );
				if( SvDef::GlobalConstantData::DecimalType < m_Type.GetCount() )
				{
					m_Type.SetCurSel( SvDef::GlobalConstantData::DecimalType );
				}
			}
			break;
		case VT_BSTR:
			{
				m_Value = m_rData.m_Value;
				if( SvDef::GlobalConstantData::TextType < m_Type.GetCount() )
				{
					m_Type.SetCurSel( SvDef::GlobalConstantData::TextType );
				}
			}
			break;
		default:
			break;
		}
		//If it already existed then do not allow to change the type
		if( SV_GUID_NULL != m_rData.m_Guid )
		{
			m_Type.EnableWindow( FALSE );
		}
		m_Description = m_rData.m_Description.c_str();

		UpdateData( false );
		return TRUE;
	}

	void GlobalConstantDlg::OnOK()
	{
		if( UpdateData( true ) )
		{
			m_rData.m_DottedName = m_Branch;
			m_rData.m_DottedName += m_Name.GetString();
			//Determine which Global Constant type Number or Text
			if( SvDef::GlobalConstantData::DecimalType == m_Type.GetCurSel() )
			{
				m_rData.m_Value = atof( m_Value );
			}
			else if( SvDef::GlobalConstantData::TextType == m_Type.GetCurSel() )
			{
				m_rData.m_Value = m_Value;
			}
			m_rData.m_Description = m_Description.GetString();
			CDialog::OnOK();
		}
	}

	void GlobalConstantDlg::DDV_GlobalConstantValue( CDataExchange* pDX, SvDef::GlobalConstantData::DataTypeEnum Type  )
	{
		SVString NewValue( m_Value );
		if( !NewValue.empty() )
		{
			switch( Type )
			{
			case SvDef::GlobalConstantData::DecimalType:
				{
					if( !std::regex_match( NewValue.cbegin(), NewValue.cend(), std::regex( RegExp_AllRealNumbers ) ) )
					{
						SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
						Exception.setMessage( SVMSG_SVO_65_ENTERED_VALUE_INVALID, m_Value, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25014_GlobalConstantNumber );
						pDX->Fail();
					}
				}
				break;
			case SvDef::GlobalConstantData::TextType:
				break;
			default:
				break;
			}
		}
	}

	void GlobalConstantDlg::DDV_GlobalName( CDataExchange* pDX, const SVString& rName )
	{
		if( pDX->m_bSaveAndValidate && rName != m_rData.m_DottedName )
		{
			SVStringVector::const_iterator Iter( m_ExistingNames.cbegin() );
			bool Failed( false );
			//Checks to see if the name of the Global Constant already exists
			while( m_ExistingNames.cend() != Iter )
			{
				if( rName == *Iter )
				{
					SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
					Exception.setMessage( SVMSG_SVO_66_GLOBAL_NAME_INVALID, rName.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25015_GlobalNameAlreadyUsed );
					Failed = true;
					break;
				}
				++Iter;
			}

			SVString NewName( rName );
			SvUl_SF::searchAndReplace( NewName, m_Branch.c_str(), _T("") );

			if( !Failed )
			{
				if( !std::regex_match( NewName.cbegin(), NewName.cend(), std::regex( RegExp_Name ) ) )
				{
					SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
					Exception.setMessage( SVMSG_SVO_65_ENTERED_VALUE_INVALID, NewName.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25016_GlobalNameInvalid );
					Failed = true;
				}
			}
			if( Failed )
			{
				pDX->Fail();
			}
		}
	}

	#pragma endregion Private Methods
} //namespace SvOg
