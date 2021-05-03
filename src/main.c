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

//#include <bluetooth/hci.h>
#include <bluetooth/hci_vs.h>

//#include <bluetooth/conn.h>
//#include <bluetooth/uuid.h>
//#include <bluetooth/gatt.h>
//#include <sys/byteorder.h>


static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");


	dk_leds_init();
	dk_buttons_init(NULL);

	err = bt_mesh_init(bt_mesh_dk_prov_init(), gus_model_handler_init());
	if (err) {
		printk("Initializing mesh failed (err %d)\n", err);
		return;
	}

	if (IS_ENABLED(CONFIG_SETTINGS)) {
	
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
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}

//todo: figure out why setting tx power doesn't affect mesh tx power
//               static const int8_t txp[] = {4, 0, -3, -8, -15, -18, -23, -30};
//                set_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV,
//                set_tx_power(BT_HCI_VS_LL_TX_POWER_LEVEL_NO_PREF,
//                             0, txp[7]);
                printk("pwr 0\n");
       

int count = 0;
        while (1) {
            ++count;
/*
            if (count == 100) {
                set_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV,
                             0, txp[7]);
                printk("pwr 7\n");
            } else if (count == 200) {
                set_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV,
                             0, txp[4]);
                printk("pwr 7\n");
            } else if (count == 300) {
                set_tx_power(BT_HCI_VS_LL_HANDLE_TYPE_ADV,
                             0, txp[0]);
                printk("pwr 0\n");
            }  else if (count > 300) {
                count = 0;
            }
            */    
            k_sleep(K_MSEC(100));
            if (get_blinker() >= 0) {
                uint16_t ledbit = dec_blinker() % 6;
                dk_set_leds(0x01 << ledbit);
            }
            else {
//                dk_set_leds(0);

            }
        }
}
