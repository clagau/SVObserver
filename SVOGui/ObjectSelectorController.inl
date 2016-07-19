//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file ObjectSelectorController.inl
/// All Rights Reserved 
//*****************************************************************************
/// Controller to display the object selector and return the selected result.
//******************************************************************************
namespace Seidenader { namespace SVOGui {

#pragma region Public Methods
	template <typename ToolsetItemSelector>
	bool ObjectSelectorController::Show(CString& name, const CString& Title, CWnd* pParent, const SVGUID& rInstanceId)
	{
		bool result = false;
		SVString InspectionName = GetInspectionName();
		SVString PPQName = GetPPQName();

		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, SVString( _T("") ) );
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, InspectionName, SVString( _T("") ) );
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, PPQName, SVString( _T("")  ));
		SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::TypeSingleObject );

		SVGUID InstanceGuid = m_InstanceID;
		if (SV_GUID_NULL == InstanceGuid)
		{
			InstanceGuid = GetToolSetGUID();
		}
		SvOsl::SelectorOptions BuildOptions( m_InspectionID, SV_SELECTABLE_FOR_EQUATION, InstanceGuid );
		SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<GlobalSelector, NoSelector, ToolsetItemSelector>( BuildOptions );

		if(0 < name.GetLength())
		{
			SVStringSet nameSet;
			nameSet.insert(name);
			SvOsl::ObjectTreeGenerator::Instance().setCheckItems(nameSet);
		}

		CString mainTabTitle;
		mainTabTitle.LoadString( IDS_SELECT_TOOLSET_OUTPUT );
		CString FilterTab;
		FilterTab.LoadString( IDS_FILTER );

		INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( Title, mainTabTitle, FilterTab, pParent );

		if( IDOK == Result )
		{
			name = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().getLocation().c_str(); 
			result = true;
		}

		return result;
	}
#pragma endregion Public Methods
}}