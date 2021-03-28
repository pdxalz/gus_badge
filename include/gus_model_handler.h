/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file
 * @brief Gus Model handler
 */

#ifndef GUS_MODEL_HANDLER_H__
#define GUS_MODEL_HANDLER_H__

#include <bluetooth/mesh.h>

#ifdef __cplusplus
extern "C" {
#endif

const struct bt_mesh_comp *gus_model_handler_init(void);

#ifdef __cplusplus
}
#endif

#endif /* GUS_MODEL_HANDLER_H__ */
