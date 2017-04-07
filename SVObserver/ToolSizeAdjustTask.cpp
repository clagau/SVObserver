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
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "EQAdjustSize.h"
#include "SVRunControlLibrary\SVRunControlLibrary.h"
#include "SVUtilityLibrary\SVString.h"
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

ToolSizeAdjustTask::ToolSizeAdjustTask(bool AllowFullsize , bool AllowAdjustSize, bool AllowAdjustPosition , SVObjectClass* POwner , int StringResourceID )  
: SVTaskObjectClass(POwner, StringResourceID)
, m_AllowFullSize(AllowFullsize)
, m_AllowAdjustSize(AllowAdjustSize)
, m_AllowAdjustPosition(AllowAdjustPosition)
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVToolSizeAdjustTaskType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVNotSetSubObjectType;

	RegisterEmbeddedObject( &m_InputModes[TSWidth], ToolSizeAdjustSizeWidthModeGuid, IDS_OBJECTNAME_TOOLSIZEADJUSTSIZEWIDTH, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_InputModes[TSHeight], ToolSizeAdjustSizeHeightModeGuid, IDS_OBJECTNAME_TOOLSIZEADJUSTSIZEHEIGHT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_InputModes[TSPositionX], ToolSizeAdjustSizePositionXModeGuid, IDS_OBJECTNAME_TOOLSIZEADJUSTPOSITIONX, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_InputModes[TSPositionY], ToolSizeAdjustSizePositionYModeGuid, IDS_OBJECTNAME_TOOLSIZEADJUSTPOSITIONY, false, SvOi::SVResetItemNone );

	SVEnumerateVector vec;
	
	vec.push_back(SVEnumeratePair(SvO::SizeAdjustTextNone, TSModes::TSNone ));
	if (m_AllowFullSize == true)
	{
		vec.push_back(SVEnumeratePair(SvO::SizeAdjustTextFullSize, TSModes::TSFullSize ));
	}
	vec.push_back(SVEnumeratePair(SvO::SizeAdjustTextFormula, TSModes::TSFormula ) );

	for( int vType  = TSPositionX ; vType < TSValuesCount ; ++vType)
	{
		m_InputModes[vType].SetEnumTypes( vec );
		m_InputModes[vType].SetDefaultValue( TSModes::TSNone, true );
		m_InputModes[vType].SetObjectAttributesAllowed( SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );
	}

	//Add Evaluation Objects 
	BOOL ok(false);
	ok = AddEvaluationObject(&m_InObjectInfoDResult[TSWidth], EQAdjustSizeWidthGuid, _T("ToolSizeAdjustWidth"));
	ASSERT(ok);
	ok = AddEvaluationObject(&m_InObjectInfoDResult[TSHeight], EQAdjustSizeHeightGuid , _T("ToolSizeAdjustHeight"));
	ASSERT(ok);

	ok = AddEvaluationObject(&m_InObjectInfoDResult[TSPositionX], EQAdjustSizePositionXGuid, _T("ToolSizeAdjustPositionX"));
	ASSERT(ok);
	ok = AddEvaluationObject(&m_InObjectInfoDResult[TSPositionY], EQAdjustSizePositionYGuid,  _T("ToolSizeAdjustPositionY"));
	ASSERT(ok);

	addDefaultInputObjects();
}

ToolSizeAdjustTask::~ToolSizeAdjustTask()
{
}

BOOL ToolSizeAdjustTask::AddEvaluationObject(SVInObjectInfoStruct* pInfo, GUID const &GuidClass, LPCTSTR Name)
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
		return false;
	}
	EQAdjustSize* pAdjustSize  = dynamic_cast<EQAdjustSize*> (pObject);
	if( nullptr == pAdjustSize)
	{
		return false;
	}

	pInfo->SetInputObjectType( pAdjustSize->GetResultGuid(), SVDoubleValueObjectType );
	pInfo->SetObject( GetObjectInfo() );
	return RegisterInputObject( pInfo, Name );
}

bool ToolSizeAdjustTask::onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	//DoNothing in onRun
	return true;
}

bool  ToolSizeAdjustTask::Run( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	//DoNothing in Run
	return true;
}

int ToolSizeAdjustTask::GetIndex()
{
	int index = 1;
	SVToolClass* pTool = GetTool();
	if (pTool)
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(pTool->GetInspection());
		if (pInspection)
		{
			index = pInspection->GetResultDataIndex();
		}
	}
	return index;
}

HRESULT ToolSizeAdjustTask::GetModes(long &rModeWidth,long &rModeHeight,long &rModePosX, long &rModePosY)
{
	rModeWidth = rModeHeight = rModePosX = rModePosY = TSModeCount;
	HRESULT hresult =  m_InputModes[TSWidth].GetValue( rModeWidth );
	if (S_OK == hresult)
	{
		hresult =  m_InputModes[TSHeight].GetValue( rModeHeight );
	}
	if (S_OK == hresult)
	{
		hresult =  m_InputModes[TSPositionX].GetValue( rModePosX );
	}
	if (S_OK == hresult)
	{
		hresult =  m_InputModes[TSPositionY].GetValue( rModePosY );
	}

	if (S_OK == hresult)
	{

		if(( rModeWidth == TSFullSize)  ||  (rModeHeight == TSFullSize)  ||  (rModePosX == TSFullSize)  ||  (rModePosY == TSFullSize))
		{
			if( ( rModeWidth != TSFullSize)  ||   (rModeHeight != TSFullSize)  ||  (rModePosX != TSFullSize)  ||  (rModePosY != TSFullSize))
			{
				hresult = SvOi::Err_16028_InvalidMode;
			}
			if( false == m_AllowFullSize )
			{
				hresult = 	SvOi::Err_16034_InvalidMode;
			}
		}
	}
	if( S_OK == hresult )
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

HRESULT ToolSizeAdjustTask::GetResultValue( TSValues val, long& rValue) const
{
	HRESULT hresult = SvOi::Err_16030_InvalidValuePointer;
	SVDoubleValueObjectClass* pValueObject =GetDResultObjects(val);
	if (nullptr != pValueObject)
	{
		double TempValue( 0.0 );
		hresult =  pValueObject->GetValue(TempValue);
		rValue = static_cast<long> (TempValue);
	}
	if (S_OK == hresult)
	{
		if( val == TSValues::TSHeight  || val == TSValues::TSWidth )
		{
			if (rValue < MinToolSize)
			{
				hresult = SvOi::Err_16031_InvalidSize;
			}
			else if (rValue > MaxToolSize)
			{
				hresult = SvOi::Err_16032_InvalidSize;
			}
		}
		else
		{
			if (rValue < 0)
			{
				hresult = SvOi::Err_16039_NegativePosition;
			}
			else if (rValue > MaxToolSize)
			{
				hresult = SvOi::Err_16040_ToLargePosition;
			}
		}
	}
	return hresult;
}

SVToolClass* ToolSizeAdjustTask::GetTool() const 
{
	return dynamic_cast<SVToolClass*> (GetOwner());
}

bool ToolSizeAdjustTask::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	///Do nothing if we are in Run Mode 
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
	{
		return true;
	}

	bool Result = SVTaskObjectClass::ResetObject(pErrorMessages);

	EAutoSize  	 AutoSizeEnable(EnableNone);
	SVToolClass* pTool = GetTool();
	if (nullptr != pTool)
	{
		AutoSizeEnable = pTool->GetAutoSizeEnabled();
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_InvalidOwner, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	bool bAllowNegativePostion = EnableNegativePosition == (AutoSizeEnable & EnableNegativePosition); 

	long ModeWidth(TSNone), ModeHeight(TSNone), ModeX(TSNone), ModeY(TSNone);
	bool bDone = (EnableNone == AutoSizeEnable);
	HRESULT hresult = S_OK;
	if (false == bDone  && Result)
	{
		hresult = GetModes(ModeWidth, ModeHeight, ModeX, ModeY);
		Result = (S_OK == hresult);
	}
	if( Result && (TSNone == ModeWidth) && (TSNone == ModeHeight) && (TSNone == ModeX) && (TSNone == ModeY))
	{
		bDone = true;
	}
	if (!bDone && Result)
	{
		long ParentOutputWidth(0), ParentOutputHeight(0), oldWidth(0), oldHeight(0);
		long  oldPosX, oldPosY;
		SVImageExtentClass ImageExtent;
		SVImageExtentClass ParentImageExtent;

		hresult = pTool->GetImageExtent(ImageExtent);

		if (S_OK == hresult)
		{
			hresult =pTool->GetParentExtent(ParentImageExtent);
		}

		if (S_OK == hresult)
		{
			hresult =ParentImageExtent.GetExtentProperty(SVExtentPropertyOutputWidth, ParentOutputWidth);
		}
		if (S_OK == hresult)
		{
			hresult =ParentImageExtent.GetExtentProperty(SVExtentPropertyOutputHeight, ParentOutputHeight);
		}
		if (S_OK == hresult)
		{
			hresult = ImageExtent.GetExtentProperty(SVExtentPropertyWidth, oldWidth);
		}

		if (S_OK == hresult)
		{
			hresult = ImageExtent.GetExtentProperty(SVExtentPropertyHeight, oldHeight);
		}
		if (S_OK  == hresult)
		{
			hresult = ImageExtent.GetExtentProperty(SVExtentPropertyPositionPointX, oldPosX);
		}
		if (S_OK  == hresult)
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

		if ((S_OK == hresult) && (false == bDone) && (TSFormula == ModeWidth ) && (EnableSize == (AutoSizeEnable & EnableSize)))
		{
			long newWidth(0);
			hresult = GetResultValue(TSWidth, newWidth);

			if ((S_OK  == hresult) && (oldWidth != newWidth))
			{
					hresult = ImageExtent.SetExtentProperty(SVExtentPropertyWidth, newWidth);
					bSetImageExtend = true;
				}
			}
		if ((S_OK  == hresult) && (false == bDone) &&  (TSFormula == ModeHeight) &&  (EnableSize == (AutoSizeEnable & EnableSize)))
			{
			long   NewHeight(0);
			hresult = GetResultValue(TSHeight, NewHeight);

			if (S_OK  == hresult)
			{
				hresult = ImageExtent.GetExtentProperty(SVExtentPropertyHeight, oldHeight);
			}

			if ((S_OK  == hresult) && (oldHeight != NewHeight))
			{
				hresult = ImageExtent.SetExtentProperty(SVExtentPropertyHeight, NewHeight);
				bSetImageExtend = true;
			}
		}
		if ((S_OK == hresult) && (false == bDone) && (TSFormula == ModeX) && (EnablePosition == (AutoSizeEnable & EnablePosition)))
		{
			long  PosX;
			hresult =  GetResultValue(TSPositionX,PosX);
			
			if (SvOi::Err_16039_NegativePosition == hresult && bAllowNegativePostion)
			{
				hresult = S_OK;
			}

			if ((S_OK  == hresult) && (oldPosX != PosX))
			{
				hresult = ImageExtent.SetExtentProperty(SVExtentPropertyPositionPointX, PosX);
				bSetImageExtend = true;
			}
		}
		if ((S_OK == hresult) && (false == bDone) && (TSFormula == ModeY) && (EnablePosition == (AutoSizeEnable & EnablePosition)))
		{
			long  PosY;
			hresult = GetResultValue(TSPositionY, PosY);
			
			if (SvOi::Err_16039_NegativePosition == hresult && bAllowNegativePostion)
			{
				hresult = S_OK;
			}
			
			if ((S_OK == hresult) && (oldPosY != PosY))
			{
				hresult = ImageExtent.SetExtentProperty(SVExtentPropertyPositionPointY, PosY);
				bSetImageExtend = true;
			}
		}

		if ((S_OK == hresult) && bSetImageExtend && (false == bDone))
		{
			hresult = pTool->SetImageExtent(GetIndex(), ImageExtent);
		}
	
		if (S_OK == hresult)
		{
			pTool->UpdateBottomAndRight(); 
		}
	}

	if ( Result && (S_OK == hresult) )
	{
		hresult = SetExtendPropertyAutoReset();
	}

	if (S_OK != hresult)
	{
		Result = false;

		SvOi::MessageTextEnum messageId = SvOi::Tid_ErrorInReset;
		SVStringVector msgList;
		if(GetTool()->GetName())
		{
			messageId = SvOi::Tid_InValidNewExtents;
			msgList.push_back(GetTool()->GetName());
		}
		SvStl::MessageContainer message;
		message.setMessage( SVMSG_SVO_58_TOOLADJUST_RESET_ERROR, messageId, msgList, SvStl::SourceFileParams(StdMessageParams), hresult );
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		Exception.setMessage( message.getMessage() );
		if (nullptr != pErrorMessages)
		{
			pErrorMessages->push_back(message);
		}
	}

	if (!Result)
	{
		SetInvalid();
		if (GetTool())
		{
			GetTool()->SetInvalid();
		}
	}

	return Result;
}

bool ToolSizeAdjustTask::IsFullSizeAllowed() const
{
	return m_AllowFullSize;
}

bool ToolSizeAdjustTask::IsAdjustSizeAllowed() const
{
	return m_AllowAdjustSize;
}

bool ToolSizeAdjustTask::IsAdjustPositionAllowed() const
{
	return m_AllowAdjustPosition;
}

SVEnumerateValueObjectClass* ToolSizeAdjustTask::GetInputMode(TSValues mode)
{
	return &m_InputModes[mode];
}

SVDoubleValueObjectClass* ToolSizeAdjustTask::GetDResultObjects(ToolSizeAdjustTask::TSValues val) const
{
	SVDoubleValueObjectClass* pValue = nullptr;

	if( m_InObjectInfoDResult[val].IsConnected() )
	{
		pValue = dynamic_cast< SVDoubleValueObjectClass* >( m_InObjectInfoDResult[val].GetInputObjectInfo().m_pObject );
	}
	return pValue;
}

HRESULT ToolSizeAdjustTask::GetExtentValues( TSValues val, long &value) const
{
	HRESULT hresult = S_OK;
	SVToolClass* pTool = GetTool();
	if (nullptr == pTool)
	{
		ASSERT(false);
		hresult = SvOi::Err_16027_InvalidOwner;
	}

	SVImageExtentClass ImageExtent;
	if (S_OK == hresult)
	{
		hresult = pTool->GetImageExtent(ImageExtent); 
	}

	if (S_OK == hresult)
	{
		switch(val)
		{
		case TSPositionX:
			hresult = ImageExtent.GetExtentProperty( SVExtentPropertyPositionPointX, value );
			break;
		case TSPositionY:
			hresult = ImageExtent.GetExtentProperty( SVExtentPropertyPositionPointY, value );
			break;
		case TSWidth:
			hresult = ImageExtent.GetExtentProperty( SVExtentPropertyWidth, value );
			break;
		case TSHeight:
			hresult = ImageExtent.GetExtentProperty( SVExtentPropertyHeight, value );
			break;
		default:
			hresult = SvOi::Err_16037_InvalidSelection;
		}
	}
	return hresult;
}

HRESULT ToolSizeAdjustTask::GetParentExtentOutputValues( TSValues val, long &value) const
{
	HRESULT hresult = S_OK;
	SVToolClass* pTool = GetTool();
	if (nullptr == pTool)
	{
		ASSERT(false);
		hresult = SvOi::Err_16027_InvalidOwner;
	}

	SVImageExtentClass ParentImageExtent;
	if (S_OK == hresult)
	{
		hresult = pTool->GetParentExtent(ParentImageExtent); 
	}

	if (S_OK == hresult)
	{
		switch (val)
		{
		case TSPositionX:
			hresult = ParentImageExtent.GetExtentProperty( SVExtentPropertyPositionPointX, value );
			break;
		case TSPositionY:
			hresult = ParentImageExtent.GetExtentProperty( SVExtentPropertyPositionPointY, value );
			break;
		case TSWidth:
			hresult = ParentImageExtent.GetExtentProperty( SVExtentPropertyOutputWidth, value );
			break;
		case TSHeight:
			hresult = ParentImageExtent.GetExtentProperty( SVExtentPropertyOutputHeight, value );
			break;
		default:
			hresult = SvOi::Err_16037_InvalidSelection;
		}
	}
	return hresult;
}

HRESULT ToolSizeAdjustTask::SetExtendPropertyAutoReset()
{
	HRESULT hresult = S_OK;
	SVToolClass* pTool = GetTool();
	if (nullptr == pTool)
	{
		ASSERT(false);
		hresult = SvOi::Err_16027_InvalidOwner;
	}

	long ModeWidth(TSNone), ModeHeight(TSNone), ModeX(TSNone), ModeY(TSNone);
	bool bWidthSetByReset(true), bHeightSetByReset(true), bPosXSetByReset(true), bPosYSetByReset(true); 

	if (S_OK == hresult)
	{
		hresult = GetModes(ModeWidth, ModeHeight, ModeX, ModeY);
	}

	if (TSNone == ModeWidth)
	{	
		bWidthSetByReset = false;
	}
	if (TSNone == ModeHeight)
	{	
		bHeightSetByReset = false;
	}
	if (TSNone == ModeX)
	{	
		bPosXSetByReset = false;
	}
	if (TSNone == ModeY)
	{	
		bPosYSetByReset = false;
	}

	SVExtentPropertyInfoStruct info;
	if (S_OK == hresult)
	{
		hresult = pTool->GetPropertyInfo( SVExtentPropertyPositionPointX, info );
		info.bSetByReset = bPosXSetByReset;
		hresult = pTool->SetExtentPropertyInfo( SVExtentPropertyPositionPointX, info );
	}
	if (S_OK == hresult)
	{
		hresult = pTool->GetPropertyInfo( SVExtentPropertyPositionPointY, info );
		info.bSetByReset = bPosYSetByReset;
		hresult = pTool->SetExtentPropertyInfo( SVExtentPropertyPositionPointY, info );
	}

	if (S_OK == hresult)
	{
		hresult = pTool->GetPropertyInfo( SVExtentPropertyWidth, info );
		info.bSetByReset = bWidthSetByReset;
		hresult = pTool->SetExtentPropertyInfo( SVExtentPropertyWidth, info );
	}
	if (S_OK == hresult)
	{
		hresult = pTool->GetPropertyInfo( SVExtentPropertyHeight, info );
		info.bSetByReset = bHeightSetByReset;
		hresult = pTool->SetExtentPropertyInfo( SVExtentPropertyHeight, info );
	}
	return hresult;
}

ToolSizeAdjustTask* ToolSizeAdjustTask::AddToFriendlist(SVToolClass* pTool, bool AllowFullsize, bool AllowAdjustSize, bool AllowAdjustPosition)
{
	HRESULT hres(E_FAIL);
	if (nullptr == pTool)
	{
		ASSERT(false);
		return nullptr;
	}
	ToolSizeAdjustTask *pToolSizeAdjustTask = nullptr;
	//Add ToolSizeAdjustTask to friendlist
	pToolSizeAdjustTask = new ToolSizeAdjustTask(AllowFullsize, AllowAdjustSize, AllowAdjustPosition, pTool);
	if (nullptr != pToolSizeAdjustTask)
	{
		pToolSizeAdjustTask->SetObjectOwner( pTool );
		pTool->AddFriend( pToolSizeAdjustTask->GetUniqueObjectID() );
		hres = S_OK;
	}
	return pToolSizeAdjustTask;
}

HRESULT ToolSizeAdjustTask::EnsureInFriendList(SVToolClass* pTool, bool AllowFullsize, bool AllowAdjustSize, bool AllowAdjustPosition)
{
	HRESULT hres(E_FAIL);
	if (nullptr == pTool)
	{
		ASSERT(false);
		return hres;
	}
	ToolSizeAdjustTask* pToolSizeAdjustTask = GetToolSizeAdjustTask(pTool);
	
	if (nullptr == pToolSizeAdjustTask)
	{
		pToolSizeAdjustTask = AddToFriendlist(pTool, AllowFullsize, AllowAdjustSize, AllowAdjustPosition );
		if (nullptr != pToolSizeAdjustTask)
		{
			pTool->ConnectAllInputs();
			pTool->CreateChildObject(pToolSizeAdjustTask);
		}
	}
	if (nullptr != pToolSizeAdjustTask)
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
	if (nullptr != pObject)
	{
		SVObjectTypeInfoStruct ToolSizeAdjustTaskInfo;
		ToolSizeAdjustTaskInfo.ObjectType = SVToolSizeAdjustTaskType;
		ToolSizeAdjustTaskInfo.SubType = 0;
		pToolSizeAdjustTask = dynamic_cast<ToolSizeAdjustTask*>(pObject->GetFriend(ToolSizeAdjustTaskInfo));
	}
	return pToolSizeAdjustTask;
}

BOOL ToolSizeAdjustTask::UseSizeAdjust( SVObjectClass *pObject)
{
	BOOL ret = false;
	ToolSizeAdjustTask* pToolSizeAdjustTask = GetToolSizeAdjustTask(pObject);
	if (pToolSizeAdjustTask)
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
		if (TSFullSize == Mode || TSFormula == Mode)
		{
			return true;
		}
	}
	return false;
}

DWORD ToolSizeAdjustTask::GetObjectColor() const
{
	return SV_DEFAULT_WHITE_COLOR;
}