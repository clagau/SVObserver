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

#ifndef SVINSPECTIONPROCESSTRANSACTIONS_H
#define SVINSPECTIONPROCESSTRANSACTIONS_H

enum SVInspectionMessageEnum
{
	SVInspectionMessageSetCHProperties,
	SVInspectionMessageGetCHProperties,
	SVInspectionMessageSetCHList,
	SVInspectionMessageGetCHList,
	SVInspectionMessageGetCHandClear,
	SVInspectionMessageGetCHMostRecent,
	SVInspectionMessageResetCH,
};

struct SVInspectionMessageDataStruct
{
	bool bProcessed;

	SVInspectionMessageDataStruct() : bProcessed(false) {}
	virtual ~SVInspectionMessageDataStruct() {}	// this struct is meant to be a base
};

struct SVInspectionMessageDataStruct_CHProperties : public SVInspectionMessageDataStruct
{
	SVScalarValueVectorType* pvecProperties;
	bool bResetObject;

	SVInspectionMessageDataStruct_CHProperties(SVScalarValueVectorType* p_pvecProperties, bool p_bResetObject)
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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVInspectionProcessTransactions.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:03:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 11:07:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Jan 2006 08:54:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Process Count to Conditional History
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:20:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
