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

#ifndef CURRENTPOWER_H_
#define CURRENTPOWER_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * CurrentPower interface listener
 */
typedef struct {
    /**
     * Handler for getting CurrentPower property
     * @param[in] objPath object path
     * @param[out] currentPower power consumption of the device in watts (W)
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetCurrentPower) (const char* objPath, double* currentPower);

    /**
     * Handler for getting Precision property
     * @param[in] objPath object path
     * @param[out] precision the number of watts the
                actual power consumption must change before CurrentPower is updated
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetPrecision) (const char* objPath, double* precision);

    /**
     * Handler for getting UpdateMinTime property
     * @param[in] objPath object path
     * @param[out] updateMinTime The minimum time between updates of the CurrentPower property in milliseconds
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetUpdateMinTime) (const char* objPath, uint16_t* updateMinTime);

} CurrentPowerListener;

/**
 * Get currentPower of CurrentPower interface
 * @param[in] objPath the object path including the interface
 * @param[out] currentPower power consumption of the device in watts (W)
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentPowerInterfaceGetCurrentPower(const char* objPath, double* currentPower);

/**
 * Set currentPower of CurrentPower of CurrentPower interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] currentPower power consumption of the device in watts (W)
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentPowerInterfaceSetCurrentPower(AJ_BusAttachment* busAttachment, const char* objPath, double currentPower);

/**
 * Get precision of CurrentPower interface
 * @param[in] objPath the object path including the interface
 * @param[out] precision the number of watts the
                actual power consumption must change before CurrentPower is updated
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentPowerInterfaceGetPrecision(const char* objPath, double* precision);

/**
 * Set precision of Precision of CurrentPower interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] precision the number of watts the
                actual power consumption must change before CurrentPower is updated
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentPowerInterfaceSetPrecision(AJ_BusAttachment* busAttachment, const char* objPath, double precision);

/**
 * Get updateMinTime of CurrentPower interface
 * @param[in] objPath the object path including the interface
 * @param[out] updateMinTime The minimum time between updates of the CurrentPower property in milliseconds
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentPowerInterfaceGetUpdateMinTime(const char* objPath, uint16_t* updateMinTime);

/**
 * Set updateMinTime of UpdateMinTime of CurrentPower interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] updateMinTime The minimum time between updates of the CurrentPower property in milliseconds
 * @return AJ_OK on success
 */
AJ_Status Hae_CurrentPowerInterfaceSetUpdateMinTime(AJ_BusAttachment* busAttachment, const char* objPath, uint16_t updateMinTime);

#endif /* CURRENTPOWER_H_ */