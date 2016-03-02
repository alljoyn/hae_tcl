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

#ifndef CLOSEDSTATUS_H_
#define CLOSEDSTATUS_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * ClosedStatus interface listener
 */
typedef struct {
    /**
     * Handler for getting IsClosed property
     * @param[in] objPath object path
     * @param[out] isClosed isClosed
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetIsClosed) (const char* objPath, bool* isClosed);

} ClosedStatusListener;

/**
 * Get isClosed of ClosedStatus interface
 * @param[in] objPath the object path including the interface
 * @param[out] isClosed isClosed
 * @return AJ_OK on success
 */
AJ_Status Hae_ClosedStatusInterfaceGetIsClosed(const char* objPath, const bool isClosed);

/**
 * Set isClosed of ClosedStatus interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] isClosed isClosed
 * @return AJ_OK on success
 */
AJ_Status Hae_ClosedStatusInterfaceSetIsClosed(AJ_BusAttachment* busAttachment, const char* objPath, bool* isClosed);

#endif /* CLOSEDSTATUS_H_ */
