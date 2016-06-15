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
#include <ajtcl/hae/interfaces/environment/TargetTemperatureLevel.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "TargetTemperatureLevelImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescEnvironmentTargetTemperatureLevel[] = {
    "$org.alljoyn.SmartSpaces.Environment.TargetTemperatureLevel",
    "@Version>q",
    "@TargetLevel=y",
    "@MaxLevel>y",
    "@SelectableTemperatureLevels>ay",
    NULL
};

typedef struct {
    uint16_t version;
    uint8_t targetLevel;
    uint8_t maxLevel;
    uint8_t* selectableTemperatureLevels;
} TargetTemperatureLevelProperties;
static size_t selectableTemperatureLevelsSize = 0;

AJ_Status TargetTemperatureLevelTargetLevelValidationCheck(const char* objPath, uint8_t targetLevel)
{
    AJ_Status status = AJ_OK;
    int i;
    TargetTemperatureLevelProperties* props = NULL;

    props = (TargetTemperatureLevelProperties*)GetProperties(objPath, TARGET_TEMPERATURE_LEVEL_INTERFACE);
    if (props) {
        if (targetLevel > props->maxLevel) {
            status = AJ_ERR_RANGE;
        } else {
            status = AJ_ERR_DISALLOWED;
            for (i = 0; i < selectableTemperatureLevelsSize; i++) {
                if (props->selectableTemperatureLevels[i] == targetLevel) {
                    status = AJ_OK;
                    break;
                }
            }
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status TargetTemperatureLevelAdjustTargetValue(const char* objPath, uint8_t* targetLevel)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureLevelProperties* props = NULL;

    props = (TargetTemperatureLevelProperties*)GetProperties(objPath, TARGET_TEMPERATURE_LEVEL_INTERFACE);
    if (props) {
        *targetLevel = (*targetLevel > props->maxLevel ? props->maxLevel : *targetLevel);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status CreateTargetTemperatureLevelInterface(void** properties)
{
    *properties = malloc(sizeof(TargetTemperatureLevelProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(TargetTemperatureLevelProperties));

    ((TargetTemperatureLevelProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}
void DestroyTargetTemperatureLevelInterface(void* properties)
{
    if (properties) {
        TargetTemperatureLevelProperties* props = (TargetTemperatureLevelProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescEnvironmentTargetTemperatureLevel[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "q")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint16_t*)val);
        } else if (!strcmp(signature, "y")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint8_t*)val);
        } else if (!strcmp(signature, "ay")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, (uint8_t*)val, selectableTemperatureLevelsSize);
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

AJ_Status TargetTemperatureLevelInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureLevelProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureLevelProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "TargetLevel", "y", &(props->targetLevel));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status TargetTemperatureLevelInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureLevelProperties* props = NULL;
    TargetTemperatureLevelListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (TargetTemperatureLevelListener*)listener;
    }
    props = (TargetTemperatureLevelProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint8_t targetLevel;
            if (lt && lt->OnGetTargetLevel) {
                status = lt->OnGetTargetLevel(objPath, &targetLevel);
                if (status == AJ_OK) {
                    props->targetLevel = targetLevel;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->targetLevel);
        }
        break;
    case 2 :
        {
            uint8_t maxLevel;
            if (lt && lt->OnGetMaxLevel) {
                status = lt->OnGetMaxLevel(objPath, &maxLevel);
                if (status == AJ_OK) {
                    props->maxLevel = maxLevel;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->maxLevel);
        }
        break;
    case 3 :
        {
            uint8_t* selectableTemperatureLevels = (uint8_t*)malloc(sizeof(uint8_t) * selectableTemperatureLevelsSize);
            if (lt && lt->OnGetSelectableTemperatureLevels) {
                status = lt->OnGetSelectableTemperatureLevels(objPath, selectableTemperatureLevels);
                if (status == AJ_OK) {
                    if (props->selectableTemperatureLevels) {
                        free(props->selectableTemperatureLevels);
                    }
                    props->selectableTemperatureLevels = (uint8_t*)malloc(sizeof(uint8_t) * selectableTemperatureLevelsSize);

                    memcpy(props->selectableTemperatureLevels, selectableTemperatureLevels, sizeof(uint8_t) * selectableTemperatureLevelsSize);
                }
            }
            if (!props->selectableTemperatureLevels) {
                free(selectableTemperatureLevels);
                return AJ_ERR_NULL;
            }

            status = AJ_MarshalArgs(replyMsg, "ay", props->selectableTemperatureLevels, sizeof(uint8_t) * selectableTemperatureLevelsSize);
            free(selectableTemperatureLevels);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status TargetTemperatureLevelInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    TargetTemperatureLevelProperties* props = (TargetTemperatureLevelProperties*)properties;
    TargetTemperatureLevelListener* lt = (TargetTemperatureLevelListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetTargetLevel) {
            status = AJ_ERR_NULL;
        } else {
            uint8_t targetLevel = 0;
            status = AJ_UnmarshalArgs(replyMsg, "y", &targetLevel);

            if (status == AJ_OK) {
                status = TargetTemperatureLevelAdjustTargetValue(objPath, &targetLevel);
                if (status != AJ_OK) {
                    return status;
                }

                status = TargetTemperatureLevelTargetLevelValidationCheck(objPath, targetLevel);
                if (status != AJ_OK) {
                    return status;
                }

                status = lt->OnSetTargetLevel(objPath, targetLevel);

                if (status == AJ_OK) {
                    if (props->targetLevel != targetLevel) {
                        props->targetLevel = targetLevel;
                        *propChanged = true;
                    }
                }
            }
        }
        break;
    case 2 :
    case 3 :
        status = AJ_ERR_DISALLOWED;
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_TargetTemperatureLevelInterfaceGetTargetLevel(const char* objPath, uint8_t* targetLevel)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureLevelProperties* props = NULL;

    if (!targetLevel) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureLevelProperties*)GetProperties(objPath, TARGET_TEMPERATURE_LEVEL_INTERFACE);
    if (props) {
        *targetLevel = props->targetLevel;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetTemperatureLevelInterfaceSetTargetLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t targetLevel)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = TargetTemperatureLevelTargetLevelValidationCheck(objPath, targetLevel);
    if (status != AJ_OK) {
        return status;
    }

    props = (TargetTemperatureLevelProperties*)GetProperties(objPath, TARGET_TEMPERATURE_LEVEL_INTERFACE);
    if (props) {
        if (props->targetLevel != targetLevel) {
            props->targetLevel = targetLevel;

            status = EmitPropChanged(busAttachment, objPath, "TargetLevel", "y", &(props->targetLevel));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetTemperatureLevelInterfaceGetMaxLevel(const char* objPath, uint8_t* maxLevel)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureLevelProperties* props = NULL;

    if (!maxLevel) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureLevelProperties*)GetProperties(objPath, TARGET_TEMPERATURE_LEVEL_INTERFACE);
    if (props) {
        *maxLevel = props->maxLevel;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetTemperatureLevelInterfaceSetMaxLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t maxLevel)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureLevelProperties*)GetProperties(objPath, TARGET_TEMPERATURE_LEVEL_INTERFACE);
    if (props) {
        if (props->maxLevel != maxLevel) {
            props->maxLevel = maxLevel;

            status = EmitPropChanged(busAttachment, objPath, "MaxLevel", "y", &(props->maxLevel));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetTemperatureLevelInterfaceGetSelectableTemperatureLevels(const char* objPath, uint8_t* selectableTemperatureLevels)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureLevelProperties* props = NULL;

    if (!selectableTemperatureLevels) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureLevelProperties*)GetProperties(objPath, TARGET_TEMPERATURE_LEVEL_INTERFACE);
    if (props) {
        if (!props->selectableTemperatureLevels) {
            return AJ_ERR_NULL;
        }
        memcpy(selectableTemperatureLevels, props->selectableTemperatureLevels, sizeof(uint8_t) * selectableTemperatureLevelsSize);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_TargetTemperatureLevelInterfaceSetSelectableTemperatureLevels(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* temperatureLevels, const size_t temperatureLevelsSize)
{
    AJ_Status status = AJ_OK;
    TargetTemperatureLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (TargetTemperatureLevelProperties*)GetProperties(objPath, TARGET_TEMPERATURE_LEVEL_INTERFACE);
    if (props) {
        if (props->selectableTemperatureLevels) {
            free(props->selectableTemperatureLevels);
        }
        selectableTemperatureLevelsSize = temperatureLevelsSize;

        props->selectableTemperatureLevels = (uint8_t*)malloc(sizeof(uint8_t) * temperatureLevelsSize);

        memcpy(props->selectableTemperatureLevels, temperatureLevels, sizeof(uint8_t) * temperatureLevelsSize);

        status = EmitPropChanged(busAttachment, objPath, "SelectableTemperatureLevels", "ay", props->selectableTemperatureLevels);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
