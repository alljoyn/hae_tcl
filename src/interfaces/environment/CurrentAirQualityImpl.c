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

#include <ajtcl/hae/interfaces/environment/CurrentAirQuality.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "CurrentAirQualityImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescEnvironmentCurrentAirQuality[] = {
    "$org.alljoyn.SmartSpaces.Environment.CurrentAirQuality",
    "@Version>q",
    "@ContaminantType>y",
    "@CurrentValue>d",
    "@MaxValue>d",
    "@MinValue>d",
    "@Precision>d",
    "@UpdateMinTime>q",
    NULL
};

typedef struct {
    uint16_t version;
    uint8_t contaminantType;
    double currentValue;
    double maxValue;
    double minValue;
    double precision;
    uint16_t updateMinTime;
} CurrentAirQualityProperties;

AJ_Status CreateCurrentAirQualityInterface(void** properties)
{
    *properties = malloc(sizeof(CurrentAirQualityProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(CurrentAirQualityProperties));

    ((CurrentAirQualityProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyCurrentAirQualityInterface(void* properties)
{
    if (properties) {
        CurrentAirQualityProperties* props = (CurrentAirQualityProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescEnvironmentCurrentAirQuality[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "q")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint16_t*)val);
        } else if (!strcmp(signature, "y")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint8_t*)val);
        } else if (!strcmp(signature, "d")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(double*)val);
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

AJ_Status CurrentAirQualityInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;
    CurrentAirQualityListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (CurrentAirQualityListener*)listener;
    }
    props = (CurrentAirQualityProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint8_t contaminantType;
            if (lt && lt->OnGetContaminantType) {
                status = lt->OnGetContaminantType(objPath, &contaminantType);
                if (status == AJ_OK) {
                    props->contaminantType = contaminantType;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->contaminantType);
        }
        break;
    case 2 :
        {
            double currentValue;
            if (lt && lt->OnGetCurrentValue) {
                status = lt->OnGetCurrentValue(objPath, &currentValue);
                if (status == AJ_OK) {
                    props->currentValue = currentValue;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "d", props->currentValue);
        }
        break;
    case 3 :
        {
            double maxValue;
            if (lt && lt->OnGetMaxValue) {
                status = lt->OnGetMaxValue(objPath, &maxValue);
                if (status == AJ_OK) {
                    props->maxValue = maxValue;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "d", props->maxValue);
        }
        break;
    case 4 :
        {
            double minValue;
            if (lt && lt->OnGetMinValue) {
                status = lt->OnGetMinValue(objPath, &minValue);
                if (status == AJ_OK) {
                    props->minValue = minValue;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "d", props->minValue);
        }
        break;
    case 5 :
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
    case 6 :
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

AJ_Status Hae_CurrentAirQualityInterfaceGetContaminantType(const char* objPath, uint8_t* contaminantType)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;

    if (!contaminantType) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentAirQualityProperties*)GetProperties(objPath, CURRENT_AIR_QUALITY_INTERFACE);
    if (props) {
        *contaminantType = props->contaminantType;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentAirQualityInterfaceSetContaminantType(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t contaminantType)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentAirQualityProperties*)GetProperties(objPath, CURRENT_AIR_QUALITY_INTERFACE);
    if (props) {
        if (props->contaminantType != contaminantType) {
            props->contaminantType = contaminantType;

            status = EmitPropChanged(busAttachment, objPath, "ContaminantType", "y", &(props->contaminantType));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentAirQualityInterfaceGetCurrentValue(const char* objPath, double* currentValue)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;

    if (!currentValue) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentAirQualityProperties*)GetProperties(objPath, CURRENT_AIR_QUALITY_INTERFACE);
    if (props) {
        *currentValue = props->currentValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentAirQualityInterfaceSetCurrentValue(AJ_BusAttachment* busAttachment, const char* objPath, const double currentValue)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentAirQualityProperties*)GetProperties(objPath, CURRENT_AIR_QUALITY_INTERFACE);
    if (props) {
        if (props->currentValue != currentValue) {
            props->currentValue = currentValue;

            status = EmitPropChanged(busAttachment, objPath, "CurrentValue", "d", &(props->currentValue));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentAirQualityInterfaceGetMaxValue(const char* objPath, double* maxValue)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;

    if (!maxValue) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentAirQualityProperties*)GetProperties(objPath, CURRENT_AIR_QUALITY_INTERFACE);
    if (props) {
        *maxValue = props->maxValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentAirQualityInterfaceSetMaxValue(AJ_BusAttachment* busAttachment, const char* objPath, const double maxValue)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentAirQualityProperties*)GetProperties(objPath, CURRENT_AIR_QUALITY_INTERFACE);
    if (props) {
        if (props->maxValue != maxValue) {
            props->maxValue = maxValue;

            status = EmitPropChanged(busAttachment, objPath, "MaxValue", "d", &(props->maxValue));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentAirQualityInterfaceGetMinValue(const char* objPath, double* minValue)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;

    if (!minValue) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentAirQualityProperties*)GetProperties(objPath, CURRENT_AIR_QUALITY_INTERFACE);
    if (props) {
        *minValue = props->minValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentAirQualityInterfaceSetMinValue(AJ_BusAttachment* busAttachment, const char* objPath, const double minValue)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentAirQualityProperties*)GetProperties(objPath, CURRENT_AIR_QUALITY_INTERFACE);
    if (props) {
        if (props->minValue != minValue) {
            props->minValue = minValue;

            status = EmitPropChanged(busAttachment, objPath, "MinValue", "d", &(props->minValue));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentAirQualityInterfaceGetPrecision(const char* objPath, double* precision)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;

    if (!precision) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentAirQualityProperties*)GetProperties(objPath, CURRENT_AIR_QUALITY_INTERFACE);
    if (props) {
        *precision = props->precision;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentAirQualityInterfaceSetPrecision(AJ_BusAttachment* busAttachment, const char* objPath, const double precision)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentAirQualityProperties*)GetProperties(objPath, CURRENT_AIR_QUALITY_INTERFACE);
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

AJ_Status Hae_CurrentAirQualityInterfaceGetUpdateMinTime(const char* objPath, uint16_t* updateMinTime)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;

    if (!updateMinTime) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentAirQualityProperties*)GetProperties(objPath, CURRENT_AIR_QUALITY_INTERFACE);
    if (props) {
        *updateMinTime = props->updateMinTime;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_CurrentAirQualityInterfaceSetUpdateMinTime(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t updateMinTime)
{
    AJ_Status status = AJ_OK;
    CurrentAirQualityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (CurrentAirQualityProperties*)GetProperties(objPath, CURRENT_AIR_QUALITY_INTERFACE);
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
