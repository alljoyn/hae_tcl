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

#include <string.h>
#include <ajtcl/hae/interfaces/operation/OnOffStatus.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "OnOffStatusImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationOnOffStatus[] = {
    "$org.alljoyn.SmartSpaces.Operation.OnOffStatus",
    "@Version>q",
    "@OnOff>b",
    NULL
};

typedef struct {
    uint16_t version;
    bool onOff;
} OnOffStatusProperties;

AJ_Status CreateOnOffStatusInterface(void** properties)
{
    *properties = malloc(sizeof(OnOffStatusProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((OnOffStatusProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyOnOffStatusInterface(void* properties)
{
    if (properties) {
        OnOffStatusProperties* props = (OnOffStatusProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescOperationOnOffStatus[0]+1); //To remove '$'
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

AJ_Status OnOffStatusInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    OnOffStatusProperties* props = NULL;
    OnOffStatusListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (OnOffStatusListener*)listener;
    }
    props = (OnOffStatusProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            bool onOff;

            if (lt && lt->OnGetOnOff) {
                status = lt->OnGetOnOff(objPath, &onOff);
                if (status == AJ_OK) {
                     props->onOff = onOff;
                }
            }

            if (!props->onOff) {
                return AJ_ERR_NULL;
            }
            status = AJ_MarshalArgs(replyMsg, "b", props->onOff);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_OnOffStatusInterfaceGetOnOff(const char* objPath, bool* onOff)
{
    AJ_Status status = AJ_OK;
    OnOffStatusProperties* props = NULL;

    if (!onOff) {
        return AJ_ERR_INVALID;
    }

    props = (OnOffStatusProperties*)GetProperties(objPath, ON_OFF_STATUS_INTERFACE);
    if (props) {
      *onOff = props->onOff;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_OnOffStatusInterfaceSetOnOff(AJ_BusAttachment* busAttachment, const char* objPath, bool onOff)
{
    AJ_Status status = AJ_OK;
    OnOffStatusProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (OnOffStatusProperties*)GetProperties(objPath, ON_OFF_STATUS_INTERFACE);
    if (props) {
        props->onOff = onOff;
        status = EmitPropChanged(busAttachment, objPath, "OnOff", "b", &(props->onOff));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
