/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *  @brief Nordic mesh light sample
 */
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh/models.h>
#include <bluetooth/mesh/dk_prov.h>
#include <dk_buttons_and_leds.h>
#include "gus_model_handler.h"
#include "tx_power.h"
#include "gus_leds.h"
#include <bluetooth/hci_vs.h>

static void bt_ready(int err)
{
    if (err)
    {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }

    printk("Bluetooth initialized\n");

    gus_leds_init();
    dk_buttons_init(NULL);

    err = bt_mesh_init(bt_mesh_dk_prov_init(), gus_model_handler_init());
    if (err)
    {
        printk("Initializing mesh failed (err %d)\n", err);
        return;
    }

    if (IS_ENABLED(CONFIG_SETTINGS))
    {

        settings_load();
    }

    /* This will be a no-op if settings_load() loaded provisioning info */
    bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);

    printk("Mesh initialized\n");
}

void main(void)
{
    int err;

    printk("Initializing....\n");

    err = bt_enable(bt_ready);
    if (err)
    {
        printk("Bluetooth init failed (err %d)\n", err);
    }

    while (1)
    {
        // manage blinking of LEDs in idle loop
        k_sleep(K_MSEC(100));
        if (get_blinker() >= 0)
        {
            uint16_t ledbit = dec_blinker() % 6;
            gus_set_leds(0x01 << ledbit);
        }
    }
}
