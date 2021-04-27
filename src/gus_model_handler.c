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

#define RL 0b00010000
#define RR 0b00000010
#define GL 0b00100000
#define GR 0b00000100
#define BL 0b00001000
#define BR 0b00000001
#define YL 0b00110000
#define YR 0b00000110
#define BLACK 0b00000000


static void display_health(enum bt_mesh_gus_cli_state state)
{
        set_blinker( state == BT_MESH_GUS_CLI_IDENTIFY ? 100 : 0);
	if (state != BT_MESH_GUS_CLI_IDENTIFY) 
        {	   
            switch((uint16_t)state) {
             case BT_MESH_GUS_CLI_HEALTHY:
                dk_set_leds(GL|GR);
            break;
            
            case BT_MESH_GUS_CLI_MASKED:
               dk_set_leds(GL|YR);
            break;
                          
            case BT_MESH_GUS_CLI_VACCINATED:
                dk_set_leds(BL|BR);
            break;

            case BT_MESH_GUS_CLI_VACCINATED_MASKED:
                dk_set_leds(BL|YR);
            break;

            case BT_MESH_GUS_CLI_INFECTED:
                dk_set_leds(RR|RL);
            break;

            case BT_MESH_GUS_CLI_VACCINATED_INFECTED:
                dk_set_leds(BL|RR);
            break;

            case BT_MESH_GUS_CLI_MASKED_INFECTED:
                dk_set_leds(YL|RR);
            break;               
                          
            case BT_MESH_GUS_CLI_VACCINATED_MASKED_INFECTED:
                dk_set_leds(YL|YR);
            break;  
                         
            case BT_MESH_GUS_CLI_OFF:
                dk_set_leds(BLACK);
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

}

static const uint8_t * spare_name(uint16_t addr)
{
    const uint8_t * spare_names[] = {
    "Alan",    "Ally",    "Brenda", "Bryan", "Carol", "Craig",
    "Dalene",  "Darrell", "Eric"    };

#define SPARE_NAME_LEN (sizeof(spare_names)/ sizeof(char *))
    return spare_names[addr % SPARE_NAME_LEN];
}


static void handle_gus_signin(struct bt_mesh_gus_cli *gus,
			       struct bt_mesh_msg_ctx *ctx,
                               uint16_t addr)
{
    const uint8_t * name = gus->name;
    size_t len = strlen(name);
    if (len < 1) {
        name = spare_name(addr);
        len = strlen(name);
    }
    printk("handle signin %d %s\n", addr, name);

    (void)bt_mesh_gus_cli_sign_in_reply(gus, ctx, name);
}

static void handle_gus_set_state(struct bt_mesh_gus_cli *gus,
				 struct bt_mesh_msg_ctx *ctx,
				 enum bt_mesh_gus_cli_state state)
{
    display_health(state);
}



static const struct bt_mesh_gus_cli_handlers gus_handlers = {
	.start = handle_gus_start,
	.sign_in = handle_gus_signin,
	.set_state = handle_gus_set_state,
};

static struct bt_mesh_gus_cli gus = {
	.handlers = &gus_handlers,
};

static struct bt_mesh_elem elements[] = {
	BT_MESH_ELEM(
		1,
		BT_MESH_MODEL_LIST(
			BT_MESH_MODEL_CFG_SRV,
			BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub)),
		BT_MESH_MODEL_LIST(BT_MESH_MODEL_GUS_CLI(&gus))),
};

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
        bt_mesh_reset();
        dk_set_leds(1);
    }
    else {
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
