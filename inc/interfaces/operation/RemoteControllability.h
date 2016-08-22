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

#ifndef REMOTECONTROLLABILITY_H_
#define REMOTECONTROLLABILITY_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/cdm/CdmControllee.h>

/**
 * RemoteControllability interface listener
 */
typedef struct {
    /**
     * Handler for getting IsControllable property
     * @param[in] objPath object path
     * @param[out] isControllable isControllable
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetIsControllable) (const char* objPath, bool* isControllable);

} RemoteControllabilityListener;

/**
 * Get isControllable of RemoteControllability interface
 * @param[in] objPath the object path including the interface
 * @param[out] isControllable isControllable
 * @return AJ_OK on success
 */
AJ_Status Cdm_RemoteControllabilityInterfaceGetIsControllable(const char* objPath, bool* isControllable);

/**
 * Set isControllable of RemoteControllability interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] isControllable isControllable
 * @return AJ_OK on success
 */
AJ_Status Cdm_RemoteControllabilityInterfaceSetIsControllable(AJ_BusAttachment* busAttachment, const char* objPath, const bool isControllable);

#endif /* REMOTECONTROLLABILITY_H_ */
