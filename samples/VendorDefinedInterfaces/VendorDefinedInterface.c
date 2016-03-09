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

#include "VendorDefinedInterface.h"

#define INTERFACE_VERSION 1

static HaeInterfaceTypes vendorDefinedInterfaceType;

const char* const intfDescVendorDefined[] = {
    VENDOR_DEFINED_INTERFACE_NAME,
    "@Version>q",
    "@TestProperty=i",
    "?TestMethod arg1<i",
    "!TestSignal",
    NULL
};

typedef struct {
    uint16_t version;
    int32_t testProperty;
} VendorDefinedProperties;

void VendorDefinedInterfaceRegistered(HaeInterfaceTypes intfType)
{
    vendorDefinedInterfaceType = intfType;
}

AJ_Status CreateVendorDefinedInterface(void** properties)
{
    *properties = malloc(sizeof(VendorDefinedProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((VendorDefinedProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyVendorDefinedInterface(void* properties)
{
    if (properties) {
        VendorDefinedProperties* props = (VendorDefinedProperties*)properties;

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
        AJ_MarshalArgs(&msg, "s", intfDescVendorDefined[0]);
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "i")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(int32_t*)val);
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

AJ_Status VendorDefinedInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex)
{
    AJ_Status status = AJ_OK;
    VendorDefinedProperties* props = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    props = (VendorDefinedProperties*)properties;

    switch (memberIndex) {
    case 1 :
        status = EmitPropChanged(busAttachment, objPath, "TestProperty", "i", &(props->testProperty));
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status VendorDefinedInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    VendorDefinedProperties* props = NULL;
    VendorDefinedListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (VendorDefinedListener*)listener;
    }
    props = (VendorDefinedProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            int32_t testProperty;
            if (lt && lt->OnGetTestProperty) {
                status = lt->OnGetTestProperty(objPath, &testProperty);
                if (status == AJ_OK) {
                    props->testProperty = testProperty;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "i", props->testProperty);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status VendorDefinedInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged)
{
    AJ_Status status = AJ_OK;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    if (!propChanged) {
        return AJ_ERR_INVALID;
    }

    VendorDefinedProperties* props = (VendorDefinedProperties*)properties;
    VendorDefinedListener* lt = (VendorDefinedListener*)listener;

    switch (memberIndex) {
    case 0 :
        status = AJ_ERR_DISALLOWED;
        break;
    case 1 :
        if (!lt->OnSetTestProperty) {
            status = AJ_ERR_NULL;
        } else {
            int32_t testProperty = 0;
            status = AJ_UnmarshalArgs(replyMsg, "i", &testProperty);

            if (status == AJ_OK) {
                status = lt->OnSetTestProperty(objPath, &testProperty);

                if (status == AJ_OK) {
                    if (props->testProperty != testProperty) {
                        props->testProperty = testProperty;
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

AJ_Status VendorDefinedInterfaceOnMethodHandler(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    VendorDefinedListener* lt = (VendorDefinedListener*)listener;

    switch (memberIndex) {
    case 2 :
        if (!lt->OnTestMethod) {
            status = AJ_ERR_NULL;
        } else {
            int32_t arg1;

            status = AJ_UnmarshalArgs(msg, "i", &arg1);

            if (status == AJ_OK) {
                status = lt->OnTestMethod(objPath, arg1);

                if (status == AJ_OK) {
                    AJ_Message reply;

                    AJ_MarshalReplyMsg(msg, &reply);

                    status = AJ_DeliverMsg(&reply);
                }
            }
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_VendorDefinedInterfaceGetTestProperty(const char* objPath, int32_t* testProperty)
{
    AJ_Status status = AJ_OK;
    VendorDefinedProperties* props = NULL;

    if (!testProperty) {
        return AJ_ERR_INVALID;
    }

    props = (VendorDefinedProperties*)GetProperties(objPath, vendorDefinedInterfaceType);
    if (props) {
        *testProperty = props->testProperty;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_VendorDefinedInterfaceSetTestProperty(AJ_BusAttachment* busAttachment, const char* objPath, int32_t* testProperty)
{
    AJ_Status status = AJ_OK;
    VendorDefinedProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!testProperty) {
        return AJ_ERR_INVALID;
    }

    props = (VendorDefinedProperties*)GetProperties(objPath, vendorDefinedInterfaceType);
    if (props) {
        if (props->testProperty != *testProperty) {
            props->testProperty = *testProperty;

            status = EmitPropChanged(busAttachment, objPath, "TestProperty", "i", &(props->testProperty));
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_VendorDefinedInterfaceEmitTestSignal(AJ_BusAttachment* busAttachment, const char* objPath, uint32_t sessionId)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t msgId = 0;

    status = MakeMsgId(objPath, vendorDefinedInterfaceType, 3, &msgId);
    if (status != AJ_OK) {
        return status;
    }

    AJ_MarshalSignal(busAttachment, &msg, msgId, NULL, sessionId, AJ_FLAG_GLOBAL_BROADCAST, 0);

    return AJ_DeliverMsg(&msg);
}
