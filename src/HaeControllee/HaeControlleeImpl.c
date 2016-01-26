/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ajtcl/hae/HaeControllee.h>
#include "HaeControlleeImpl.h"
#include "../interfaces/operation/AudioVolumeImpl.h"
#include "../interfaces/operation/ChannelImpl.h"

#define HAE_OBJECT_LIST_INDEX AJAPP_OBJECTS_LIST_INDEX

static const char* const* intfDescs[MAX_BASIC_INTERFACE_TYPE];
static InterfaceCreator intfCreator[MAX_BASIC_INTERFACE_TYPE] = { NULL, };
static InterfaceDestructor intfDestructor[MAX_BASIC_INTERFACE_TYPE] = { NULL, };
static OnGetProperty onGetProperty[MAX_BASIC_INTERFACE_TYPE] = { NULL, };
static OnSetProperty onSetProperty[MAX_BASIC_INTERFACE_TYPE] = { NULL, };
static EmitPropertiesChanged emitPropertiesChanged[MAX_BASIC_INTERFACE_TYPE] = { NULL, };
static OnMethodHandler onMethodHandler[MAX_BASIC_INTERFACE_TYPE] = { NULL, };
static HaeObjectInfo* objInfoFirst;
static HaeObjectInfo* objInfoLast;
static AJ_Object* ajObjects;
static VendorDefinedInterfaceInfo* vendorDefinedIntfInfoFirst;
static VendorDefinedInterfaceInfo* vendorDefinedIntfInfoLast;

static VendorDefinedInterfaceInfo* GetVendorDefinedInterfaceInfo(HaeInterfaceTypes intfType)
{
    VendorDefinedInterfaceInfo* intfInfo = NULL;

    intfInfo = vendorDefinedIntfInfoFirst;
    while (intfInfo) {
        if (intfInfo->intfType == intfType) {
            break;
        }
        intfInfo = intfInfo->intfNext;
    }

    return intfInfo;
}

AJ_Status Hae_Init()
{
    intfDescs[AUDIO_VOLUME_INTERFACE] = intfDescOperationAudioVolume;
    intfDescs[CHANNEL_INTERFACE] = intfDescOperationChannel;

    intfCreator[AUDIO_VOLUME_INTERFACE] = CreateAudioVolumeInterface;
    intfCreator[CHANNEL_INTERFACE] = CreateChannelInterface;

    intfDestructor[AUDIO_VOLUME_INTERFACE] = DestroyAudioVolumeInterface;
    intfDestructor[CHANNEL_INTERFACE] = DestroyChannelInterface;

    onGetProperty[AUDIO_VOLUME_INTERFACE] = AudioVolumeInterfaceOnGetProperty;
    onGetProperty[CHANNEL_INTERFACE] = ChannelInterfaceOnGetProperty;

    onSetProperty[AUDIO_VOLUME_INTERFACE] = AudioVolumeInterfaceOnSetProperty;
    onSetProperty[CHANNEL_INTERFACE] = ChannelInterfaceOnSetProperty;

    emitPropertiesChanged[AUDIO_VOLUME_INTERFACE] = AudioVolumeInterfaceEmitPropertiesChanged;
    emitPropertiesChanged[CHANNEL_INTERFACE] = ChannelInterfaceEmitPropertiesChanged;

    onMethodHandler[CHANNEL_INTERFACE] = ChannelInterfaceOnMethodHandler;

    objInfoFirst = NULL;
    objInfoLast = NULL;

    vendorDefinedIntfInfoFirst = NULL;
    vendorDefinedIntfInfoLast = NULL;

    return AJ_OK;
}

void Hae_Deinit()
{
    HaeObjectInfo* objInfo = objInfoFirst;
    HaeObjectInfo* tempObjInfo = NULL;
    HaeInterfaceInfo* intfInfo = NULL;
    HaeInterfaceInfo* tempIntfInfo = NULL;
    VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = NULL;
    VendorDefinedInterfaceInfo* tempVendorDefinedIntfInfo = NULL;

    while (objInfo) {
        intfInfo = objInfo->intfFirst;

        while(intfInfo) {
            if (intfInfo->intfType > UNDEFINED_INTERFACE && intfInfo->intfType < VENDOR_DEFINED_INTERFACE) {
                if (intfDestructor[intfInfo->intfType]) {
                    intfDestructor[intfInfo->intfType](intfInfo->properties);
                }
            } else if (intfInfo->intfType > VENDOR_DEFINED_INTERFACE) {
                vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(intfInfo->intfType);
                if (vendorDefinedIntfInfo) {
                    if (vendorDefinedIntfInfo->handler) {
                        if (vendorDefinedIntfInfo->handler->InterfaceDestructor) {
                            vendorDefinedIntfInfo->handler->InterfaceDestructor(intfInfo->properties);
                        }
                    }
                }
            }

            tempIntfInfo = intfInfo;
            intfInfo = intfInfo->intfNext;
            free(tempIntfInfo);
        }

        if (objInfo->ajIntfDesc) {
            free(objInfo->ajIntfDesc);
        }
        tempObjInfo = objInfo;
        objInfo = objInfo->objNext;
        free(tempObjInfo);
    }

    vendorDefinedIntfInfo = vendorDefinedIntfInfoFirst;
    while (vendorDefinedIntfInfo) {
        tempVendorDefinedIntfInfo = vendorDefinedIntfInfo;
        vendorDefinedIntfInfo = vendorDefinedIntfInfo->intfNext;
        free(tempVendorDefinedIntfInfo);
    }

    if (ajObjects) {
        free(ajObjects);
    }
}

static HaeObjectInfo* FindObject(const char* objPath)
{
    HaeObjectInfo* objInfo = objInfoFirst;

    if (!objPath) {
        return NULL;
    }

    while (objInfo) {
        if (!strcmp(objPath, objInfo->path)) {
            break;
        } else {
            objInfo = objInfo->objNext;
        }
    }

    return objInfo;
}

AJ_Status Hae_CreateInterface(HaeInterfaceTypes intfType, const char* objPath, void* listener)
{
    AJ_Status status = AJ_OK;
    HaeObjectInfo* objInfo = NULL;
    VendorDefinedInterfaceHandler* vendorDefinedIntfHandler = NULL;

    if (!objPath) {
        return AJ_ERR_INVALID;
    }

    if (intfType > UNDEFINED_INTERFACE && intfType < VENDOR_DEFINED_INTERFACE) {
        if (!intfCreator[intfType]) {
            return AJ_ERR_NULL;
        }
    } else if (intfType > VENDOR_DEFINED_INTERFACE) {
        VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(intfType);
        if (!vendorDefinedIntfInfo) {
            return AJ_ERR_NULL;
        }

        vendorDefinedIntfHandler = vendorDefinedIntfInfo->handler;
        if (!vendorDefinedIntfHandler) {
            return AJ_ERR_NULL;
        }
    } else {
        return AJ_ERR_INVALID;
    }

    HaeInterfaceInfo* intfInfo = (HaeInterfaceInfo*)malloc(sizeof(HaeInterfaceInfo));
    if (!intfInfo) {
        return AJ_ERR_RESOURCES;
    }
    intfInfo->intfType = intfType;
    intfInfo->listener = listener;

    if (!vendorDefinedIntfHandler) {
        status = intfCreator[intfType](&intfInfo->properties);
    } else {
        status = vendorDefinedIntfHandler->InterfaceCreator(&intfInfo->properties);
    }
    if (status != AJ_OK) {
        return status;
    }

    objInfo = FindObject(objPath);
    if (!objInfo) {
        objInfo = (HaeObjectInfo*)malloc(sizeof(HaeObjectInfo));
        if (!objInfo) {
            free(intfInfo);
            return AJ_ERR_RESOURCES;
        }

        {
            HaeObjectInfo tempObj = { objPath, intfInfo, intfInfo, NULL, NULL };
            memcpy(objInfo, &tempObj, sizeof(tempObj));
        }
        if (!objInfoFirst) {
            objInfoFirst = objInfo;
        }
        if (objInfoLast) {
            objInfoLast->objNext = objInfo;
        }
        objInfoLast = objInfo;
    } else {
        objInfo->intfLast->intfNext = intfInfo;
        objInfo->intfLast = intfInfo;
    }

    return AJ_OK;
}

static void CleanInterfaceDescs()
{
    HaeObjectInfo* objInfo = objInfoFirst;

    while (objInfo) {
        if (objInfo->ajIntfDesc) {
            free(objInfo->ajIntfDesc);
        }

        objInfo = objInfo->objNext;
    }
}

AJ_Status Hae_Start()
{
    int i=0, j=1;
    HaeObjectInfo* objInfo = NULL;
    int numOfObjs = 1;
    HaeInterfaceInfo* intfInfo = NULL;
    int numOfIntfs = 0;

    if (!objInfoFirst || !objInfoLast) {
        return AJ_ERR_UNEXPECTED;
    }

    objInfo = objInfoFirst;
    while (objInfo != objInfoLast) {
        objInfo = objInfo->objNext;
        numOfObjs++;
    }

    ajObjects = (AJ_Object*)malloc(sizeof(AJ_Object) * (numOfObjs+1)); //+1 is for last NULL
    if (!ajObjects) {
        return AJ_ERR_RESOURCES;
    }

    objInfo = objInfoFirst;
    while (objInfo) {
        intfInfo = objInfo->intfFirst;
        numOfIntfs = 1;
        while (intfInfo != objInfo->intfLast) {
            intfInfo = intfInfo->intfNext;
            numOfIntfs++;
        }

        objInfo->ajIntfDesc = (AJ_InterfaceDescription*)malloc(sizeof(AJ_InterfaceDescription) * (numOfIntfs+2)); //+2 are for AJ_PropertiesIface and last NULL
        if (!objInfo->ajIntfDesc) {
            CleanInterfaceDescs();
            free(ajObjects);
            return AJ_ERR_RESOURCES;
        }

        objInfo->ajIntfDesc[0] = AJ_PropertiesIface;
        intfInfo = objInfo->intfFirst;
        j=1;
        while (intfInfo) {
            if (intfInfo->intfType > UNDEFINED_INTERFACE && intfInfo->intfType < VENDOR_DEFINED_INTERFACE) {
                objInfo->ajIntfDesc[j] = intfDescs[intfInfo->intfType];
            } else if (intfInfo->intfType > VENDOR_DEFINED_INTERFACE) {
                VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(intfInfo->intfType);
                if (vendorDefinedIntfInfo) {
                    objInfo->ajIntfDesc[j] = vendorDefinedIntfInfo->intfDesc;
                }
            }
            intfInfo = intfInfo->intfNext;
            j++;
        }
        objInfo->ajIntfDesc[j] = NULL;

        {
            AJ_Object temp_obj = { objInfo->path, objInfo->ajIntfDesc, AJ_OBJ_FLAG_ANNOUNCED };
            memcpy(&ajObjects[i], &temp_obj, sizeof(temp_obj));
        }
        objInfo = objInfo->objNext;
        i++;
    }

    AJ_PrintXML(ajObjects);

    AJ_RegisterObjectList(ajObjects, HAE_OBJECT_LIST_INDEX);

    return AJ_OK;
}

static bool IsHaeMsg(uint32_t msgId)
{
    if ((msgId >> 24) == HAE_OBJECT_LIST_INDEX)
        return true;

    return false;
}

static uint8_t GetObjectIndex(uint32_t msgId)
{
    return (uint8_t)((msgId & 0x00FF0000) >> 16);
}

static uint8_t GetInterfaceIndex(uint32_t msgId)
{
    return (uint8_t)((msgId & 0x0000FF00) >> 8);
}

static uint8_t GetMemberIndex(uint32_t msgId)
{
    return (uint8_t)(msgId & 0x0000FF);
}

static HaeObjectInfo* GetObjectInfo(uint8_t objIndex)
{
    HaeObjectInfo* objInfo = objInfoFirst;
    uint8_t i = 0;

    for (i=0; i<objIndex; i++) {
        if (!objInfo) {
            break;
        }

        objInfo = objInfo->objNext;
    }

    return objInfo;
}

static HaeInterfaceInfo* GetInterfaceInfoOfObject(HaeObjectInfo* objInfo, uint8_t intfIndex)
{
    uint8_t i=1;

    if (!objInfo) {
        return NULL;
    }

    if (intfIndex < 1) { //0 is org.freedesktop.DBus.Properties
        return NULL;
    }

    HaeInterfaceInfo* intfInfo = objInfo->intfFirst;

    for(i=1; i<intfIndex; i++) {
        intfInfo = intfInfo->intfNext;
        if (!intfInfo) {
            break;
        }
    }

    return intfInfo;
}

static AJ_Status PropGetHandler(AJ_Message* replyMsg, uint32_t propId, void* context)
{
    uint8_t objIndex = GetObjectIndex(propId);
    uint8_t intfIndex = GetInterfaceIndex(propId);
    uint8_t memberIndex = GetMemberIndex(propId);
    AJ_Status status = AJ_OK;

    HaeObjectInfo* objInfo = GetObjectInfo(objIndex);
    if (objInfo) {
        HaeInterfaceInfo* intfInfo = GetInterfaceInfoOfObject(objInfo, intfIndex);
        if (intfInfo) {
            if (intfInfo->intfType > UNDEFINED_INTERFACE && intfInfo->intfType < VENDOR_DEFINED_INTERFACE) {
                if (onGetProperty[intfInfo->intfType]) {
                    status = onGetProperty[intfInfo->intfType](replyMsg, objInfo->path, intfInfo->properties, memberIndex, intfInfo->listener);
                }
            } else if (intfInfo->intfType > VENDOR_DEFINED_INTERFACE) {
                VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(intfInfo->intfType);
                if (!vendorDefinedIntfInfo) {
                    status = AJ_ERR_NULL;
                }

                if (vendorDefinedIntfInfo->handler) {
                    if (vendorDefinedIntfInfo->handler->OnGetProperty) {
                        status = vendorDefinedIntfInfo->handler->OnGetProperty(replyMsg, objInfo->path, intfInfo->properties, memberIndex, intfInfo->listener);
                    } else {
                        status = AJ_ERR_NULL;
                    }
                } else {
                    status = AJ_ERR_NULL;
                }
            } else {
                status = AJ_ERR_INVALID;
            }
        } else {
            status = AJ_ERR_NULL;
        }
    } else {
        status = AJ_ERR_NULL;
    }

    return status;
}

static AJ_Status PropSetHandler(AJ_Message* replyMsg, uint32_t propId, void* context)
{
    uint8_t objIndex = GetObjectIndex(propId);
    uint8_t intfIndex = GetInterfaceIndex(propId);
    uint8_t memberIndex = GetMemberIndex(propId);
    HaePropertiesChanged* propChanged = (HaePropertiesChanged*)context;
    AJ_Status status = AJ_OK;

    HaeObjectInfo* objInfo = GetObjectInfo(objIndex);
    if (objInfo) {
        HaeInterfaceInfo* intfInfo = GetInterfaceInfoOfObject(objInfo, intfIndex);
        if (intfInfo) {
            propChanged->intfType = intfInfo->intfType;
            propChanged->properties = intfInfo->properties;
            propChanged->member_index = memberIndex;
            if (intfInfo->intfType > UNDEFINED_INTERFACE && intfInfo->intfType < VENDOR_DEFINED_INTERFACE) {
                if (onSetProperty[intfInfo->intfType]) {
                    status = onSetProperty[intfInfo->intfType](replyMsg, objInfo->path, intfInfo->properties, memberIndex, intfInfo->listener, &propChanged->changed);
                }
            } else if (intfInfo->intfType > VENDOR_DEFINED_INTERFACE) {
                VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(intfInfo->intfType);
                if (!vendorDefinedIntfInfo) {
                    status = AJ_ERR_NULL;
                }

                if (vendorDefinedIntfInfo->handler) {
                    if (vendorDefinedIntfInfo->handler->OnSetProperty) {
                        status = vendorDefinedIntfInfo->handler->OnSetProperty(replyMsg, objInfo->path, intfInfo->properties, memberIndex, intfInfo->listener, &propChanged->changed);
                    } else {
                        status = AJ_ERR_NULL;
                    }
                } else {
                    status = AJ_ERR_NULL;
                }
            } else {
                status = AJ_ERR_INVALID;
            }
        } else {
            status = AJ_ERR_NULL;
        }
    } else {
        status = AJ_ERR_NULL;
    }

    return status;
}

AJSVC_ServiceStatus Hae_MessageProcessor(AJ_BusAttachment* busAttachment, AJ_Message* msg, AJ_Status* status)
{
    AJSVC_ServiceStatus serviceStatus = AJSVC_SERVICE_STATUS_HANDLED;

    if (IsHaeMsg(msg->msgId)) {
        uint8_t objIndex = GetObjectIndex(msg->msgId);
        uint8_t intfIndex = GetInterfaceIndex(msg->msgId);
        uint8_t memberIndex = GetMemberIndex(msg->msgId);
        HaeObjectInfo* objInfo = GetObjectInfo(objIndex);

        if (objInfo) {
            if (intfIndex == (uint8_t)0) { //org.freedesktop.DBus.Properties handling
                if (memberIndex == AJ_PROP_GET) {
                    *status = AJ_BusPropGet(msg, PropGetHandler, NULL);
                } else if (memberIndex == AJ_PROP_SET) {
                    HaePropertiesChanged propChanged;
                    *status = AJ_BusPropSet(msg, PropSetHandler, &propChanged);
                    if (*status == AJ_OK && propChanged.changed) {
                        if (propChanged.intfType > UNDEFINED_INTERFACE && propChanged.intfType < VENDOR_DEFINED_INTERFACE) {
                            if (emitPropertiesChanged[propChanged.intfType]) {
                                emitPropertiesChanged[propChanged.intfType](busAttachment, objInfo->path, propChanged.properties, propChanged.member_index);
                            }
                        } else if (propChanged.intfType > VENDOR_DEFINED_INTERFACE) {
                            VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(propChanged.intfType);
                            if (vendorDefinedIntfInfo) {
                                if (vendorDefinedIntfInfo->handler) {
                                    if (vendorDefinedIntfInfo->handler->EmitPropertiesChanged) {
                                        vendorDefinedIntfInfo->handler->EmitPropertiesChanged(busAttachment, objInfo->path, propChanged.properties, propChanged.member_index);
                                    }
                                }
                            }
                        }
                    }
                } else {
                    *status = AJ_ERR_INVALID;
                }
            } else { //method
                HaeInterfaceInfo* intfInfo = GetInterfaceInfoOfObject(objInfo, intfIndex);
                if (intfInfo) {
                    if (intfInfo->intfType > UNDEFINED_INTERFACE && intfInfo->intfType < VENDOR_DEFINED_INTERFACE) {
                        if (onMethodHandler[intfInfo->intfType]) {
                            *status = onMethodHandler[intfInfo->intfType](msg, objInfo->path, memberIndex, intfInfo->listener);
                        } else {
                            *status = AJ_ERR_INVALID;
                        }
                    } else if (intfInfo->intfType > VENDOR_DEFINED_INTERFACE) {
                        VendorDefinedInterfaceInfo* vendorDefinedIntfInfo = GetVendorDefinedInterfaceInfo(intfInfo->intfType);
                        if (!vendorDefinedIntfInfo) {
                            *status = AJ_ERR_NULL;
                        }

                        if (vendorDefinedIntfInfo->handler) {
                            if (vendorDefinedIntfInfo->handler->OnMethodHandler) {
                                *status = vendorDefinedIntfInfo->handler->OnMethodHandler(msg, objInfo->path, memberIndex, intfInfo->listener);
                            } else {
                                *status = AJ_ERR_NULL;
                            }
                        } else {
                            *status = AJ_ERR_NULL;
                        }
                    } else {
                        *status = AJ_ERR_INVALID;
                    }
                } else {
                    *status = AJ_ERR_NULL;
                }
            }
        } else {
            *status = AJ_ERR_NULL;
        }
    } else {
        serviceStatus = AJSVC_SERVICE_STATUS_NOT_HANDLED;
    }

    return serviceStatus;
}

void* GetProperties(const char* objPath, HaeInterfaceTypes intfType)
{
    HaeObjectInfo* objInfo = NULL;

    if (!objPath) {
        return NULL;
    }

    objInfo = FindObject(objPath);
    if (objInfo) {
        HaeInterfaceInfo* intfInfo = objInfo->intfFirst;

        while (intfInfo) {
            if (intfInfo->intfType == intfType) {
                break;
            }

            intfInfo = intfInfo->intfNext;
        }

        if (intfInfo) {
            return intfInfo->properties;
        }
    }

    return NULL;
}

AJ_Status MakeMsgId(const char* objPath, HaeInterfaceTypes intfType, uint8_t memberIndex, uint32_t* msgId)
{
    HaeObjectInfo* objInfo = objInfoFirst;
    HaeInterfaceInfo* intfInfo = NULL;
    uint8_t objIndex = -1;
    uint8_t intfIndex = 0;

    if (!objPath) {
        return AJ_ERR_INVALID;
    }

    if (!msgId) {
        return AJ_ERR_INVALID;
    }

    *msgId = HAE_OBJECT_LIST_INDEX << 24;

    while (objInfo) {
        objIndex++;

        if (!strcmp(objInfo->path, objPath)) {
            break;
        } else {
            objInfo = objInfo->objNext;
        }
    }

    if (!objInfo) {
        return AJ_ERR_NO_MATCH;
    }

    *msgId |= (objIndex << 16);

    intfInfo = objInfo->intfFirst;
    while (intfInfo) {
        intfIndex++;
        if (intfInfo->intfType == intfType) {
            break;
        } else {
            intfInfo = intfInfo->intfNext;
        }
    }

    if (!intfInfo) {
        return AJ_ERR_NO_MATCH;
    }

    *msgId |= (intfIndex << 8);
    *msgId |= memberIndex;

    return AJ_OK;
}

AJ_Status MakePropChangedId(const char* objPath, uint32_t* msgId)
{
    HaeObjectInfo* objInfo = objInfoFirst;
    uint8_t objIndex = -1;

    if (!objPath) {
        return AJ_ERR_INVALID;
    }

    if (!msgId) {
        return AJ_ERR_INVALID;
    }

    *msgId = HAE_OBJECT_LIST_INDEX << 24;

    while (objInfo) {
        objIndex++;

        if (!strcmp(objInfo->path, objPath)) {
            break;
        } else {
            objInfo = objInfo->objNext;
        }
    }

    if (!objInfo) {
        return AJ_ERR_NO_MATCH;
    }

    *msgId |= (objIndex << 16);
    *msgId |= AJ_PROP_CHANGED;

    return AJ_OK;
}

static bool CheckAlreadyRegistered(const char* intfName)
{
    VendorDefinedInterfaceInfo* intfInfo = NULL;

    intfInfo = vendorDefinedIntfInfoFirst;
    while (intfInfo) {
        if(!strcmp(intfName, intfInfo->intfName)) {
            break;
        }
        intfInfo = intfInfo->intfNext;
    }

    if (intfInfo) {
        return true;
    } else {
        return false;
    }
}

static int GetNumOfVendorInterfaces()
{
    VendorDefinedInterfaceInfo* intfInfo = NULL;
    int numOfIntfs = 1;

    intfInfo = vendorDefinedIntfInfoFirst;
    while (intfInfo != vendorDefinedIntfInfoLast) {
        intfInfo = intfInfo->intfNext;
        numOfIntfs++;
    }

    return numOfIntfs;
}

AJ_Status Hae_RegisterVendorDefinedInterface(const char* intfName, const char* const* intfDesc, VendorDefinedInterfaceHandler* handler, HaeInterfaceTypes* intfType)
{
    if (!intfName || !intfDesc) {
        return AJ_ERR_INVALID;
    }

    if (!handler || !handler->InterfaceRegistered || !handler->InterfaceCreator || !handler->InterfaceDestructor) {
        return AJ_ERR_INVALID;
    }

    if (!intfType) {
        return AJ_ERR_INVALID;
    }

    if(CheckAlreadyRegistered(intfName)) {
        return AJ_ERR_DISALLOWED;
    }

    VendorDefinedInterfaceInfo* intfInfo = (VendorDefinedInterfaceInfo*)malloc(sizeof(VendorDefinedInterfaceInfo));
    if (!intfInfo) {
        return AJ_ERR_RESOURCES;
    }

    {
        VendorDefinedInterfaceInfo tempInfo = { intfName, UNDEFINED_INTERFACE, intfDesc, handler, NULL };
        memcpy(intfInfo, &tempInfo, sizeof(tempInfo));
    }
    if (!vendorDefinedIntfInfoFirst) {
        vendorDefinedIntfInfoFirst = intfInfo;
    }
    if (vendorDefinedIntfInfoLast) {
        vendorDefinedIntfInfoLast->intfNext = intfInfo;
    }
    vendorDefinedIntfInfoLast = intfInfo;
    intfInfo->intfType = VENDOR_DEFINED_INTERFACE + GetNumOfVendorInterfaces();
    handler->InterfaceRegistered(intfInfo->intfType);
    *intfType = intfInfo->intfType;

    return AJ_OK;
}
