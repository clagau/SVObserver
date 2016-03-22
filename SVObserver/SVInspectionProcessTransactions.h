//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionProcessTransactions
//* .File Name       : $Workfile:   SVInspectionProcessTransactions.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:03:06  $
//******************************************************************************

#pragma once

enum SVInspectionMessageEnum
{
	SVInspectionMessageSetCHProperties,
	SVInspectionMessageGetCHProperties,
	SVInspectionMessageSetCHList,
	SVInspectionMessageGetCHList,
	SVInspectionMessageGetCHandClear,
	SVInspectionMessageGetCHMostRecent,
};

struct SVInspectionMessageDataStruct
{
	bool bProcessed;

	SVInspectionMessageDataStruct() : bProcessed(false) {}
	virtual ~SVInspectionMessageDataStruct() {}	// this struct is meant to be a base
};

struct SVInspectionMessageDataStruct_CHProperties : public SVInspectionMessageDataStruct
{
	SVScalarValueVector* pvecProperties;
	bool bResetObject;

	SVInspectionMessageDataStruct_CHProperties(SVScalarValueVector* p_pvecProperties, bool p_bResetObject)
		: pvecProperties(p_pvecProperties), bResetObject(p_bResetObject) {}
};

struct SVInspectionMessageDataStruct_CHReset : public SVInspectionMessageDataStruct
{
	SVInspectionMessageDataStruct_CHReset() {}
};

struct SVInspectionMessageDataStruct_CHList : public SVInspectionMessageDataStruct
{
	std::vector <SVScalarValue>* pvecData;
	std::vector <SVScalarValue>* pvecImages;
	std::vector <SVScalarValue>* pvecConditionals;
	bool bResetObject;

	SVInspectionMessageDataStruct_CHList(std::vector <SVScalarValue>* p_pvecData, std::vector <SVScalarValue>* p_pvecImages, std::vector <SVScalarValue>* p_pvecConditionals, bool p_bResetObject )
		: pvecData(p_pvecData), pvecImages(p_pvecImages), pvecConditionals(p_pvecConditionals), bResetObject(p_bResetObject) {}
};
struct SVInspectionMessageDataStruct_CHGet : public SVInspectionMessageDataStruct
{
	std::vector <SVScalarValue>* pvecData;
	std::vector <SVImageBufferStruct>* pvecImages;
	std::vector <SVScalarValue>* pvecConditionals;
	long* plProcessCount;

	SVInspectionMessageDataStruct_CHGet(std::vector <SVScalarValue>* p_pvecData, std::vector <SVImageBufferStruct>* p_pvecImages, std::vector <SVScalarValue>* p_pvecConditionals, long* p_plProcessCount )
		: pvecData(p_pvecData), pvecImages(p_pvecImages), pvecConditionals(p_pvecConditionals), plProcessCount(p_plProcessCount) {}
};
struct SVInspectionMessageDataStruct_CHGetAll : public SVInspectionMessageDataStruct
{
	std::vector < std::vector <SVScalarValue> >* pvecData;
	std::vector < std::vector <SVImageBufferStruct> >* pvecImages;
	std::vector < std::vector <SVScalarValue> >* pvecConditionals;
	std::vector < long >* pvecProcessCount;

	SVInspectionMessageDataStruct_CHGetAll(std::vector < std::vector <SVScalarValue> >* p_pvecData, std::vector < std::vector <SVImageBufferStruct> >* p_pvecImages, std::vector < std::vector <SVScalarValue> >* p_pvecConditionals, std::vector<long>* p_pvecProcessCount )
		: pvecData(p_pvecData), pvecImages(p_pvecImages), pvecConditionals(p_pvecConditionals), pvecProcessCount(p_pvecProcessCount) {}
};

struct SVInspectionTransactionStruct
{
	DWORD dwMessage;
	//Loki::Functor<> callback
	HANDLE hCompletionEvent;
	SVInspectionMessageDataStruct* pData;

	SVInspectionTransactionStruct() : dwMessage(0), hCompletionEvent(NULL), pData(NULL) {}
	SVInspectionTransactionStruct(DWORD p_dwMessage, HANDLE p_hCompletionEvent, SVInspectionMessageDataStruct* p_pData) : dwMessage(p_dwMessage), hCompletionEvent(p_hCompletionEvent), pData(p_pData) {}
};


