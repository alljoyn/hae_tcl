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
#include <ajtcl/hae/interfaces/operation/OffControl.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "OffControlImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationOffControl[] = {
    "$org.alljoyn.SmartSpaces.Operation.OffControl",
    "@Version>q",
    "?SwitchOff",
    NULL
};

typedef struct {
    uint16_t version;
} OffControlProperties;

AJ_Status CreateOffControlInterface(void** properties)
{
    *properties = malloc(sizeof(OffControlProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }
    memset(*properties, 0, sizeof(OffControlProperties));

    ((OffControlProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;

    return AJ_OK;
}

void DestroyOffControlInterface(void* properties)
{
    if (properties) {
        OffControlProperties* props = (OffControlProperties*)properties;

        free(props);
    }
}

AJ_Status OffControlInterfaceOnMethodHandler(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener, HaePropertiesChangedByMethod* propChangedByMethod)
{
    AJ_Status status = AJ_OK;

    if (!listener) {
        return AJ_ERR_INVALID;
    }

    if (!propChangedByMethod) {
        return AJ_ERR_INVALID;
    }

    OffControlListener* lt = (OffControlListener*)listener;

    switch (memberIndex) {
    case 1 :
        if (!lt->OnSwitchOff) {
            status = AJ_ERR_NULL;
        } else {
            ErrorCode errorCode = NOT_ERROR;
            AJ_Message reply;
            status = lt->OnSwitchOff(objPath, &errorCode);

            AJ_MarshalReplyMsg(msg, &reply);
            if (status != AJ_OK) {
                AJ_MarshalErrorMsgWithInfo(msg, &reply, GetInterfaceErrorName(errorCode), GetInterfaceErrorMessage(errorCode));
            }
            status = AJ_DeliverMsg(&reply);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}
