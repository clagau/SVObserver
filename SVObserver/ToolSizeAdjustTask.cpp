//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//******************************************************************************
///Set Tool position and Size of a tool during resetObject
/// Set the tool position and size to full Image or in accordance with a formula
/// class is in friendlist  of corresponding tool 
/// the friendlist of this class holds the Svevaluationobjects 
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ToolSizeAdjustTask.h"
#include "SVGuiExtentUpdater.h"
#include "SVTool.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVInspectionProcess.h" 
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVSVIMStateClass.h"
#include "SVStatusLibrary\MessageManagerResource.h"
#include "TextDefinesSvO.h"
#include "EQAdjustSize.h"
#include "SVRunControlLibrary\SVRunControlLibrary.h"
#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma endregion Declarations


SV_IMPLEMENT_CLASS( ToolSizeAdjustTask, ToolSizeAdjustTaskGuid )


	const long ToolSizeAdjustTask::MinToolSize = 1; //< Minimum  allowed WindowSize
	const long ToolSizeAdjustTask::MaxToolSize = 100000; //< Maximum allowed WindowSize 

	ToolSizeAdjustTask::ToolSizeAdjustTask(bool AllowFullsize , bool AllowAdjustSize, bool AllowAdjustPosition , SVObjectClass* POwner , int StringResourceID )  : SVTaskObjectClass(POwner, StringResourceID),
	m_AllowFullSize(AllowFullsize), m_AllowAdjustSize(AllowAdjustSize), m_AllowAdjustPosition(AllowAdjustPosition)
{
	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolSizeAdjustTaskType;
	outObjectInfo.ObjectTypeInfo.SubType = 0;

	RegisterEmbeddedObject( & m_InputModes[TSWidth], ToolSizeAdjustSizeWidthModeGuid, IDS_OBJECTNAME_TOOLSIZEADJUSTSIZEWIDTH, false, SVResetItemNone );
	RegisterEmbeddedObject( & m_InputModes[TSHeight], ToolSizeAdjustSizeHeightModeGuid, IDS_OBJECTNAME_TOOLSIZEADJUSTSIZEHEIGHT, false, SVResetItemNone );
	RegisterEmbeddedObject( & m_InputModes[TSPositionX], ToolSizeAdjustSizePositionXModeGuid, IDS_OBJECTNAME_TOOLSIZEADJUSTPOSITIONX, false, SVResetItemNone );
	RegisterEmbeddedObject( & m_InputModes[TSPositionY], ToolSizeAdjustSizePositionYModeGuid, IDS_OBJECTNAME_TOOLSIZEADJUSTPOSITIONY, false, SVResetItemNone );


	SVEnumerateVector vec;
	
	vec.push_back(SVEnumeratePair(SvO::SizeAdjustTextNone,TSModes::TSNone ));
	if(m_AllowFullSize == true)
	{
		vec.push_back(SVEnumeratePair(SvO::SizeAdjustTextFullSize,TSModes::TSFullSize ));
	}
	vec.push_back(SVEnumeratePair(SvO::SizeAdjustTextFormula,TSModes::TSFormula ) );

	for( int vType  = TSPositionX ; vType < TSValuesCount ; ++vType)
	{
		m_InputModes[vType].SetEnumTypes( vec);
		m_InputModes[vType].SetDefaultValue(  TSModes::TSNone , TRUE );
		m_InputModes[vType].ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	}


	//Add Evaluation Objects 
	BOOL ok(FALSE);
	ok = AddEvaluationObject(&m_InObjectInfoDResult[TSWidth], EQAdjustSizeWidthGuid, _T( "ToolSizeAdjustWidth"));
	ASSERT(ok);
	ok = AddEvaluationObject(&m_InObjectInfoDResult[TSHeight], EQAdjustSizeHeightGuid , _T( "ToolSizeAdjustHeight"));
	ASSERT(ok);

	ok = AddEvaluationObject(&m_InObjectInfoDResult[TSPositionX], EQAdjustSizePositionXGuid, _T( "ToolSizeAdjustPositionX"));
	ASSERT(ok);
	ok = AddEvaluationObject(&m_InObjectInfoDResult[TSPositionY], EQAdjustSizePositionYGuid,  _T( "ToolSizeAdjustPositionY"));
	ASSERT(ok);
	m_Status = S_OK;

	addDefaultInputObjects();
}


ToolSizeAdjustTask::~ToolSizeAdjustTask()
{
	;
}


BOOL ToolSizeAdjustTask::AddEvaluationObject(SVInObjectInfoStruct* pInfo, GUID const &GuidClass,  LPCTSTR Name)
{
	SVObjectClass* pObject(nullptr);
	SVObjectManagerClass::Instance().ConstructObject( GuidClass, pObject );
	if( nullptr != pObject )
	{
		pObject->SetObjectOwner( this );
		AddFriend( pObject->GetUniqueObjectID() );
	}
	else
	{
		return FALSE;
	}
	EQAdjustSize* pAdjustSize  = dynamic_cast<EQAdjustSize*> (pObject);
	if( nullptr == pAdjustSize)
	{
		return FALSE;
	}


	pInfo->SetInputObjectType( pAdjustSize->GetResultGuid(), SVDoubleValueObjectType );
	pInfo->SetObject( GetObjectInfo() );
	return RegisterInputObject( pInfo, Name );

}




BOOL ToolSizeAdjustTask::onRun(SVRunStatusClass& RRunStatus)
{
	//DoNothing in onRun
	return TRUE;

}

BOOL  ToolSizeAdjustTask::Run( SVRunStatusClass& RRunStatus )
{
	//DoNothing in Run
	return TRUE;
}



BOOL ToolSizeAdjustTask::OnValidate() 
{
	
	BOOL ret = (S_OK ==  m_Status);
	ret = ret && SVTaskObjectClass::OnValidate();

	if(FALSE == ret)
	{
		SetInvalid();
	}
	return ret;
}

int ToolSizeAdjustTask::GetIndex()
{
	int index = 1;
	if(GetTool() &&GetTool()->GetInspection())
	{
		index = GetTool()->GetInspection()->GetResultDataIndex();

	}
	return index;
};

HRESULT ToolSizeAdjustTask::GetModes(long &rModeWidth,long &rModeHeight,long &rModePosX, long &rModePosY  )
{
	rModeWidth = rModeHeight = rModePosX = rModePosY = TSModeCount;
	HRESULT hresult =  m_InputModes[TSWidth].GetValue( rModeWidth );
	if(S_OK  == hresult)
	{
		hresult =  m_InputModes[TSHeight].GetValue( rModeHeight );
	}
	if(S_OK  == hresult)
	{
		hresult =  m_InputModes[TSPositionX].GetValue( rModePosX );
	}
	if(S_OK  == hresult)
	{
		hresult =  m_InputModes[TSPositionY].GetValue( rModePosY );
	}


	if( S_OK  == hresult)
	{

		if(( rModeWidth == TSFullSize)  ||  (rModeHeight == TSFullSize)  ||  (rModePosX == TSFullSize)  ||  (rModePosY == TSFullSize))
		{
			if( ( rModeWidth != TSFullSize)  ||   (rModeHeight != TSFullSize)  ||  (rModePosX != TSFullSize)  ||  (rModePosY != TSFullSize))
			{
				hresult = SvOi::Err_16028_InvalidMode;
			}
			if(m_AllowFullSize == false)
			{
				hresult = 	SvOi::Err_16034_InvalidMode;
			}
		}
	}
	if( S_OK  == hresult)
	{
		if( ((rModeWidth == TSFormula) ||  (rModeHeight == TSFormula) ) && false == m_AllowAdjustSize)
		{
			hresult = 	SvOi::Err_16035_InvalidMode;
		}
		if( ((rModePosX == TSFormula) ||  (rModePosY == TSFormula) ) && false== m_AllowAdjustPosition)
		{
			hresult = 	SvOi::Err_16036_InvalidMode;
		}

	}


	return hresult;
}

HRESULT ToolSizeAdjustTask::GetResultValue( TSValues val, long &value) const
{
	HRESULT hresult = SvOi::Err_16030_InvalidValuePointer;
	SVDoubleValueObjectClass* pValueObject =GetDResultObjects(val);
	if(nullptr !=  pValueObject)
	{
		hresult =  pValueObject->GetValue(value);

	}
	if(S_OK == hresult)
	{
		if( val == TSValues::TSHeight  || val == TSValues::TSWidth )
		{
			if(value  < MinToolSize )
			{
				hresult = SvOi::Err_16031_InvalidSize;
			}
			else if (value > MaxToolSize)
			{
				hresult = SvOi::Err_16032_InvalidSize;
			}
		}
		else
		{

			if (value < 0)
			{
				hresult = SvOi::Err_16039_NegativePosition;
			}
			else if (value > MaxToolSize)
			{
				hresult = SvOi::Err_16040_ToLargePosition;
			}
		}
	}
	return hresult;

}

SVToolClass*  ToolSizeAdjustTask::GetTool() const 
{
	return dynamic_cast<SVToolClass* > (GetOwner());
}

HRESULT ToolSizeAdjustTask::ResetObject()
{
	///Do nothing if we are in Run Mode 
	if(SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		return S_OK;
	}
	m_Status = S_OK;

	SVToolClass* pTool(nullptr);
	HRESULT hresult = SVTaskObjectClass::ResetObject();
	EAutoSize  	 AutoSizeEnable(EnableNone);
	if(S_OK  == hresult)
	{
		pTool = GetTool();
	}
	if ( pTool == nullptr)
	{
		hresult = SvOi::Err_16027_InvalidOwner;
		
	}
	else
	{
		AutoSizeEnable = pTool->GetAutoSizeEnabled();
	}

	bool bAllowNegativePostion = (AutoSizeEnable &  EnableNegativePosition) ==  EnableNegativePosition ; 

	long ModeWidth(TSNone), ModeHeight(TSNone), ModeX(TSNone), ModeY(TSNone);
	bool bDone = false;


	if(S_OK  == hresult)
	{
		bDone =  (AutoSizeEnable == 0) ;
	}
	if(false == bDone  && S_OK  == hresult)
	{
		hresult =  GetModes(ModeWidth, ModeHeight, ModeX, ModeY);
	}
	if( (S_OK  == hresult) && (ModeWidth == TSNone)&& (ModeHeight == TSNone)&& (ModeX==TSNone)&& (ModeY == TSNone))
	{
		bDone = true;
	}
	if(!bDone && S_OK  == hresult)
	{

		long ParentOutputWidth(0), ParentOutputHeight(0), oldWidth(0), oldHeight(0);
		long  oldPosX,oldPosY;
		SVImageExtentClass ImageExtent;
		SVImageExtentClass ParentImageExtent;

		if( S_OK  == hresult)
		{
			hresult = pTool->GetImageExtent(ImageExtent);
		}

		if( S_OK  == hresult)
		{
			hresult =pTool->GetParentExtent(ParentImageExtent);

		}

		if( S_OK  == hresult)
		{
			hresult =ParentImageExtent.GetExtentProperty(SVExtentPropertyOutputWidth, ParentOutputWidth);
		}
		if( S_OK  == hresult)
		{
			hresult =ParentImageExtent.GetExtentProperty(SVExtentPropertyOutputHeight, ParentOutputHeight);
		}
		if( S_OK  == hresult)
		{
			hresult = ImageExtent.GetExtentProperty(SVExtentPropertyWidth, oldWidth);
		}

		if(S_OK  == hresult)
		{
			hresult = ImageExtent.GetExtentProperty(SVExtentPropertyHeight, oldHeight);

		}
		if(S_OK  == hresult)
		{
			hresult = ImageExtent.GetExtentProperty(SVExtentPropertyPositionPointX, oldPosX);
		}
		if(S_OK  == hresult)
		{
			hresult = ImageExtent.GetExtentProperty(SVExtentPropertyPositionPointY, oldPosY);
		}



		bool bSetImageExtend(false);
		
		if(( S_OK  == hresult) && (ModeWidth == TSFullSize) &&  (AutoSizeEnable == EnableSizeAndPosition))
		{

			/////Set To Full size (Image Extent = parent Extent)
			if( ( S_OK  == hresult) && (oldHeight != ParentOutputHeight || oldWidth != ParentOutputWidth || oldPosY != 0 || oldPosX != 0  ))
			{
				hresult = pTool->SetImageExtentToParent(GetIndex()); 
			}
			bDone = true;
		}

		if((S_OK  == hresult) && (false==bDone) &&  (ModeWidth == TSFormula) &&  ((AutoSizeEnable & EnableSize)== EnableSize ))
		{


			long newWidth(0);
			hresult = GetResultValue(TSWidth, newWidth);


			if((S_OK  == hresult) && (oldWidth != newWidth))
			{
				hresult = ImageExtent.SetExtentProperty(SVExtentPropertyWidth, newWidth);
				bSetImageExtend = true;
			}
		}
		if((S_OK  == hresult) && (false==bDone) &&  (ModeHeight == TSFormula) &&  ((AutoSizeEnable & EnableSize)== EnableSize))
		{
			long   NewHeight(0) ;
			hresult = GetResultValue(TSHeight, NewHeight);

			if(S_OK  == hresult)
			{
				hresult = 	ImageExtent.GetExtentProperty(SVExtentPropertyHeight, oldHeight);
			}


			if((S_OK  == hresult) && (oldHeight != NewHeight))
			{
				hresult = ImageExtent.SetExtentProperty(SVExtentPropertyHeight, NewHeight);
				bSetImageExtend = true;
			}
		}
		if((S_OK  == hresult) && (false==bDone) &&  (ModeX == TSFormula) &&  ((AutoSizeEnable & EnablePosition) == EnablePosition ))
		{
			long  PosX ;
			hresult =  GetResultValue(TSPositionX,PosX);
			
			if(SvOi::Err_16039_NegativePosition == hresult && bAllowNegativePostion)
			{
				hresult = S_OK;
			}



			if((S_OK  == hresult  ) && (oldPosX != PosX))
			{
				hresult = ImageExtent.SetExtentProperty(SVExtentPropertyPositionPointX, PosX);
				bSetImageExtend = true;
			}
		}
		if((S_OK  == hresult) && (false==bDone) &&  (ModeY == TSFormula) &&  ((AutoSizeEnable & EnablePosition) == EnablePosition)   )
		{
			long  PosY ;
			hresult =  GetResultValue(TSPositionY,PosY);
			
			if(SvOi::Err_16039_NegativePosition == hresult && bAllowNegativePostion)
			{
				hresult = S_OK;
			}
			
			
			if((S_OK  == hresult) && (oldPosY != PosY))
			{
				hresult = ImageExtent.SetExtentProperty(SVExtentPropertyPositionPointY, PosY);
				bSetImageExtend = true;
			}
		}

		if((S_OK  == hresult) && bSetImageExtend && (false==bDone))
		{
			hresult =pTool->SetImageExtent(GetIndex(),ImageExtent);
		}
	
		if(S_OK  == hresult)
		{
			pTool->UpdateBottomAndRight(); 
		}

}

	if(hresult == S_FALSE || hresult == E_FAIL)
	{	
		hresult = SvOi::Err_16033_SetExtensionFailed;
	}

	if((S_OK  == hresult) )
	{
		hresult =SetExtendPropertyAutoReset();
	}

	m_Status = hresult ; 
	return hresult;
}








DWORD_PTR	ToolSizeAdjustTask::ProcessResetAllObject( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext ) 
{
	DWORD_PTR DwResult = SVMR_SUCCESS;
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	if(dwPureMessageID == SVMSGID_RESET_ALL_OBJECTS)
		{
		DwResult = SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext );
			HRESULT ResetStatus = ResetObject();
			if( ResetStatus != S_OK )
			{
				SetInvalid();
				if(GetTool())
				{
					GetTool()->SetInvalid();
				}
				BOOL SilentReset = static_cast<BOOL> (DwMessageValue);
				SvOi::MessageTextEnum messageId = SvOi::Tid_ErrorInReset;
				SVStringArray msgList;
				if(GetTool()->GetName())
				{
					messageId = SvOi::Tid_InValidNewExtents;
					msgList.push_back(GetTool()->GetName());
				}
				SvStl::MsgTypeEnum mode = SilentReset ? SvStl::LogOnly :  SvStl::LogAndDisplay;
				SvStl::MessageMgrDisplayAndNotify Exception(mode);
				Exception.setMessage( SVMSG_SVO_58_TOOLADJUST_RESET_ERROR, messageId, msgList, StdMessageParams, ResetStatus );
				DwResult = SVMR_NO_SUCCESS | DwResult;
			}
			else
			{
				DwResult = SVMR_SUCCESS| DwResult;
			}
		
	}
	return  DwResult  ;
		}


DWORD_PTR	ToolSizeAdjustTask::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext ) 
{

	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	if(dwPureMessageID == SVMSGID_RESET_ALL_OBJECTS)
	{
		return SVMR_SUCCESS;
	}
	else
	{
		return  SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext );
	}
}




SVDoubleValueObjectClass* ToolSizeAdjustTask::GetDResultObjects(ToolSizeAdjustTask::TSValues val) const
{
	SVDoubleValueObjectClass* pValue = nullptr;

	if( m_InObjectInfoDResult[val].IsConnected() )
	{
		pValue = dynamic_cast< SVDoubleValueObjectClass* >(  m_InObjectInfoDResult[val].GetInputObjectInfo().PObject );
	}
	return pValue;
}

HRESULT ToolSizeAdjustTask::GetExtentValues( TSValues val, long &value) const
{
	HRESULT hresult = S_OK;
	SVToolClass*  pTool =  GetTool();
	if(nullptr == pTool  )
	{
		ASSERT( FALSE);
		hresult = SvOi::Err_16027_InvalidOwner;
	}

	SVImageExtentClass ImageExtent;
	if(hresult == S_OK)
	{
		hresult = pTool->GetImageExtent(ImageExtent); 
	}

	if(hresult == S_OK)
	{

		switch(val)
		{
		case TSPositionX:
			hresult = ImageExtent.GetExtentProperty( SVExtentPropertyPositionPointX,value );
			break;
		case TSPositionY:
			hresult = ImageExtent.GetExtentProperty( SVExtentPropertyPositionPointY,value );
			break;
		case TSWidth:
			hresult = ImageExtent.GetExtentProperty( SVExtentPropertyWidth,value );
			break;

		case TSHeight:
			hresult = ImageExtent.GetExtentProperty( SVExtentPropertyHeight,value );
			break;
		default :
			hresult = SvOi::Err_16037_InvalidSelection;

		}


	}

	return hresult;
}

HRESULT ToolSizeAdjustTask::GetParentExtentOutputValues( TSValues val, long &value) const
{
	HRESULT hresult = S_OK;
	SVToolClass*  pTool =  GetTool();
	if(nullptr == pTool  )
	{
		ASSERT( FALSE);
		hresult = SvOi::Err_16027_InvalidOwner;
	}

	SVImageExtentClass ParentImageExtent;
	if(hresult == S_OK)
	{
		hresult = pTool->GetParentExtent(ParentImageExtent); 
	}

	if(hresult == S_OK)
	{
		switch(val)
		{
		case TSPositionX:
			hresult = ParentImageExtent.GetExtentProperty( SVExtentPropertyPositionPointX,value );
			break;
		case TSPositionY:
			hresult = ParentImageExtent.GetExtentProperty( SVExtentPropertyPositionPointY,value );
			break;
		case TSWidth:
			hresult = ParentImageExtent.GetExtentProperty( SVExtentPropertyOutputWidth,value );
			break;

		case TSHeight:
			hresult = ParentImageExtent.GetExtentProperty( SVExtentPropertyOutputHeight,value );
			break;
		default :
			hresult = SvOi::Err_16037_InvalidSelection;

		}


	}

	return hresult;
}



HRESULT ToolSizeAdjustTask::SetExtendPropertyAutoReset()
{

	HRESULT hresult = S_OK;
	SVToolClass*  pTool =  GetTool();
	if(nullptr == pTool  )
	{
		ASSERT( FALSE);
		hresult = SvOi::Err_16027_InvalidOwner;
	}

	long ModeWidth(TSNone), ModeHeight(TSNone), ModeX(TSNone), ModeY(TSNone);
	bool bWidthSetByReset(true),bHeightSetByReset(true),bPosXSetByReset(true),bPosYSetByReset(true); 

	if(S_OK  == hresult)
	{
		hresult =  GetModes(ModeWidth, ModeHeight, ModeX, ModeY);
	}

	if(ModeWidth == TSNone )
	{	
		bWidthSetByReset = false;
	}
	if(ModeHeight == TSNone )
	{	
		bHeightSetByReset = false;
	}
	if(ModeX == TSNone )
	{	
		bPosXSetByReset = false;
	}
	if(ModeY == TSNone )
	{	
		bPosYSetByReset = false;
	}


	SVExtentPropertyInfoStruct info;
	if(S_OK  == hresult)
	{
		hresult = pTool->GetPropertyInfo( SVExtentPropertyPositionPointX, info );
		info.bSetByReset = bPosXSetByReset;
		hresult = pTool->SetExtentPropertyInfo( SVExtentPropertyPositionPointX, info );
	}
	if(S_OK  == hresult)
	{
		hresult = pTool->GetPropertyInfo( SVExtentPropertyPositionPointY, info );
		info.bSetByReset = bPosYSetByReset;
		hresult = pTool->SetExtentPropertyInfo( SVExtentPropertyPositionPointY, info );
	}

	if(S_OK  == hresult)
	{
		hresult = pTool->GetPropertyInfo( SVExtentPropertyWidth, info );
		info.bSetByReset = bWidthSetByReset;
		hresult = pTool->SetExtentPropertyInfo( SVExtentPropertyWidth, info );
	}
	if(S_OK  == hresult)
	{
		hresult = pTool->GetPropertyInfo( SVExtentPropertyHeight, info );
		info.bSetByReset = bHeightSetByReset;
		hresult = pTool->SetExtentPropertyInfo( SVExtentPropertyHeight, info );
	}
	return hresult;
}


ToolSizeAdjustTask*  ToolSizeAdjustTask::AddToFriendlist(SVToolClass *pTool, bool AllowFullsize , bool AllowAdjustSize , bool AllowAdjustPosition )
{
	HRESULT hres(E_FAIL);
	if(nullptr == pTool)
	{
		ASSERT(FALSE);
		return nullptr;
	}
	ToolSizeAdjustTask *pToolSizeAdjustTask = nullptr;
	//Add ToolSizeAdjustTask to friendlist
	pToolSizeAdjustTask = new ToolSizeAdjustTask(AllowFullsize,AllowAdjustSize,AllowAdjustPosition,pTool);
	if(nullptr != pToolSizeAdjustTask)
	{
		//Add(pToolSizeAdjustTask);
		pToolSizeAdjustTask->SetObjectOwner( pTool );
		pTool->AddFriend( pToolSizeAdjustTask->GetUniqueObjectID() );
		hres = S_OK;
	}

	return pToolSizeAdjustTask;
}


HRESULT ToolSizeAdjustTask::EnsureInFriendList(SVToolClass *pTool, bool AllowFullsize , bool AllowAdjustSize , bool AllowAdjustPosition )
{
	HRESULT hres(E_FAIL);
	if(nullptr == pTool)
	{
		ASSERT(FALSE);
		return hres;
	}
	ToolSizeAdjustTask* pToolSizeAdjustTask = nullptr; 
	pToolSizeAdjustTask  = GetToolSizeAdjustTask(pTool);
	
	if(nullptr == pToolSizeAdjustTask)
	{
		pToolSizeAdjustTask = AddToFriendlist(pTool, AllowFullsize , AllowAdjustSize , AllowAdjustPosition );
		if(nullptr != pToolSizeAdjustTask)
		{
			::SVSendMessage(pTool, SVM_CONNECT_ALL_INPUTS, reinterpret_cast<DWORD_PTR>(pToolSizeAdjustTask), NULL);
			::SVSendMessage(pTool, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pToolSizeAdjustTask), NULL);
		}
	}
	if(nullptr != pToolSizeAdjustTask)
	{
		pToolSizeAdjustTask->m_AllowAdjustPosition = AllowAdjustPosition;
		pToolSizeAdjustTask->m_AllowAdjustSize = AllowAdjustSize;
		pToolSizeAdjustTask->m_AllowFullSize = AllowFullsize;
		hres = pToolSizeAdjustTask->SetExtendPropertyAutoReset();
	}
	return hres;
}


ToolSizeAdjustTask* ToolSizeAdjustTask::GetToolSizeAdjustTask(SVObjectClass *pObject)
{
	ToolSizeAdjustTask* pToolSizeAdjustTask = nullptr;
	if(nullptr != pObject)
	{
		SVObjectTypeInfoStruct ToolSizeAdjustTaskInfo;
		ToolSizeAdjustTaskInfo.ObjectType = SVToolSizeAdjustTaskType;
		ToolSizeAdjustTaskInfo.SubType = 0;
		pToolSizeAdjustTask = dynamic_cast<ToolSizeAdjustTask*>(pObject->GetFriend( ToolSizeAdjustTaskInfo));
	}
	return pToolSizeAdjustTask;
}


BOOL ToolSizeAdjustTask::UseSizeAdjust( SVObjectClass *pObject)
{

	BOOL ret = FALSE;
	ToolSizeAdjustTask* pToolSizeAdjustTask =  GetToolSizeAdjustTask(pObject);
	if(pToolSizeAdjustTask)
	{
		ret = pToolSizeAdjustTask->UseAutoMode();
	}

	return ret;

}


BOOL ToolSizeAdjustTask::UseAutoMode() const
{
	long Mode(TSModeCount);
	for( int vType  = TSPositionX ; vType < TSValuesCount ; ++vType)
	{
		HRESULT hresult =  m_InputModes[vType].GetValue( Mode );
		if(Mode == TSFullSize || Mode == TSFormula)
		{
			return TRUE;
		}
	}
	return false;
}


DWORD ToolSizeAdjustTask::GetObjectColor() const
{
	return SV_DEFAULT_WHITE_COLOR;
}