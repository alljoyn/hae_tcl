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

#include <ajtcl/hae/interfaces/operation/RemoteControllability.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "RemoteControllabilityImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationRemoteControllability[] = {
    "$org.alljoyn.SmartSpaces.Operation.RemoteControllability",
    "@Version>q",
    "@IsControllable=b",
    NULL
};

typedef struct {
    uint16_t version;
    bool isControllable;
} RemoteControllabilityProperties;

AJ_Status CreateRemoteControllabilityInterface(void** properties)
{
    *properties = malloc(sizeof(RemoteControllabilityProperties));
    if(!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(RemoteControllabilityProperties));

    ((RemoteControllabilityProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyRemoteControllabilityInterface(void* properties)
{
    if (properties) {
        RemoteControllabilityProperties* props = (RemoteControllabilityProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescOperationRemoteControllability[0]+1); //To remove '$'
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

AJ_Status RemoteControllabilityInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    RemoteControllabilityProperties* props = NULL;
    RemoteControllabilityListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (RemoteControllabilityListener*)listener;
    }
    props = (RemoteControllabilityProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            bool isControllable;
            if (lt && lt->OnGetIsControllable) {
                status = lt->OnGetIsControllable(objPath, &isControllable);
                if (status == AJ_OK) {
                    props->isControllable = isControllable;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "b", props->isControllable);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_RemoteControllabilityInterfaceGetIsControllable(const char* objPath, bool* isControllable)
{
    AJ_Status status = AJ_OK;
    RemoteControllabilityProperties* props = NULL;

    if (!isControllable) {
        return AJ_ERR_INVALID;
    }

    props = (RemoteControllabilityProperties*)GetProperties(objPath, REMOTE_CONTROLLABILITY_INTERFACE);
    if (props) {
        *isControllable = props->isControllable;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_RemoteControllabilityInterfaceSetIsControllable(AJ_BusAttachment* busAttachment, const char* objPath, const bool isControllable)
{
    AJ_Status status = AJ_OK;
    RemoteControllabilityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (RemoteControllabilityProperties*)GetProperties(objPath, REMOTE_CONTROLLABILITY_INTERFACE);
    if (props) {
        props->isControllable = isControllable;
        status = EmitPropChanged(busAttachment, objPath, "isControllable", "b", &(props->isControllable));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}


