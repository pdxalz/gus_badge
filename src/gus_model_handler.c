/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh/models.h>
#include <dk_buttons_and_leds.h>
#include "gus_model_handler.h"
#include "gus_cli.h"


uint16_t blinker = 0;
uint16_t get_blinker(void) {return blinker;}
void set_blinker(uint16_t val) {blinker = val;}
uint16_t dec_blinker(void) {return --blinker;}

///////////////////// PROCEDURES

static void display_health(enum bt_mesh_gus_cli_state state)
{
        set_blinker( state == BT_MESH_GUS_CLI_IDENTIFY ? 100 : 0);
	if (state != BT_MESH_GUS_CLI_IDENTIFY) 
        {	   
            switch((uint16_t)state) {
             case BT_MESH_GUS_CLI_HEALTHY:
                dk_set_leds(0b00100100);
            break;
            
            case BT_MESH_GUS_CLI_MASKED:
               dk_set_leds(0b00100110);
            break;
                          
            case BT_MESH_GUS_CLI_VACCINATED:
                dk_set_leds(0b00100101);
            break;

            case BT_MESH_GUS_CLI_VACCINATED_MASKED:
                dk_set_leds(0b00100111);
            break;

            case BT_MESH_GUS_CLI_INFECTED:
                dk_set_leds(0b00010010);
            break;

            case BT_MESH_GUS_CLI_VACCINATED_INFECTED:
                dk_set_leds(0b00010011);
            break;

            case BT_MESH_GUS_CLI_INFECTED_MASKED:
                dk_set_leds(0b00010110);
            break;               
                          
            case BT_MESH_GUS_CLI_VACCINATED_MASKED_INFECTED:
                dk_set_leds(0b00010111);
            break;               
            }
        }
}


//////////////////////////////
//  Health server
//////////////////////////////

static void attention_on(struct bt_mesh_model *mod)
{
	set_blinker(100);
}

static void attention_off(struct bt_mesh_model *mod)
{
	set_blinker(0);
	dk_set_leds(DK_NO_LEDS_MSK);
}

static const struct bt_mesh_health_srv_cb health_srv_cb = {
	.attn_on = attention_on,
	.attn_off = attention_off,
};

static struct bt_mesh_health_srv health_srv = {
	.cb = &health_srv_cb,
};

BT_MESH_HEALTH_PUB_DEFINE(health_pub, 0);

// ******************************************************************************
// ***************************** GUS model setup *******************************
// ******************************************************************************

static void handle_gus_start(struct bt_mesh_gus_cli *gus)
{
    printk("started gus\n");
	
}

static void handle_gus_set_state(struct bt_mesh_gus_cli *gus,
				 struct bt_mesh_msg_ctx *ctx,
				 enum bt_mesh_gus_cli_state state)
{
    printk("set state:  %x %d\n", ctx->addr, state);
    display_health(state);
}



static const struct bt_mesh_gus_cli_handlers gus_handlers = {
	.start = handle_gus_start,
	.set_state = handle_gus_set_state,
};

static struct bt_mesh_gus_cli gus = {
	.handlers = &gus_handlers,
};





#if 0
static struct bt_mesh_elem elements[] = {
	BT_MESH_ELEM(
		1, BT_MESH_MODEL_LIST(
			BT_MESH_MODEL_CFG_SRV,
			BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
                        BT_MESH_MODEL_GUS_CLI(&gus)),
		BT_MESH_MODEL_NONE),
};
#else
static struct bt_mesh_elem elements[] = {
	BT_MESH_ELEM(
		1,
		BT_MESH_MODEL_LIST(
			BT_MESH_MODEL_CFG_SRV,
			BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub)),
		BT_MESH_MODEL_LIST(BT_MESH_MODEL_GUS_CLI(&gus))),
};
#endif





static const struct bt_mesh_comp comp = {
	.cid = CONFIG_BT_COMPANY_ID,
	.elem = elements,
	.elem_count = ARRAY_SIZE(elements),
};

void model_handler_set_state(uint16_t addr, uint8_t state)
{
    int err;

    enum bt_mesh_gus_cli_state gus_state = state;
        printk("identify %d\n", addr);
    err = bt_mesh_gus_cli_state_set(&gus, addr, gus_state);
    if (err) {
        printk("identify %d %d failed %d\n", addr,state, err);
    }
}


static void button_handler_cb(uint32_t pressed, uint32_t changed)
{
    if (!bt_mesh_is_provisioned()) {
        return;
    }

    if ((pressed & changed & BIT(1))) {
 //       bt_mesh_reset();
         model_handler_set_state(3, 1);
        dk_set_leds(1);
    }
    else {
         model_handler_set_state(4, 1);
        dk_set_leds(2);

    }


}

const struct bt_mesh_comp *gus_model_handler_init(void)
{
	static struct button_handler button_handler = {
		.cb = button_handler_cb,
	};

	dk_button_handler_add(&button_handler);



	return &comp;
}
