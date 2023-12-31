//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#include "SVIOLibrary/SVIOEntryHostStruct.h"
#include "RemoteMonitorListController.h"
#pragma endregion Includes

namespace  SvOi
{
class IObjectWriter;
}

class SVIODoc;

class SVIOController : public SVObjectClass
{
	///This class does not need to call SV_DECLARE_CLASS as it is a base class and only derived classes are instantiated
	//SV_DECLARE_CLASS

public:
	explicit SVIOController(LPCTSTR ObjectName);
	SVIOController( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIOCONTROLLER );

	virtual ~SVIOController();

	virtual void ResetName() override;
	virtual void SetName( LPCTSTR StrString ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	void SetIODoc(SVIODoc* pDoc); 
	SVIODoc* GetIODoc() const;

	void initializeOutputs();

	typedef SvXml::SVXMLMaterialsTree SVTreeType;

	HRESULT SetModuleReady( bool p_Value );

	SVIOEntryHostStructPtr GetModuleReady();

	bool SetupRemoteMonitorList(SVConfigurationObject* pConfig);
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
	HRESULT ActivateRemoteMonitorList(const std::string& rListName, bool bActivate);
	
	//Return the number of active Monitorlist
	int GetActiveMonitorListCount() const; 

	HRESULT SetRemoteMonitorListProductFilter(const std::string& rListName, SvSml::SVProductFilterEnum rFilter);
	HRESULT GetRemoteMonitorListProductFilter(const std::string& rListName, SvSml::SVProductFilterEnum& rFilter) const;
	void BuildPPQMonitorList(PPQMonitorList& ppqMonitorList) const;

private:
	void LocalIntialize();
	void LocalDestroy();

	SVIOEntryHostStructPtr m_pModuleReady;

	RemoteMonitorListController m_RemoteMonitorListController;
	SVIODoc* m_pIODoc{nullptr};
	bool m_discreteIO{false};
};


