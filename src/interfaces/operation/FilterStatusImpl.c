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
#include <ajtcl/hae/interfaces/operation/FilterStatus.h>
#include "../../HaeControllee/HaeControlleeImpl.h"
#include "FilterStatusImpl.h"

#define INTERFACE_VERSION 1

const char* const intfDescOperationFilterStatus[] = {
    "$org.alljoyn.SmartSpaces.Operation.FilterStatus",
    "@Version>q",
    "@ExpectedLifeInDays>q",
    "@IsCleanable>b",
    "@OrderPercentage>y",
    "@Manufacturer>s",
    "@PartNumber>s",
    "@Url>s",
    "@LifeRemaining>y",
    NULL
};

typedef struct {
    uint16_t version;
    uint16_t expectedLifeInDays;
    bool isCleanable;
    uint8_t orderPercentage;
    char* manufacturer;
    char* partNumber;
    char* url;
    uint8_t lifeRemaining;
    bool manufacturerInit;
    bool partNumberInit;
    bool urlInit;
} FilterStatusProperties;

AJ_Status CreateFilterStatusInterface(void** properties)
{
    *properties = malloc(sizeof(FilterStatusProperties));
    if (!(*properties)) {
        return AJ_ERR_RESOURCES;
    }

    ((FilterStatusProperties*)*properties)->version = (uint16_t)INTERFACE_VERSION;
    ((FilterStatusProperties*)*properties)->manufacturerInit = false;
    ((FilterStatusProperties*)*properties)->partNumberInit = false;
    ((FilterStatusProperties*)*properties)->urlInit = false;

    return AJ_OK;
}

void DestroyFilterStatusInterface(void* properties)
{
    if (properties) {
        FilterStatusProperties* props = (FilterStatusProperties*)properties;
        if (props->manufacturer) {
            free(props->manufacturer);
        }
        if (props->partNumber) {
            free(props->partNumber);
        }
        if (props->url) {
            free(props->url);
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
        AJ_MarshalArgs(&msg, "s", intfDescOperationFilterStatus[0]+1); //To remove '$'
        AJ_MarshalContainer(&msg, &array, AJ_ARG_ARRAY);
        AJ_MarshalContainer(&msg, &strc, AJ_ARG_DICT_ENTRY);
        if (!strcmp(signature, "q")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(uint16_t*)val);
        } else if (!strcmp(signature, "b")) {
            AJ_MarshalArgs(&msg, "sv", propName, signature, *(bool*)val);
        } else if (!strcmp(signature, "y")) {
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

AJ_Status FilterStatusInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;
    FilterStatusListener* lt = NULL;

    if (!properties) {
        return AJ_ERR_INVALID;
    }

    if (listener) {
        lt = (FilterStatusListener*)listener;
    }
    props = (FilterStatusProperties*)properties;

    switch (memberIndex) {
    case 0 :
        status = AJ_MarshalArgs(replyMsg, "q", props->version);
        break;
    case 1 :
        {
            uint16_t expectedLifeInDays;
            if (lt && lt->OnGetExpectedLifeInDays) {
                status = lt->OnGetExpectedLifeInDays(objPath, &expectedLifeInDays);
                if (status == AJ_OK) {
                     props->expectedLifeInDays = expectedLifeInDays;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "q", props->expectedLifeInDays);
        }
        break;
    case 2 :
        {
            bool isCleanable;
            if (lt && lt->OnGetIsCleanable) {
                status = lt->OnGetIsCleanable(objPath, &isCleanable);
                if (status == AJ_OK) {
                     props->isCleanable = isCleanable;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "b", props->isCleanable);
        }
        break;
    case 3 :
        {
            uint8_t orderPercentage;
            if (lt && lt->OnGetOrderPercentage) {
                status = lt->OnGetOrderPercentage(objPath, &orderPercentage);
                if (status == AJ_OK) {
                     props->orderPercentage = orderPercentage;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->orderPercentage);
        }
        break;
    case 4 :
        {
            char manufacturer[128];  //?? how estimate proper buffer size when the listener is exist
            if (lt && lt->OnGetManufacturer) {
                status = lt->OnGetManufacturer(objPath, manufacturer);
                if (status == AJ_OK) {
                    if (props->manufacturer) {
                        free(props->manufacturer);
                    }
                    props->manufacturer = (char*) malloc(strlen(manufacturer) + 1);
                    if (!props->manufacturer) {
                        return AJ_ERR_RESOURCES;
                    }
                    strncpy(props->manufacturer, manufacturer, strlen(manufacturer));
                    props->manufacturer[strlen(manufacturer)] = '\0';
                }
            }
            if (!props->manufacturer) {
                return AJ_ERR_NULL;
            }
            status = AJ_MarshalArgs(replyMsg, "s", props->manufacturer);
        }
        break;
    case 5 :
        {
            char partNumber[128];  //?? how estimate proper buffer size when the listener is exist
            if (lt && lt->OnGetPartNumber) {
                status = lt->OnGetPartNumber(objPath, partNumber);
                if (status == AJ_OK) {
                    if (props->partNumber) {
                        free(props->partNumber);
                    }
                    props->partNumber = (char*) malloc(strlen(partNumber) + 1);
                    if (!props->partNumber) {
                        return AJ_ERR_RESOURCES;
                    }
                    strncpy(props->partNumber, partNumber, strlen(partNumber));
                    props->partNumber[strlen(partNumber)] = '\0';
                }
            }
            if (!props->partNumber) {
                return AJ_ERR_NULL;
            }
            status = AJ_MarshalArgs(replyMsg, "s", props->partNumber);
        }
        break;
    case 6 :
        {
            char url[128];  //?? how estimate proper buffer size when the listener is exist
            if (lt && lt->OnGetUrl) {
                status = lt->OnGetUrl(objPath, url);
                if (status == AJ_OK) {
                    if (props->url) {
                        free(props->url);
                    }
                    props->url = (char*) malloc(strlen(url) + 1);
                    if (!props->url) {
                        return AJ_ERR_RESOURCES;
                    }
                    strncpy(props->url, url, strlen(url));
                    props->url[strlen(url)] = '\0';
                }
            }
            if (!props->url) {
                return AJ_ERR_NULL;
            }
            status = AJ_MarshalArgs(replyMsg, "s", props->url);
        }
        break;
    case 7 :
        {
            uint8_t lifeRemaining;
            if (lt && lt->OnGetLifeRemaining) {
                status = lt->OnGetLifeRemaining(objPath, &lifeRemaining);
                if (status == AJ_OK) {
                     props->lifeRemaining = lifeRemaining;
                }
            }
            status = AJ_MarshalArgs(replyMsg, "y", props->lifeRemaining);
        }
        break;
    default:
        status = AJ_ERR_INVALID;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceGetExpectedLifeInDays(const char* objPath, uint16_t* value)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!value) {
        return AJ_ERR_INVALID;
    }

    props = (FilterStatusProperties*)GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        *value = props->expectedLifeInDays;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceSetExpectedLifeInDays(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t value)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (FilterStatusProperties*)GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        props->expectedLifeInDays = value;
        status = EmitPropChanged(busAttachment, objPath, "ExpectedLifeInDays", "q", &(props->expectedLifeInDays));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceGetIsCleanable(const char* objPath, bool* isCleanable)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!isCleanable) {
        return AJ_ERR_INVALID;
    }

    props = (FilterStatusProperties*)GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        *isCleanable = props->isCleanable;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceSetIsCleanable(AJ_BusAttachment* busAttachment, const char* objPath, const bool isCleanable)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    props = (FilterStatusProperties*)GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        props->isCleanable = isCleanable;
        status = EmitPropChanged(busAttachment, objPath, "IsCleanable", "b", &(props->isCleanable));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceGetOrderPercentage(const char* objPath, uint8_t* value)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!value) {
        return AJ_ERR_INVALID;
    }

    props = (FilterStatusProperties*)GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        *value = props->orderPercentage;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceSetOrderPercentage(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t value)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (value != 255 && value > 100) {
        return AJ_ERR_RANGE;
    }

    props = (FilterStatusProperties*)GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        props->orderPercentage = value;
        status = EmitPropChanged(busAttachment, objPath, "OrderPercentage", "y", &(props->orderPercentage));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceGetManufacturer(const char* objPath, char* manufacturer)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!manufacturer) {
        return AJ_ERR_INVALID;
    }

    props = (FilterStatusProperties*) GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        if (!props->manufacturer) {
            return AJ_ERR_NULL;
        }
        strncpy(manufacturer, props->manufacturer, strlen(props->manufacturer));
        manufacturer[strlen(props->manufacturer)] = '\0';
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceSetManufacturer(AJ_BusAttachment* busAttachment, const char* objPath, const char* manufacturer)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!manufacturer) {
        return AJ_ERR_INVALID;
    }

    props = (FilterStatusProperties*) GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        if (props->manufacturerInit) { //const
            return AJ_ERR_FAILURE;
        } else {
            props->manufacturer = (char*) malloc(strlen(manufacturer) + 1);
            if (!props->manufacturer) {
                status = AJ_ERR_RESOURCES;
            } else {
                strncpy(props->manufacturer, manufacturer, strlen(manufacturer));
                props->manufacturer[strlen(manufacturer)] = '\0';
                props->manufacturerInit = true;
            }
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceGetPartNumber(const char* objPath, char* partNumber)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!partNumber) {
        return AJ_ERR_INVALID;
    }

    props = (FilterStatusProperties*) GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        if (!props->partNumber) {
            return AJ_ERR_NULL;
        }
        strncpy(partNumber, props->partNumber, strlen(props->partNumber));
        partNumber[strlen(props->partNumber)] = '\0';
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceSetPartNumber(AJ_BusAttachment* busAttachment, const char* objPath, const char* partNumber)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!partNumber) {
        return AJ_ERR_INVALID;
    }

    props = (FilterStatusProperties*) GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        if (props->partNumberInit) { //const
            return AJ_ERR_FAILURE;
        } else {
            props->partNumber = (char*) malloc(strlen(partNumber) + 1);
            if (!props->partNumber) {
                status = AJ_ERR_RESOURCES;
            } else {
                strncpy(props->partNumber, partNumber, strlen(partNumber));
                props->partNumber[strlen(partNumber)] = '\0';
                props->partNumberInit = true;
            }
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceGetUrl(const char* objPath, char* url)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!url) {
        return AJ_ERR_INVALID;
    }

    props = (FilterStatusProperties*) GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        if (!props->url) {
            return AJ_ERR_NULL;
        }
        strncpy(url, props->url, strlen(props->url));
        url[strlen(props->url)] = '\0';
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceSetUrl(AJ_BusAttachment* busAttachment, const char* objPath, const char* url)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (!url) {
        return AJ_ERR_INVALID;
    }

    props = (FilterStatusProperties*) GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        if (props->urlInit) { //const
            return AJ_ERR_FAILURE;
        } else {
            props->url = (char*) malloc(strlen(url) + 1);
            if (!props->url) {
                status = AJ_ERR_RESOURCES;
            } else {
                strncpy(props->url, url, strlen(url));
                props->url[strlen(url)] = '\0';
                props->urlInit = true;
            }
        }
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceGetLifeRemaining(const char* objPath, uint8_t* value)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!value) {
        return AJ_ERR_INVALID;
    }

    props = (FilterStatusProperties*)GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        *value = props->lifeRemaining;
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}

AJ_Status Hae_FilterStatusInterfaceSetLifeRemaining(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t value)
{
    AJ_Status status = AJ_OK;
    FilterStatusProperties* props = NULL;

    if (!busAttachment) {
        return AJ_ERR_INVALID;
    }

    if (value > 100) {
        return AJ_ERR_RANGE;
    }

    props = (FilterStatusProperties*)GetProperties(objPath, FILTER_STATUS_INTERFACE);
    if (props) {
        props->lifeRemaining = value;
        status = EmitPropChanged(busAttachment, objPath, "LifeRemaining", "y", &(props->lifeRemaining));
    } else {
        status = AJ_ERR_NO_MATCH;
    }

    return status;
}
