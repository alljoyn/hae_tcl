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

#include <math.h>
#include <ajtcl/hae/interfaces/environment/TargetTemperature.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "TargetTemperatureImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescEnvironmentTargetTemperature[] = {
    "$org.alljoyn.SmartSpaces.Environment.TargetTemperature",
    "@Version>q",
    "@TargetValue=d",
    "@MinValue>d",
    "@MaxValue>d",
    "@StepValue>d",
    NULL
};

typedef struct {
    uint16_t version;
    double targetValue;
    double minValue;
    double maxValue;
    double stepValue;
} TargetTemperatureProperties;

AJ_Status TargetTemperatureTargetValueValidationCheck(const char* objPath, double targetValue)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureProperties* props = NULL;

    props = (TargetTemperatureProperties*)GetProperties(objPath, TARGET_TEMPERATURE_INTERFACE);
    if (props) {
        if (targetValue < props->minValue || targetValue > props->maxValue) {
            status = AJ_ERR_RANGE;
        } else if (props->stepValue != 0 && fmod(targetValue - props->minValue, props->stepValue) != 0.0) {
            status = AJ_ERR_INVALID;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status TargetTemperatureAdjustTargetValue(const char* objPath, double* targetValue)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureProperties* props = NULL;

    props = (TargetTemperatureProperties*)GetProperties(objPath, TARGET_TEMPERATURE_INTERFACE);
    if (props) {
        if (props->stepValue == 0.0) {
            return status;
        } else {
            double div = *targetValue / props->stepValue;
            double value = floor(div + 0.5) * props->stepValue;

            *targetValue = (value < props->minValue) ? props->minValue : (value > props->maxValue ? props->maxValue : value);
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status CreateTargetTemperatureInterface(void** properties)
{
    *properties = malloc(sizeof(TargetTemperatureProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((TargetTemperatureProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}
void DestroyTargetTemperatureInterface(void* properties)
{
    if (properties) {
        TargetTemperatureProperties* props = (TargetTemperatureProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescEnvironmentTargetTemperature[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "d")) {
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

AJ_Status TargetTemperatureInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "TargetValue", "d", &(props->targetValue));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status TargetTemperatureInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureProperties* props = NULL;
    TargetTemperatureListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (TargetTemperatureListener*)listener;
    }
    props = (TargetTemperatureProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            double targetValue;
            if (lt && lt->OnGetTargetValue) {
                status = lt->OnGetTargetValue(objPath, &targetValue);
                if (status == AJ_OK) {
                    props->targetValue = targetValue;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "d", props->targetValue);
        }
        break;
    case 2 :
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
            double stepValue;
            if (lt && lt->OnGetStepValue) {
                status = lt->OnGetStepValue(objPath, &stepValue);
                if (status == AJ_OK) {
                    props->stepValue = stepValue;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "d", props->stepValue);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status TargetTemperatureInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    TargetTemperatureProperties* props = (TargetTemperatureProperties*)properties;
    TargetTemperatureListener* lt = (TargetTemperatureListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetTargetValue) {
            status = AJ_ERR_NULL;
        } else {
            double targetValue = 0;
            status = AJ_UnmarshalArgs(replyMsg, "d", &targetValue);

            if (status == AJ_OK) {
                status = TargetTemperatureAdjustTargetValue(objPath, &targetValue);
                if (status != AJ_OK) {
                    return AJ_OK;
                }

                status = lt->OnSetTargetValue(objPath, targetValue);

                if (status == AJ_OK) {
                    if (props->targetValue != targetValue) {
                        props->targetValue = targetValue;
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

AJ_Status Hae_TargetTemperatureInterfaceGetTargetValue(const char* objPath, double* targetValue)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureProperties* props = NULL;

    if (!targetValue) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureProperties*)GetProperties(objPath, TARGET_TEMPERATURE_INTERFACE);
    if (props) {
        *targetValue = props->targetValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetTemperatureInterfaceSetTargetValue(AJ_BusAttachment* busAttachment, const char* objPath, const double targetValue)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = TargetTemperatureTargetValueValidationCheck(objPath, targetValue);
    if (status != AJ_OK) {
        return status;
    }

    props = (TargetTemperatureProperties*)GetProperties(objPath, TARGET_TEMPERATURE_INTERFACE);
    if (props) {
        if (props->targetValue != targetValue) {
            props->targetValue = targetValue;

            status = EmitPropChanged(busAttachment, objPath, "TargetValue", "d", &(props->targetValue));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetTemperatureInterfaceGetMinValue(const char* objPath, double* minValue)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureProperties* props = NULL;

    if (!minValue) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureProperties*)GetProperties(objPath, TARGET_TEMPERATURE_INTERFACE);
    if (props) {
        *minValue = props->minValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetTemperatureInterfaceSetMinValue(AJ_BusAttachment* busAttachment, const char* objPath, const double minValue)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureProperties*)GetProperties(objPath, TARGET_TEMPERATURE_INTERFACE);
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

AJ_Status Hae_TargetTemperatureInterfaceGetMaxValue(const char* objPath, double* maxValue)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureProperties* props = NULL;

    if (!maxValue) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureProperties*)GetProperties(objPath, TARGET_TEMPERATURE_INTERFACE);
    if (props) {
        *maxValue = props->maxValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetTemperatureInterfaceSetMaxValue(AJ_BusAttachment* busAttachment, const char* objPath, const double maxValue)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureProperties*)GetProperties(objPath, TARGET_TEMPERATURE_INTERFACE);
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

AJ_Status Hae_TargetTemperatureInterfaceGetStepValue(const char* objPath, double* stepValue)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureProperties* props = NULL;

    if (!stepValue) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureProperties*)GetProperties(objPath, TARGET_TEMPERATURE_INTERFACE);
    if (props) {
        *stepValue = props->stepValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetTemperatureInterfaceSetStepValue(AJ_BusAttachment* busAttachment, const char* objPath, const double stepValue)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureProperties*)GetProperties(objPath, TARGET_TEMPERATURE_INTERFACE);
    if (props) {
        if (props->stepValue != stepValue) {
            props->stepValue = stepValue;

            status = EmitPropChanged(busAttachment, objPath, "StepValue", "d", &(props->stepValue));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
