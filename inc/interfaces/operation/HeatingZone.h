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

#ifndef HEATINGZONE_H_
#define HEATINGZONE_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/cdm/CdmControllee.h>

/**
 * WaterLevel interface listener
 */
typedef struct {
    /**
     * Handler for getting NumberOfHeatingZones property
     * @param[in] objPath object path
     * @param[out] numberOfHeatingZones number of heating zones
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetNumberOfHeatingZones) (const char* objPath, uint8_t* numberOfHeatingZones);

    /**
     * Handler for getting MaxHeatingLevels property
     * @param[in] objPath object path
     * @param[out] maxHeatingLevels max heating levels
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMaxHeatingLevels) (const char* objPath, uint8_t* maxHeatingLevels);

    /**
     * Handler for getting HeatingLevels property
     * @param[in] objPath object path
     * @param[out] heatingLevels heating levels
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetHeatingLevels) (const char* objPath, uint8_t* heatingLevels);

} HeatingZoneListener;

/**
 * Get NumberOfHeatingZones of HeatingZone interface
 * @param[in] objPath the object path including the interface
 * @param[out] numberOfHeatingZones number of heating zones
 * @return AJ_OK on success
 */
AJ_Status Cdm_HeatingZoneInterfaceGetNumberOfHeatingZones(const char* objPath, uint8_t* numberOfHeatingZones);

/**
 * Set NumberOfHeatingZones of HeatingZone interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] numberOfHeatingZones number of heating zones
 * @return AJ_OK on success
 */
AJ_Status Cdm_HeatingZoneInterfaceSetNumberOfHeatingZones(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t numberOfHeatingZones);

/**
 * Get MaxHeatingLevels of HeatingZone interface
 * @param[in] objPath the object path including the interface
 * @param[out] maxHeatingLevels max heating levels
 * @return AJ_OK on success
 */
AJ_Status Cdm_HeatingZoneInterfaceGetMaxHeatingLevels(const char* objPath, uint8_t* maxHeatingLevels);

/**
 * Set MaxHeatingLevels of HeatingZone interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] maxHeatingLevels max heating levels
 * @param[in] maxHeatingLevelsSize max heating levels size
 * @return AJ_OK on success
 */
AJ_Status Cdm_HeatingZoneInterfaceSetMaxHeatingLevels(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* maxHeatingLevels, const size_t maxHeatingLevelsSize);

/**
 * Get HeatingLevels of HeatingZone interface
 * @param[in] objPath the object path including the interface
 * @param[out] heatingLevels heating levels
 * @return AJ_OK on success
 */
AJ_Status Cdm_HeatingZoneInterfaceGetHeatingLevels(const char* objPath, uint8_t* heatingLevels);

/**
 * Set HeatingLevels of HeatingZone interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] heatingLevels heating levels
 * @param[in] heatingLevelsSize heating levels size
 * @return AJ_OK on success
 */
AJ_Status Cdm_HeatingZoneInterfaceSetHeatingLevels(AJ_BusAttachment* busAttachment, const char* objPath, const uint8_t* heatingLevels, const size_t heatingLevelsSize);

#endif /* HEATINGZONE_H_ */
