/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file
 * @defgroup bt_mesh_gus_cli
 * @{
 * @brief API for the Bluetooth Mesh Gus Client model.
 */

#ifndef BT_MESH_GUS_CLI_H__
#define BT_MESH_GUS_CLI_H__

#include <bluetooth/mesh.h>
#include <bluetooth/mesh/model_types.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define GUS_NAME_LEN 12
#define NUM_PROXIMITY_REPORTS  4
#define CONFIG_BT_MESH_GUS_NAME_LENGTH 16

/* .. include_startingpoint_gus_cli_rst_1 */
/** Company ID of the Bluetooth Mesh Gus Client model. */
#define BT_MESH_GUS_CLI_VENDOR_COMPANY_ID    0xFFFF  // not a real company

/** Model ID of the Bluetooth Mesh Gus Client model. */
#define BT_MESH_GUS_CLI_VENDOR_MODEL_ID      0x0042 // answer to life, universe & everything

/** Sign in opcode. */
#define BT_MESH_GUS_CLI_OP_SIGN_IN BT_MESH_MODEL_OP_3(0x04, \
				       BT_MESH_GUS_CLI_VENDOR_COMPANY_ID)

/** Sign in reply opcode. */
#define BT_MESH_GUS_CLI_OP_SIGN_IN_REPLY BT_MESH_MODEL_OP_3(0x05, \
				       BT_MESH_GUS_CLI_VENDOR_COMPANY_ID)

/** Set State opcode. */
#define BT_MESH_GUS_CLI_OP_SET_STATE BT_MESH_MODEL_OP_3(0x06, \
				       BT_MESH_GUS_CLI_VENDOR_COMPANY_ID)

/** Set name opcode. */
#define BT_MESH_GUS_CLI_OP_SET_NAME BT_MESH_MODEL_OP_3(0x07, \
				       BT_MESH_GUS_CLI_VENDOR_COMPANY_ID)

/**  Report opcode. */
#define BT_MESH_GUS_CLI_OP_REPORT BT_MESH_MODEL_OP_3(0x08, \
				       BT_MESH_GUS_CLI_VENDOR_COMPANY_ID)

/** Report reply opcode. */
#define BT_MESH_GUS_CLI_OP_REPORT_REPLY BT_MESH_MODEL_OP_3(0x09, \
				       BT_MESH_GUS_CLI_VENDOR_COMPANY_ID)

/** Check proximity opcode. */
#define BT_MESH_GUS_CLI_OP_CHECK_PROXIMITY BT_MESH_MODEL_OP_3(0x0A, \
				       BT_MESH_GUS_CLI_VENDOR_COMPANY_ID)

/* .. include_endpoint_gus_cli_rst_1 */

#define BT_MESH_GUS_CLI_MSG_MINLEN_MESSAGE 1
#define BT_MESH_GUS_CLI_MSG_MAXLEN_MESSAGE (\
				     CONFIG_BT_MESH_GUS_NAME_LENGTH \
				     + 1) /* + \0 */
#define BT_MESH_GUS_CLI_MSG_LEN_SIGN_IN_REPLY (CONFIG_BT_MESH_GUS_NAME_LENGTH + 1)
#define BT_MESH_GUS_CLI_MSG_LEN_SET_STATE 1
#define BT_MESH_GUS_CLI_MSG_LEN_REPORT_REPLY (NUM_PROXIMITY_REPORTS*(2+1))
#define BT_MESH_GUS_CLI_MSG_LEN_REQUEST 0


/** Bluetooth Mesh Gus Client state values. */
enum bt_mesh_gus_cli_state {
	BT_MESH_GUS_CLI_IDENTIFY,
	BT_MESH_GUS_CLI_HEALTHY,
	BT_MESH_GUS_CLI_INFECTED,
	BT_MESH_GUS_CLI_MASKED,
	BT_MESH_GUS_CLI_MASKED_INFECTED,
	BT_MESH_GUS_CLI_VACCINATED,
	BT_MESH_GUS_CLI_VACCINATED_INFECTED,
	BT_MESH_GUS_CLI_VACCINATED_MASKED,
	BT_MESH_GUS_CLI_VACCINATED_MASKED_INFECTED,	
        BT_MESH_GUS_CLI_OFF,
};

/* Forward declaration of the Bluetooth Mesh Gus Client model context. */
struct bt_mesh_gus_cli;

/* .. include_startingpoint_gus_cli_rst_2 */
/** @def BT_MESH_MODEL_GUS_CLI
 *
 * @brief Bluetooth Mesh Gus Client model composition data entry.
 *
 * @param[in] _gus Pointer to a @ref bt_mesh_gus_cli instance.
 */
#define BT_MESH_MODEL_GUS_CLI(_gus)                               \
		BT_MESH_MODEL_VND_CB(BT_MESH_GUS_CLI_VENDOR_COMPANY_ID,   \
			BT_MESH_GUS_CLI_VENDOR_MODEL_ID,                      \
			_bt_mesh_gus_cli_op, &(_gus)->pub,                    \
			BT_MESH_MODEL_USER_DATA(struct bt_mesh_gus_cli,       \
						_gus),                        \
			&_bt_mesh_gus_cli_cb)
/* .. include_endpoint_gus_cli_rst_2 */

/** Bluetooth Mesh Gus Client model handlers. */
struct bt_mesh_gus_cli_handlers {
	/** @brief Called after the node has been provisioned, or after all
	 * mesh data has been loaded from persistent storage.
	 *
	 * @param[in] cli Gus Client instance that has been started.
	 */
	void (*const start)(struct bt_mesh_gus_cli *gus);

	/** @brief Handler for a sign in message.
	 *
	 * @param[in] cli Gus client instance that received the text message.
	 * @param[in] ctx Context of the incoming message.
	 * @param[in] addr address of sender.
	 */
	void (*const sign_in)(struct bt_mesh_gus_cli *gus,
			       struct bt_mesh_msg_ctx *ctx,
                               uint16_t addr);

	/** @brief Handler for a sign in reply.
	 *
	 * @param[in] cli Gus client instance that received the reply.
	 * @param[in] ctx Context of the incoming message.
	 * @param[in] msg Pointer to a name terminated with
	 * a null character, '\0'.
	 */
	void (*const sign_in_reply)(struct bt_mesh_gus_cli *gus,
				    struct bt_mesh_msg_ctx *ctx,
				      const uint8_t *msg);

	/** @brief Handler for a set state message.
	 *
	 * @param[in] cli gus client instance that received the text message.
	 * @param[in] ctx Context of the incoming message.
	 * @param[in] state of a Gus Client
	 * the message.
	 */
	void (*const set_state)(struct bt_mesh_gus_cli *gus,
			       struct bt_mesh_msg_ctx *ctx,
			       enum bt_mesh_gus_cli_state state);

	/** @brief Handler for a set name message.
	 *
	 * @param[in] cli Gus client that received the text message.
	 * @param[in] ctx Context of the incoming message.
	 * @param[in] msg Pointer to a received name terminated with
	 * a null character, '\0'.
	 */
	void (*const set_name)(struct bt_mesh_gus_cli *gus,
				      struct bt_mesh_msg_ctx *ctx,
				      const uint8_t *msg);

	/** @brief Handler for a report request.
	 *
	 * @param[in] cli Gus client instance that received the text message.
	 * @param[in] ctx Context of the incoming message.
	 */
	void (*const report_request)(struct bt_mesh_gus_cli *gus,
			       struct bt_mesh_msg_ctx *ctx);

	/** @brief Handler for a reply on a report request.
	 *
	 * @param[in] cli Gus client instance that received the reply.
	 * @param[in] ctx Context of the incoming message.
	 * @param[in] msg Pointer to a proximity report structure
	 * a null character, '\0'.	 */
	void (*const report_reply)(struct bt_mesh_gus_cli *gus,
				    struct bt_mesh_msg_ctx *ctx,
				      const uint8_t *msg);

	/** @brief Handler for a check proximity message.
	 *
	 * @param[in] cli Gus client instance that received the text message.
	 * @param[in] ctx Context of the incoming message.
	 */
	void (*const check_proximity)(struct bt_mesh_gus_cli *gus,
			      struct bt_mesh_msg_ctx *ctx);

};

/* .. include_startingpoint_gus_cli_rst_3 */
/**
 * Bluetooth Mesh Gus Client model context.
 */
struct bt_mesh_gus_cli {
	/** Access model pointer. */
	struct bt_mesh_model *model;
	/** Publish parameters. */
	struct bt_mesh_model_pub pub;
	/** Publication message. */
	struct net_buf_simple pub_msg;
	/** badge name. */
	uint8_t buf[BT_MESH_MODEL_BUF_LEN(BT_MESH_GUS_CLI_OP_SET_NAME,
					  BT_MESH_GUS_CLI_MSG_MAXLEN_MESSAGE)];
        uint8_t name[CONFIG_BT_MESH_GUS_NAME_LENGTH+1];
	/** Handler function structure. */
	const struct bt_mesh_gus_cli_handlers *handlers;
	/** Current Presence value. */
	enum bt_mesh_gus_cli_state state;
};
/* .. include_endpoint_gus_cli_rst_3 */

/** @brief Publish the sign in request to the mesh network.
 *
 * @param[in] gus     Gus Client model instance to sign into.
 *
 * @retval 0 Successfully set the preceive and sent the message.
 * @retval -EADDRNOTAVAIL Publishing is not configured.
 * @retval -EAGAIN The device has not been provisioned.
 */
int bt_mesh_gus_cli_sign_in(struct bt_mesh_gus_cli *gus);

/** @brief Send a reply for the sign in request to the mesh network.
 *
 * @param[in] gus     Gus Client model instance to sign into.
 * @param[in] name Pointer to a name. Must be terminated with
 * a null character, '\0'.
 *
 * @retval 0 Successfully set the preceive and sent the message.
 * @retval -EADDRNOTAVAIL Publishing is not configured.
 * @retval -EAGAIN The device has not been provisioned.
 */
int bt_mesh_gus_cli_sign_in_reply(struct bt_mesh_gus_cli *gus, 
                                    struct bt_mesh_msg_ctx *ctx, 
                                    const uint8_t * name);
/** @brief Set the client state.
 *
 * @param[in] gus     Gus Client model instance to set presence on.
 * @param[in] addr    Address of the gus client to set the state of.
 * @param[in] state   New client state.
 *
 * @retval 0 Successfully set the preceive and sent the message.
 * @retval -EADDRNOTAVAIL Publishing is not configured.
 * @retval -EAGAIN The device has not been provisioned.
 */
int bt_mesh_gus_cli_state_set(struct bt_mesh_gus_cli *gus,
				  uint16_t addr,
				  enum bt_mesh_gus_cli_state state);

/** @brief Set the client name.
 *
 * @param[in] cli Gus Client model instance.
 * @param[in] addr    Address of the gus client to set the state of.
 * @param[in] name Pointer to a name. Must be terminated with
 * a null character, '\0'.
 *
 * @retval 0 Successfully sent the message.
 * @retval -EADDRNOTAVAIL Publishing is not configured.
 * @retval -EAGAIN The device has not been provisioned.
 */
int bt_mesh_gus_cli_name_set(struct bt_mesh_gus_cli *gus,
				  uint16_t addr,
				  const uint8_t *name);

/** @brief Request a proximity report.
 *
 * @param[in] gus     Gus Client model instance to sign into.
 * @param[in] addr    Address of the gus client.
 *
 * @retval 0 Successfully set the preceive and sent the message.
 * @retval -EADDRNOTAVAIL Publishing is not configured.
 * @retval -EAGAIN The device has not been provisioned.
 */
int bt_mesh_gus_cli_report_request(struct bt_mesh_gus_cli *gus,
				  uint16_t addr);

/** @brief Proximity report reply.
 *
 * @param[in] gus     Gus Client model instance to sign into.
 * @param[in] report Pointer array of bytes containing report
 *
 * @retval 0 Successfully set the preceive and sent the message.
 * @retval -EADDRNOTAVAIL Publishing is not configured.
 * @retval -EAGAIN The device has not been provisioned.
 */
int bt_mesh_gus_cli_report_reply(struct bt_mesh_gus_cli *gus,
				  struct bt_mesh_msg_ctx *ctx, 
				  const uint8_t *report);

/** @brief Check Proximity.
 *
 * @param[in] gus     Gus Client model instance to sign into.
 *
 * @retval 0 Successfully set the preceive and sent the message.
 * @retval -EADDRNOTAVAIL Publishing is not configured.
 * @retval -EAGAIN The device has not been provisioned.
 */
int bt_mesh_gus_cli_check_proximity(struct bt_mesh_gus_cli *gus);

/** @cond INTERNAL_HIDDEN */
extern const struct bt_mesh_model_op _bt_mesh_gus_cli_op[];
extern const struct bt_mesh_model_cb _bt_mesh_gus_cli_cb;
/** @endcond */

#ifdef __cplusplus
}
#endif

#endif /* BT_MESH_GUS_CLI_H__ */

/** @} */
