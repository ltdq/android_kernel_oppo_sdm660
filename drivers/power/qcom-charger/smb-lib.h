/* Copyright (c) 2016 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SMB2_CHARGER_H
#define __SMB2_CHARGER_H
#include <linux/types.h>
#include <linux/irqreturn.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/consumer.h>

enum print_reason {
	PR_INTERRUPT	= BIT(0),
	PR_REGISTER	= BIT(1),
	PR_MISC		= BIT(2),
};

enum smb_voters {
	DEFAULT_VOTER = 0,
	USER_VOTER,
	PD_VOTER,
	NUM_VOTERS,
};

struct smb_regulator {
	struct regulator_dev	*rdev;
	struct regulator_desc	rdesc;
};

struct smb_irq_data {
	void		*parent_data;
	const char	*name;
};

struct smb_chg_param {
	const char	*name;
	u16		reg;
	int		min_u;
	int		max_u;
	int		step_u;
};

struct smb_params {
	struct smb_chg_param	fcc;
	struct smb_chg_param	fv;
	struct smb_chg_param	usb_icl;
	struct smb_chg_param	dc_icl;
};

struct smb_charger {
	struct device		*dev;
	struct regmap		*regmap;
	struct smb_params	param;
	int			*debug_mask;

	/* locks */
	spinlock_t		write_lock;
	struct mutex		ps_change_lock;

	/* power supplies */
	struct power_supply		*batt_psy;
	struct power_supply		*usb_psy;
	struct power_supply_desc	usb_psy_desc;

	/* regulators */
	struct smb_regulator	*vbus_vreg;
	struct smb_regulator	*vconn_vreg;
	struct regulator	*dpdm_reg;

	/* votables */
	struct votable		*usb_suspend_votable;
	struct votable		*dc_suspend_votable;
	struct votable		*fcc_votable;
	struct votable		*usb_icl_votable;
	struct votable		*dc_icl_votable;
	struct votable		*pd_allowed_votable;

	/* work */
	struct delayed_work	hvdcp_detect_work;
	struct delayed_work	ps_change_timeout_work;

	/* cached status */
	int			voltage_min_uv;
	int			voltage_max_uv;
	bool			pd_active;
	bool			vbus_present;
};

int smblib_read(struct smb_charger *chg, u16 addr, u8 *val);
int smblib_masked_write(struct smb_charger *chg, u16 addr, u8 mask, u8 val);
int smblib_write(struct smb_charger *chg, u16 addr, u8 val);

int smblib_enable_charging(struct smb_charger *chg, bool enable);

int smblib_usb_suspend_vote_callback(struct device *dev,
		int disable, int client, int last_disable, int last_client);
int smblib_dc_suspend_vote_callback(struct device *dev,
		int disable, int client, int last_disable, int last_client);
int smblib_fcc_vote_callback(struct device *dev,
		int fcc_ua, int client, int last_fcc_ua, int last_client);
int smblib_usb_icl_vote_callback(struct device *dev,
		int icl_ua, int client, int last_icl_ua, int last_client);
int smblib_dc_icl_vote_callback(struct device *dev,
		int icl_ua, int client, int last_icl_ua, int last_client);

int smblib_vbus_regulator_enable(struct regulator_dev *rdev);
int smblib_vbus_regulator_disable(struct regulator_dev *rdev);
int smblib_vbus_regulator_is_enabled(struct regulator_dev *rdev);

int smblib_vconn_regulator_enable(struct regulator_dev *rdev);
int smblib_vconn_regulator_disable(struct regulator_dev *rdev);
int smblib_vconn_regulator_is_enabled(struct regulator_dev *rdev);

irqreturn_t smblib_handle_debug(int irq, void *data);
irqreturn_t smblib_handle_batt_psy_changed(int irq, void *data);
irqreturn_t smblib_handle_usb_psy_changed(int irq, void *data);
irqreturn_t smblib_handle_usb_plugin(int irq, void *data);
irqreturn_t smblib_handle_usb_source_change(int irq, void *data);
irqreturn_t smblib_handle_usb_typec_change(int irq, void *data);

int smblib_get_prop_input_suspend(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_batt_present(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_batt_capacity(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_batt_status(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_batt_charge_type(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_batt_health(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_set_prop_input_suspend(struct smb_charger *chg,
				const union power_supply_propval *val);

int smblib_get_prop_usb_present(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_usb_online(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_usb_suspend(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_usb_voltage_now(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_usb_current_max(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_typec_cc_orientation(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_typec_mode(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_typec_power_role(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_get_prop_pd_allowed(struct smb_charger *chg,
				union power_supply_propval *val);
int smblib_set_prop_usb_current_max(struct smb_charger *chg,
				const union power_supply_propval *val);
int smblib_set_prop_usb_voltage_min(struct smb_charger *chg,
				const union power_supply_propval *val);
int smblib_set_prop_usb_voltage_max(struct smb_charger *chg,
				const union power_supply_propval *val);
int smblib_set_prop_typec_power_role(struct smb_charger *chg,
				const union power_supply_propval *val);
int smblib_set_prop_pd_active(struct smb_charger *chg,
				const union power_supply_propval *val);

int smblib_init(struct smb_charger *chg);
#endif /* __SMB2_CHARGER_H */
