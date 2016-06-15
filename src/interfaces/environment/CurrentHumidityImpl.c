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

#include <ajtcl/hae/interfaces/environment/CurrentHumidity.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "CurrentHumidityImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescEnvironmentCurrentHumidity[] = {
    "$org.alljoyn.SmartSpaces.Environment.CurrentHumidity",
    "@Version>q",
    "@CurrentValue>y",
    "@MaxValue>y",
    NULL
};

typedef struct {
    uint16_t version;
    uint8_t currentValue;
    uint8_t maxValue;
} CurrentHumidityProperties;

AJ_Status CreateCurrentHumidityInterface(void** properties)
{
    *properties = malloc(sizeof(CurrentHumidityProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(CurrentHumidityProperties));

    ((CurrentHumidityProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}
void DestroyCurrentHumidityInterface(void* properties)
{
    if (properties) {
        CurrentHumidityProperties* props = (CurrentHumidityProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescEnvironmentCurrentHumidity[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "q")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint16_t*)val);
        } else if (!strcmp(signature, "y")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint8_t*)val);
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

AJ_Status CurrentHumidityInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    CurrentHumidityProperties* props = NULL;
    CurrentHumidityListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (CurrentHumidityListener*)listener;
    }
    props = (CurrentHumidityProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint8_t currentValue;
            if (lt && lt->OnGetCurrentValue) {
                status = lt->OnGetCurrentValue(objPath, &currentValue);
                if (status == AJ_OK) {
                    props->currentValue = currentValue;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->currentValue);
        }
        break;
    case 2 :
        {
            uint8_t maxValue;
            if (lt && lt->OnGetMaxValue) {
                status = lt->OnGetMaxValue(objPath, &maxValue);
                if (status == AJ_OK) {
                    props->maxValue = maxValue;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->maxValue);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_CurrentHumidityInterfaceGetCurrentValue(const char* objPath, uint8_t* currentValue)
{
    AJ_Status status = AJ_OK;
    CurrentHumidityProperties* props = NULL;

    if (!currentValue) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentHumidityProperties*)GetProperties(objPath, CURRENT_HUMIDITY_INTERFACE);
    if (props) {
        *currentValue = props->currentValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentHumidityInterfaceSetCurrentValue(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t currentValue)
{
    AJ_Status status = AJ_OK;
    CurrentHumidityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentHumidityProperties*)GetProperties(objPath, CURRENT_HUMIDITY_INTERFACE);
    if (props) {
        if (props->currentValue != currentValue) {
            props->currentValue = currentValue;

            status = EmitPropChanged(busAttachment, objPath, "CurrentValue", "y", &(props->currentValue));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentHumidityInterfaceGetMaxValue(const char* objPath, uint8_t* maxValue)
{
    AJ_Status status = AJ_OK;
    CurrentHumidityProperties* props = NULL;

    if (!maxValue) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentHumidityProperties*)GetProperties(objPath, CURRENT_HUMIDITY_INTERFACE);
    if (props) {
        *maxValue = props->maxValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentHumidityInterfaceSetMaxValue(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t maxValue)
{
    AJ_Status status = AJ_OK;
    CurrentHumidityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentHumidityProperties*)GetProperties(objPath, CURRENT_HUMIDITY_INTERFACE);
    if (props) {
        if (props->maxValue != maxValue) {
            props->maxValue = maxValue;

            status = EmitPropChanged(busAttachment, objPath, "MaxValue", "y", &(props->maxValue));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
