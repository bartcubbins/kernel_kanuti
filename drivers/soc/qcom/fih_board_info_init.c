/*
* Copyright(C) 2011-2014 Foxconn International Holdings, Ltd. All rights reserved
*/

#include <linux/export.h>
#include <linux/module.h>
#include <linux/err.h>

#include <linux/fih_hw_info.h>

#include <soc/qcom/smem.h>

#define BAND_ID_SHIFT_MASK 16

struct smem_oem_info
{
  unsigned int hwid;
  unsigned int hwid_verify; /*BSP-ELuo-HWID_VERIFY-00*/
  char   nonHLOS_version[32];
  char   nonHLOS_git_head[64];
  unsigned int power_on_cause;
  uint8_t imei_1[16];
  uint8_t imei_2[16];
  uint32_t imei_status;
};

void fih_set_hwid2smem(void)
{
  unsigned int hwid = 0;
  struct smem_oem_info *fih_smem_info = smem_alloc(SMEM_ID_VENDOR0, sizeof(*fih_smem_info), 0, SMEM_ANY_HOST_FLAG);

  hwid |= (BAND_REX&0xff) << BAND_ID_SHIFT_MASK;
  fih_smem_info->hwid = hwid;
}

int __init fih_board_info_init(void)
{
	fih_set_hwid2smem();
	return 0;
}
subsys_initcall(fih_board_info_init);