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

#ifndef ENERGYUSAGE_H_
#define ENERGYUSAGE_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * EnergyUsage interface listener
 */
typedef struct {
    /**
     * Handler for getting CumulativeEnergy property
     * @param[in] objPath object path
     * @param[out] cumulativeEnergy nsumption of the device in kWHr
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetCumulativeEnergy) (const char* objPath, double* cumulativeEnergy);

    /**
     * Handler for getting Precision property
     * @param[in] objPath object path
     * @param[out] precision he number of kWHr the actual energy consumption must change before CumulativeEnergy is updated
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetPrecision) (const char* objPath, double* precision);

    /**
     * Handler for getting UpdateMinTime property
     * @param[in] objPath object path
     * @param[out] updateMinTime The minimum time between updates of the CumulativeEnergy property in milliseconds
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetUpdateMinTime) (const char* objPath, uint16_t* updateMinTime);

    /**
     * Handler for ResetCumulativeEnergy method
     * @param[in] objPath object path
     * @return AJ_OK on success
     */
    AJ_Status (*OnResetCumulativeEnergy) (const char* objPath);
} EnergyUsageListener;

/**
 * Get cumulativeEnergy of EnergyUsage interface
 * @param[in] objPath the object path including the interface
 * @param[out] cumulativeEnergy nsumption of the device in kWHr
 * @return AJ_OK on success
 */
AJ_Status Hae_EnergyUsageInterfaceGetCumulativeEnergy(const char* objPath, double* cumulativeEnergy);

/**
 * Set cumulativeEnergy of EnergyUsage interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] cumulativeEnergy nsumption of the device in kWHr
 * @return AJ_OK on success
 */
AJ_Status Hae_EnergyUsageInterfaceSetCumulativeEnergy(AJ_BusAttachment* busAttachment, const char* objPath, double cumulativeEnergy);

/**
 * Get precision of EnergyUsage interface
 * @param[in] objPath the object path including the interface
 * @param[out] precision The minimum time between updates of the CumulativeEnergy property in milliseconds
 * @return AJ_OK on success
 */
AJ_Status Hae_EnergyUsageInterfaceGetPrecision(const char* objPath, double* precision);

/**
 * Set precision of EnergyUsage interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] precision The minimum time between updates of the CumulativeEnergy property in milliseconds
 * @return AJ_OK on success
 */
AJ_Status Hae_EnergyUsageInterfaceSetPrecision(AJ_BusAttachment* busAttachment, const char* objPath, double precision);

/**
 * Get updateMinTime of EnergyUsage interface
 * @param[in] objPath the object path including the interface
 * @param[out] updateMinTime The minimum time between updates of the CumulativeEnergy property in milliseconds
 * @return AJ_OK on success
 */
AJ_Status Hae_EnergyUsageInterfaceGetUpdateMinTime(const char* objPath, uint16_t* updateMinTime);

/**
 * Set updateMinTime of EnergyUsage interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] updateMinTime The minimum time between updates of the CumulativeEnergy property in milliseconds
 * @return AJ_OK on success
 */
AJ_Status Hae_EnergyUsageInterfaceSetUpdateMinTime(AJ_BusAttachment* busAttachment, const char* objPath, uint16_t updateMinTime);

#endif /* ENERGYUSAGE_H_ */