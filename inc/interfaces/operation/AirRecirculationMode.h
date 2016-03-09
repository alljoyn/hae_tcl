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

#ifndef AIRRECIRCULATIONMODE_H_
#define AIRRECIRCULATIONMODE_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * AirRecirculationMode interface listener
 */
typedef struct {
    /**
     * Handler for getting IsRecirculating property
     * @param[in] objPath object path
     * @param[out] isRecirculating isRecirculating
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetIsRecirculating) (const char* objPath, bool* isRecirculating);

    /**
     * Handler for setting IsRecirculating property
     * @param[in] objPath object path
     * @param[out] isRecirculating isRecirculating
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetIsRecirculating) (const char* objPath, bool isRecirculating);

} AirRecirculationModeListener;

/**
 * Get isRecirculating of AirRecirculationMode interface
 * @param[in] objPath the object path including the interface
 * @param[out] isRecirculating isRecirculating
 * @return AJ_OK on success
 */
AJ_Status Hae_AirRecirculationModeInterfaceGetIsRecirculating(const char* objPath, bool* isRecirculating);

/**
 * Set isRecirculating of AirRecirculationMode interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] isRecirculating isRecirculating
 * @return AJ_OK on success
 */
AJ_Status Hae_AirRecirculationModeInterfaceSetIsRecirculating(AJ_BusAttachment* busAttachment, const char* objPath, bool isRecirculating);


#endif /* AIRRECIRCULATIONMODE_H_ */