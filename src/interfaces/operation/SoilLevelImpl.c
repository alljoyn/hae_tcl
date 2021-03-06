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
#include <ajtcl/cdm/interfaces/operation/SoilLevel.h>
#include "../../CdmControllee/CdmControlleeImpl.h"
#include "SoilLevelImpl.h"

#define INTERFACE_VERSION 1
#define MAX_SELECTABLE_LEVELS_SIZE 7

const char* const intfDescOperationSoilLevel[] = {
    "$org.alljoyn.SmartSpaces.Operation.SoilLevel",
    "@Version>q",
    "@MaxLevel>y",
    "@TargetLevel=y",
    "@SelectableLevels>ay",
    NULL
};

typedef struct {
    uint16_t version;
    uint8_t maxLevel;
    uint8_t targetLevel;
    uint8_t* selectableLevels;
} SoilLevelProperties;

static size_t listSize = 0;

AJ_Status SoilLevelTargetLevelValidationCheck(const char* objPath, uint8_t targetLevel)
{
    AJ_Status status = AJ_OK;
    SoilLevelProperties* props = NULL;
    props = (SoilLevelProperties*)GetProperties(objPath, SOIL_LEVEL_INTERFACE);

    if (props) {
        int i = 0;
        bool check = false;

        if (targetLevel > props->maxLevel) {
            status = AJ_ERR_RANGE;
        }
        else {
            for (i = 0; i < listSize; i++) {
                if (props->selectableLevels[i] == targetLevel) {
                    check = true;
                    break;
                }
            }
            if (!check) {
                return AJ_ERR_RANGE;
           }
       }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status CreateSoilLevelInterface(void** properties)
{
    *properties = malloc(sizeof(SoilLevelProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(SoilLevelProperties));

    ((SoilLevelProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;
    ((SoilLevelProperties*)*properties)->selectableLevels = NULL;

    return AJ_OK;
}

void DestroySoilLevelInterface(void* properties)
{
    if (properties) {
        SoilLevelProperties* props = (SoilLevelProperties*)properties;
        if (props->selectableLevels) {
            free(props->selectableLevels);
        }

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
        AJ_MarshalArgs(&msg, "s", intfDescOperationSoilLevel[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "y")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint8_t*)val);
        } else if (!strcmp(signature, "ay")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, (uint8_t*)val, listSize);
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

AJ_Status SoilLevelInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    SoilLevelProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (SoilLevelProperties*)properties;

    switch (memberIndex) {
    case 2 :
        status = EmitPropChanged(busAttachment, objPath, "TargetLevel", "y", &(props->targetLevel));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status SoilLevelInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    SoilLevelProperties* props = NULL;
    SoilLevelListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (SoilLevelListener*)listener;
    }
    props = (SoilLevelProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
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
    case 2 :
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
    case 3:
        {
            uint8_t* selectableLevels = (uint8_t*)malloc(sizeof(uint8_t) * listSize);
            if (lt && lt->OnGetSelectableLevels) {
                status = lt->OnGetSelectableLevels(objPath, selectableLevels);
                if (status == AJ_OK) {
                    if (props->selectableLevels) {
                        free(props->selectableLevels);
                    }
                    props->selectableLevels = (uint8_t*)malloc(sizeof(uint8_t) * listSize);

                    memcpy(props->selectableLevels, selectableLevels, sizeof(uint8_t) * listSize);
                }
            }
            if (!props->selectableLevels) {
                free(selectableLevels);
                return AJ_ERR_NULL;
            }

            status = AJ_MarshalArgs(replyMsg, "ay", props->selectableLevels, sizeof(uint8_t) * listSize);
            free(selectableLevels);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status SoilLevelInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    SoilLevelProperties* props = (SoilLevelProperties*)properties;
    SoilLevelListener* lt = (SoilLevelListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 2 :
        if (!lt->OnSetTargetLevel) {
            status = AJ_ERR_NULL;
        } else {
            uint8_t targetLevel = 0;
            status = AJ_UnmarshalArgs(replyMsg, "y", &targetLevel);

            if (status == AJ_OK) {
                status = SoilLevelTargetLevelValidationCheck(objPath, targetLevel);

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
    case 3 :
        status = AJ_ERR_DISALLOWED;
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Cdm_SoilLevelInterfaceGetMaxLevel(const char* objPath, uint8_t* maxLevel)
{
    AJ_Status status = AJ_OK;
    SoilLevelProperties* props = NULL;

    if (!maxLevel) {
        return AJ_ERR_INVALID;
    }

    props = (SoilLevelProperties*)GetProperties(objPath, SOIL_LEVEL_INTERFACE);
    if (props) {
        *maxLevel = props->maxLevel;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_SoilLevelInterfaceSetMaxLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t maxLevel)
{
    AJ_Status status = AJ_OK;
    SoilLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (SoilLevelProperties*)GetProperties(objPath, SOIL_LEVEL_INTERFACE);
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

AJ_Status Cdm_SoilLevelInterfaceGetTargetLevel(const char* objPath, uint8_t* targetLevel)
{
    AJ_Status status = AJ_OK;
    SoilLevelProperties* props = NULL;

    if (!targetLevel) {
        return AJ_ERR_INVALID;
    }

    props = (SoilLevelProperties*)GetProperties(objPath, SOIL_LEVEL_INTERFACE);
    if (props) {
        *targetLevel = props->targetLevel;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_SoilLevelInterfaceSetTargetLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t targetLevel)
{
    AJ_Status status = AJ_OK;
    SoilLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = SoilLevelTargetLevelValidationCheck(objPath, targetLevel);

    if (status != AJ_OK) {
        return status;
    }

    props = (SoilLevelProperties*)GetProperties(objPath, SOIL_LEVEL_INTERFACE);
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

AJ_Status Cdm_SoilLevelInterfaceGetSelectableLevels(const char* objPath, uint8_t* selectableLevels)
{
    AJ_Status status = AJ_OK;
    SoilLevelProperties* props = NULL;

    if (!selectableLevels) {
        return AJ_ERR_INVALID;
    }

    props = (SoilLevelProperties*)GetProperties(objPath, SOIL_LEVEL_INTERFACE);
    if (props) {
        if (!props->selectableLevels) {
            return AJ_ERR_NULL;
        }
        memcpy(selectableLevels, props->selectableLevels, sizeof(uint8_t) * listSize);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Cdm_SoilLevelInterfaceSetSelectableLevels(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* selectableLevels, const size_t supportedModeSize)
{
    AJ_Status status = AJ_OK;
    SoilLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!selectableLevels) {
        return AJ_ERR_INVALID;
    }

    if (supportedModeSize > MAX_SELECTABLE_LEVELS_SIZE) {
        return AJ_ERR_INVALID;
    }

    props = (SoilLevelProperties*)GetProperties(objPath, SOIL_LEVEL_INTERFACE);
    if (props) {
        if (props->selectableLevels) {
            free(props->selectableLevels);
        }
        listSize = supportedModeSize;

        props->selectableLevels = (uint8_t*)malloc(sizeof(uint8_t) * listSize);

        memcpy(props->selectableLevels, selectableLevels, sizeof(uint8_t) * listSize);

        status = EmitPropChanged(busAttachment, objPath, "SelectableLevels", "ay", props->selectableLevels);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
