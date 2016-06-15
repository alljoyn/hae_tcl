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
#include <ajtcl/hae/interfaces/environment/WaterLevel.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "WaterLevelImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescEnvironmentWaterLevel[] = {
    "$org.alljoyn.SmartSpaces.Environment.WaterLevel",
    "@Version>q",
    "@SupplySource>y",
    "@CurrentLevel>y",
    "@MaxLevel>y",
    NULL
};

typedef struct {
    uint16_t version;
    uint8_t supplySource;
    uint8_t currentLevel;
    uint8_t maxLevel;
} WaterLevelProperties;

AJ_Status CurrentLevelValidationCheck(const char* objPath, uint8_t currentLevel)
{
    AJ_Status status = AJ_OK;
    WaterLevelProperties* props = NULL;


    props = (WaterLevelProperties*)GetProperties(objPath, WATER_LEVEL_INTERFACE);
    if (props) {
        if (currentLevel > props->maxLevel) {
            status = AJ_ERR_RANGE;
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status CreateWaterLevelInterface(void** properties)
{
    *properties = malloc(sizeof(WaterLevelProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(WaterLevelProperties));

    ((WaterLevelProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyWaterLevelInterface(void* properties)
{
    if (properties) {
        WaterLevelProperties* props = (WaterLevelProperties*)properties;
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
        AJ_MarshalArgs(&msg, "s", intfDescEnvironmentWaterLevel[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "y")) {
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

AJ_Status WaterLevelInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    WaterLevelProperties* props = NULL;
    WaterLevelListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (WaterLevelListener*)listener;
    }
    props = (WaterLevelProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint8_t supplySource;
            if (lt && lt->OnGetSupplySource) {
                status = lt->OnGetSupplySource(objPath, &supplySource);
                if (status == AJ_OK) {
                    props->supplySource = supplySource;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->supplySource);
        }
        break;
    case 2 :
        {
            uint8_t currentLevel;
            if (lt && lt->OnGetCurrentLevel) {
                status = lt->OnGetCurrentLevel(objPath, &currentLevel);
                 if (status == AJ_OK) {
                      props->currentLevel = currentLevel;
                 }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->currentLevel);
        }
        break;
    case 3 :
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
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_WaterLevelInterfaceGetSupplySource(const char* objPath, uint8_t* supplySource)
{
    AJ_Status status = AJ_OK;
    WaterLevelProperties* props = NULL;

    if (!supplySource) {
        return AJ_ERR_INVALID;
    }

    props = (WaterLevelProperties*)GetProperties(objPath, WATER_LEVEL_INTERFACE);
    if (props) {
        *supplySource = props->supplySource;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WaterLevelInterfaceSetSupplySource(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t supplySource)
{
    AJ_Status status = AJ_OK;
    WaterLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (WaterLevelProperties*)GetProperties(objPath, WATER_LEVEL_INTERFACE);
    if (props) {
        if (props->supplySource != supplySource) {
            props->supplySource = supplySource;

            status = EmitPropChanged(busAttachment, objPath, "SupplySource", "y", &(props->supplySource));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WaterLevelInterfaceGetCurrentLevel(const char* objPath, uint8_t* currentLevel)
{
    AJ_Status status = AJ_OK;
    WaterLevelProperties* props = NULL;

    if (!currentLevel) {
        return AJ_ERR_INVALID;
    }

    props = (WaterLevelProperties*)GetProperties(objPath, WATER_LEVEL_INTERFACE);
    if (props) {
        *currentLevel = props->currentLevel;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WaterLevelInterfaceSetCurrentLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t currentLevel)
{
    AJ_Status status = AJ_OK;
    WaterLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    status = CurrentLevelValidationCheck(objPath, currentLevel);
    if (status != AJ_OK) {
        return status;
    }

    props = (WaterLevelProperties*)GetProperties(objPath, WATER_LEVEL_INTERFACE);
    if (props) {
        if (props->currentLevel != currentLevel) {
            props->currentLevel = currentLevel;

            status = EmitPropChanged(busAttachment, objPath, "CurrentLevel", "y", &(props->currentLevel));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WaterLevelInterfaceGetMaxLevel(const char* objPath, uint8_t* maxLevel)
{
    AJ_Status status = AJ_OK;
    WaterLevelProperties* props = NULL;

    if (!maxLevel) {
        return AJ_ERR_INVALID;
    }

    props = (WaterLevelProperties*)GetProperties(objPath, WATER_LEVEL_INTERFACE);
    if (props) {
        *maxLevel = props->maxLevel;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_WaterLevelInterfaceSetMaxLevel(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t maxLevel)
{
    AJ_Status status = AJ_OK;
    WaterLevelProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (WaterLevelProperties*)GetProperties(objPath, WATER_LEVEL_INTERFACE);
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

