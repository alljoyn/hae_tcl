/**
 * @file
 */
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

#ifndef VENDORDEFINEDINTERFACE_H_
#define VENDORDEFINEDINTERFACE_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/cdm/CdmControllee.h>

#define VENDOR_DEFINED_INTERFACE_NAME "com.foo.bar.test"

const char* const intfDescVendorDefined[6];

/**
 * VendorDefined interface listener
 */
typedef struct {
    /**
     * Handler for getting TestProperty property
     * @param[in] objPath object path
     * @param[out] testProperty testProperty
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetTestProperty) (const char* objPath, int32_t* testProperty);

    /**
     * Handler for setting TestProperty property
     * @param[in] objPath object path
     * @param[in] testProperty testProperty
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetTestProperty) (const char* objPath, int32_t* testProperty);

    /**
     * Handler for TestMethod method
     * @param[in] objPath object path
     * @return AJ_OK on success
     */
    AJ_Status (*OnTestMethod) (const char* objPath, int32_t arg1);
} VendorDefinedListener;

void VendorDefinedInterfaceRegistered(CdmInterfaceTypes intfType);
AJ_Status CreateVendorDefinedInterface(void** properties);
void DestroyVendorDefinedInterface(void* properties);
AJ_Status VendorDefinedInterfaceOnGetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener);
AJ_Status VendorDefinedInterfaceOnSetProperty(AJ_Message* replyMsg, const char* objPath, void* properties, uint8_t memberIndex, void* listener, bool* propChanged);
AJ_Status VendorDefinedInterfaceEmitPropertiesChanged(AJ_BusAttachment* busAttachment, const char* objPath, void* properties, uint8_t memberIndex);
AJ_Status VendorDefinedInterfaceOnMethodHandler(AJ_Message* msg, const char* objPath, uint8_t memberIndex, void* listener, CdmPropertiesChangedByMethod* propChangedByMethod);

/**
 * Get TestProperty of VendorDefined interface
 * @param[in] objPath the object path including the interface
 * @param[out] testProperty testProperty
 * @return AJ_OK on success
 */
AJ_Status Cdm_VendorDefinedInterfaceGetTestProperty(const char* objPath, int32_t* testProperty);

/**
 * Set TestProperty of VendorDefined interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] testProperty testProperty
 * @return AJ_OK on success
 */
AJ_Status Cdm_VendorDefinedInterfaceSetTestProperty(AJ_BusAttachment* busAttachment, const char* objPath, int32_t testProperty);

/**
 * Emit TestSignal signal of VendorDefined interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] sessionId session id
 * @return AJ_OK on success
 */
AJ_Status Cdm_VendorDefinedInterfaceEmitTestSignal(AJ_BusAttachment* busAttachment, const char* objPath, uint32_t sessionId);

#endif /* VENDORDEFINEDINTERFACE_H_ */
