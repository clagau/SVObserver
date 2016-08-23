//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the Configuration Obsolete Item checker
//******************************************************************************

namespace Seidenader { namespace  SVXMLLibrary
{
	#pragma region String Defines
	LPCWSTR const TAG_ALIAS = L"alias ";
	LPCWSTR const TAG_SINGLE_QUOTE = L"\'";
	LPCSTR const TAG_VIPER = "Viper_";
	LPCSTR const TAG_1394 = "Matrox_1394";
	LPCSTR const TAG_GAGETOOL = "Gage Tool";
	LPCSTR const TAG_PROFILETOOL = "Profile Tool";
	LPCSTR const TAG_OCROCVANALYZER = "OCR  OCV Analyzer";
	LPCSTR const TAG_OCROCVGRAYANALYZER = "OCR  OCV Gray Scale Analyzer";

	LPCTSTR const ITEM_ANALOG_CAMERA = _T("Analog Camera - Obsolete");
	LPCTSTR const ITEM_1394_CAMERA = _T("1394 Camera - Obsolete");
	LPCTSTR const ITEM_GAGE_TOOL = _T("Gage Tool - Obsolete");
	LPCTSTR const ITEM_PROFILE_TOOL = _T("Profile Tool - Obsolete");
	LPCTSTR const ITEM_FAST_OCR = _T("Fast OCR - Obsolete");
	LPCTSTR const ITEM_PRODUCTTYPE = _T("Unknown Product Type");
	#pragma endregion String Defines

	struct ObsoleteItem
	{
		std::string m_tag;
		std::string m_itemType;
		int m_errorNo;
		ObsoleteItem(const std::string& tag, const std::string& itemType, int errorNo)
		: m_tag(tag), m_itemType(itemType), m_errorNo(errorNo)
		{
		}
	};
	typedef std::deque<ObsoleteItem> ObsoleteItems;

		
		
	template< typename SVTreeType >
	HRESULT CheckObsoleteItems( SVTreeType& rTree, const unsigned long& ulSVOConfigVersion, CString& rItemType, int& errorCode )
	{
		HRESULT hr = S_OK;
		if (0x00072800 > ulSVOConfigVersion) // if 7.40 or greater no need to check
		{
			hr = HasObsoleteItem(rTree, rItemType, errorCode);
		}
		return hr;
	}
		
		
	template<typename TreeType>
	static bool HasAcquisitionDevice(TreeType& rTree, const ObsoleteItems& rItems, CString& rItemType, int& rErrorCode)
	{
		TreeType::SVBranchHandle hChild( nullptr );
		TreeType::SVBranchHandle hBoardChild( nullptr );

		bool bFound = false;
		bool bOk = SVNavigateTree::GetItemBranch( rTree, CTAG_ACQUISITION_DEVICE, nullptr, hChild );
		hBoardChild = rTree.getFirstBranch( hChild );

		while ( !bFound && bOk && nullptr != hBoardChild )
		{
			const std::string& BoardName = rTree.getBranchName( hBoardChild );
			for (ObsoleteItems::const_iterator it = rItems.begin();it != rItems.end() && !bFound; ++it)
			{
				size_t pos = BoardName.find(it->m_tag);
				if (std::string::npos != pos) 
				{
					bFound = true;
					rErrorCode = it->m_errorNo;
					rItemType = it->m_itemType.c_str();
				}
			}
			hBoardChild = rTree.getNextBranch( hChild, hBoardChild );
		}
		return bFound;
	}
		
	template<typename TreeType>
	static bool HasTaskObject(TreeType& rTree, const ObsoleteItems& rItems, CString& rItemType, int& rErrorCode)
	{
		bool bFound = false;

		TreeType::SVBranchHandle htiChild(nullptr );
		if( SVNavigateTree::GetItemBranch( rTree, CTAG_INSPECTION, nullptr, htiChild ) )
		{
			TreeType::SVBranchHandle htiSubChild( rTree.getFirstBranch( htiChild ) );
		
			while( !bFound && nullptr != htiSubChild )
			{
				TreeType::SVBranchHandle htiDataChild( rTree.getFirstBranch( htiSubChild ) );
			
				while( !bFound && nullptr != htiDataChild )
				{
					const std::string& DataName = rTree.getBranchName( htiDataChild );

					if ( 0 == DataName.compare( CTAG_SVIPDOC ) )
					{
						TreeType::SVBranchHandle htiSVIPDoc = htiDataChild;
						_variant_t value;
						if ( SVNavigateTree::GetItem( rTree, CTAG_TOOLSET_SCRIPT, htiSVIPDoc, value ) )
						{
							_bstr_t str( value );
							for (ObsoleteItems::const_iterator it = rItems.begin();it != rItems.end() && !bFound; ++it)
							{
								std::wstring alias_tag( TAG_ALIAS );
								alias_tag += TAG_SINGLE_QUOTE;
								alias_tag += SvUl::to_utf16( it->m_tag );
								alias_tag += TAG_SINGLE_QUOTE;

								if ( nullptr != wcsstr( static_cast<LPCWSTR>(str), alias_tag.c_str() ) )
								{
									bFound = true;
									rErrorCode = it->m_errorNo;
									rItemType = it->m_itemType.c_str();
								}
							}
						}
						else
						{
							for (ObsoleteItems::const_iterator it = rItems.begin();it != rItems.end() && !bFound; ++it)
							{
								TreeType::SVBranchHandle node(nullptr );
								if ( SVNavigateTree::GetItemBranch( rTree, it->m_tag.c_str(), htiSubChild, node ) )
								{
									bFound = true;
									rErrorCode = it->m_errorNo;
									rItemType = it->m_itemType.c_str();
								}
							}
						}
					}
					htiDataChild = rTree.getNextBranch( htiSubChild, htiDataChild );
				}
				htiSubChild = rTree.getNextBranch( htiChild, htiSubChild );
			}
		}
		return bFound;
	}
		
	template<typename TreeType>
	static bool HasInvalidProductType(TreeType& rTree, CString& rItemType, int& rErrorCode)
	{
		bool bInvalid = false;
		TreeType::SVBranchHandle hChild(nullptr);

		if (SVNavigateTree::GetItemBranch(rTree, CTAG_ENVIRONMENT, nullptr, hChild))
		{
			_variant_t svValue;

			if (SVNavigateTree::GetItem(rTree, CTAG_CONFIGURATION_TYPE, hChild, svValue))
			{
				int iType = svValue;
				if (!SvTi::SVHardwareManifest::IsValidProductType(static_cast<SVIMProductEnum>(iType)))
				{
					bInvalid = true;
					rErrorCode = SvOi::Err_15043_UnknownProductType;
					rItemType = ITEM_PRODUCTTYPE;
				}
			}
		}
		return bInvalid;
	}

	template<typename TreeType>
	HRESULT HasObsoleteItem(TreeType& rTree, CString& rItemType, int& rErrorCode)
	{
		ObsoleteItems cameraItems = boost::assign::list_of<>
			( ObsoleteItem(TAG_VIPER, ITEM_ANALOG_CAMERA, SvOi::Err_15038_AnalogCamera) )
			( ObsoleteItem(TAG_1394, ITEM_1394_CAMERA, SvOi::Err_15039_1394Camera) )
		;
		ObsoleteItems taskItems = boost::assign::list_of<>
			( ObsoleteItem(TAG_GAGETOOL, ITEM_GAGE_TOOL, SvOi::Err_15040_GageTool) )
			( ObsoleteItem(TAG_PROFILETOOL, ITEM_PROFILE_TOOL, SvOi::Err_15041_ProfileTool) )
			( ObsoleteItem(TAG_OCROCVANALYZER, ITEM_FAST_OCR, SvOi::Err_15042_FastOCR) )
			( ObsoleteItem(TAG_OCROCVGRAYANALYZER, ITEM_FAST_OCR, SvOi::Err_15042_FastOCR) )
		;
		HRESULT hr = S_OK;
		rErrorCode = 0;
		if (!HasAcquisitionDevice(rTree, cameraItems, rItemType, rErrorCode))
		{
			if (!HasTaskObject(rTree, taskItems, rItemType, rErrorCode))
			{
				HasInvalidProductType(rTree, rItemType, rErrorCode);
			}
		}
		if (rErrorCode)
		{
			hr = CLASS_E_CLASSNOTAVAILABLE;
		}
		return hr;
	}
} /* namespace SVXMLLibrary */ } /* namespace Seidenader */
