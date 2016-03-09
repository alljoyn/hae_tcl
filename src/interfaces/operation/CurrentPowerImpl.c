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

#include <ajtcl/hae/interfaces/operation/CurrentPower.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "CurrentPowerImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationCurrentPower[] = {
    "$org.alljoyn.SmartSpaces.Operation.CurrentPower",
    "@Version>q",
    "@CurrentPower>d",
    "@Precision>d",
    "@UpdateMinTime>q",
    NULL
};

typedef struct {
    uint16_t version;
    double currentPower;
    double precision;
    uint16_t updateMinTime;
} CurrentPowerProperties;

AJ_Status CreateCurrentPowerInterface(void** properties)
{
    *properties = malloc(sizeof(CurrentPowerProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((CurrentPowerProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyCurrentPowerInterface(void* properties)
{
    if (properties) {
        CurrentPowerProperties* props = (CurrentPowerProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescOperationCurrentPower[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "d")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(double*)val);
        } else if (!strcmp(signature, "q")) {
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

AJ_Status CurrentPowerInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    CurrentPowerProperties* props = NULL;
    CurrentPowerListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (CurrentPowerListener*)listener;
    }
    props = (CurrentPowerProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            double currentPower;

            if (lt && lt->OnGetCurrentPower) {
                status = lt->OnGetCurrentPower(objPath, &currentPower);
                if (status == AJ_OK) {
                    props->currentPower = currentPower;
                }
            }

            status = AJ_MarshalArgs(replyMsg, "d", props->currentPower);
        }
        break;
    case 2 :
        {
            double precision;

            if (lt && lt->OnGetPrecision) {
                status = lt->OnGetPrecision(objPath, &precision);
                if (status == AJ_OK) {
                    props->precision = precision;
                }
            }

            status = AJ_MarshalArgs(replyMsg, "d", props->precision);
        }
        break;
    case 3 :
        {
            uint16_t updateMinTime;

            if (lt && lt->OnGetUpdateMinTime) {
                status = lt->OnGetUpdateMinTime(objPath, &updateMinTime);
                if (status == AJ_OK) {
                    props->updateMinTime = updateMinTime;
                }
            }

            status = AJ_MarshalArgs(replyMsg, "q", props->updateMinTime);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}


AJ_Status Hae_CurrentPowerInterfaceGetCurrentPower(const char* objPath, double* currentPower)
{
    AJ_Status status = AJ_OK;
    CurrentPowerProperties* props = NULL;

    if (!currentPower) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentPowerProperties*)GetProperties(objPath, CURRENT_POWER_INTERFACE);
    if (props) {
        *currentPower = props->currentPower;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentPowerInterfaceSetCurrentPower(AJ_BusAttachment* busAttachment, const char* objPath, double currentPower)
{
    AJ_Status status = AJ_OK;
    CurrentPowerProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentPowerProperties*)GetProperties(objPath, CURRENT_POWER_INTERFACE);
    if (props) {
        props->currentPower = currentPower;

        status = EmitPropChanged(busAttachment, objPath, "CurrentPower", "d", &(props->currentPower));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentPowerInterfaceGetPrecision(const char* objPath, double* precision)
{
    AJ_Status status = AJ_OK;
    CurrentPowerProperties* props = NULL;

    if (!precision) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentPowerProperties*)GetProperties(objPath, CURRENT_POWER_INTERFACE);
    if (props) {
        *precision = props->precision;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentPowerInterfaceSetPrecision(AJ_BusAttachment* busAttachment, const char* objPath, double precision)
{
    AJ_Status status = AJ_OK;
    CurrentPowerProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentPowerProperties*)GetProperties(objPath, CURRENT_POWER_INTERFACE);
    if (props) {
        props->precision = precision;

        status = EmitPropChanged(busAttachment, objPath, "Precision", "d", &(props->precision));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentPowerInterfaceGetUpdateMinTime(const char* objPath, uint16_t* updateMinTime)
{
    AJ_Status status = AJ_OK;
    CurrentPowerProperties* props = NULL;

    if (!updateMinTime) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentPowerProperties*)GetProperties(objPath, CURRENT_POWER_INTERFACE);
    if (props) {
        *updateMinTime = props->updateMinTime;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentPowerInterfaceSetUpdateMinTime(AJ_BusAttachment* busAttachment, const char* objPath, uint16_t updateMinTime)
{
    AJ_Status status = AJ_OK;
    CurrentPowerProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentPowerProperties*)GetProperties(objPath, CURRENT_POWER_INTERFACE);
    if (props) {
        props->updateMinTime = updateMinTime;

        status = EmitPropChanged(busAttachment, objPath, "UpdateMinTime", "q", &(props->updateMinTime));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
