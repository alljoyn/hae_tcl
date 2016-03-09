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
#include <ajtcl/hae/interfaces/operation/EnergyUsage.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "EnergyUsageImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationEnergyUsage[] = {
    "$org.alljoyn.SmartSpaces.Operation.EnergyUsage",
    "@Version>q",
    "@CumulativeEnergy>d",
    "@Precision>d",
    "@UpdateMinTime>q",
    "?ResetCumulativeEnergy",
    NULL
};

typedef struct {
    uint16_t version;
    double cumulativeEnergy;
    double precision;
    uint16_t updateMinTime;
} EnergyUsageProperties;

AJ_Status CreateEnergyUsageInterface(void** properties)
{
    *properties = malloc(sizeof(EnergyUsageProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((EnergyUsageProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyEnergyUsageInterface(void* properties)
{
    if (properties) {
        EnergyUsageProperties* props = (EnergyUsageProperties*)properties;

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

    EnergyUsageProperties* props = NULL;

    props = (EnergyUsageProperties*)GetProperties(objPath, ENERGY_USAGE_INTERFACE);
    if (!props) {
        status = AJ_ERR_NO_MATCH;
    }


    status = MakePropChangedId(objPath, &msgId);
    if (status == AJ_OK) {
        AJ_MarshalSignal(busAttachment, &msg, msgId, NULL, 0, AJ_FLAG_GLOBAL_BROADCAST, 0);
        AJ_MarshalArgs(&msg, "s", intfDescOperationEnergyUsage[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "d")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(double*)val);
        } else if (!strcmp(signature, "q")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, (uint16_t*)val);
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

AJ_Status EnergyUsageInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    EnergyUsageProperties* props = NULL;
    EnergyUsageListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (EnergyUsageListener*)listener;
    }
    props = (EnergyUsageProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            double cumulativeEnergy;

            if (lt && lt->OnGetCumulativeEnergy) {
                status = lt->OnGetCumulativeEnergy(objPath, &cumulativeEnergy);
                 if (status == AJ_OK) {
                      props->cumulativeEnergy = cumulativeEnergy;
                 }
            }

            status = AJ_MarshalArgs(replyMsg, "d", props->cumulativeEnergy);
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

AJ_Status EnergyUsageInterfaceOnMethodHandler(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    EnergyUsageListener* lt = (EnergyUsageListener*)listener;

    switch (memberIndex) {
    case 4 :
        if (!lt->OnResetCumulativeEnergy) {
            status = AJ_ERR_NULL;
        } else {
            status = lt->OnResetCumulativeEnergy(objPath);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_EnergyUsageInterfaceGetCumulativeEnergy(const char* objPath, double* cumulativeEnergy)
{
    AJ_Status status = AJ_OK;
    EnergyUsageProperties* props = NULL;

    props = (EnergyUsageProperties*)GetProperties(objPath, ENERGY_USAGE_INTERFACE);
    if (props) {
      *cumulativeEnergy = props->cumulativeEnergy;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_EnergyUsageInterfaceSetCumulativeEnergy(AJ_BusAttachment* busAttachment, const char* objPath, const double cumulativeEnergy)
{
    AJ_Status status = AJ_OK;
    EnergyUsageProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (EnergyUsageProperties*)GetProperties(objPath, ENERGY_USAGE_INTERFACE);
    if (props) {
        if (props->cumulativeEnergy != cumulativeEnergy) {
            props->cumulativeEnergy = cumulativeEnergy;

            status = EmitPropChanged(busAttachment, objPath, "CumulativeEnergy", "d", &(props->cumulativeEnergy));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_EnergyUsageInterfaceGetPrecision(const char* objPath, double* precision)
{
    AJ_Status status = AJ_OK;
    EnergyUsageProperties* props = NULL;

    props = (EnergyUsageProperties*)GetProperties(objPath, ENERGY_USAGE_INTERFACE);
    if (props) {
      *precision = props->precision;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_EnergyUsageInterfaceSetPrecision(AJ_BusAttachment* busAttachment, const char* objPath, const double precision)
{
    AJ_Status status = AJ_OK;
    EnergyUsageProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (EnergyUsageProperties*)GetProperties(objPath, ENERGY_USAGE_INTERFACE);
    if (props) {
        if (props->precision != precision) {
            props->precision = precision;

            status = EmitPropChanged(busAttachment, objPath, "Precision", "d", &(props->precision));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_EnergyUsageInterfaceGetUpdateMinTime(const char* objPath, uint16_t* updateMinTime)
{
    AJ_Status status = AJ_OK;
    EnergyUsageProperties* props = NULL;

    props = (EnergyUsageProperties*)GetProperties(objPath, ENERGY_USAGE_INTERFACE);
    if (props) {
      *updateMinTime = props->updateMinTime;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_EnergyUsageInterfaceSetUpdateMinTime(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t updateMinTime)
{
    AJ_Status status = AJ_OK;
    EnergyUsageProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (EnergyUsageProperties*)GetProperties(objPath, ENERGY_USAGE_INTERFACE);
    if (props) {
        if (props->updateMinTime != updateMinTime) {
            props->updateMinTime = updateMinTime;

            status = EmitPropChanged(busAttachment, objPath, "UpdateMinTime", "q", &(props->updateMinTime));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
