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

#ifndef ALERTS_H_
#define ALERTS_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>
#include <ajtcl/hae/interfaces/HaeInterfaceErrors.h>

/**
 * AlertRecord struct
 */
typedef struct {
    uint8_t severity;
    uint16_t alertCode;
    bool needAcknowledgement;
} AlertRecord;

/**
 * AlertCodesDescriptor struct
 */
typedef struct {
    uint16_t alertCode;
    char* description;
} AlertCodesDescriptor;

/**
 * Alerts interface listener
 */
typedef struct {
    /**
     * Handler for getting Alerts property
     * @param[in] objPath object path
     * @param[out] alerts List of current pending alerts
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetAlerts) (const char* objPath, AlertRecord* alerts);
    /**
     * Handler for GetAlertCodesDescription method
     * @param[in] objPath object path
     * @param[in] languageTag Language to be used in the output strings
     * @param[out] description Alert codes description
     * @param[out] alertsNumber Alerts number
     * @param[out] errorCode error code
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetAlertCodesDescription) (const char* objPath, const char* languageTag,
        AlertCodesDescriptor** description, size_t* alertsNumber, ErrorCode* errorCode);

    /**
     * Handler for AcknowledgeAlert method
     * @param[in] objPath object path
     * @param[in] alertCode Alert code of the alert to be acknowledged
     * @param[out] errorCode error code
     * @return AJ_OK on success
     */
    AJ_Status (*OnAcknowledgeAlert) (const char* objPath, const uint16_t alertCode, ErrorCode* errorCode);

    /**
     * Handler for AcknowledgeAllAlerts method
     * @param[in] objPath object path
     * @param[out] errorCode error code
     * @return AJ_OK on success
     */
    AJ_Status (*OnAcknowledgeAllAlerts) (const char* objPath, ErrorCode* errorCode);

} AlertsListener;


/**
 * Get Alerts of Alerts interface
 * @param[in] objPath the object path including the interface
 * @param[out] alerts List of current pending alerts
 * @return AJ_OK on success
 */
AJ_Status Hae_AlertsInterfaceGetAlerts(const char* objPath, AlertRecord* alerts);

/**
 * Set Alerts of Alerts interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] alerts List of current pending alerts
 * @param[in] alertsNumber Alerts number
 * @return AJ_OK on success
 */
AJ_Status Hae_AlertsInterfaceSetAlerts(AJ_BusAttachment* busAttachment, const char* objPath,
    const AlertRecord* alerts, const size_t alertsNumber);


#endif /* ALERTS_H_ */
