/* Copyright (c) 2018, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "mpm.h"

const struct mpm_pin mpm_msm8939_gic_chip_data[] = {
	{2, 216}, /* tsens_upper_lower_int */
	{49, 172}, /* usb1_hs_async_wakeup_irq */
	{58, 166}, /* usb_hs_irq */
	{53, 104}, /* mdss_irq */
	{62, 222}, /* ee0_krait_hlos_spmi_periph_irq */
	{-1},
};

const struct mpm_pin mpm_msm8939_gpio_chip_data[] = {
	{3, 108},
	{4, 1},
	{5, 5},
	{6, 9},
	{7, 107},
	{8, 98},
	{9, 97},
	{10, 11},
	{11, 69},
	{12, 12},
	{13, 13},
	{14, 20},
	{15, 62},
	{16, 54},
	{17, 21},
	{18, 52},
	{19, 25},
	{20, 51},
	{21, 50},
	{22, 28},
	{23, 31},
	{24, 34},
	{25, 35},
	{26, 36},
	{27, 37},
	{28, 38},
	{29, 49},
	{30, 109},
	{31, 110},
	{32, 111},
	{33, 112},
	{34, 113},
	{35, 114},
	{36, 115},
	{37, 117},
	{38, 118},
	{39, 120},
	{40, 121},
	{50, 66},
	{51, 68},
	{-1},
};
