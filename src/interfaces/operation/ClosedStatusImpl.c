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

#include <ajtcl/hae/interfaces/operation/ClosedStatus.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "ClosedStatusImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationClosedStatus[] = {
    "$org.alljoyn.SmartSpaces.Operation.ClosedStatus",
    "@Version>q",
    "@IsClosed=b",
    NULL
};

typedef struct {
    uint16_t version;
    bool isClosed;
} ClosedStatusProperties;

AJ_Status CreateClosedStatusInterface(void** properties)
{
    *properties = malloc(sizeof(ClosedStatusProperties));
    if(!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((ClosedStatusProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyClosedStatusInterface(void* properties)
{
    if (properties) {
        ClosedStatusProperties* props = (ClosedStatusProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescOperationClosedStatus[0]+1); //To remove '$'
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

AJ_Status ClosedStatusInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    ClosedStatusProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (ClosedStatusProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "IsClosed", "b", &(props->isClosed));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status ClosedStatusInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    ClosedStatusProperties* props = NULL;
    ClosedStatusListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (ClosedStatusListener*)listener;
    }
    props = (ClosedStatusProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            bool isClosed;
            if (lt && lt->OnGetIsClosed) {
                status = lt->OnGetIsClosed(objPath, &isClosed);
                if (status == AJ_OK) {
                    props->isClosed = isClosed;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "b", props->isClosed);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_ClosedStatusInterfaceGetIsClosed(const char* objPath, bool* isClosed)
{
    AJ_Status status = AJ_OK;
    ClosedStatusProperties* props = NULL;

    if (!isClosed) {
        return AJ_ERR_INVALID;
    }

    props = (ClosedStatusProperties*)GetProperties(objPath, CLOSED_STATUS_INTERFACE);
    if (props) {
        *isClosed = props->isClosed;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_ClosedStatusInterfaceSetIsClosed(AJ_BusAttachment* busAttachment, const char* objPath, uint8_t* isClosed)
{
    AJ_Status status = AJ_OK;
    ClosedStatusProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!isClosed) {
        return AJ_ERR_INVALID;
    }

    props = (ClosedStatusProperties*)GetProperties(objPath, CLOSED_STATUS_INTERFACE);
    if (props) {
        if (props->isClosed != *isClosed) {
            props->isClosed = *isClosed;

            status = EmitPropChanged(busAttachment, objPath, "IsClosed", "b", &(props->isClosed));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}


