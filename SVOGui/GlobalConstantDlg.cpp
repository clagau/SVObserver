//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the dialog to edit or add a Global Constant value 
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <comutil.h>
#include "GlobalConstantDlg.h"
#include "TextDefinesSvOg.h"
#include "SVTextEditDialog.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObjectLibrary/SVObjectNameInfo.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManagerResource.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#pragma endregion Includes

namespace Seidenader { namespace SVOGui
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
	GlobalConstantDlg::GlobalConstantDlg( SvOi::GlobalConstantData& rData, CWnd* pParent /*nullptr*/ )
	  : CDialog(GlobalConstantDlg::IDD, pParent)
	, m_rData( rData )
	, m_Branch( CString(SvOl::FqnGlobal) + _T(".") )
	{

	}

	GlobalConstantDlg::~GlobalConstantDlg()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	void GlobalConstantDlg::setExistingNames( const SVStringArray& rNames )
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
		SVString Name( m_Branch );
		Name += m_Name.GetString();

		DDV_GlobalName( pDX, Name );
		DDX_Control(pDX, IDC_GLOBAL_TYPE, m_Type);
		DDX_Text(pDX, IDC_GLOBAL_VALUE, m_Value);
		int CurrentSelection = m_Type.GetCurSel();
		if( -1 != CurrentSelection && SvOi::GlobalConstantData::DataTypeMax >  CurrentSelection )
		{
			SvOi::GlobalConstantData::DataTypeEnum CurrentType( static_cast<SvOi::GlobalConstantData::DataTypeEnum> (CurrentSelection) );
			DDV_GlobalConstantValue( pDX, CurrentType );
		}
		DDX_Text(pDX, IDC_GLOBAL_DESCRIPTION, m_Description);
	}

	BOOL GlobalConstantDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();

		for(int i=0; i < SvOi::GlobalConstantData::DataTypeMax; i++ )
		{
			m_Type.AddString( GlobalConstantTypes[i] );
		}

		//Set default type as Decimal
		if( SvOi::GlobalConstantData::DecimalType < m_Type.GetCount() )
		{
			m_Type.SetCurSel( SvOi::GlobalConstantData::DecimalType );
		}

		if( !m_rData.m_DottedName.empty() )
		{
			SVObjectNameInfo ParseName;

			ParseName.ParseObjectName( m_rData.m_DottedName );
			m_Name = ParseName.m_NameArray[ParseName.m_NameArray.size() - 1].c_str();
			ParseName.RemoveBottomName();
			m_Branch = ParseName.GetObjectArrayName().c_str();
			m_Branch += _T(".");
		}

		switch( m_rData.m_Value.vt )
		{
		case VT_R8:
			{
				m_Value.Format( _T("%.06f"), m_rData.m_Value.dblVal );
				if( SvOi::GlobalConstantData::DecimalType < m_Type.GetCount() )
				{
					m_Type.SetCurSel( SvOi::GlobalConstantData::DecimalType );
				}
			}
			break;
		case VT_BSTR:
			{
				m_Value = m_rData.m_Value;
				if( SvOi::GlobalConstantData::TextType < m_Type.GetCount() )
				{
					m_Type.SetCurSel( SvOi::GlobalConstantData::TextType );
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
			if( SvOi::GlobalConstantData::DecimalType == m_Type.GetCurSel() )
			{
				m_rData.m_Value = atof( m_Value );
			}
			else if( SvOi::GlobalConstantData::TextType == m_Type.GetCurSel() )
			{
				m_rData.m_Value = m_Value;
			}
			m_rData.m_Description = m_Description.GetString();
			CDialog::OnOK();
		}
	}

	void GlobalConstantDlg::DDV_GlobalConstantValue( CDataExchange* pDX, SvOi::GlobalConstantData::DataTypeEnum Type  )
	{
		SVString NewValue( m_Value );
		if( !NewValue.empty() )
		{
			switch( Type )
			{
			case SvOi::GlobalConstantData::DecimalType:
				{
					if( !NewValue.matchesRegularExpression( RegExp_AllRealNumbers ) )
					{
						SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
						Exception.setMessage( SVMSG_SVO_65_ENTERED_VALUE_INVALID, m_Value, StdMessageParams, SvOi::Err_25014_GlobalConstantNumber );
						pDX->Fail();
					}
				}
				break;
			case SvOi::GlobalConstantData::TextType:
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
			SVStringArray::const_iterator Iter( m_ExistingNames.cbegin() );
			bool Failed( false );
			//Checks to see if the name of the Global Constant already exists
			while( m_ExistingNames.cend() != Iter )
			{
				if( rName == *Iter )
				{
					SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
					Exception.setMessage( SVMSG_SVO_66_GLOBAL_NAME_INVALID, rName.c_str(), StdMessageParams, SvOi::Err_25015_GlobalNameAlreadyUsed );
					Failed = true;
					break;
				}
				++Iter;
			}

			SVString NewName( rName );
			NewName.replace( m_Branch.GetString(), _T("") );

			if( !Failed )
			{
				if( !NewName.matchesRegularExpression( RegExp_Name ) )
				{
					SvStl::MessageMgrDisplayAndNotify Exception( SvStl::LogAndDisplay );
					Exception.setMessage( SVMSG_SVO_65_ENTERED_VALUE_INVALID, NewName.c_str(), StdMessageParams, SvOi::Err_25016_GlobalNameInvalid );
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
} /* namespace SVOGui */ } /* namespace Seidenader */