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

#ifndef CDMCONTROLLEEIMPL_H_
#define CDMCONTROLLEEIMPL_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/cdm/CdmControllee.h>

typedef struct cdmIntfInfo {
    CdmInterfaceTypes intfType;
    void* properties;
    void* listener;
    struct cdmIntfInfo* intfNext;
} CdmInterfaceInfo;

typedef struct cdmObjInfo {
    const char* path;
    CdmInterfaceInfo* intfFirst;
    CdmInterfaceInfo* intfLast;
    AJ_InterfaceDescription* ajIntfDesc;
    struct cdmObjInfo* objNext;
} CdmObjectInfo;

typedef struct {
    CdmInterfaceTypes intfType;
    void* properties;
    uint8_t member_index;
    bool changed;
} CdmPropertiesChanged;

typedef struct vendorDefinedIntfInfo {
    const char* intfName;
    CdmInterfaceTypes intfType;
    const char* const* intfDesc;
    VendorDefinedInterfaceHandler* handler;
    struct vendorDefinedIntfInfo* intfNext;
} VendorDefinedInterfaceInfo;

typedef AJ_Status (*InterfaceCreator)(void** properties);
typedef void (*InterfaceDestructor)(void* properties);
typedef AJ_Status (*OnGetProperty)(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener);
typedef AJ_Status (*OnSetProperty)(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged);
typedef AJ_Status (*EmitPropertiesChanged)(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex);
typedef AJ_Status (*OnMethodHandler)(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener, CdmPropertiesChangedByMethod* propChangedByMethod);

#endif /* CDMCONTROLLEEIMPL_H_ */
