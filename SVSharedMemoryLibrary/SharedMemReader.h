//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SharedMemReader.h 
/// All Rights Reserved 
//*****************************************************************************
///// Class encapsulate function to open  and read from the  shared Memory
//******************************************************************************
#pragma once
#pragma region Includes
#include "MLProduct.h"
#include "MonitorListStore.h"
#include "MLCpyContainer.h"
#include "Definitions/StringTypeDef.h"
#include "ObjectInterfaces/ITriggerRecordControllerR.h"
#include "SVProtoBuf/SVRC.h"
#pragma endregion Includes

namespace SvSml
{

class SharedMemReader
{
public:
	///return values for member functions 
	enum retvalues
	{
		fail = 0,
		success,
		last
	};
	SharedMemReader(void);
	~SharedMemReader(void);

	///Function call to m_MLContainer
	int GetAllActiveMonitorlistName(SvDef::StringVector& monotorlistnames) const
	{
		return m_MLContainer.GetAllActiveMonitorlistName(monotorlistnames);
	}
	bool IsActiveMonitorList(const std::string& Monitorlistname) const
	{
		return m_MLContainer.IsActiveMonitorList(Monitorlistname);
	}

	bool QueryListName(const SvPb::QueryListNameRequest& req, SvPb::QueryListNameResponse& resp, SvPenv::Error& err) const
	{
		return m_MLContainer.QueryListName(req, resp, err);
	}

	bool QueryListItem(const SvPb::QueryListItemRequest& req, SvPb::QueryListItemResponse& resp, SvPenv::Error& err) const
	{
		return m_MLContainer.QueryListItem(req, resp, err);
	}

	DWORD GetVersion() const
	{
		return m_MLContainer.GetVersion();
	}

	///Reload MonitorMap, PPQReader, Open Imagestores and create the imageBuffer 
	///the version number is saved in MLCpyContainer
	void Reload(DWORD version);

	///close Connection to data container clear ML_Container 
	void Clear();

	///Fills MLProduct for Monitorlistname and Trigger, last product for Trigger = -1 returns sucess if successful
	/// the readerslot is released if releaseslot = true;
	// if pLastProduct has the same trigger as rProduct last is returned 
	retvalues GetRejectData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, bool releaseslot);
	///Fills MLProduct for Monitorlistname and Trigger, last product for Trigger = -1 returns sucess if successful
	/// the readerslot is released if releaseslot = true;
	// if pLastProduct has the same trigger as rProduct last is returned 
	retvalues GetProductData(LPCTSTR Monitorlist, int TriggerNumber, MLProduct* pProduct, bool  releaseslot);

	///Fills Failstatus  for Monitorlistname  returns sucess if successful
	///if pLastFailstatus has the same trigger as pLastFailstatus would have last is returned
	retvalues GetFailstatus(LPCTSTR Monitorlist, vecpProd* pFailstatus, vecpProd*  pLastFailstatus);

	SVMatroxBuffer GetImageBuffer(int triggerRecordId, int inspectionId, int imageIndex);

	//private:
	MonitorListStore      m_MonitorListStore;
	MLCpyContainer m_MLContainer;  //Container holds MonitorlistInformation  
private:
	

	///parameter for GetProduct
	struct GetProdPar
	{
		bool reject {false};    //< if trigger == -1 the the latest trigger is assumed.
		bool releaseTrigger {false};  //< release trigger immediately 

	};

	SharedMemReader::retvalues fillTrcInFailstatusList(LPCTSTR Monitorlist, vecpProd& rFailstatus);

	SharedMemReader::retvalues fillProductWithTR(int trigger, SvOi::ITriggerRecordControllerR& rTRC, const MonitorListCpy& rML, MLProduct& rProduct, bool firstAlreadySet = false);
	SharedMemReader::retvalues fillProductWithLastTR(SvOi::ITriggerRecordControllerR& rTRC, const MonitorListCpy& rML, MLProduct& rProduct);
	//************************************
	// Parameter: int trigger  -1 for last trigger or reject
	// return last when recent trigger == PreviousTrigger
	//************************************
	//retvalues GetProduct(const GetProdPar& par,LPCSTR monitorlist, int Trigger, int PreviousTrigger, MLProduct* pProduct);
	retvalues _GetProduct(const GetProdPar& par, LPCTSTR Monitorlist, int trigger, MLProduct* pProduct);

};
} //namespace SvSml
