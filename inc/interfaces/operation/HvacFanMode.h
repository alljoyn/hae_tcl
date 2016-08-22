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

#ifndef HVACFANMODE_H_
#define HVACFANMODE_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/cdm/CdmControllee.h>

/**
 * HvacFanMode interface listener
 */
typedef struct {
    /**
     * Handler for getting Mode property
     * @param[in] objPath object path
     * @param[out] mode mode of device
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetMode) (const char* objPath, uint16_t* mode);

    /**
     * Handler for setting Mode property
     * @param[in] objPath object path
     * @param[in] mode mode of device
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetMode) (const char* objPath, const uint16_t mode);

    /**
     * Handler for getting SupportedModes property
     * @param[in] objPath object path
     * @param[out] supportedModes supported modes
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetSupportedModes) (const char* objPath, uint16_t* supportedModes);

} HvacFanModeListener;

/**
 * Get mode of HvacFanMode interface
 * @param[in] objPath the object path including the interface
 * @param[out] mode mode of device
 * @return AJ_OK on success
 */
AJ_Status Cdm_HvacFanModeInterfaceGetMode(const char* objPath, uint16_t* mode);

/**
 * Set mode of HvacFanMode interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] mode mode of device
 * @return AJ_OK on success
 */
AJ_Status Cdm_HvacFanModeInterfaceSetMode(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t mode);

/**
 * Get supportedModes of HvacFanMode interface
 * @param[in] objPath the object path including the interface
 * @param[out] supportedModes supported modes
 * @return AJ_OK on success
 */
AJ_Status Cdm_HvacFanModeInterfaceGetSupportedModes(const char* objPath, uint16_t* supportedModes);

/**
 * Set supportedModes of HvacFanMode interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] supportedModes supported modes
 * @return AJ_OK on success
 */
AJ_Status Cdm_HvacFanModeInterfaceSetSupportedModes(AJ_BusAttachment* busAttachment, const char* objPath, const uint16_t* supportedModes, const size_t supportedModeSize);

#endif /* HVACFANMODE_H_ */
