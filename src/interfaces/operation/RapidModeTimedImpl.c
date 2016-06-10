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

#include <ajtcl/hae/interfaces/operation/RapidModeTimed.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "RapidModeTimedImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationRapidModeTimed[] = {
    "$org.alljoyn.SmartSpaces.Operation.RapidModeTimed",
    "@Version>q",
    "@RapidModeMinutesRemaining=q",
    "@MaxSetMinutes>q",
    NULL
};

typedef struct {
    uint16_t version;
    uint16_t rapidModeMinutesRemaining;
    uint16_t maxSetMinutes;
    bool maxSetMinutesInit;
} RapidModeTimedProperties;

AJ_Status RapidModeTimedCheckRapidModeMinutesRemainingForValidation(const char* objPath, uint16_t value)
{
    AJ_Status status = AJ_OK;
    RapidModeTimedProperties* props = NULL;

    props = (RapidModeTimedProperties*)GetProperties(objPath, RAPID_MODE_TIMED_INTERFACE);
    if (props) {
        if (value > props->maxSetMinutes) {
            status = AJ_ERR_RANGE;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status CreateRapidModeTimedInterface(void** properties)
{
    *properties = malloc(sizeof(RapidModeTimedProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((RapidModeTimedProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;
    ((RapidModeTimedProperties*)*properties)->maxSetMinutesInit = false;

    return AJ_OK;
}

void DestroyRapidModeTimedInterface(void* properties)
{
    if (properties) {
        RapidModeTimedProperties* props = (RapidModeTimedProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescOperationRapidModeTimed[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "q")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint16_t*)val);
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

AJ_Status RapidModeTimedInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    RapidModeTimedProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (RapidModeTimedProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "RapidModeMinutesRemaining", "q", &(props->rapidModeMinutesRemaining));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status RapidModeTimedInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    RapidModeTimedProperties* props = NULL;
    RapidModeTimedListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (RapidModeTimedListener*)listener;
    }
    props = (RapidModeTimedProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint16_t value;
            if (lt && lt->OnGetRapidModeMinutesRemaining) {
                status = lt->OnGetRapidModeMinutesRemaining(objPath, &value);
                if (status == AJ_OK) {
                    props->rapidModeMinutesRemaining = value;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "q", props->rapidModeMinutesRemaining);
        }
        break;
    case 2 :
        {
            uint16_t value;
            if (lt && lt->OnGetMaxSetMinutes) {
                status = lt->OnGetMaxSetMinutes(objPath, &value);
                if (status == AJ_OK) {
                    props->maxSetMinutes = value;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "q", props->maxSetMinutes);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status RapidModeTimedInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    RapidModeTimedProperties* props = (RapidModeTimedProperties*)properties;
    RapidModeTimedListener* lt = (RapidModeTimedListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetRapidModeMinutesRemaining) {
            status = AJ_ERR_NULL;
        } else {
            uint16_t value = 0;
            status = AJ_UnmarshalArgs(replyMsg, "q", &value);

            if (status == AJ_OK) {
                status = RapidModeTimedCheckRapidModeMinutesRemainingForValidation(objPath, value);
                if (status != AJ_OK) {
                    return status;
                }

                status = lt->OnSetRapidModeMinutesRemaining(objPath, value);
                if (status == AJ_OK) {
                    if (props->rapidModeMinutesRemaining != value) {
                        props->rapidModeMinutesRemaining = value;
                        *propChanged = true;
                    }
                }
            }
        }
        break;
    case 2 :
        status = AJ_ERR_DISALLOWED;
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_RapidModeTimedInterfaceGetRapidModeMinutesRemaining(const char* objPath, uint16_t* value)
{
    AJ_Status status = AJ_OK;
    RapidModeTimedProperties* props = NULL;

    if (!value) {
        return AJ_ERR_INVALID;
    }

    props = (RapidModeTimedProperties*)GetProperties(objPath, RAPID_MODE_TIMED_INTERFACE);
    if (props) {
        *value = props->rapidModeMinutesRemaining;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_RapidModeTimedInterfaceSetRapidModeMinutesRemaining(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t value)
{
    AJ_Status status = AJ_OK;
    RapidModeTimedProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = RapidModeTimedCheckRapidModeMinutesRemainingForValidation(objPath, value);

    if (status != AJ_OK) {
        return status;
    }

    props = (RapidModeTimedProperties*)GetProperties(objPath, RAPID_MODE_TIMED_INTERFACE);
    if (props) {
        if (props->rapidModeMinutesRemaining != value) {
            props->rapidModeMinutesRemaining = value;

            status = EmitPropChanged(busAttachment, objPath, "RapidModeMinutesRemaining", "q", &(props->rapidModeMinutesRemaining));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_RapidModeTimedInterfaceGetMaxSetMinutes(const char* objPath, uint16_t* value)
{
    AJ_Status status = AJ_OK;
    RapidModeTimedProperties* props = NULL;

    if (!value) {
        return AJ_ERR_INVALID;
    }

    props = (RapidModeTimedProperties*)GetProperties(objPath, RAPID_MODE_TIMED_INTERFACE);
    if (props) {
        *value = props->maxSetMinutes;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_RapidModeTimedInterfaceSetMaxSetMinutes(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t value)
{
    AJ_Status status = AJ_OK;
    RapidModeTimedProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (RapidModeTimedProperties*)GetProperties(objPath, RAPID_MODE_TIMED_INTERFACE);
    if (props) {
        if (props->maxSetMinutesInit) {
            return AJ_ERR_FAILURE;
        } else {
            props->maxSetMinutes = value;
            props->maxSetMinutesInit = true;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
