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

#ifndef ONOFFSTATUS_H_
#define ONOFFSTATUS_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/cdm/CdmControllee.h>

/**
 * OnOffStatus interface listener
 */
typedef struct {
    /**
     * Handler for getting OnOff property
     * @param[in] objPath object path
     * @param[out] onOff Current on/off state of the appliance
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetOnOff) (const char* objPath, bool* onOff);

} OnOffStatusListener;

/**
 * Get OnOff of OnOffStatus interface
 * @param[in] objPath the object path including the interface
 * @param[out] onOff Current on/off state of the appliance
 * @return AJ_OK on success
 */
AJ_Status Cdm_OnOffStatusInterfaceGetOnOff(const char* objPath, bool* onOff);

/**
 * Set onOff of OnOff of OnOffStatus interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] onOff Current on/off state of the appliance
 * @return AJ_OK on success
 */
AJ_Status Cdm_OnOffStatusInterfaceSetOnOff(AJ_BusAttachment* busAttachment, const char* objPath, const bool onOff);

#endif /* ONOFFSTATUS_H_ */
