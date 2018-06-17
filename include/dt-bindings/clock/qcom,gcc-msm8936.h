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

/* CPU clocks */
#define CLK_A53SS_C0_PLL		0
#define CLK_A53SS_C1_PLL		1
#define CLK_A53SS_CCI_PLL		2

#define GPLL0_AO			0

/* Hardware clocks */
#define GCC_XO				0
#define GCC_XO_A			1

/* MDSS clocks */
#define BYTE0_CLK_SRC			0
#define PCLK0_CLK_SRC			1
#define GCC_MDSS_BYTE0_CLK		2
#define GCC_MDSS_PCLK0_CLK		3

/* PLLs */
#define GPLL0				0
#define GPLL0_VOTE			1
#define GPLL1				2
#define GPLL1_VOTE			3
#define GPLL2				4
#define GPLL2_VOTE			5
#define GPLL3				6
#define GPLL3_VOTE			7
#define GPLL4				8
#define GPLL6				9

/* RCGs */
#define APSS_AHB_CLK_SRC		10
#define CAMSS_AHB_CLK_SRC		11
#define CSI0_CLK_SRC			12
#define CSI1_CLK_SRC			13
#define CSI2_CLK_SRC			14
#define VFE0_CLK_SRC			15
#define MDP_CLK_SRC			16
#define GFX3D_CLK_SRC			17
#define BLSP1_QUP1_I2C_APPS_CLK		18
#define BLSP1_QUP2_I2C_APPS_CLK		19
#define BLSP1_QUP3_I2C_APPS_CLK		20
#define BLSP1_QUP4_I2C_APPS_CLK		21
#define BLSP1_QUP5_I2C_APPS_CLK		22
#define BLSP1_QUP6_I2C_APPS_CLK		23
#define BLSP1_UART1_APPS_CLK		24
#define BLSP1_UART2_APPS_CLK		25
#define CCI_CLK_SRC			26
#define CAMSS_GP0_CLK_SRC		27
#define CAMSS_GP1_CLK_SRC		28
#define JPEG0_CLK_SRC			29
#define MCLK0_CLK_SRC			30
#define MCLK1_CLK_SRC			31
#define MCLK2_CLK_SRC			32
#define CSI0PHYTIMER_CLK_SRC		33
#define CSI1PHYTIMER_CLK_SRC		34
#define CPP_CLK_SRC			35
#define GP1_CLK_SRC			36
#define GP2_CLK_SRC			37
#define GP3_CLK_SRC			38
#define ESC0_CLK_SRC			39
#define VSYNC_CLK_SRC			40
#define PDM2_CLK_SRC			41
#define SDCC1_APPS_CLK_SRC		42
#define SDCC2_APPS_CLK_SRC		43
#define USB_HS_SYSTEM_CLK_SRC		44
#define USB_FS_SYSTEM_CLK_SRC		45
#define USB_FS_IC_CLK_SRC		46
#define VCODEC0_CLK_SRC			47

/* Voteable Clocks */
#define GCC_BLSP1_AHB_CLK		48
#define GCC_BOOT_ROM_AHB_CLK		49
#define GCC_PRNG_AHB_CLK		50
#define GCC_APSS_TCU_CLK		51
#define GCC_GFX_TBU_CLK			52
#define GCC_GFX_TCU_CLK			53
#define GCC_GTCU_AHB_CLK		54
#define GCC_JPEG_TBU_CLK		55
#define GCC_MDP_TBU_CLK			56
#define GCC_SMMU_CFG_CLK		57
#define GCC_VENUS_TBU_CLK		58
#define GCC_VFE_TBU_CLK			59
#define GCC_TBU_CPP_CLK			60
#define GCC_MDP_RT_TBU_CLK		61

/* Branches */
#define GCC_BLSP1_QUP1_I2C_APPS_CLK	62
#define GCC_BLSP1_QUP2_I2C_APPS_CLK	63
#define GCC_BLSP1_QUP3_I2C_APPS_CLK	64
#define GCC_BLSP1_QUP4_I2C_APPS_CLK	65
#define GCC_BLSP1_QUP5_I2C_APPS_CLK	66
#define GCC_BLSP1_QUP6_I2C_APPS_CLK	67
#define GCC_BLSP1_UART1_APPS_CLK	68
#define GCC_BLSP1_UART2_APPS_CLK	69
#define GCC_CAMSS_CCI_AHB_CLK		70
#define GCC_CAMSS_CCI_CLK		71
#define GCC_CAMSS_CSI0_AHB_CLK		72
#define GCC_CAMSS_CSI0_CLK		73
#define GCC_CAMSS_CSI0PHY_CLK		74
#define GCC_CAMSS_CSI0PIX_CLK		75
#define GCC_CAMSS_CSI0RDI_CLK		76
#define GCC_CAMSS_CSI1_AHB_CLK		77
#define GCC_CAMSS_CSI1_CLK		78
#define GCC_CAMSS_CSI1PHY_CLK		79
#define GCC_CAMSS_CSI1PIX_CLK		80
#define GCC_CAMSS_CSI1RDI_CLK		81
#define GCC_CAMSS_CSI2_AHB_CLK		82
#define GCC_CAMSS_CSI2_CLK		83
#define GCC_CAMSS_CSI2PHY_CLK		84
#define GCC_CAMSS_CSI2PIX_CLK		85
#define GCC_CAMSS_CSI2RDI_CLK		86
#define GCC_CAMSS_CSI_VFE0_CLK		87
#define GCC_CAMSS_GP0_CLK		88
#define GCC_CAMSS_GP1_CLK		89
#define GCC_CAMSS_ISPIF_AHB_CLK		90
#define GCC_CAMSS_JPEG0_CLK		91
#define GCC_CAMSS_JPEG_AHB_CLK		92
#define GCC_CAMSS_JPEG_AXI_CLK		93
#define GCC_CAMSS_MCLK0_CLK		94
#define GCC_CAMSS_MCLK1_CLK		95
#define GCC_CAMSS_MCLK2_CLK		96
#define GCC_CAMSS_MICRO_AHB_CLK		97
#define GCC_CAMSS_CSI0PHYTIMER_CLK	98
#define GCC_CAMSS_CSI1PHYTIMER_CLK	99
#define GCC_CAMSS_AHB_CLK		100
#define GCC_CAMSS_TOP_AHB_CLK		101
#define GCC_CAMSS_CPP_AHB_CLK		102
#define GCC_CAMSS_CPP_CLK		103
#define GCC_CAMSS_VFE0_CLK		104
#define GCC_CAMSS_VFE_AHB_CLK		105
#define GCC_CAMSS_VFE_AXI_CLK		106
#define GCC_OXILI_GMEM_CLK		107
#define GCC_GP1_CLK			108
#define GCC_GP2_CLK			109
#define GCC_GP3_CLK			110
#define GCC_MDSS_AHB_CLK		111
#define GCC_MDSS_AXI_CLK		112
#define GCC_MDSS_ESC0_CLK		113
#define GCC_MDSS_MDP_CLK		114
#define GCC_MDSS_VSYNC_CLK		115
#define GCC_MSS_CFG_AHB_CLK		116
#define GCC_OXILI_AHB_CLK		117
#define GCC_OXILI_TIMER_CLK		118
#define GCC_OXILI_GFX3D_CLK		119
#define GCC_PDM2_CLK			120
#define GCC_PDM_AHB_CLK			121
#define GCC_SDCC1_AHB_CLK		122
#define GCC_SDCC1_APPS_CLK		123
#define GCC_SDCC2_AHB_CLK		124
#define GCC_SDCC2_APPS_CLK		125
#define GCC_USB2A_PHY_SLEEP_CLK		126
#define GCC_USB_HS_AHB_CLK		127
#define GCC_USB_HS_SYSTEM_CLK		128
#define GCC_USB_FS_AHB_CLK		129
#define GCC_USB_FS_IC_CLK		130
#define GCC_USB_FS_SYSTEM_CLK		131
#define GCC_VENUS0_AHB_CLK		132
#define GCC_VENUS0_AXI_CLK		133
#define GCC_VENUS0_VCODEC0_CLK		134
#define GCC_VENUS0_CORE0_VCODEC0_CLK	135
#define GCC_VENUS0_CORE1_VCODEC0_CLK	136
#define GCC_BIMC_GFX_CLK		137
#define GCC_BIMC_GPU_CLK		138
#define WCNSS_M_CLK			139

/* Crypto clocks */
#define GCC_CRYPTO_CLK			140
#define GCC_CRYPTO_AHB_CLK		141
#define GCC_CRYPTO_AXI_CLK		142
#define CRYPTO_CLK_SRC			143

/* QoS Reference clock */
#define GCC_SNOC_QOSGEN_CLK		144

#endif
