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
#include <ajtcl/cdm/interfaces/operation/RepeatMode.h>
#include "../../CdmControllee/CdmControlleeImpl.h"
#include "RepeatModeImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationRepeatMode[] = {
    "$org.alljoyn.SmartSpaces.Operation.RepeatMode",
    "@Version>q",
    "@RepeatMode=b",
    NULL
};

typedef struct {
    uint16_t version;
    bool repeatMode;
} RepeatModeProperties;

AJ_Status CreateRepeatModeInterface(void** properties)
{
    *properties = malloc(sizeof(RepeatModeProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(RepeatModeProperties));

    ((RepeatModeProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyRepeatModeInterface(void* properties)
{
    if (properties) {
        RepeatModeProperties* props = (RepeatModeProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescOperationRepeatMode[0]+1); //To remove '$'
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

AJ_Status RepeatModeInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    RepeatModeProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (RepeatModeProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "RepeatMode", "b", &(props->repeatMode));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status RepeatModeInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    RepeatModeProperties* props = NULL;
    RepeatModeListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (RepeatModeListener*)listener;
    }
    props = (RepeatModeProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            bool repeatMode;
            if (lt && lt->OnGetRepeatMode) {
                status = lt->OnGetRepeatMode(objPath, &repeatMode);
                if (status == AJ_OK) {
                    props->repeatMode = repeatMode;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "b", props->repeatMode);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status RepeatModeInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    RepeatModeProperties* props = (RepeatModeProperties*)properties;
    RepeatModeListener* lt = (RepeatModeListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetRepeatMode) {
            status = AJ_ERR_NULL;
        } else {
            bool repeatMode = 0;
            status = AJ_UnmarshalArgs(replyMsg, "b", &repeatMode);

            if (status == AJ_OK) {
                status = lt->OnSetRepeatMode(objPath, repeatMode);

                if (status == AJ_OK) {
                    if (props->repeatMode != repeatMode) {
                        props->repeatMode = repeatMode;
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

AJ_Status Cdm_RepeatModeInterfaceGetRepeatMode(const char* objPath, bool* repeatMode)
{
    AJ_Status status = AJ_OK;
    RepeatModeProperties* props = NULL;

    if (!repeatMode) {
        return AJ_ERR_INVALID;
    }

    props = (RepeatModeProperties*)GetProperties(objPath, REPEAT_MODE_INTERFACE);
    if (props) {
        *repeatMode = props->repeatMode;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_RepeatModeInterfaceSetRepeatMode(AJ_BusAttachment* busAttachment, const char* objPath, const bool repeatMode)
{
    AJ_Status status = AJ_OK;
    RepeatModeProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!repeatMode) {
        return AJ_ERR_INVALID;
    }

    props = (RepeatModeProperties*)GetProperties(objPath, REPEAT_MODE_INTERFACE);
    if (props) {
        if (props->repeatMode != repeatMode) {
            props->repeatMode = repeatMode;

            status = EmitPropChanged(busAttachment, objPath, "RepeatMode", "b", &(props->repeatMode));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
