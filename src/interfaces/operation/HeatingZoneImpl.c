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
#include <ajtcl/hae/interfaces/operation/HeatingZone.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "HeatingZoneImpl.h"

#define INTERFACE_VERSION 1
#define MAX_HEATING_ZONES_NUM 7

const char* const intfDescOperationHeatingZone[] = {
    "$org.alljoyn.SmartSpaces.Operation.HeatingZone",
    "@Version>q",
    "@NumberOfHeatingZones>y",
    "@MaxHeatingLevels>ay",
    "@HeatingLevels>ay",
    NULL
};

typedef struct {
    uint16_t version;
    uint8_t numberOfHeatingZones;
    uint8_t* maxHeatingLevels;
    uint8_t* heatingLevels;
} HeatingZoneProperties;

static size_t listSize = 0;
static size_t maxListSize = 0;

AJ_Status HeatingZoneHeatingLevelsValidationCheck(const char* objPath, uint8_t* heatingLevels)
{
    AJ_Status status = AJ_OK;
    HeatingZoneProperties* props = NULL;
    props = (HeatingZoneProperties*)GetProperties(objPath, HEATING_ZONE_INTERFACE);

    if (props) {
        int i = 0;

        for (i = 0; i < listSize; i++) {
            if ( heatingLevels > props->maxHeatingLevels) {
                status = AJ_ERR_RANGE;
            }
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status CreateHeatingZoneInterface(void** properties)
{
    *properties = malloc(sizeof(HeatingZoneProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(HeatingZoneProperties));

    ((HeatingZoneProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;
    ((HeatingZoneProperties*)*properties)->maxHeatingLevels = NULL;
    ((HeatingZoneProperties*)*properties)->heatingLevels = NULL;

    return AJ_OK;
}

void DestroyHeatingZoneInterface(void* properties)
{
    if (properties) {
        HeatingZoneProperties* props = (HeatingZoneProperties*)properties;
        if (props->maxHeatingLevels) {
            free(props->maxHeatingLevels);
        }
        if (props->heatingLevels) {
            free(props->heatingLevels);
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
        AJ_MarshalArgs(&msg, "s", intfDescOperationHeatingZone[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "y")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint8_t*)val);
        } else if (!strcmp(signature, "ay")) {
            if (!strcmp(propName, "MaxHeatingLevels")) {
                AJ_MarshalArgs(&msg, "sv", propName, signature, (uint8_t*)val, maxListSize);
            } else if (!strcmp(propName, "HeatingLevels")) {
                AJ_MarshalArgs(&msg, "sv", propName, signature, (uint8_t*)val, listSize);
            } else {
                return AJ_ERR_INVALID;
            }
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

AJ_Status HeatingZoneInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    HeatingZoneProperties* props = NULL;
    HeatingZoneListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (HeatingZoneListener*)listener;
    }
    props = (HeatingZoneProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint8_t numberOfHeatingZones;
            if (lt && lt->OnGetNumberOfHeatingZones) {
                status = lt->OnGetNumberOfHeatingZones(objPath, &numberOfHeatingZones);
                if (status == AJ_OK) {
                    props->numberOfHeatingZones = numberOfHeatingZones;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->numberOfHeatingZones);
        }
        break;
    case 2 :
        {
            uint8_t* maxHeatingLevels = (uint8_t*)malloc(sizeof(uint8_t) * maxListSize);
            if (lt && lt->OnGetMaxHeatingLevels) {
                status = lt->OnGetMaxHeatingLevels(objPath, maxHeatingLevels);
                if (status == AJ_OK) {
                    if (props->maxHeatingLevels) {
                        free(props->maxHeatingLevels);
                    }
                    props->maxHeatingLevels = (uint8_t*)malloc(sizeof(uint8_t) * maxListSize);

                    memcpy(props->maxHeatingLevels, maxHeatingLevels, sizeof(uint8_t) * maxListSize);
                }
            }
            if (!props->maxHeatingLevels) {
                free(maxHeatingLevels);
                return AJ_ERR_NULL;
            }

            status = AJ_MarshalArgs(replyMsg, "ay", props->maxHeatingLevels, sizeof(uint8_t) * maxListSize);
            free(maxHeatingLevels);
        }
        break;
    case 3:
        {
            uint8_t* heatingLevels = (uint8_t*)malloc(sizeof(uint8_t) * listSize);
            if (lt && lt->OnGetHeatingLevels) {
                status = lt->OnGetHeatingLevels(objPath, heatingLevels);
                if (status == AJ_OK) {
                    if (props->heatingLevels) {
                        free(props->heatingLevels);
                    }
                    props->heatingLevels = (uint8_t*)malloc(sizeof(uint8_t) * listSize);

                    memcpy(props->heatingLevels, heatingLevels, sizeof(uint8_t) * listSize);
                }
            }
            if (!props->heatingLevels) {
                free(heatingLevels);
                return AJ_ERR_NULL;
            }

            status = AJ_MarshalArgs(replyMsg, "ay", props->heatingLevels, sizeof(uint8_t) * listSize);
            free(heatingLevels);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_HeatingZoneInterfaceGetNumberOfHeatingZones(const char* objPath, uint8_t* numberOfHeatingZones)
{
    AJ_Status status = AJ_OK;
    HeatingZoneProperties* props = NULL;

    if (!numberOfHeatingZones) {
        return AJ_ERR_INVALID;
    }

    props = (HeatingZoneProperties*)GetProperties(objPath, HEATING_ZONE_INTERFACE);
    if (props) {
        *numberOfHeatingZones = props->numberOfHeatingZones;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_HeatingZoneInterfaceSetNumberOfHeatingZones(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t numberOfHeatingZones)
{
    AJ_Status status = AJ_OK;
    HeatingZoneProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (HeatingZoneProperties*)GetProperties(objPath, HEATING_ZONE_INTERFACE);
    if (props) {
        if (props->numberOfHeatingZones != numberOfHeatingZones) {
            props->numberOfHeatingZones = numberOfHeatingZones;

            status = EmitPropChanged(busAttachment, objPath, "NumberOfHeatingZones", "y", &(props->numberOfHeatingZones));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_HeatingZoneInterfaceGetMaxHeatingLevels(const char* objPath, uint8_t* maxHeatingLevels)
{
    AJ_Status status = AJ_OK;
    HeatingZoneProperties* props = NULL;

    if (!maxHeatingLevels) {
        return AJ_ERR_INVALID;
    }

    props = (HeatingZoneProperties*)GetProperties(objPath, HEATING_ZONE_INTERFACE);
    if (props) {
        if (!props->maxHeatingLevels) {
            return AJ_ERR_NULL;
        }
        memcpy(maxHeatingLevels, props->maxHeatingLevels, sizeof(uint8_t) * maxListSize);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_HeatingZoneInterfaceSetMaxHeatingLevels(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* maxHeatingLevels, const size_t supportedModeSize)
{
    AJ_Status status = AJ_OK;
    HeatingZoneProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!maxHeatingLevels) {
        return AJ_ERR_INVALID;
    }

    if (supportedModeSize > MAX_HEATING_ZONES_NUM) {
        return AJ_ERR_INVALID;
    }

    props = (HeatingZoneProperties*)GetProperties(objPath, HEATING_ZONE_INTERFACE);
    if (props) {
        if (props->maxHeatingLevels) {
            free(props->maxHeatingLevels);
        }
        maxListSize = supportedModeSize;

        props->maxHeatingLevels = (uint8_t*)malloc(sizeof(uint8_t) * maxListSize);

        memcpy(props->maxHeatingLevels, maxHeatingLevels, sizeof(uint8_t) * maxListSize);

        status = EmitPropChanged(busAttachment, objPath, "MaxHeatingLevels", "ay", props->maxHeatingLevels);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_HeatingZoneInterfaceGetHeatingLevels(const char* objPath, uint8_t* heatingLevels)
{
    AJ_Status status = AJ_OK;
    HeatingZoneProperties* props = NULL;

    if (!heatingLevels) {
        return AJ_ERR_INVALID;
    }

    props = (HeatingZoneProperties*)GetProperties(objPath, HEATING_ZONE_INTERFACE);
    if (props) {
        if (!props->heatingLevels) {
            return AJ_ERR_NULL;
        }
        memcpy(heatingLevels, props->heatingLevels, sizeof(uint8_t) * listSize);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_HeatingZoneInterfaceSetHeatingLevels(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* heatingLevels, const size_t supportedModeSize)
{
    AJ_Status status = AJ_OK;
    HeatingZoneProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!heatingLevels) {
        return AJ_ERR_INVALID;
    }

    if (supportedModeSize > MAX_HEATING_ZONES_NUM) {
        return AJ_ERR_INVALID;
    }

    props = (HeatingZoneProperties*)GetProperties(objPath, HEATING_ZONE_INTERFACE);
    if (props) {
        if (props->heatingLevels) {
            free(props->heatingLevels);
        }
        listSize = supportedModeSize;

        props->heatingLevels = (uint8_t*)malloc(sizeof(uint8_t) * listSize);

        memcpy(props->heatingLevels, heatingLevels, sizeof(uint8_t) * listSize);

        status = EmitPropChanged(busAttachment, objPath, "HeatingLevels", "ay", props->heatingLevels);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
