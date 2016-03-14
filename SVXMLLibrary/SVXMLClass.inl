//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLClass
//* .File Name       : $Workfile:   SVXMLClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:50:20  $
//******************************************************************************

#include <assert.h>

#include "SVLibrary/SVBStr.h"		// SVBStr

#include "SVXMLLibraryGlobals.h"	// SVXMLLibraryGlobals
#include "SVXMLEncryptionClass.h"	// SVXMLEncryptionClass
#include "SVXMLSVRSchemaClass.h"	// SVXMLSVRSchemaClass
#include "SVVariantCustomTypes.h"
#include "SVXMLSafeArrayConverter.h"
#include "SVUtilityLibrary\SVUtilityGlobals.h"

template<typename SVT_TREE>
HRESULT SVXMLClass::CopyTreeToXMLFile (SVT_TREE& rTree, long alSVOCurrentVersion, BSTR abstrFormat, BSTR abstrRevisionHistory, BSTR abstrFileName)
{
	HRESULT	hr = 0;
	unsigned long ulCheckSum;
	SVXML::IXMLDOMElementPtr oDOMRootPtr;

	while (1)
	{
		if (svmlInitialized == 0)
		{
			hr = -1635;
			break;
		}

		hr = CopyTreeToDOM(rTree, alSVOCurrentVersion, abstrFormat, abstrRevisionHistory);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

   	if (svmlUseCheckSums)
		{
			hr = GetRootNode (oDOMRootPtr);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			hr = CalculateNodeCheckSums (oDOMRootPtr, &ulCheckSum, g_lXMLCheckSumCreate, NULL);

			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}

		hr = CopyDOMToXMLFile (abstrFileName);
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	}
	return hr;
}



template<typename SVT_TREE>
HRESULT SVXMLClass::CopyTreeToDOM (SVT_TREE& rTree, long alSVOCurrentVersion, BSTR abstrFormat, BSTR abstrRevisionHistory)
{
	HRESULT hr = 0;

	typename SVT_TREE::SVBranchHandle l_TreeRootHandle( nullptr );

	SVXML::IXMLDOMElementPtr oDOMRootPtr;
	SVXML::IXMLDOMElementPtr oDOMDestinationBranchPtr;

	SVBStr bstrNameSpace;

	oDOMRootPtr = NULL;

	while (1)
	{
		if (svmlInitialized == 0)
		{
			hr = -1636;
			break;
		}

//-   This clear will do DOM clear (which sets header) and set the schema.
		hr = Clear ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		bstrNameSpace = (BSTR) ((_bstr_t) "x-schema:#" + (_bstr_t) g_csSVR00001SchemaName);

		svmopDOM->SetNameSpace (bstrNameSpace);

//-	The root, which should have been created in Clear (), must already 
//-	exist prior to calling SetRevisionHistory () or AddToRevisionHistory ().
		if (svmlUseRevisionHistoryInfo)
		{
			hr = SetRevisionHistory (abstrRevisionHistory);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

			hr = AddToRevisionHistory (alSVOCurrentVersion, abstrFormat);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}

		if (m_lUseEncryption)
		{
			hr = m_opEncryption->SetEncryption ();
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
			else
			if (hr == 3)
			{
//-			Encryption branch already existed.
				hr = -1897;
				break;
			}

//-		This is just to emphisize that this value in these two classes must
//-		be the same (shadowed data).
			hr = m_opEncryption->GetIsEncrypted (&m_lIsEncrypted);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}

//-		Here is where we set the encryption method to 1.
			hr = m_opEncryption->SetEncryptionMethod (1);
			if( SEV_SUCCESS != SV_SEVERITY( hr ) )
			{
				break;
			}
		}

		l_TreeRootHandle = rTree.getRoot();
		if( nullptr == l_TreeRootHandle )
		{
			break;
		}

		hr = GetRootNode( oDOMRootPtr );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

//-	There should be a root. It should have been created in Clear ().
		if (oDOMRootPtr == NULL)
		{
			hr = -1695;
			break;
		}

 		hr = CopyTreeNodeToDOMNode( rTree, l_TreeRootHandle, oDOMDestinationBranchPtr );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = AppendChildToDOMNode( oDOMRootPtr, oDOMDestinationBranchPtr );
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}

		hr = svmopDOM->ClearNameSpace ();
		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	}
	return hr;
}


template<typename SVT_TREE>
HRESULT	SVXMLClass::CopyTreeNodeToDOMNode( SVT_TREE& rTree, const typename SVT_TREE::SVBranchHandle& alTreeNodeHandle, SVXML::IXMLDOMElementPtr& arDOMParentNodePtr )
{
	HRESULT	l_Status( S_OK );

	SVXML::IXMLDOMElementPtr oDOMNewParentElementPtr;

	if( svmlInitialized == 0 )
	{
		l_Status = -1637;
	}

	if( l_Status == S_OK )
	{
		_bstr_t bstrTreeNodeName;

		bstrTreeNodeName = rTree.getBranchName( alTreeNodeHandle ).c_str();

		if( rTree.isRoot( alTreeNodeHandle ) )
		{
			l_Status = CreateDOMNode( oDOMNewParentElementPtr, bstrTreeNodeName, VT_SVBASENODE );
		}
		else
		{
			l_Status = CreateDOMNode( oDOMNewParentElementPtr, bstrTreeNodeName ); 
		}

		if( l_Status == S_OK )
		{
			l_Status = AppendCarriageReturnToDOMNode( oDOMNewParentElementPtr );
		}
	}

	if( l_Status == S_OK )
	{
		if( rTree.hasLeaves( alTreeNodeHandle ) )
		{
			if( l_Status == S_OK )
			{
				SVT_TREE::SVLeafHandle pLeaf;

				pLeaf = rTree.getFirstLeaf( alTreeNodeHandle );

				while( rTree.isValidLeaf( alTreeNodeHandle, pLeaf ) )
				{
					SVXML::IXMLDOMElementPtr oDOMNewChildElementPtr;

					l_Status = CopyTreeDataToDOMData( rTree, pLeaf, oDOMNewChildElementPtr );

					if( l_Status == S_OK )
					{
						l_Status = AppendChildToDOMNode( oDOMNewParentElementPtr, oDOMNewChildElementPtr );

						if( l_Status == S_OK )
						{
							l_Status = AppendCarriageReturnToDOMNode( oDOMNewParentElementPtr );
						}
					}

					if( l_Status == S_OK )
					{
						pLeaf = rTree.getNextLeaf( alTreeNodeHandle, pLeaf );
					}
				}

				if( SUCCEEDED( l_Status ) )
				{
					l_Status = S_OK;
				}
			}
			else
			{
				l_Status = S_OK;
			}
		}
	}

	if( l_Status == S_OK )
	{
		if( rTree.hasBranches( alTreeNodeHandle ) )
		{
			if( l_Status == S_OK )
			{
				SVT_TREE::SVBranchHandle l_Child( nullptr );
				SVT_TREE::SVBranchHandle l_Parent( alTreeNodeHandle );

				l_Child = rTree.getFirstBranch( l_Parent );

				while( nullptr != l_Child )
				{
					SVXML::IXMLDOMElementPtr oDOMNewChildElementPtr;

					l_Status = CopyTreeNodeToDOMNode( rTree, l_Child, oDOMNewChildElementPtr );

					if( l_Status == S_OK )
					{
						l_Status = AppendChildToDOMNode( oDOMNewParentElementPtr, oDOMNewChildElementPtr );

						if( l_Status == S_OK )
						{
							l_Status = AppendCarriageReturnToDOMNode( oDOMNewParentElementPtr );
						}
					}

					if( l_Status == S_OK )
					{
						l_Child = rTree.getNextBranch( l_Parent, l_Child );
					}
				}

				if( SUCCEEDED( l_Status ) )
				{
					l_Status = S_OK;
				}
			}
			else
			{
				l_Status = S_OK;
			}
		}
	}

	if( l_Status == S_OK )
	{
		arDOMParentNodePtr = oDOMNewParentElementPtr;
	}

	return l_Status;
}

template<typename SVT_TREE>
HRESULT SVXMLClass::CopyTreeDataToDOMData (SVT_TREE& rTree, const typename SVT_TREE::SVLeafHandle& alNodeHandle, SVXML::IXMLDOMElementPtr& arDOMDataPtr)
{
	HRESULT hr = 0;

	VARIANT vTreeNodeData;

	_bstr_t TreeNodeName;

	SVXML::IXMLDOMElementPtr oDOMDataPtr;

	::VariantInit (&vTreeNodeData);

	while (1)
	{
		TreeNodeName = rTree.getLeafName( alNodeHandle ).c_str();
		if( 0 == TreeNodeName.length() )
		{
			break;
		}

		vTreeNodeData = rTree.getLeafData( alNodeHandle  );
		if( VT_EMPTY == vTreeNodeData.vt )
		{
			break;
		}

//-	If the data type is a safe array type, then the safe array will be 
//-   treated as a "node" element with each of the individual elements being 
//-	considered "data" elements.
		if (vTreeNodeData.vt & VT_ARRAY)
		{
//-		oDOMDataPtr will come back as a node element; not a data 
//-		element.
			hr = SVXMLSafeArrayConverter::CopySafeArrayToDOMNode( *this, TreeNodeName, vTreeNodeData.parray, oDOMDataPtr );
		}
		else
		{
			hr = CreateDOMData( oDOMDataPtr, TreeNodeName, &vTreeNodeData );
		}

		if( SEV_SUCCESS != SV_SEVERITY( hr ) )
		{
			break;
		}
		break;
	}

	VariantClear( &vTreeNodeData );

	if( SEV_SUCCESS != SV_SEVERITY( hr ) )
	{
	}
	else
	{
		arDOMDataPtr = oDOMDataPtr;
	}
	return hr;
}


