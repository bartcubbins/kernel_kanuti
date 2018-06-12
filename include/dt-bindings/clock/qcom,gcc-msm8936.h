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

#ifndef _DT_BINDINGS_CLK_MSM_GCC_8936_H
#define _DT_BINDINGS_CLK_MSM_GCC_8936_H

#define GPLL0				0
#define GPLL1				1
#define GPLL2				2
#define GPLL3				3
#define GPLL4				4
#define GPLL6				5

#define APSS_AHB_CLK_SRC		6
#define CAMSS_AHB_CLK_SRC		7
#define CSI0_CLK_SRC			8
#define CSI1_CLK_SRC			9
#define CSI2_CLK_SRC			10
#define VFE0_CLK_SRC			11
#define MDP_CLK_SRC			12
#define GFX3D_CLK_SRC			13

#define BLSP1_QUP1_I2C_APPS_CLK		14
#define BLSP1_QUP2_I2C_APPS_CLK		15
#define BLSP1_QUP3_I2C_APPS_CLK		16
#define BLSP1_QUP4_I2C_APPS_CLK		17
#define BLSP1_QUP5_I2C_APPS_CLK		18
#define BLSP1_QUP6_I2C_APPS_CLK		19
#define BLSP1_UART1_APPS_CLK		20
#define BLSP1_UART2_APPS_CLK		21

#define CCI_CLK_SRC			22
#define CAMSS_GP0_CLK_SRC		23
#define CAMSS_GP1_CLK_SRC		24
#define JPEG0_CLK_SRC			25

#define MCLK0_CLK_SRC			26
#define MCLK1_CLK_SRC			27
#define MCLK2_CLK_SRC			28

#define CSI0PHYTIMER_CLK_SRC		29
#define CSI1PHYTIMER_CLK_SRC		30

#define CPP_CLK_SRC			31
#define GP1_CLK_SRC			32
#define GP2_CLK_SRC			33
#define GP3_CLK_SRC			34
#define ESC0_CLK_SRC			35
#define VSYNC_CLK_SRC			36
#define PDM2_CLK_SRC			37
#define SDCC1_APPS_CLK_SRC		38
#define SDCC2_APPS_CLK_SRC		39
#define USB_HS_SYSTEM_CLK_SRC		40
#define USB_FS_SYSTEM_CLK_SRC		41
#define USB_FS_IC_CLK_SRC		42
#define VCODEC0_CLK_SRC			43

/* Voteable Clocks */
#define GCC_BLSP1_AHB_CLK		44
#define GCC_BOOT_ROM_AHB_CLK		45
#define GCC_PRNG_AHB_CLK		46
#define GCC_APSS_TCU_CLK		47
#define GCC_GFX_TBU_CLK			48
#define GCC_GFX_TCU_CLK			49
#define GCC_GTCU_AHB_CLK		50
#define GCC_JPEG_TBU_CLK		51
#define GCC_MDP_TBU_CLK			52
#define GCC_SMMU_CFG_CLK		53
#define GCC_VENUS_TBU_CLK		54
#define GCC_VFE_TBU_CLK			55
#define GCC_TBU_CPP_CLK			56
#define GCC_MDP_RT_TBU_CLK		57

/* Branches */
#define GCC_BLSP1_QUP1_I2C_APPS_CLK	58
#define GCC_BLSP1_QUP2_I2C_APPS_CLK	59
#define GCC_BLSP1_QUP3_I2C_APPS_CLK	60
#define GCC_BLSP1_QUP4_I2C_APPS_CLK	61
#define GCC_BLSP1_QUP5_I2C_APPS_CLK	62
#define GCC_BLSP1_QUP6_I2C_APPS_CLK	63
#define GCC_BLSP1_UART1_APPS_CLK	64
#define GCC_BLSP1_UART2_APPS_CLK	65

#define GCC_CAMSS_CCI_AHB_CLK		66
#define GCC_CAMSS_CCI_CLK		67
#define GCC_CAMSS_CSI0_AHB_CLK		68
#define GCC_CAMSS_CSI0_CLK		69
#define GCC_CAMSS_CSI0PHY_CLK		70
#define GCC_CAMSS_CSI0PIX_CLK		71
#define GCC_CAMSS_CSI0RDI_CLK		72
#define GCC_CAMSS_CSI1_AHB_CLK		73
#define GCC_CAMSS_CSI1_CLK		74
#define GCC_CAMSS_CSI1PHY_CLK		75
#define GCC_CAMSS_CSI1PIX_CLK		76
#define GCC_CAMSS_CSI1RDI_CLK		77
#define GCC_CAMSS_CSI2_AHB_CLK		78
#define GCC_CAMSS_CSI2_CLK		79
#define GCC_CAMSS_CSI2PHY_CLK		80
#define GCC_CAMSS_CSI2PIX_CLK		81
#define GCC_CAMSS_CSI2RDI_CLK		82
#define GCC_CAMSS_CSI_VFE0_CLK		83
#define GCC_CAMSS_GP0_CLK		84
#define GCC_CAMSS_GP1_CLK		85
#define GCC_CAMSS_ISPIF_AHB_CLK		86
#define GCC_CAMSS_JPEG0_CLK		87
#define GCC_CAMSS_JPEG_AHB_CLK		88
#define GCC_CAMSS_JPEG_AXI_CLK		89
#define GCC_CAMSS_MCLK0_CLK		90
#define GCC_CAMSS_MCLK1_CLK		91
#define GCC_CAMSS_MCLK2_CLK		92
#define GCC_CAMSS_MICRO_AHB_CLK		93
#define GCC_CAMSS_CSI0PHYTIMER_CLK	94
#define GCC_CAMSS_CSI1PHYTIMER_CLK	95
#define GCC_CAMSS_AHB_CLK		96
#define GCC_CAMSS_TOP_AHB_CLK		97
#define GCC_CAMSS_CPP_AHB_CLK		98
#define GCC_CAMSS_CPP_CLK		99
#define GCC_CAMSS_VFE0_CLK		100
#define GCC_CAMSS_VFE_AHB_CLK		101
#define GCC_CAMSS_VFE_AXI_CLK		102

#endif
