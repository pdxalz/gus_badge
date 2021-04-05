/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh/models.h>
#include <dk_buttons_and_leds.h>
#include "gus_model_handler.h"


////////////////// FORWORDS
//gus
static void gus_set(struct bt_mesh_lvl_srv *srv, struct bt_mesh_msg_ctx *ctx,
		    const struct bt_mesh_lvl_set *set,
		    struct bt_mesh_lvl_status *rsp);

static void gus_get(struct bt_mesh_lvl_srv *srv, struct bt_mesh_msg_ctx *ctx,
		    struct bt_mesh_lvl_status *rsp);



////////////////// STRUCTS
//gus
static const struct bt_mesh_lvl_srv_handlers gus_handlers = {
	.set = gus_set,
	.get = gus_get,
};

struct gus_ctx {
	struct bt_mesh_lvl_srv srv;
	uint32_t remaining;
	uint16_t value;
};

static struct gus_ctx gus_ctx = {
		.srv = BT_MESH_LVL_SRV_INIT(&gus_handlers),
};

uint16_t blinker = 0;
uint16_t get_blinker(void) {return blinker;}
void set_blinker(uint16_t val) {blinker = val;}
uint16_t dec_blinker(void) {return --blinker;}

enum gus_mode
{
  gm_alert,
  gm_healthy,
  gm_infected,
  gm_innoculated,
  gm_red_green,
  gm_blue_green,
  gm_red_blue,
  gm_black
};

///////////////////// PROCEDURES
static void gus_status(struct gus_ctx *gus, struct bt_mesh_lvl_status *status)
{
	status->target = gus->value;
}


static void gus_set(struct bt_mesh_lvl_srv *srv, struct bt_mesh_msg_ctx *ctx,
		    const struct bt_mesh_lvl_set *set,
		    struct bt_mesh_lvl_status *rsp)
{
	struct gus_ctx *gus = CONTAINER_OF(srv, struct gus_ctx, srv);

	if (set->lvl != gus->value || set->lvl == gm_alert) 
        {
    
            switch((uint16_t)set->lvl) {
            case gm_alert:
                set_blinker(100);
            break;
           
            case gm_healthy:
            case 0x2000:
                dk_set_leds(0b00100100);
            break;
            
            case gm_infected:
            case 0x4000:
                dk_set_leds(0b00010010);
            break;

            case gm_innoculated:
            case 0x6000:
                dk_set_leds(0b00001001);
            break;

            case gm_red_green:
            case 0x8000:
                dk_set_leds(0b00010100);
            break;

            case gm_blue_green:
            case 0xa000:
                dk_set_leds(0b00001100);
            break;

            case gm_red_blue:
            case 0xc000:
                dk_set_leds(0b00010001);
            break;
                          
            case gm_black:
            case 0xe000:
                dk_set_leds(0b00000000);
            break;               
            }
            gus->value = set->lvl;
        }

	if (rsp) {
		gus_status(gus, rsp);
	}
}


static void gus_get(struct bt_mesh_lvl_srv *srv, struct bt_mesh_msg_ctx *ctx,
		    struct bt_mesh_lvl_status *rsp)
{
	struct gus_ctx *gus = CONTAINER_OF(srv, struct gus_ctx, srv);

	gus_status(gus, rsp);
}

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

static struct bt_mesh_elem elements[] = {
	BT_MESH_ELEM(
		1, BT_MESH_MODEL_LIST(
			BT_MESH_MODEL_CFG_SRV,
			BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
                        BT_MESH_MODEL_LVL_SRV(&gus_ctx.srv)),
		BT_MESH_MODEL_NONE),
};

static const struct bt_mesh_comp comp = {
	.cid = CONFIG_BT_COMPANY_ID,
	.elem = elements,
	.elem_count = ARRAY_SIZE(elements),
};


uint16_t count=0;
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
