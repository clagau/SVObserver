//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecurityStorage.h
//* .File Name       : $Workfile:   SVSecurityStorage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:04:52  $
//******************************************************************************

#pragma once

#pragma region Includes 
//Moved to precompiled header: #include <vector>
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVAccessPointNode.h"
#pragma endregion Includes

class SVSecurityStorage
{
public:
	friend class SVSecuritySetupPage;

	SVSecurityStorage();
	virtual ~SVSecurityStorage();

	HRESULT Load(LPCTSTR FileName);
	HRESULT Add( long lID, LPCTSTR Name, LPCTSTR NTGroup = nullptr , bool ForcePrompt = false );
	HRESULT ProtectData( long lID );

	bool GetUseLogon() const;
	bool GetAutoEdit() const;
	void SetUser( LPCTSTR strUser );
	void SetPW( LPCTSTR strPW );
	const std::string& GetCurrentUser() const;
	const std::string& GetCurrentPassword() const;

	// Use these functions to setup access
	HRESULT Save(LPCTSTR pFileName);
	HRESULT SetNTGroup( long lID, LPCTSTR strGroup );
	HRESULT SetForcedPrompt( long lID, BOOL bForce );
	HRESULT SetUseAutoEdit( bool p_bUse);
	long GetUserTimeout() const;
	HRESULT SetUseLogon( bool bUse );
	HRESULT SetUserTimeout(long lTime);

	void ClearUser( );
	void ClearPW( );


	SVAccessPointNode* FindByID( long lID );


protected:
	typedef std::vector < SVAccessPointNode > SVAccessPointNodeVectorArray;

	HRESULT GetMaterialsTree( SVMaterialsTree::SVTreeContainer& rTree ) const;
	HRESULT GetChildMaterialsTree( SVMaterialsTree::SVTreeContainer& rTree, SVAccessPointNodeVectorArray::const_iterator& p_rNodeIter ) const;

	HRESULT ProcessMaterialsTree( SVAccessPointNodeVectorArray& p_rNewArray, const SVMaterialsTree::SVTreeContainer& rTree );
	HRESULT ProcessChild( SVAccessPointNodeVectorArray& p_rNewArray, const SVMaterialsTree::SVTreeContainer& rTree );

	HRESULT ProcessRoot( const SVMaterialsTree::SVTreeContainer& rTree );


	// Manage Data to tree...
	long m_lCurrentData;
	long m_lCurrentIndex;

	// Settings
	long m_lTimeout;
	bool m_bUseLogon;
	bool m_bAutoEdit;

	// The Tree Stored as an array
	SVAccessPointNodeVectorArray m_aNodes;

	std::string m_CurrentUser;
	std::string m_CurrentPW;

};

