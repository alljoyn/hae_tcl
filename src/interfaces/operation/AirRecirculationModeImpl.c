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

#include <ajtcl/hae/interfaces/operation/AirRecirculationMode.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "AirRecirculationModeImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationAirRecirculationMode[] = {
    "$org.alljoyn.SmartSpaces.Operation.AirRecirculationMode",
    "@Version>q",
    "@IsRecirculating=b",
    NULL
};

typedef struct {
    uint16_t version;
    bool isRecirculating;
} AirRecirculationModeProperties;

AJ_Status CreateAirRecirculationModeInterface(void** properties)
{
    *properties = malloc(sizeof(AirRecirculationModeProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(AirRecirculationModeProperties));

    ((AirRecirculationModeProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyAirRecirculationModeInterface(void* properties)
{
    if (properties) {
        AirRecirculationModeProperties* props = (AirRecirculationModeProperties*)properties;

        free(props);
    }
}

static AJ_Status EmitPropChanged(AJ_BusAttachment* busAttachment, const char* objPath, const char* propName, const char* signature, void* val)
{
    AJ_Status status = AJ_OK;
    uint32_t msgId = 0;
    AJ_Message msg;
    AJ_Arg array, strc;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = MakePropChangedId(objPath, &msgId);
    if (status == AJ_OK) {
        AJ_MarshalSignal(busAttachment, &msg, msgId, NULL, 0, AJ_FLAG_GLOBAL_BROADCAST, 0);
        AJ_MarshalArgs(&msg, "s", intfDescOperationAirRecirculationMode[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "b")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(bool*)val);
        } else {
            return AJ_ERR_SIGNATURE;
        }
        AJ_MarshalCloseContainer(&msg, &strc);
        AJ_MarshalCloseContainer(&msg, &array);
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalCloseContainer(&msg, &array);
        status = AJ_DeliverMsg(&msg);
    }

    return status;
}

AJ_Status AirRecirculationModeInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    AirRecirculationModeProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (AirRecirculationModeProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "IsRecirculating", "b", &(props->isRecirculating));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status AirRecirculationModeInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    AirRecirculationModeProperties* props = NULL;
    AirRecirculationModeListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (AirRecirculationModeListener*)listener;
    }
    props = (AirRecirculationModeProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            bool isRecirculating;
            if (lt && lt->OnGetIsRecirculating) {
                status = lt->OnGetIsRecirculating(objPath, &isRecirculating);
                if (status == AJ_OK) {
                    props->isRecirculating = isRecirculating;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "b", props->isRecirculating);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status AirRecirculationModeInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    AirRecirculationModeProperties* props = (AirRecirculationModeProperties*)properties;
    AirRecirculationModeListener* lt = (AirRecirculationModeListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetIsRecirculating) {
            status = AJ_ERR_NULL;
        } else {
            bool isRecirculating = 0;
            status = AJ_UnmarshalArgs(replyMsg, "b", &isRecirculating);

            if (status == AJ_OK) {
                status = lt->OnSetIsRecirculating(objPath, isRecirculating);

                if (status == AJ_OK) {
                    if (props->isRecirculating != isRecirculating) {
                        props->isRecirculating = isRecirculating;
                        *propChanged = true;
                    }
                }
            }
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_AirRecirculationModeInterfaceGetIsRecirculating(const char* objPath, bool* isRecirculating)
{
    AJ_Status status = AJ_OK;
    AirRecirculationModeProperties* props = NULL;

    if (!isRecirculating) {
        return AJ_ERR_INVALID;
    }

    props = (AirRecirculationModeProperties*)GetProperties(objPath, AIR_RECIRCULATION_MODE_INTERFACE);
    if (props) {
        *isRecirculating = props->isRecirculating;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_AirRecirculationModeInterfaceSetIsRecirculating(AJ_BusAttachment* busAttachment, const char* objPath, const bool isRecirculating)
{
    AJ_Status status = AJ_OK;
    AirRecirculationModeProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (AirRecirculationModeProperties*)GetProperties(objPath, AIR_RECIRCULATION_MODE_INTERFACE);
    if (props) {
        props->isRecirculating = isRecirculating;

        status = EmitPropChanged(busAttachment, objPath, "IsRecirculating", "b", &(props->isRecirculating));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
