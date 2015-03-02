///******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// This file defines defines for the objects. (located before in SVObjectLibrary)
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

//@HACK [MZA] This namespace is disabled because the defines are used in a lot of files and adding a namespace would required to change them.
//namespace Seidenader
//{
//	namespace ObjectInterfaces
//	{	
		// Use the RTTI 
#define SV_IS_KIND_OF( XP, XCLASSNAME )	\
	( ( XP != NULL ) && ( dynamic_cast< XCLASSNAME* >( XP ) != NULL ) )

enum SVMessageFlags
{
	SVMFUnknown = 0x00000000,
	SVMFSetDefaultInputs = 0x00000001,
	SVMFResetObject = 0x00000002,
	SVMFResetInspection = 0x00000004,
};

		////////////////////////////////////////////////////////////////////////////////
		//	Global Object Defines
		////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////
		// Message Queueing
		const UINT SVM_NOTIFY_ALL                           = 0x00000000;	// Default for all Messages
		const UINT SVM_NOTIFY_FIRST_RESPONDING              = 0x01000000;
		const UINT SVM_NOTIFY_ONLY_THIS                     = 0x02000000;
		const UINT SVM_NOTIFY_ONLY_CHILDREN                 = 0x04000000;
		const UINT SVM_NOTIFY_FRIENDS                       = 0x08000000;
		const UINT SVM_NOTIFY_ONLY_FRIENDS                  = 0x18000000;

		const UINT SVM_NOTIFY_BACKWARDS                     = 0x80000000;

		////////////////////////////////////////////////////////////////////////////////
		// Message Result Types
		const UINT SVM_SVMR_RESULT                          = 0x00100000;
		const UINT SVM_POINTER_RESULT                       = ( 0x00200000 | SVM_NOTIFY_FIRST_RESPONDING );

		////////////////////////////////////////////////////////////////////////////////
		// Message Direction
		const UINT SVM_PARENT_TO_CHILD                      = 0x00400000;
		const UINT SVM_CHILD_TO_PARENT                      = 0x00800000;

		////////////////////////////////////////////////////////////////////////////////
		// Message Separator
		const UINT SVM_PURE_MESSAGE	                        = 0x0000ffff;	// allow 16 bits for message qualifiers

		////////////////////////////////////////////////////////////////////////////////
		// Message Response ( Result: SVM_SVMR_RESULT )
		const UINT_PTR SVMR_NOT_PROCESSED                       = 0x00000000;	// Default for all messages, if not processed! 
		// Also valid for SVM_POINTER_RESULT messages, 
		// if they are not processed or not succesfully!!!
		// NEVER CHANGE THIS VALUE!!!
		const UINT_PTR SVMR_SUCCESS                             = 0x80000000;
		const UINT_PTR SVMR_NO_SUCCESS                          = 0xffffffff;

		////////////////////////////////////////////////////////////////////////////////
		// Message Types
		const UINT SVM_GET_MESSAGE                          = 0x00010000;
		const UINT SVM_SET_MESSAGE                          = 0x00020000;
		const UINT SVM_CREATE_CLOSE_MESSAGE                 = 0x00040000;
		const UINT SVM_UPDATE_MESSAGE                       = 0x00080000;


		////////////////////////////////////////////////////////////////////////////////
		// Note: Message IDs are limited to the lower 16 bits
		// and are sequential numbers not bit settings and each message Id must be unique

		////////////////////////////////////////////////////////////////////////////////
		// Get Messages...
		// ...returns SVObjectClass* or SVMR_NOT_PROCESSED

		// ...use second message parameter ( DwMessageValue ) to specify the object requesting
		// Note: if second parameter is specified - stop when requesting object encountered.
		// ...use third message parameter ( DwMessageContext ) to specify the objectTypeInfoStruct! 
		// ( SVObjectTypeInfoStruct->ObjectType => SVObjectTypeEnum && SVObjectTypeInfoStruct->SubType => SVObjectSubTypeEnum )
		// ...returns SVObjectClass* or SVMR_NOT_PROCESSED
		const UINT SVMSGID_GETFIRST_OBJECT                  = 0x00000100;
		const UINT SVM_GETFIRST_OBJECT                      = ( SVMSGID_GETFIRST_OBJECT | SVM_POINTER_RESULT | SVM_GET_MESSAGE | SVM_NOTIFY_FIRST_RESPONDING | SVM_NOTIFY_FRIENDS );

		/*
		// ...use result of SVM_GETFIRST_OBJECT as second message parameter ( DwMessageValue ) to find next specified object!
		// ...use third message parameter ( DwMessageContext ) to specify the object!
		// ( SVObjectTypeInfoStruct->ObjectType => SVObjectTypeEnum && SVObjectTypeInfoStruct->objectType => SVObjectSubTypeEnum )
		// ...returns SVObjectClass* or SVMR_NOT_PROCESSED
		const UINT SVMSGID_GETNEXT_OBJECT                   = 0x00000101;
		const UINT SVM_GETNEXT_OBJECT                       = ( SVMSGID_GETNEXT_OBJECT | SVM_POINTER_RESULT | SVM_GET_MESSAGE );
		*/

		// ...use a LPCTSTR as second message parameter ( DwMessageValue ) to specify the name
		// ...returns SVObjectClass* or SVMR_NOT_PROCESSED
		const UINT SVMSGID_GET_OBJECT_BY_NAME               = 0x00000102;
		const UINT SVM_GET_OBJECT_BY_NAME                   = ( SVMSGID_GET_OBJECT_BY_NAME | SVM_POINTER_RESULT | SVM_GET_MESSAGE | SVM_NOTIFY_ONLY_THIS );

		// ...returns SVImageInfoClass* or SVMR_NOT_PROCESSED
		const UINT SVMSGID_GETFIRST_IMAGE_INFO              = 0x00000103;
		const UINT SVM_GETFIRST_IMAGE_INFO                  = ( SVMSGID_GETFIRST_IMAGE_INFO | SVM_POINTER_RESULT | SVM_GET_MESSAGE );

		// ... use second message parameter ( DwMessageValue ) to specify a SVClassInfoStructListClass to be filled
		// ...use third message parameter ( DwMessageContext ) to specify the objectTypeInfo! 
		// ( SVObjectTypeInfoStruct->ObjectType => SVObjectTypeEnum && SVObjectTypeInfoStruct->SubType => SVObjectSubTypeEnum )
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_GETAVAILABLE_OBJECTS             = 0x00000104;
		const UINT SVM_GETAVAILABLE_OBJECTS                 = ( SVMSGID_GETAVAILABLE_OBJECTS | SVM_SVMR_RESULT | SVM_GET_MESSAGE );

		// ... use second message parameter ( DwMessageValue ) to pass a SVInputInfoListClass Pointer as 
		//		the interface list to fill.
		// If you only want the input interface of this particular object ( without regarding friend or child interfaces )
		//	use message queueing specifier SVM_NOTIFY_ONLY_THIS which can be 'or-ed' to the Message ID.
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_GET_INPUT_INTERFACE              = 0x00000105;
		const UINT SVM_GET_INPUT_INTERFACE                  = ( SVMSGID_GET_INPUT_INTERFACE | SVM_SVMR_RESULT | SVM_GET_MESSAGE );

		const UINT SVMSGID_GET_IMAGE_BAND0_OBJECT           = 0x00000106;
		const UINT SVM_GET_IMAGE_BAND0_OBJECT               = ( SVMSGID_GET_IMAGE_BAND0_OBJECT | SVM_POINTER_RESULT | SVM_GET_MESSAGE);


		////////////////////////////////////////////////////////////////////////////////
		// Set Messages...
		// ...use second message parameter ( DwMessageValue ) as SVObjectClass*
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		//const UINT SVM_SET_OBJECT							= ( 0x00000100 | SVM_SVMR_RESULT | SVM_SET_MESSAGE | SVM_NOTIFY_ONLY_THIS )

		// ...use second message parameter ( DwMessageValue ) as objectID ( GUID* )
		// ...use third message parameter ( DwMessageContext ) as SVObjectClass*
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_REPLACE_OBJECT                   = 0x00000500;
		const UINT SVM_REPLACE_OBJECT                       = ( SVMSGID_REPLACE_OBJECT | SVM_SVMR_RESULT | SVM_SET_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_ONLY_THIS );

		// ...use second message parameter ( DwMessageValue ) as objectID ( GUID* )
		// ...use third message parameter ( DwMessageContext ) as embeddedID ( GUID* ) 
		// ...returns pointer to embedded SVObjectClass
		const UINT SVMSGID_OVERWRITE_OBJECT                 = 0x00000501;
		const UINT SVM_OVERWRITE_OBJECT                     = ( SVMSGID_OVERWRITE_OBJECT  | SVM_POINTER_RESULT | SVM_SET_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_ONLY_THIS );

		// SEJ July 8,1999 - New Message
		// ...use second message parameter ( DwMessageValue ) as objectID ( GUID* )
		// ...use third message parameter ( DwMessageContext ) as SVObjectScriptDataObjectStruct*
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_SET_OBJECT_VALUE                 = 0x00000600;
		const UINT SVM_SET_OBJECT_VALUE                     = ( SVMSGID_SET_OBJECT_VALUE | SVM_SVMR_RESULT | SVM_SET_MESSAGE | SVM_NOTIFY_ONLY_THIS );

		////////////////////////////////////////////////////////////////////////////////
		// Create, Open, Close Messages...
		// ...use second message parameter ( DwMessageValue ) as SVObjectLevelCreateStruct*
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_CREATE_ALL_OBJECTS               = 0x00000700;
		//const UINT SVM_CREATE_ALL_OBJECTS                 = ( SVMSGID_CREATE_ALL_OBJECTS | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_PARENT_TO_CHILD );
		const UINT SVM_CREATE_ALL_OBJECTS                   = ( SVMSGID_CREATE_ALL_OBJECTS | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_ONLY_CHILDREN );

		// Send this message to the object owner to create an object.
		// If the owner object is not created yet, it returns SVMR_NOT_PROCESSED.
		// Otherwise the owner object sends SVM_CREATE_ALL_OBJECTS to the child object
		// and returns the result of this message.
		// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_CREATE_CHILD_OBJECT              = 0x00000701;
		const UINT SVM_CREATE_CHILD_OBJECT                  = ( SVMSGID_CREATE_CHILD_OBJECT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_NOTIFY_ONLY_THIS );

		// ...use second message parameter ( DwMessageValue ) as SVObjectLevelCreateStruct*
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_CONNECT_ALL_OBJECTS               = 0x00000702;
		const UINT SVM_CONNECT_ALL_OBJECTS                   = ( SVMSGID_CONNECT_ALL_OBJECTS | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_ONLY_CHILDREN );

		// Send this message to the object owner to connect an object.
		// If the owner object is not created yet, it returns SVMR_NOT_PROCESSED.
		// Otherwise the owner object sends SVM_CONNECT_ALL_OBJECTS to the child object
		// and returns the result of this message.
		// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_CONNECT_CHILD_OBJECT              = 0x00000703;
		const UINT SVM_CONNECT_CHILD_OBJECT                  = ( SVMSGID_CONNECT_CHILD_OBJECT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_NOTIFY_ONLY_THIS );

		// An object sends this message to the object which must be notfied
		//	 that the object is just created
		// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of 
		//		the object which sends this message.
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_NOTIFY_ABOUT_CREATION            = 0x00000704;
		const UINT SVM_NOTIFY_ABOUT_CREATION                = ( SVMSGID_NOTIFY_ABOUT_CREATION | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE );

		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_CONNECT_ALL_INPUTS               = 0x00000800;
		const UINT SVM_CONNECT_ALL_INPUTS                   = ( SVMSGID_CONNECT_ALL_INPUTS | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_FRIENDS );

		// Send this message to the object which needs to be connected
		// ...use second message parameter ( DwMessageValue ) as pointer to InObjectInfo ( SVInObjectInfoStruct* )
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_CONNECT_OBJECT_INPUT             = 0x00000801;
		const UINT SVM_CONNECT_OBJECT_INPUT	                = ( SVMSGID_CONNECT_OBJECT_INPUT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_CHILD_TO_PARENT | SVM_NOTIFY_ONLY_THIS );

		// Send this message to the object which needs to be disconnected
		// ...use second message parameter ( DwMessageValue ) as pointer to InObjectInfo ( SVInObjectInfoStruct* )
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_DISCONNECT_OBJECT_INPUT          = 0x00000802;
		const UINT SVM_DISCONNECT_OBJECT_INPUT              = ( SVMSGID_DISCONNECT_OBJECT_INPUT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_CHILD_TO_PARENT | SVM_NOTIFY_ONLY_THIS );

		// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the object which sends this message!
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_CLOSE_OBJECT                     = 0x00000900;
		const UINT SVM_CLOSE_OBJECT                         = ( SVMSGID_CLOSE_OBJECT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_NOTIFY_ONLY_THIS );

		// SEJ Aug 6,1999 - New Message
		// Send this message to the parent of the Object to be destroyed
		// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
		const UINT SVMSGID_DESTROY_CHILD_OBJECT             = 0x00000901;
		const UINT SVM_DESTROY_CHILD_OBJECT                 = ( SVMSGID_DESTROY_CHILD_OBJECT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_NOTIFY_ONLY_THIS);

		// SEJ Sep 23,1999 - New Message
		// Send this message to the owner of the Friend Object to be destroyed
		// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the friend object
		const UINT SVMSGID_DESTROY_FRIEND_OBJECT            = 0x00000902;
		const UINT SVM_DESTROY_FRIEND_OBJECT                = ( SVMSGID_DESTROY_FRIEND_OBJECT | SVM_SVMR_RESULT | SVM_CREATE_CLOSE_MESSAGE | SVM_NOTIFY_ONLY_THIS);

		// Send this message to the owner of the Analyzer to disconnect extra image(s) from the view(s)
		// ...use second message parameter ( DwMessageValue ) as SVAnalyzerClass* owner of the image
		const UINT SVMSGID_DISCONNECT_IMAGE_OBJECT          = 0x00000903;

		////////////////////////////////////////////////////////////////////////////////
		// Update Messages...

		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_RESET_ALL_OBJECTS                = 0x00001004;
		const UINT SVM_RESET_ALL_OBJECTS                    = ( SVMSGID_RESET_ALL_OBJECTS | SVM_SVMR_RESULT | SVM_UPDATE_MESSAGE | SVM_NOTIFY_FRIENDS );

		// ...A Message sent when system goes 'offline' i.e. from running to not running.
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_GOING_OFFLINE                    = 0x00001005;
		const UINT SVM_GOING_OFFLINE                        = ( SVMSGID_GOING_OFFLINE | SVM_SVMR_RESULT | SVM_UPDATE_MESSAGE | SVM_NOTIFY_ALL | SVM_PARENT_TO_CHILD );

		// ...A Message sent when Toolset/Tools get renamed.
		// ...use second message parameter ( DwMessageValue ) as pointer ot SVObjectClass w/new Name
		// ...use third message parameter ( DwMessageContext ) as original Name ( LPCTSTR )
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_OBJECT_RENAMED                   = 0x00001006;
		const UINT SVM_OBJECT_RENAMED                       = ( SVMSGID_OBJECT_RENAMED | SVM_SVMR_RESULT | SVM_UPDATE_MESSAGE | SVM_NOTIFY_ALL | SVM_NOTIFY_FRIENDS | SVM_PARENT_TO_CHILD );

		// ...A Message sent when system goes 'online' i.e. from not running to running.
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_GOING_ONLINE                     = 0x00001007;
		const UINT SVM_GOING_ONLINE                         = ( SVMSGID_GOING_ONLINE | SVM_SVMR_RESULT | SVM_UPDATE_MESSAGE | SVM_NOTIFY_ALL | SVM_PARENT_TO_CHILD );

		// ...Is sent as an notification to an tool when the image source changes.
		//
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_IMAGE_SOURCE_CHANGED             = 0x0000100B;
		const UINT SVM_IMAGE_SOURCE_CHANGED                 = ( SVMSGID_IMAGE_SOURCE_CHANGED | SVM_SVMR_RESULT | SVM_UPDATE_MESSAGE | SVM_NOTIFY_ONLY_THIS );

		// ...Is sent before the Toolset runs to copy all the result data forward
		// ...This is necessary because of conditionally executing tools
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_COPY_RESULT_DATA_FORWARD         = 0x00001101;
		const UINT SVM_COPY_RESULT_DATA_FORWARD             = ( SVMSGID_COPY_RESULT_DATA_FORWARD | SVM_SVMR_RESULT | SVM_SET_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_ALL );

		// ...Is sent before the Toolset runs to copy all the result images forward
		// ...This is necessary because of conditionally executing tools
		// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
		const UINT SVMSGID_COPY_RESULT_IMAGES_FORWARD       = 0x00001102;
		const UINT SVM_COPY_RESULT_IMAGES_FORWARD           = ( SVMSGID_COPY_RESULT_IMAGES_FORWARD | SVM_SVMR_RESULT | SVM_SET_MESSAGE | SVM_PARENT_TO_CHILD | SVM_NOTIFY_ALL );

		/////////////////////////////////////////////////////////////////////////////
		//
		//
		// Output Object attributes - 19 Aug 1999 - frb.
		//
		const UINT SV_NO_ATTRIBUTES              = 0x00000000;  // default
		const UINT SV_VIEWABLE                   = 0x00000001;
		const UINT SV_PUBLISHABLE                = 0x00000002;
		const UINT SV_ARCHIVABLE                 = 0x00000004;
		const UINT SV_SELECTABLE_FOR_EQUATION    = 0x00000008;
		const UINT SV_EMBEDABLE                  = 0x00000010;
		const UINT SV_SELECTABLE_FOR_STATISTICS  = 0x00000020;
		const UINT SV_ARCHIVABLE_IMAGE           = 0x00000040;  // 11 Oct 1999 - frb.
		const UINT SV_PRINTABLE                  = 0x00000080;
		const UINT SV_REMOTELY_SETABLE           = 0x00000100;  // RY 2002 12 19
		const UINT SV_SETABLE_ONLINE             = 0x00000200;
		const UINT SV_EXTENT_OBJECT              = 0x00000400;
		const UINT SV_HIDDEN                     = 0x00000800;  // EB/TB 2004 05 10
		const UINT SV_PUBLISH_RESULT_IMAGE       = 0x00001000;
		const UINT SV_CH_CONDITIONAL             = 0x00002000;  // conditional history
		const UINT SV_CH_IMAGE                   = 0x00004000;  // conditional history
		const UINT SV_CH_VALUE                   = 0x00008000;  // conditional history
		const UINT SV_DD_VALUE                   = 0x00010000;  // Selected for DataDefinition Value
		const UINT SV_DD_IMAGE                   = 0x00020000;  // Selected for DataDefinition Image

		// this was the mis-named SV_ALL_ATTRIBUTES
		const UINT SV_DEFAULT_ATTRIBUTES  = (
			SV_VIEWABLE
			| SV_PUBLISHABLE
			| SV_ARCHIVABLE
			| SV_SELECTABLE_FOR_EQUATION
			| SV_EMBEDABLE
			| SV_SELECTABLE_FOR_STATISTICS
			| SV_PRINTABLE
			| SV_ARCHIVABLE_IMAGE
			);

		const UINT SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES = 
			(
			SV_VIEWABLE
			| SV_PUBLISHABLE
			| SV_ARCHIVABLE
			| SV_SELECTABLE_FOR_EQUATION
			| SV_EMBEDABLE
			| SV_SELECTABLE_FOR_STATISTICS
			| SV_PRINTABLE
			| SV_CH_VALUE
			| SV_CH_CONDITIONAL
			| SV_DD_VALUE
			);

		const UINT SV_DEFAULT_IMAGE_OBJECT_ATTRIBUTES =
			(
			SV_ARCHIVABLE_IMAGE
			| SV_PUBLISH_RESULT_IMAGE
			| SV_CH_IMAGE
			| SV_DD_IMAGE
			);
//	} //	namespace ObjectInterfaces
//} //namespace Seidenader