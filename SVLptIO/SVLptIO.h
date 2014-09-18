//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptIO
//* .File Name       : $Workfile:   SVLptIO.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Sep 2014 15:57:34  $
//******************************************************************************

#pragma once
#include <boost/function.hpp>

typedef boost::function<void()> IrqHandler;

class SVLptIO
{
#pragma region public
public:
	SVLptIO();
	virtual ~SVLptIO();

	 HRESULT GetDataPort(UCHAR& value) const;
	 HRESULT SetDataPort(UCHAR value);

	 HRESULT GetStatusPort(UCHAR& value) const;

	 HRESULT GetControlPort(UCHAR& value) const;
	 HRESULT SetControlPort(UCHAR value);

	 HRESULT IsBidirectionalSupported(bool& biDirectional) const;

	 HRESULT ActivateReadMode();
	 HRESULT ActivateWriteMode();

	 bool IsActive() const;
	 HRESULT SetActive(bool bActivate);

	 HRESULT DisableAckInterrupt();
	 HRESULT EnableAckInterrupt(IrqHandler func);

#pragma endregion public

#pragma region private
private:
	void Destroy();
	static void OnLPTInterrupt();
#pragma endregion private
 };

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLptIO\SVLptIO.h_v  $
 * 
 *    Rev 1.0   17 Sep 2014 15:57:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  943
 * SCR Title:  Parallel Port I/O for 64 Bit and Windows 7
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
