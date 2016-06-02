//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOController
//* .File Name       : $Workfile:   SVIOController.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.12  $
//* .Check In Date   : $Date:   12 Dec 2014 13:10:18  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"

#include "SVXMLLibrary/SVXMLMaterialsTree.h"

#include "SVInfoStructs.h"
#include "SVRemoteOutputDataController.h"
#include "SVRemoteOutputObject.h"
#include "RemoteMonitorListController.h"
#pragma endregion Includes

class SVIODoc;

class SVIOController : public SVObjectClass
{
	SV_DECLARE_CLASS( SVIOController );

public:
	SVIOController( LPCSTR ObjectName );
	SVIOController( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIOCONTROLLER );

	virtual ~SVIOController();

	virtual BOOL SetObjectDepth( int NewObjectDepth );
	virtual BOOL SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex );
	virtual BOOL SetImageDepth( long lDepth );

	virtual void ResetName();
	virtual void SetName( const CString& StrString );

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	virtual HRESULT ResetObject();
	virtual BOOL OnValidate();

	void SetIODoc(SVIODoc* pDoc); 
	SVIODoc* GetIODoc() const;

	BOOL RebuildOutputList();

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	bool GetParameters( SVObjectXMLWriter& rWriter ) const;

	HRESULT SetModuleReady( bool p_Value );
	HRESULT SetRaidErrorBit( bool p_Value );

	SVIOEntryHostStructPtr GetModuleReady();
	SVIOEntryHostStructPtr GetRaidErrorBit();

	SVGUID GetRemoteOutputController() const;
	size_t GetRemoteOutputGroupCount() const;
	void SetupRemoteOutput( SVConfigurationObject* p_pConfig );
	HRESULT ClearRemoteOutputUnUsedData( );
	HRESULT GetRemoteOutputGroupNames( std::vector<CString>& p_astrPPQs );
	SVRemoteOutputGroup* GetRemoteOutputGroup( const CString& p_strRemoteGroupID ) const;
	size_t GetRemoteOutputGroupItemCount( const CString& p_strRemoteGroupID ) const;
	HRESULT GetRemoteOutputItem( const CString& p_strRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem ) const;
	SVRemoteOutputObject* GetFirstRemoteOutputObject( const CString& p_strRemoteGroupId );
	HRESULT AddRemoteOutputItem( const CString& p_strRemoteGroupId, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const CString p_strPPQ );
	HRESULT DeleteRemoteOutput( const CString& p_strRemoteGroupId );
	HRESULT DeleteRemoteOutputEntry( const CString& p_strRemoteGroupId, SVRemoteOutputObject* p_pOutputObject );
	HRESULT RemoteOutputValidateInputs();

	bool SetupRemoteMonitorList(SVConfigurationObject* pConfig);
	void ClearRemoteMonitorList();
	const RemoteMonitorList& GetRemoteMonitorList() const;
	void SetRemoteMonitorList(const RemoteMonitorList& rList);

	//************************************
	// Method:    ReplaceOrAddMonitorList
	// Description:  Replace a monitor list if it already exists or add a new monitor list to the configuration.
	// Parameter: const RemoteMonitorNamedList & rList
	// Returns:   void
	//************************************
	void ReplaceOrAddMonitorList( const RemoteMonitorNamedList& rList );
	void ShowMonitorListTab();
	void ValidateRemoteMonitorList();
	HRESULT ActivateRemoteMonitorList(const SVString& listName, bool bActivate);
	void GetActiveRemoteMonitorList(RemoteMonitorList& rActiveList) const;
	HRESULT SetRemoteMonitorListProductFilter(const SVString& listName, SvSml::SVProductFilterEnum rFilter);
	HRESULT GetRemoteMonitorListProductFilter(const SVString& listName, SvSml::SVProductFilterEnum& rFilter) const;
	void BuildPPQMonitorList(PPQMonitorList& ppqMonitorList) const;

protected:
	SVIOEntryHostStructPtr m_pModuleReady;
	SVIOEntryHostStructPtr m_pRaidErrorBit;

	SVRemoteOutputDataController* m_pRemoteOutputController;
	RemoteMonitorListController m_RemoteMonitorListController;

private:
	void LocalIntialize();
	void LocalDestroy();

	SVIODoc* m_pIODoc;
};


