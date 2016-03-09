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

#ifndef WINDDIRECTION_H_
#define WINDDIRECTION_H_

#include <ajtcl/alljoyn.h>
#include <ajtcl/hae/HaeControllee.h>

/**
 * WindDirection interface listener
 */
typedef struct {
    /**
     * Handler for getting HorizontalDirection property
     * @param[in] objPath object path
     * @param[out] horizontalDirection the horizontal direction
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetHorizontalDirection) (const char* objPath, uint16_t* horizontalDirection);

    /**
     * Handler for setting HorizontalDirection property
     * @param[in] objPath object path
     * @param[out] horizontalDirection the horizontal direction
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetHorizontalDirection) (const char* objPath, uint16_t horizontalDirection);

    /**
     * Handler for getting HorizontalMax property
     * @param[in] objPath object path
     * @param[out] HorizontalMax the horizontal maximum
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetHorizontalMax) (const char* objPath, uint16_t* horizontalMax);

    /**
     * Handler for getting HorizontalAutoMode property
     * @param[in] objPath object path
     * @param[out] horizontalAutoMode the horizontal auto mode
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetHorizontalAutoMode) (const char* objPath, uint8_t* horizontalAutoMode);

    /**
     * Handler for setting HorizontalAutoMode property
     * @param[in] objPath object path
     * @param[out] horizontalAutoMode the horizontal auto mode
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetHorizontalAutoMode) (const char* objPath, uint8_t horizontalAutoMode);

    /**
     * Handler for getting VerticalDirection property
     * @param[in] objPath object path
     * @param[out] verticalDirection the vertical direction
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetVerticalDirection) (const char* objPath, uint16_t* verticalDirection);

    /**
     * Handler for setting VerticalDirection property
     * @param[in] objPath object path
     * @param[out] verticalDirection the vertical direction
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetVerticalDirection) (const char* objPath, uint16_t verticalDirection);

    /**
     * Handler for getting VerticalMax property
     * @param[in] objPath object path
     * @param[out] verticalMax the vertical maximum
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetVerticalMax) (const char* objPath, uint16_t* verticalMax);

    /**
     * Handler for getting VerticalAutoMode property
     * @param[in] objPath object path
     * @param[out] verticalAutoMode the vertical auto mode
     * @return AJ_OK on success
     */
    AJ_Status (*OnGetVerticalAutoMode) (const char* objPath, uint8_t* verticalAutoMode);

    /**
     * Handler for setting VerticalAutoMode property
     * @param[in] objPath object path
     * @param[out] verticalAutoMode the vertical auto mode
     * @return AJ_OK on success
     */
    AJ_Status (*OnSetVerticalAutoMode) (const char* objPath, uint8_t verticalAutoMode);

} WindDirectionListener;


/**
 * Get the horizontal direction of WindDirection interface
 * @param[in] objPath the object path including the interface
 * @param[out] horizontalDirection the horizontal direction
 * @return AJ_OK on success
 */
AJ_Status Hae_WindDirectionInterfaceGetHorizontalDirection(const char* objPath, uint16_t* horizontalDirection);

/**
 * Set the horizontal direction of WindDirection interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] horizontalDirection the horizontal direction
 * @return AJ_OK on success
 */
AJ_Status Hae_WindDirectionInterfaceSetHorizontalDirection(AJ_BusAttachment* busAttachment, const char* objPath, uint16_t horizontalDirection);

/**
 * Get the horizontal maximum of WindDirection interface
 * @param[in] objPath the object path including the interface
 * @param[out] horizontalMax the horizontal maximum
 * @return AJ_OK on success
 */
AJ_Status Hae_WindDirectionInterfaceGetHorizontalMax(const char* objPath, uint16_t* horizontalMax);

/**
 * Set the horizontal maximum of WindDirection interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] horizontalMax the horizontal maximum
 * @return AJ_OK on success
 */
AJ_Status Hae_WindDirectionInterfaceSetHorizontalMax(AJ_BusAttachment* busAttachment, const char* objPath, uint16_t horizontalMax);

/**
 * Get the horizontal auto mode of WindDirection interface
 * @param[in] objPath the object path including the interface
 * @param[out] horizontalAutoMode the horizontal auto mode
 * @return AJ_OK on success
 */
AJ_Status Hae_WindDirectionInterfaceGetHorizontalAutoMode(const char* objPath, uint8_t* horizontalAutoMode);

/**
 * Set the horizontal auto mode of WindDirection interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] horizontalAutoMode the horizontal auto mode
 * @return AJ_OK on success
 */
AJ_Status Hae_WindDirectionInterfaceSetHorizontalAutoMode(AJ_BusAttachment* busAttachment, const char* objPath, uint8_t horizontalAutoMode);

/**
 * Get the vertical direction of WindDirection interface
 * @param[in] objPath the object path including the interface
 * @param[out] verticalDirection the vertical direction
 * @return AJ_OK on success
 */
AJ_Status Hae_WindDirectionInterfaceGetVerticalDirection(const char* objPath, uint16_t* verticalDirection);

/**
 * Set the vertical direction of WindDirection interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] verticalDirection the vertical direction
 * @return AJ_OK on success
 */
AJ_Status Hae_WindDirectionInterfaceSetVerticalDirection(AJ_BusAttachment* busAttachment, const char* objPath, uint16_t verticalDirection);

/**
 * Get the vertical maximum of WindDirection interface
 * @param[in] objPath the object path including the interface
 * @param[out] verticalMax the vertical maximum
 * @return AJ_OK on success
 */
AJ_Status Hae_WindDirectionInterfaceGetVerticalMax(const char* objPath, uint16_t* verticalMax);

/**
 * Set the vertical maximum of WindDirection interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] verticalMax the vertical maximum
 * @return AJ_OK on success
 */
AJ_Status Hae_WindDirectionInterfaceSetVerticalMax(AJ_BusAttachment* busAttachment, const char* objPath, uint16_t verticalMax);

/**
 * Get the vertical auto mode of WindDirection interface
 * @param[in] objPath the object path including the interface
 * @param[out] verticalAutoMode the vertical auto mode
 * @return AJ_OK on success
 */
AJ_Status Hae_WindDirectionInterfaceGetVerticalAutoMode(const char* objPath, uint8_t* verticalAutoMode);

/**
 * Set the vertical auto mode of WindDirection interface
 * @param[in] busAttachment bus attachment
 * @param[in] objPath the object path including the interface
 * @param[in] verticalAutoMode the vertical auto mode
 * @return AJ_OK on success
 */
AJ_Status Hae_WindDirectionInterfaceSetVerticalAutoMode(AJ_BusAttachment* busAttachment, const char* objPath, uint8_t verticalAutoMode);

#endif /* WINDDIRECTION_H_ */
