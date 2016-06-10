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
#include <ajtcl/hae/interfaces/environment/TargetHumidity.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "TargetHumidityImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescEnvironmentTargetHumidity[] = {
    "$org.alljoyn.SmartSpaces.Environment.TargetHumidity",
    "@Version>q",
    "@TargetValue=y",
    "@MinValue>y",
    "@MaxValue>y",
    "@StepValue>y",
    "@SelectableHumidityLevels>ay",
    NULL
};

typedef struct {
    uint16_t version;
    uint8_t targetValue;
    uint8_t minValue;
    uint8_t maxValue;
    uint8_t stepValue;
    uint8_t* selectableHumidityLevels;
} TargetHumidityProperties;
static size_t selectableHumidityLevelsSize = 0;

AJ_Status TargetHumidityTargetValueValidationCheck(const char* objPath, uint8_t targetValue)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    props = (TargetHumidityProperties*)GetProperties(objPath, TARGET_HUMIDITY_INTERFACE);
    if (props) {
        if (props->maxValue != props->minValue) {
            if (targetValue < props->minValue || targetValue > props->maxValue) {
                status = AJ_ERR_RANGE;
            } else if (props->stepValue != 0 && (targetValue - props->minValue)%props->stepValue != 0) {
                status = AJ_ERR_INVALID;
            }
        } else {
            int i;
            for (i = 0; i < selectableHumidityLevelsSize; ++i) {
                if (props->selectableHumidityLevels[i] == targetValue)
                    return AJ_OK;
            }
            status = AJ_ERR_NO_MATCH;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status TargetHumidityAdjustTargetValue(const char* objPath, uint8_t* targetValue)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    props = (TargetHumidityProperties*)GetProperties(objPath, TARGET_HUMIDITY_INTERFACE);
    if (props) {
        if (props->maxValue != props->minValue) {
            if (props->stepValue == 0) {
                return status;
            } else {
                uint8_t div = *targetValue / props->stepValue;
                uint8_t value = div * props->stepValue;

                *targetValue = (value < props->minValue) ? props->minValue : (value > props->maxValue ? props->maxValue : value);
            }
        } else {
            return status;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status CreateTargetHumidityInterface(void** properties)
{
    *properties = malloc(sizeof(TargetHumidityProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((TargetHumidityProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}
void DestroyTargetHumidityInterface(void* properties)
{
    if (properties) {
        TargetHumidityProperties* props = (TargetHumidityProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescEnvironmentTargetHumidity[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "q")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint16_t*)val);
        } else if (!strcmp(signature, "y")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint8_t*)val);
        } else if (!strcmp(signature, "ay")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, (uint8_t*)val, selectableHumidityLevelsSize);
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

AJ_Status TargetHumidityInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (TargetHumidityProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "TargetValue", "y", &(props->targetValue));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status TargetHumidityInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;
    TargetHumidityListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (TargetHumidityListener*)listener;
    }
    props = (TargetHumidityProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint8_t targetValue;
            if (lt && lt->OnGetTargetValue) {
                status = lt->OnGetTargetValue(objPath, &targetValue);
                if (status == AJ_OK) {
                    props->targetValue = targetValue;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->targetValue);
        }
        break;
    case 2 :
        {
            uint8_t minValue;
            if (lt && lt->OnGetMinValue) {
                status = lt->OnGetMinValue(objPath, &minValue);
                if (status == AJ_OK) {
                    props->minValue = minValue;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->minValue);
        }
        break;
     case 3 :
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
    case 4 :
        {
            uint8_t stepValue;
            if (lt && lt->OnGetStepValue) {
                status = lt->OnGetStepValue(objPath, &stepValue);
                if (status == AJ_OK) {
                    props->stepValue = stepValue;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->stepValue);
        }
        break;
    case 5 :
        {
            uint8_t* selectableHumidityLevels = (uint8_t*)malloc(sizeof(uint8_t) * selectableHumidityLevelsSize);
            if (lt && lt->OnGetSelectableHumidityLevels) {
                status = lt->OnGetSelectableHumidityLevels(objPath, selectableHumidityLevels);
                if (status == AJ_OK) {
                    if (props->selectableHumidityLevels) {
                        free(props->selectableHumidityLevels);
                    }
                    props->selectableHumidityLevels = (uint8_t*)malloc(sizeof(uint8_t) * selectableHumidityLevelsSize);

                    memcpy(props->selectableHumidityLevels, selectableHumidityLevels, sizeof(uint8_t) * selectableHumidityLevelsSize);
                }
            }
            if (!props->selectableHumidityLevels) {
                free(selectableHumidityLevels);
                return AJ_ERR_NULL;
            }

            status = AJ_MarshalArgs(replyMsg, "ay", props->selectableHumidityLevels, sizeof(uint8_t) * selectableHumidityLevelsSize);
            free(selectableHumidityLevels);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status TargetHumidityInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    TargetHumidityProperties* props = (TargetHumidityProperties*)properties;
    TargetHumidityListener* lt = (TargetHumidityListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetTargetValue) {
            status = AJ_ERR_NULL;
        } else {
            uint8_t targetValue = 0;
            status = AJ_UnmarshalArgs(replyMsg, "y", &targetValue);

            if (status == AJ_OK) {
                TargetHumidityAdjustTargetValue(objPath, &targetValue);
                status = TargetHumidityTargetValueValidationCheck(objPath, targetValue);
                if (status != AJ_OK) {
                    return status;
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
    case 3 :
    case 4 :
    case 5 :
    case 6 :
        status = AJ_ERR_DISALLOWED;
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_TargetHumidityInterfaceGetTargetValue(const char* objPath, uint8_t* targetValue)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    if (!targetValue) {
        return AJ_ERR_INVALID;
    }

    props = (TargetHumidityProperties*)GetProperties(objPath, TARGET_HUMIDITY_INTERFACE);
    if (props) {
        *targetValue = props->targetValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetHumidityInterfaceSetTargetValue(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t targetValue)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = TargetHumidityTargetValueValidationCheck(objPath, targetValue);
    if (status != AJ_OK) {
        return status;
    }

    props = (TargetHumidityProperties*)GetProperties(objPath, TARGET_HUMIDITY_INTERFACE);
    if (props) {
        if (props->targetValue != targetValue) {
            props->targetValue = targetValue;

            status = EmitPropChanged(busAttachment, objPath, "TargetValue", "y", &(props->targetValue));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetHumidityInterfaceGetMinValue(const char* objPath, uint8_t* minValue)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    if (!minValue) {
        return AJ_ERR_INVALID;
    }

    props = (TargetHumidityProperties*)GetProperties(objPath, TARGET_HUMIDITY_INTERFACE);
    if (props) {
        *minValue = props->minValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetHumidityInterfaceSetMinValue(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t minValue)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TargetHumidityProperties*)GetProperties(objPath, TARGET_HUMIDITY_INTERFACE);
    if (props) {
        if (props->minValue != minValue) {
            props->minValue = minValue;

            status = EmitPropChanged(busAttachment, objPath, "MinValue", "y", &(props->minValue));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetHumidityInterfaceGetMaxValue(const char* objPath, uint8_t* maxValue)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    if (!maxValue) {
        return AJ_ERR_INVALID;
    }

    props = (TargetHumidityProperties*)GetProperties(objPath, TARGET_HUMIDITY_INTERFACE);
    if (props) {
        *maxValue = props->maxValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetHumidityInterfaceSetMaxValue(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t maxValue)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TargetHumidityProperties*)GetProperties(objPath, TARGET_HUMIDITY_INTERFACE);
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

AJ_Status Hae_TargetHumidityInterfaceGetStepValue(const char* objPath, uint8_t* stepValue)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    if (!stepValue) {
        return AJ_ERR_INVALID;
    }

    props = (TargetHumidityProperties*)GetProperties(objPath, TARGET_HUMIDITY_INTERFACE);
    if (props) {
        *stepValue = props->stepValue;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetHumidityInterfaceSetStepValue(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t stepValue)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TargetHumidityProperties*)GetProperties(objPath, TARGET_HUMIDITY_INTERFACE);
    if (props) {
        if (props->stepValue != stepValue) {
            props->stepValue = stepValue;

            status = EmitPropChanged(busAttachment, objPath, "StepValue", "y", &(props->stepValue));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetHumidityInterfaceGetSelectableHumidityLevels(const char* objPath, uint8_t* selectableHumidityLevels)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    if (!selectableHumidityLevels) {
        return AJ_ERR_INVALID;
    }

    props = (TargetHumidityProperties*)GetProperties(objPath, TARGET_HUMIDITY_INTERFACE);
    if (props) {
        if (!props->selectableHumidityLevels) {
            return AJ_ERR_NULL;
        }
        memcpy(selectableHumidityLevels, props->selectableHumidityLevels, sizeof(uint8_t) * selectableHumidityLevelsSize);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetHumidityInterfaceSetSelectableHumidityLevels(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* humidityLevels, const size_t humidityLevelsSize)
{
    AJ_Status status = AJ_OK;
    TargetHumidityProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TargetHumidityProperties*)GetProperties(objPath, TARGET_HUMIDITY_INTERFACE);
    if (props) {
        if (props->selectableHumidityLevels) {
            free(props->selectableHumidityLevels);
        }
        selectableHumidityLevelsSize = humidityLevelsSize;

        props->selectableHumidityLevels = (uint8_t*)malloc(sizeof(uint8_t) * humidityLevelsSize);

        memcpy(props->selectableHumidityLevels, humidityLevels, sizeof(uint8_t) * humidityLevelsSize);

        status = EmitPropChanged(busAttachment, objPath, "SelectableHumidityLevels", "ay", props->selectableHumidityLevels);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
