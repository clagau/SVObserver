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
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVIODoc;

class SVIOController : public SVObjectClass
{
	SV_DECLARE_CLASS( SVIOController );

public:
	SVIOController( LPCTSTR ObjectName );
	SVIOController( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIOCONTROLLER );

	virtual ~SVIOController();

	virtual void SetObjectDepth( int NewObjectDepth ) override;
	virtual void SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex ) override;
	virtual bool SetImageDepth( long lDepth ) override;

	virtual void ResetName() override;
	virtual void SetName( LPCTSTR StrString ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	void SetIODoc(SVIODoc* pDoc); 
	SVIODoc* GetIODoc() const;

	bool RebuildOutputList();

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	BOOL SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent );

	bool GetParameters( SvXml::SVObjectXMLWriter& rWriter ) const;

	HRESULT SetModuleReady( bool p_Value );
	bool SetRaidErrorBit( bool p_Value );

	SVIOEntryHostStructPtr GetModuleReady();
	SVIOEntryHostStructPtr GetRaidErrorBit();

	SVGUID GetRemoteOutputController() const;
	size_t GetRemoteOutputGroupCount() const;
	void SetupRemoteOutput( SVConfigurationObject* p_pConfig );
	HRESULT ClearRemoteOutputUnUsedData( );
	HRESULT GetRemoteOutputGroupNames( SVStringVector& rPPQs );
	SVRemoteOutputGroup* GetRemoteOutputGroup( const SVString& rRemoteGroupId ) const;
	size_t GetRemoteOutputGroupItemCount( const SVString& rRemoteGroupId ) const;
	HRESULT GetRemoteOutputItem( const SVString& rRemoteGroupId, long l_lIndex, SVRemoteOutputObject*& p_rItem ) const;
	SVRemoteOutputObject* GetFirstRemoteOutputObject( const SVString& rRemoteGroupId );
	HRESULT AddRemoteOutputItem( const SVString& rRemoteGroupId, SVRemoteOutputObject*& p_pNewOutput, GUID p_InputObjectID, const SVString& rPPQ );
	HRESULT DeleteRemoteOutput( const SVString& rRemoteGroupId );
	HRESULT DeleteRemoteOutputEntry( const SVString& rRemoteGroupId, SVRemoteOutputObject* p_pOutputObject );
	HRESULT RemoteOutputValidateInputs();

	bool SetupRemoteMonitorList(SVConfigurationObject* pConfig);
	void ClearRemoteMonitorList();
	const RemoteMonitorListMap& GetRemoteMonitorList() const;
	void SetRemoteMonitorList(const RemoteMonitorListMap& rList);

	//************************************
	// Method:    ReplaceOrAddMonitorList
	// Description:  Replace a monitor list if it already exists or add a new monitor list to the configuration.
	// Parameter: const RemoteMonitorNamedList & rList
	// Returns:   void
	//************************************
	void ReplaceOrAddMonitorList( const RemoteMonitorNamedList& rList );
	void ShowMonitorListTab();
	void ValidateRemoteMonitorList();
	//! Activate or deactivate the Monitorlist.
	//!	When the Monitorlist is activated all Other Monitorlist which belong to the same PPQ are deactivated
	//! \param listName [in]
	//! \param bActivate [in]
	//! \returns S_OK when unsuccessfully 
	HRESULT ActivateRemoteMonitorList(const SVString& rListName, bool bActivate);
	void GetActiveRemoteMonitorList(RemoteMonitorListMap& rActiveList) const;
	
	//Return the number of active Monitorlist
	int GetActiveMonitorListCount() const; 

	HRESULT SetRemoteMonitorListProductFilter(const SVString& rListName, SvSml::SVProductFilterEnum rFilter);
	HRESULT GetRemoteMonitorListProductFilter(const SVString& rListName, SvSml::SVProductFilterEnum& rFilter) const;
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


