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

#ifndef HID_H_
#define HID_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * InputEvent struct
 */
typedef struct {
    uint16_t type;
    uint16_t code;
    int32_t value;
} InputEvent;

/**
 * SupportedInputEvent struct
 */
typedef struct {
    uint16_t type;
    uint16_t code;
    int32_t min;
    int32_t max;
} SupportedInputEvent;

typedef struct {
    /**
     * Handler for getting SupportedInputEvent property
     * @param[in] objPath object path
     * @param[out] supportedEvents List of supported input events by a device.
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetSupportedEvents) (const char* objPath, SupportedInputEvent* supportedEvents);

    /**
     * Handler for InjectEvents method
     * @param[in] objPath object path
     * @param[in] inputEvent Inject the user input events for human interface devices
     * @param[in] numRecords InputEvent Record count
     * @return AJ_OK on success
     */
    AJ_Status (*OnInjectEvents) (const char* objPath, const InputEvent* inputEvent, size_t numRecords);
} HidListener;

/**
 * Get displayLanguage of Hid interface
 * @param[in] objPath the object path including the interface
 * @param[out] supportedEvents List of supported input events by a device.
 * @return AJ_OK on success
 */
AJ_Status Hae_HidInterfaceGetSupportedEvents(const char* objPath, SupportedInputEvent* supportedEvents);

/**
 * Set displayLanguage of Hid interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] supportedEvents List of supported input events by a device.
 * @param[in] supportedEventListSize SupportedInputEvent List size
 * @return AJ_OK on success
 */
AJ_Status Hae_HidInterfaceSetSupportedEvents(AJ_BusAttachment* busAttachment, const char* objPath, const SupportedInputEvent* supportedEvents, size_t supportedEventListSize);

#endif /* HID_H_ */
