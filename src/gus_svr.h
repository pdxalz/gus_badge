/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef BT_MESH_GUS_SVR_H__
#define BT_MESH_GUS_SVR_H__

#include <bluetooth/mesh.h>
#include <bluetooth/mesh/model_types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_BT_MESH_GUS_NAME_LENGTH 12   // max length of a name
#define NUM_PROXIMITY_REPORTS 6             // number of proximity records
                                            // sent in a message

/** Company ID of the Bluetooth Mesh Gus model. */
#define BT_MESH_GUS_VENDOR_COMPANY_ID    0xFFFF  // not a real company

/** Model ID of the Bluetooth Mesh Gus model. */
#define BT_MESH_GUS_VENDOR_MODEL_ID      0x0042 // answer to life, universe &
                                                // everything

/** Sign in opcode. */
#define BT_MESH_GUS_OP_SIGN_IN BT_MESH_MODEL_OP_3(0x04, \
				       BT_MESH_GUS_VENDOR_COMPANY_ID)

/** Sign in reply opcode. */
#define BT_MESH_GUS_OP_SIGN_IN_REPLY BT_MESH_MODEL_OP_3(0x05, \
				       BT_MESH_GUS_VENDOR_COMPANY_ID)

/** Set State opcode. */
#define BT_MESH_GUS_OP_SET_STATE BT_MESH_MODEL_OP_3(0x06, \
				       BT_MESH_GUS_VENDOR_COMPANY_ID)

/** Set name opcode. */
#define BT_MESH_GUS_OP_SET_NAME BT_MESH_MODEL_OP_3(0x07, \
				       BT_MESH_GUS_VENDOR_COMPANY_ID)

/**  Report opcode. */
#define BT_MESH_GUS_OP_REPORT BT_MESH_MODEL_OP_3(0x08, \
				       BT_MESH_GUS_VENDOR_COMPANY_ID)

/** Report reply opcode. */
#define BT_MESH_GUS_OP_REPORT_REPLY BT_MESH_MODEL_OP_3(0x09, \
				       BT_MESH_GUS_VENDOR_COMPANY_ID)

/** Check proximity opcode. */
#define BT_MESH_GUS_OP_CHECK_PROXIMITY BT_MESH_MODEL_OP_3(0x0A, \
				       BT_MESH_GUS_VENDOR_COMPANY_ID)


struct gus_report_data {
    uint16_t addr;
    int8_t rssi;
    }; 


#define BT_MESH_GUS_MSG_MINLEN_MESSAGE 1
#define BT_MESH_GUS_MSG_MAXLEN_MESSAGE (\
				     CONFIG_BT_MESH_GUS_NAME_LENGTH \
				     + 1) /* + \0 */
#define BT_MESH_GUS_MSG_LEN_SIGN_IN_REPLY (CONFIG_BT_MESH_GUS_NAME_LENGTH+1)
#define BT_MESH_GUS_MSG_LEN_SET_STATE 1
#define BT_MESH_GUS_MSG_LEN_REPORT_REPLY (NUM_PROXIMITY_REPORTS*sizeof(struct gus_report_data)+1)
#define BT_MESH_GUS_MSG_LEN_REQUEST 0


/** Bluetooth Mesh Gus state values. */
enum bt_mesh_gus_state {
	BT_MESH_GUS_IDENTIFY,
	BT_MESH_GUS_HEALTHY,
	BT_MESH_GUS_INFECTED,
	BT_MESH_GUS_MASKED,
	BT_MESH_GUS_MASKED_INFECTED,
	BT_MESH_GUS_VACCINATED,
	BT_MESH_GUS_VACCINATED_INFECTED,
	BT_MESH_GUS_VACCINATED_MASKED,
	BT_MESH_GUS_VACCINATED_MASKED_INFECTED,	
        BT_MESH_GUS_OFF,
};

/* Forward declaration of the Bluetooth Mesh Gus model context. */
struct bt_mesh_gus;

/** @def BT_MESH_MODEL_GUS_SVR
 *
 * @brief Bluetooth Mesh Gus server model composition data entry.
 *
 * @param[in] _gus Pointer to a @ref bt_mesh_gus_svr instance.
 */
#define BT_MESH_MODEL_GUS_SVR(_gus)                               \
		BT_MESH_MODEL_VND_CB(BT_MESH_GUS_VENDOR_COMPANY_ID,   \
			BT_MESH_GUS_VENDOR_MODEL_ID,                      \
			_bt_mesh_gus_svr_op, &(_gus)->pub,                    \
			BT_MESH_MODEL_USER_DATA(struct bt_mesh_gus,       \
						_gus),                        \
			&_bt_mesh_gus_svr_cb)

/** Bluetooth Mesh Gus Server model handlers. */
struct bt_mesh_gus_handlers {
	/** @brief Called after the node has been provisioned, or after all
	 * mesh data has been loaded from persistent storage.
	 *
	 * @param[in] svr Gus Server instance that has been started.
	 */
	void (*const start)(struct bt_mesh_gus *gus);

	/** @brief Handler for a sign in message.
	 *
	 * @param[in] Gus Server instance that received the text message.
	 * @param[in] ctx Context of the incoming message.
	 * @param[in] addr address of sender.
	 */
	void (*const sign_in)(struct bt_mesh_gus *gus,
			       struct bt_mesh_msg_ctx *ctx,
                               uint16_t addr);

	/** @brief Handler for a set state message.
	 *
	 * @param[in] gus Server instance that received the text message.
	 * @param[in] ctx Context of the incoming message.
	 * @param[in] state of a Gus Server
	 * the message.
	 */
	void (*const set_state)(struct bt_mesh_gus *gus,
			       struct bt_mesh_msg_ctx *ctx,
			       enum bt_mesh_gus_state state);

	/** @brief Handler for a set name message.
	 *
	 * @param[in] Gus Server that received the text message.
	 * @param[in] ctx Context of the incoming message.
	 * @param[in] msg Pointer to a received name terminated with
	 * a null character, '\0'.
	 */
	void (*const set_name)(struct bt_mesh_gus *gus,
				      struct bt_mesh_msg_ctx *ctx,
				      const uint8_t *msg);

	/** @brief Handler for a report request.
	 *
	 * @param[in] Gus Server instance that received the text message.
	 * @param[in] ctx Context of the incoming message.
	 */
	void (*const report_request)(struct bt_mesh_gus *gus,
			       struct bt_mesh_msg_ctx *ctx);

	/** @brief Handler for a reply on a report request.
	 *
	 * @param[in] Gus Server instance that received the reply.
	 * @param[in] ctx Context of the incoming message.
	 * @param[in] msg Pointer to a proximity report structure
	 * a null character, '\0'.	 */
	void (*const report_reply)(struct bt_mesh_gus *gus,
				    struct bt_mesh_msg_ctx *ctx,
				      const uint8_t *msg);

	/** @brief Handler for a check proximity message.
	 *
	 * @param[in] Gus Server instance that received the text message.
	 * @param[in] ctx Context of the incoming message.
	 * @param[in] addr address of sender.
	 */
	void (*const check_proximity)(struct bt_mesh_gus *gus,
			       struct bt_mesh_msg_ctx *ctx,
                               uint16_t addr);


};


/**
 * Bluetooth Mesh Gus Server model context.
 */
struct bt_mesh_gus {
	/** Access model pointer. */
	struct bt_mesh_model *model;
	/** Publish parameters. */
	struct bt_mesh_model_pub pub;
	/** Publication message. */
	struct net_buf_simple pub_msg;
	/** badge name. */
	uint8_t buf[BT_MESH_MODEL_BUF_LEN(BT_MESH_GUS_OP_SET_NAME,
					  BT_MESH_GUS_MSG_MAXLEN_MESSAGE)];
        uint8_t name[CONFIG_BT_MESH_GUS_NAME_LENGTH+1];
	/** Handler function structure. */
	const struct bt_mesh_gus_handlers *handlers;
	/** Current Presence value. */
	enum bt_mesh_gus_state state;
};



/** @brief Set the server state.
 *
 * @param[in] gus     Gus Server model instance to set presence on.
 * @param[in] addr    Address of the gus server to set the state of.
 * @param[in] state   New server state.
 *
 * @retval 0 Successfully set the preceive and sent the message.
 * @retval -EADDRNOTAVAIL Publishing is not configured.
 * @retval -EAGAIN The device has not been provisioned.
 */
int bt_mesh_gus_svr_state_set(struct bt_mesh_gus *gus,
				  uint16_t addr,
				  enum bt_mesh_gus_state state);

/** @brief Proximity report reply.
 *
 * @param[in] gus     Gus server model instance to sign into.
 * @param[in] report Pointer array of bytes containing report
 *
 * @retval 0 Successfully set the preceive and sent the message.
 * @retval -EADDRNOTAVAIL Publishing is not configured.
 * @retval -EAGAIN The device has not been provisioned.
 */
int bt_mesh_gus_svr_report_reply(struct bt_mesh_gus *gus,
				  struct bt_mesh_msg_ctx *ctx, 
				  const uint8_t *report);

/** @brief Check Proximity.
 *
 * @param[in] gus     Gus server model instance to sign into.
 *
 * @retval 0 Successfully set the preceive and sent the message.
 * @retval -EADDRNOTAVAIL Publishing is not configured.
 * @retval -EAGAIN The device has not been provisioned.
 */
int bt_mesh_gus_svr_check_proximity(struct bt_mesh_gus *gus);

/** @cond INTERNAL_HIDDEN */
extern const struct bt_mesh_model_op _bt_mesh_gus_svr_op[];
extern const struct bt_mesh_model_cb _bt_mesh_gus_svr_cb;
/** @endcond */

#ifdef __cplusplus
}
#endif

#endif /* BT_MESH_GUS_SVR_H__ */

/** @} */
