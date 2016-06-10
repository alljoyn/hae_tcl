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

#ifndef FILTERSTATUS_H_
#define FILTERSTATUS_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * FilterStatus interface listener
 */
typedef struct {
    /**
     * Handler for getting ExpectedLifeInDays property
     * @param[in] objPath object path
     * @param[out] value expected life in days
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetExpectedLifeInDays) (const char* objPath, uint16_t* value);

    /**
     * Handler for getting IsCleanable property
     * @param[in] objPath object path
     * @param[out] isCleanable is cleanable
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetIsCleanable) (const char* objPath, bool* isCleanable);

    /**
     * Handler for getting OrderPercentage property
     * @param[in] objPath object path
     * @param[out] value order percentage
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetOrderPercentage) (const char* objPath, uint8_t* value);

    /**
     * Handler for getting Manufacturer property
     * @param[in] objPath object path
     * @param[out] manufacturer manufacturer
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetManufacturer) (const char* objPath, char* manufacturer);

    /**
     * Handler for getting PartNumber property
     * @param[in] objPath object path
     * @param[out] partNumber part number
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetPartNumber) (const char* objPath, char* partNumber);

    /**
     * Handler for getting Url property
     * @param[in] objPath object path
     * @param[out] url url
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetUrl) (const char* objPath, char* url);

    /**
     * Handler for getting LifeRemaining property
     * @param[in] objPath object path
     * @param[out] value life remaining
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetLifeRemaining) (const char* objPath, uint8_t* value);
} FilterStatusListener;

/**
 * Get expected life in days of FilterStatus interface
 * @param[in] objPath the object path including the interface
 * @param[out] value expected life in days
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceGetExpectedLifeInDays(const char* objPath, uint16_t* value);

/**
 * Set expected life in days of FilterStatus interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] value expected life in days
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceSetExpectedLifeInDays(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t value);

/**
 * Get is cleanable of FilterStatus interface
 * @param[in] objPath the object path including the interface
 * @param[out] isCleanable is cleanable
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceGetIsCleanable(const char* objPath, bool* isCleanable);

/**
 * Set is cleanable of FilterStatus interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] isCleanable is cleanable
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceSetIsCleanable(AJ_BusAttachment* busAttachment, const char* objPath, const bool isCleanable);

/**
 * Get order percentage of FilterStatus interface
 * @param[in] objPath the object path including the interface
 * @param[out] value order percentage
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceGetOrderPercentage(const char* objPath, uint8_t* value);

/**
 * Set order percentage of FilterStatus interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] value order percentage
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceSetOrderPercentage(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t value);

/**
 * Get manufacturer of FilterStatus interface
 * @param[in] objPath the object path including the interface
 * @param[out] manufacturer manufacturer
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceGetManufacturer(const char* objPath, char* manufacturer);

/**
 * Set manufacturer of FilterStatus interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] manufacturer manufacturer
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceSetManufacturer(AJ_BusAttachment* busAttachment, const char* objPath, const char* manufacturer);

/**
 * Get part number of FilterStatus interface
 * @param[in] objPath the object path including the interface
 * @param[out] partNumber part number
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceGetPartNumber(const char* objPath, char* partNumber);

/**
 * Set part number of FilterStatus interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] partNumber part number
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceSetPartNumber(AJ_BusAttachment* busAttachment, const char* objPath, const char* partNumber);

/**
 * Get url of FilterStatus interface
 * @param[in] objPath the object path including the interface
 * @param[out] url url
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceGetUrl(const char* objPath, char* url);

/**
 * Set url of FilterStatus interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] url url
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceSetUrl(AJ_BusAttachment* busAttachment, const char* objPath, const char* url);

/**
 * Get life remaining of FilterStatus interface
 * @param[in] objPath the object path including the interface
 * @param[out] value life remaining
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceGetLifeRemaining(const char* objPath, uint8_t* value);

/**
 * Set life remaining of FilterStatus interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] value life remaining
 * @return AJ_OK on success
 */
AJ_Status Hae_FilterStatusInterfaceSetLifeRemaining(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t value);

#endif /* FILTERSTATUS_H_ */
