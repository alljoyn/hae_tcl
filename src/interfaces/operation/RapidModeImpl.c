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
#include <ajtcl/hae/interfaces/operation/RapidMode.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "RapidModeImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationRapidMode[] = {
    "$org.alljoyn.SmartSpaces.Operation.RapidMode",
    "@Version>q",
    "@RapidMode=b",
    NULL
};

typedef struct {
    uint16_t version;
    bool rapidMode;
} RapidModeProperties;

AJ_Status CreateRapidModeInterface(void** properties)
{
    *properties = malloc(sizeof(RapidModeProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((RapidModeProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyRapidModeInterface(void* properties)
{
    if (properties) {
        RapidModeProperties* props = (RapidModeProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescOperationRapidMode[0]+1); //To remove '$'
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

AJ_Status RapidModeInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    RapidModeProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (RapidModeProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "RapidMode", "b", &(props->rapidMode));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status RapidModeInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    RapidModeProperties* props = NULL;
    RapidModeListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (RapidModeListener*)listener;
    }
    props = (RapidModeProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            bool rapidMode;

            if (lt && lt->OnGetRapidMode) {
                status = lt->OnGetRapidMode(objPath, &rapidMode);
                if (status == AJ_OK) {
                    props->rapidMode = rapidMode;
                }
            }
            if (!props->rapidMode) {
                return AJ_ERR_NULL;
            }
            status = AJ_MarshalArgs(replyMsg, "b", props->rapidMode);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status RapidModeInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    RapidModeProperties* props = (RapidModeProperties*)properties;
    RapidModeListener* lt = (RapidModeListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetRapidMode) {
            status = AJ_ERR_NULL;
        } else {
            bool rapidMode = 0;
            status = AJ_UnmarshalArgs(replyMsg, "b", &rapidMode);

            if (status == AJ_OK) {
                status = lt->OnSetRapidMode(objPath, rapidMode);

                if (status == AJ_OK) {
                    if (props->rapidMode != rapidMode) {
                        props->rapidMode = rapidMode;
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

AJ_Status Hae_RapidModeInterfaceGetRapidMode(const char* objPath, bool* rapidMode)
{
    AJ_Status status = AJ_OK;
    RapidModeProperties* props = NULL;

    if (!rapidMode) {
        return AJ_ERR_INVALID;
    }

    props = (RapidModeProperties*)GetProperties(objPath, RAPID_MODE_INTERFACE);
    if (props) {
        *rapidMode = props->rapidMode;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_RapidModeInterfaceSetRapidMode(AJ_BusAttachment* busAttachment, const char* objPath, const bool rapidMode)
{
    AJ_Status status = AJ_OK;
    RapidModeProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (RapidModeProperties*)GetProperties(objPath, RAPID_MODE_INTERFACE);
    if (props) {
        if (props->rapidMode != rapidMode) {
            props->rapidMode = rapidMode;

            status = EmitPropChanged(busAttachment, objPath, "RapidMode", "b", &(props->rapidMode));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
