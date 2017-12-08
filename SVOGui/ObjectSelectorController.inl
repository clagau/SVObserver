//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file ObjectSelectorController.inl
/// All Rights Reserved 
//*****************************************************************************
/// Controller to display the object selector and return the selected result.
//******************************************************************************

namespace SvOg
{

#pragma region Public Methods
	template <typename ToolsetItemSelector>
	bool ObjectSelectorController::Show( std::string& rName, const std::string& rTitle, CWnd* pParent, const SVGUID& rInstanceId)
	{
		bool result = false;
		std::string InspectionName = GetInspectionName();
		std::string PPQName = GetPPQName();

		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterInput, InspectionName, std::string( _T("") ) );
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, InspectionName, std::string( _T("") ) );
		SvOsl::ObjectTreeGenerator::Instance().setLocationFilter( SvOsl::ObjectTreeGenerator::FilterOutput, PPQName, std::string( _T("")  ));
		SvOsl::ObjectTreeGenerator::Instance().setSelectorType( SvOsl::ObjectTreeGenerator::TypeSingleObject );

		SVGUID InstanceGuid = m_InstanceID;
		if (SV_GUID_NULL == InstanceGuid)
		{
			InstanceGuid = GetToolSetGUID();
		}
		SvOsl::SelectorOptions BuildOptions( m_InspectionID, SvDef::SV_SELECTABLE_FOR_EQUATION, InstanceGuid );
		SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<GlobalSelector, NoSelector, ToolsetItemSelector>( BuildOptions );

		if(!rName.empty())
		{
			SvDef::StringSet nameSet;
			nameSet.insert(rName);
			SvOsl::ObjectTreeGenerator::Instance().setCheckItems(nameSet);
		}

		std::string mainTabTitle =	SvUl::LoadStdString( IDS_SELECT_TOOLSET_OUTPUT );
		std::string FilterTab =	SvUl::LoadStdString( IDS_FILTER );

		INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog( rTitle.c_str(), mainTabTitle.c_str(), FilterTab.c_str(), pParent );

		if( IDOK == Result )
		{
			rName = SvOsl::ObjectTreeGenerator::Instance().getSingleObjectResult().m_Location.c_str();
			result = true;
		}

		return result;
	}
#pragma endregion Public Methods
} //namespace SvOg
