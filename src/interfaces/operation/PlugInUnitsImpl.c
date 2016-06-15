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
#include <ajtcl/hae/interfaces/operation/PlugInUnits.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "PlugInUnitsImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationPlugInUnits[4] = {
    "$org.alljoyn.SmartSpaces.Operation.PlugInUnits",
    "@Version>q",
    "@PlugInUnits>a(oub)",
    NULL
};

typedef struct {
    uint16_t version;
    PlugInInfo* units;
} PlugInUnitsProperties;

static size_t plugInUnitsSize = 0;

AJ_Status CreatePlugInUnitsInterface(void** properties)
{
    *properties = malloc(sizeof(PlugInUnitsProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(PlugInUnitsProperties));

    ((PlugInUnitsProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;
    ((PlugInUnitsProperties*)*properties)->units = NULL;

    return AJ_OK;
}

void DestroyPlugInUnitsInterface(void* properties)
{
    if (properties) {
        PlugInUnitsProperties* props = (PlugInUnitsProperties*)properties;
        if (props->units) {
            int i = 0;
            for (i = 0; i < plugInUnitsSize; i++) {
                free((props->units)[i].objectPath);
            }
            free(props->units);
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
        AJ_MarshalArgs(&msg, "s", intfDescOperationPlugInUnits[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "y")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint8_t*)val);
        } else if (!strcmp(signature, "a(oub)")) {
            PlugInInfo* plugInInfo = (PlugInInfo*)val;
            AJ_Arg array2, strc2;
            int i = 0;

            AJ_MarshalArgs(&msg, "s", propName);
            AJ_MarshalVariant(&msg, signature);
            AJ_MarshalContainer(&msg, &array2, AJ_ARG_ARRAY);
            for (i = 0; i < plugInUnitsSize; i++) {
                AJ_MarshalContainer(&msg, &strc2, AJ_ARG_STRUCT);
                AJ_MarshalArgs(&msg, "oub", plugInInfo[i].objectPath,
                                            plugInInfo[i].deviceId,
                                            plugInInfo[i].pluggedIn );
                AJ_MarshalCloseContainer(&msg, &strc2);
            }
            AJ_MarshalCloseContainer(&msg, &array2);
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

AJ_Status PlugInUnitsInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    PlugInUnitsProperties* props = NULL;
    PlugInUnitsListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (PlugInUnitsListener*)listener;
    }
    props = (PlugInUnitsProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            PlugInInfo* units = (PlugInInfo*)malloc(sizeof(PlugInInfo) * plugInUnitsSize);

            if (lt && lt->OnGetPlugInUnits) {
                status = lt->OnGetPlugInUnits(objPath, units);
                if (status == AJ_OK) {
                    if (props->units) {
                        int i = 0;
                        for (i = 0; i < plugInUnitsSize; i++) {
                            free((props->units)[i].objectPath);
                        }
                        free(props->units);
                    }
                    props->units = (PlugInInfo*)malloc(sizeof(PlugInInfo) * plugInUnitsSize);
                    memcpy(props->units, units, sizeof(PlugInInfo) * plugInUnitsSize);
                    int i = 0;
                    for (i = 0; i < plugInUnitsSize; i++) {
                        (props->units)[i].objectPath= (char*)malloc(strlen(units[i].objectPath) + 1);
                        strncpy((props->units)[i].objectPath, units[i].objectPath, strlen(units[i].objectPath));
                        ((props->units)[i].objectPath)[strlen(units[i].objectPath)] = '\0';
                    }
                }
            }
            if (!props->units) {
                free(units);
                return AJ_ERR_NULL;
            }
            AJ_Arg array, strc;
            AJ_MarshalContainer(replyMsg, &array, AJ_ARG_ARRAY);
            int i = 0;
            for (i = 0; i < plugInUnitsSize; i++) {
                AJ_MarshalContainer(replyMsg, &strc, AJ_ARG_STRUCT);
                status = AJ_MarshalArgs(replyMsg, "oub",  (props->units)[i].objectPath,
                                                            (props->units)[i].deviceId,
                                                            (props->units)[i].pluggedIn);
                AJ_MarshalCloseContainer(replyMsg, &strc);
            }
            AJ_MarshalCloseContainer(replyMsg, &array);

            free(units);

        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_PlugInUnitsInterfaceGetPlugInUnits(const char* objPath, PlugInInfo* units)
{
    AJ_Status status = AJ_OK;
    PlugInUnitsProperties* props = NULL;

    if (!units) {
        return AJ_ERR_INVALID;
    }

    props = (PlugInUnitsProperties*)GetProperties(objPath, PLUG_IN_UNITS_INTERFACE);

    if (props) {
        if (!props->units) {
            return AJ_ERR_NULL;
        }

        memcpy(units, props->units, sizeof(PlugInInfo) * plugInUnitsSize);

    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_PlugInUnitsInterfaceSetPlugInUnits(AJ_BusAttachment* busAttachment, const char* objPath, const PlugInInfo* units, const size_t plugInInfoSize)
{
    AJ_Status status = AJ_OK;
    PlugInUnitsProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!units) {
        return AJ_ERR_INVALID;
    }

    props = (PlugInUnitsProperties*)GetProperties(objPath, PLUG_IN_UNITS_INTERFACE);
    if (props) {
        if (props->units) {
            int i = 0;
            for (i = 0; i < plugInUnitsSize; i++) {
                free((props->units)[i].objectPath);
            }
            free(props->units);
        }
        plugInUnitsSize = plugInInfoSize;
        props->units = (PlugInInfo*)malloc(sizeof(PlugInInfo) * plugInUnitsSize);

        memcpy(props->units, units, sizeof(PlugInInfo) * plugInUnitsSize);
        int i = 0;
        for (i = 0; i < plugInUnitsSize; i++) {
            (props->units)[i].objectPath= (char*)malloc(strlen(units[i].objectPath) + 1);
            strncpy((props->units)[i].objectPath, units[i].objectPath, strlen(units[i].objectPath));
            ((props->units)[i].objectPath)[strlen(units[i].objectPath)] = '\0';
        }

        status = EmitPropChanged(busAttachment, objPath, "PlugInUnits", "a(oub)", props->units);
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
