/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file
 * @brief Gus Model handler
 */

#ifndef TX_POWER_H__
#define TX_POWER_H__

#ifdef __cplusplus
extern "C" {
#endif

void set_tx_power(uint8_t handle_type, uint16_t handle, int8_t tx_pwr_lvl);

#ifdef __cplusplus
}
#endif

#endif /* GUS_MODEL_HANDLER_H__ */
