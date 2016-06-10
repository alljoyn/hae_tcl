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
#include <ajtcl/hae/interfaces/operation/ResourceSaving.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "ResourceSavingImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationResourceSaving[] = {
    "$org.alljoyn.SmartSpaces.Operation.ResourceSaving",
    "@Version>q",
    "@ResourceSavingMode=b",
    NULL
};

typedef struct {
    uint16_t version;
    bool resourceSavingMode;
} ResourceSavingProperties;

AJ_Status CreateResourceSavingInterface(void** properties)
{
    *properties = malloc(sizeof(ResourceSavingProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((ResourceSavingProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyResourceSavingInterface(void* properties)
{
    if (properties) {
        ResourceSavingProperties* props = (ResourceSavingProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescOperationResourceSaving[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
          if (!strcmp(signature, "b")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(bool*)val);
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

AJ_Status ResourceSavingInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    ResourceSavingProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (ResourceSavingProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "ResourceSavingMode", "b", &(props->resourceSavingMode));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status ResourceSavingInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    ResourceSavingProperties* props = NULL;
    ResourceSavingListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (ResourceSavingListener*)listener;
    }
    props = (ResourceSavingProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            bool resourceSavingMode;
            if (lt && lt->OnGetResourceSavingMode) {
                status = lt->OnGetResourceSavingMode(objPath, &resourceSavingMode);
                if (status == AJ_OK) {
                    props->resourceSavingMode = resourceSavingMode;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "b", props->resourceSavingMode);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status ResourceSavingInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    ResourceSavingProperties* props = (ResourceSavingProperties*)properties;
    ResourceSavingListener* lt = (ResourceSavingListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetResourceSavingMode) {
            status = AJ_ERR_NULL;
        } else {
            bool resourceSavingMode = 0;
            status = AJ_UnmarshalArgs(replyMsg, "b", &resourceSavingMode);

            if (status == AJ_OK) {
                status = lt->OnSetResourceSavingMode(objPath, resourceSavingMode);

                if (status == AJ_OK) {
                    if (props->resourceSavingMode != resourceSavingMode) {
                        props->resourceSavingMode = resourceSavingMode;
                        *propChanged = true;
                    }
                }
            }
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_ResourceSavingInterfaceGetResourceSavingMode(const char* objPath, bool* resourceSavingMode)
{
    AJ_Status status = AJ_OK;
    ResourceSavingProperties* props = NULL;

    if (!resourceSavingMode) {
        return AJ_ERR_INVALID;
    }

    props = (ResourceSavingProperties*)GetProperties(objPath, RESOURCE_SAVING_INTERFACE);
    if (props) {
        *resourceSavingMode = props->resourceSavingMode;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_ResourceSavingInterfaceSetResourceSavingMode(AJ_BusAttachment* busAttachment, const char* objPath, const bool resourceSavingMode)
{
    AJ_Status status = AJ_OK;
    ResourceSavingProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!resourceSavingMode) {
        return AJ_ERR_INVALID;
    }

    props = (ResourceSavingProperties*)GetProperties(objPath, RESOURCE_SAVING_INTERFACE);
    if (props) {
        if (props->resourceSavingMode != resourceSavingMode) {
            props->resourceSavingMode = resourceSavingMode;

            status = EmitPropChanged(busAttachment, objPath, "ResourceSavingMode", "b", &(props->resourceSavingMode));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
