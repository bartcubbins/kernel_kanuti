/*
 * Copyright (C) 2018, Pavel Dubrova <pashadubrova@gmail.com>
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

#ifndef __DRIVERS_CLK_QCOM_VDD_LEVEL_8936_H
#define __DRIVERS_CLK_QCOM_VDD_LEVEL_8936_H

#include <linux/regulator/rpm-smd-regulator.h>
#include <linux/regulator/consumer.h>

#define VDD_DIG_FMAX_MAP1(l1, f1)				\
	.vdd_class = &vdd_dig,					\
	.rate_max = (unsigned long[VDD_DIG_NUM]) {		\
		[VDD_DIG_##l1] = (f1),				\
	},							\
	.num_rate_max = VDD_DIG_NUM

#define VDD_DIG_FMAX_MAP2(l1, f1, l2, f2)			\
	.vdd_class = &vdd_dig,					\
	.rate_max = (unsigned long[VDD_DIG_NUM]) {		\
		[VDD_DIG_##l1] = (f1),				\
		[VDD_DIG_##l2] = (f2),				\
	},							\
	.num_rate_max = VDD_DIG_NUM

#define VDD_DIG_FMAX_MAP3(l1, f1, l2, f2, l3, f3)		\
	.vdd_class = &vdd_dig,					\
	.rate_max = (unsigned long[VDD_DIG_NUM]) {		\
		[VDD_DIG_##l1] = (f1),				\
		[VDD_DIG_##l2] = (f2),				\
		[VDD_DIG_##l3] = (f3),				\
	},							\
	.num_rate_max = VDD_DIG_NUM

#define VDD_DIG_FMAX_MAP4(l1, f1, l2, f2, l3, f3, l4, f4)	\
	.vdd_class = &vdd_dig,					\
	.rate_max = (unsigned long[VDD_DIG_NUM]) {		\
		[VDD_DIG_##l1] = (f1),				\
		[VDD_DIG_##l2] = (f2),				\
		[VDD_DIG_##l3] = (f3),				\
		[VDD_DIG_##l4] = (f4),				\
	},							\
	.num_rate_max = VDD_DIG_NUM

enum vdd_dig_levels {
	VDD_DIG_NONE,
	VDD_DIG_LOW,
	VDD_DIG_NOMINAL,
	VDD_DIG_NOMINAL_PLUS,
	VDD_DIG_HIGH,
	VDD_DIG_NUM
};

enum vdd_sr2_pll_levels {
	VDD_SR2_PLL_OFF,
	VDD_SR2_PLL_SVS,
	VDD_SR2_PLL_NOM,
	VDD_SR2_PLL_TUR,
	VDD_SR2_PLL_NUM,
};

enum vdd_hf_pll_levels {
	VDD_HF_PLL_OFF,
	VDD_HF_PLL_SVS,
	VDD_HF_PLL_NOM,
	VDD_HF_PLL_TUR,
	VDD_HF_PLL_NUM,
};

static int vdd_corner[] = {
	RPM_REGULATOR_CORNER_NONE,		/* VDD_DIG_NONE */
	RPM_REGULATOR_CORNER_SVS_SOC,		/* VDD_DIG_LOW */
	RPM_REGULATOR_CORNER_NORMAL,		/* VDD_DIG_NOMINAL */
	RPM_REGULATOR_CORNER_TURBO,		/* VDD_DIG_NOMINAL_PLUS */
	RPM_REGULATOR_CORNER_SUPER_TURBO,	/* VDD_DIG_HIGH */
};

static int vdd_sr2_levels[] = {
	0,	 RPM_REGULATOR_CORNER_NONE,		/* VDD_SR2_PLL_OFF */
	1800000, RPM_REGULATOR_CORNER_SVS_SOC,		/* VDD_SR2_PLL_SVS */
	1800000, RPM_REGULATOR_CORNER_NORMAL,		/* VDD_SR2_PLL_NOM */
	1800000, RPM_REGULATOR_CORNER_SUPER_TURBO,	/* VDD_SR2_PLL_TUR */
};

static int vdd_hf_levels[] = {
	0,	 RPM_REGULATOR_CORNER_NONE,		/* VDD_HF_PLL_OFF */
	1800000, RPM_REGULATOR_CORNER_SVS_SOC,		/* VDD_HF_PLL_SVS */
	1800000, RPM_REGULATOR_CORNER_NORMAL,		/* VDD_HF_PLL_NOM */
	1800000, RPM_REGULATOR_CORNER_SUPER_TURBO,	/* VDD_HF_PLL_TUR */
};

#endif
