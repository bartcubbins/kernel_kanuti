/*
 * Copyright (C) 2007 Google, Inc.
 * Copyright (c) 2009-2017, The Linux Foundation. All rights reserved.
 * Author: San Mehat <san@android.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __ARCH_ARM_MACH_MSM_PM_H
#define __ARCH_ARM_MACH_MSM_PM_H

#include <linux/types.h>
#include <linux/cpuidle.h>
#include <asm/smp_plat.h>
#include <dt-bindings/msm/pm.h>

#if !defined(CONFIG_SMP)
#define msm_secondary_startup NULL
#elif defined(CONFIG_CPU_V7)
#define msm_secondary_startup secondary_startup
#else
#define msm_secondary_startup secondary_holding_pen
#endif

enum msm_pm_sleep_mode {
	MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT,
	MSM_PM_SLEEP_MODE_RETENTION,
	MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE,
	MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
	MSM_PM_SLEEP_MODE_FASTPC,
	MSM_PM_SLEEP_MODE_POWER_COLLAPSE_SUSPEND,
	MSM_PM_SLEEP_MODE_NR,
	MSM_PM_SLEEP_MODE_NOT_SELECTED,
};

enum msm_pm_l2_scm_flag {
	MSM_SCM_L2_ON = 0,
	MSM_SCM_L2_OFF = 1,
	MSM_SCM_L2_GDHS = 3,
	MSM_SCM_L3_PC_OFF = 4,
};

#define MSM_PM_MODE(cpu, mode_nr)  ((cpu) * MSM_PM_SLEEP_MODE_NR + (mode_nr))

struct msm_pm_sleep_status_data {
	void *base_addr;
	uint32_t mask;
};

struct latency_level {
	int affinity_level;
	int reset_level;
	const char *level_name;
};

/**
 * lpm_cpu_pre_pc_cb(): API to get the L2 flag to pass to TZ
 *
 * @cpu: cpuid of the CPU going down.
 *
 * Returns the l2 flush flag enum that is passed down to TZ during power
 * collaps
 */
enum msm_pm_l2_scm_flag lpm_cpu_pre_pc_cb(unsigned int cpu);

bool msm_cpu_pm_enter_sleep(enum msm_pm_sleep_mode mode, bool from_idle);
bool msm_pm_retention_enabled(void);
static inline void msm_arch_idle(void)
{
	mb();
	wfi();
}

#ifdef CONFIG_MSM_PM

void lpm_cpu_hotplug_enter(unsigned int cpu);
s32 msm_cpuidle_get_deep_idle_latency(void);
int msm_pm_collapse(unsigned long unused);

/**
 * lpm_get_latency() - API to get latency for a low power mode
 * @latency_level:	pointer to structure with below elements
 * affinity_level: The level (CPU/L2/CCI etc.) for which the
 *	latency is required.
 *	LPM_AFF_LVL_CPU : CPU level
 *	LPM_AFF_LVL_L2  : L2 level
 *	LPM_AFF_LVL_CCI : CCI level
 * reset_level: Can be passed "LPM_RESET_LVL_GDHS" for
 *	low power mode with control logic power collapse or
 *	"LPM_RESET_LVL_PC" for low power mode with control and
 *	memory logic power collapse or "LPM_RESET_LVL_RET" for
 *	retention mode.
 * level_name: Pointer to the cluster name for which the latency
 *	is required or NULL if the minimum value out of all the
 *	clusters is to be returned. For CPU level, the name of the
 *	L2 cluster to be passed. For CCI it has no effect.
 * @latency:	address to get the latency value.
 *
 * latency value will be for the particular cluster or the minimum
 * value out of all the clusters at the particular affinity_level
 * and reset_level.
 *
 * Return: 0 for success; Error number for failure.
 */
int lpm_get_latency(struct latency_level *level, uint32_t *latency);

#else

static inline void lpm_cpu_hotplug_enter(unsigned int cpu)
{
	msm_arch_idle();
};

static inline s32 msm_cpuidle_get_deep_idle_latency(void) { return 0; }
#define msm_pm_collapse NULL

static inline int lpm_get_latency(struct latency_level *level,
						uint32_t *latency)
{
	return 0;
}
#endif

extern dma_addr_t msm_pc_debug_counters_phys;

#endif  /* __ARCH_ARM_MACH_MSM_PM_H */
