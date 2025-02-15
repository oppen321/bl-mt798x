/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2021 MediaTek Inc. All Rights Reserved.
 *
 * Author: Weijie Gao <weijie.gao@mediatek.com>
 *
 * Generic data upgrading helper
 */

#ifndef _UPGRADE_HELPER_H_
#define _UPGRADE_HELPER_H_

#include <linux/types.h>
#include <stdbool.h>

enum data_post_upgrade_action {
	UPGRADE_ACTION_NONE,
	UPGRADE_ACTION_REBOOT,
	UPGRADE_ACTION_BOOT,
	UPGRADE_ACTION_CUSTOM,
};

struct data_part_entry;

typedef int (*data_handler_t)(void *priv, const struct data_part_entry *dpe,
			      const void *data, size_t size);

struct data_part_entry {
	const char *name;
	const char *abbr;
	const char *env_name;
	enum data_post_upgrade_action post_action;
	const char *custom_action_prompt;
	void *priv;
	data_handler_t validate;
	data_handler_t write;
	data_handler_t do_post_action;
};

extern void board_upgrade_data_parts(const struct data_part_entry **dpes,
				     u32 *count);

#ifdef CONFIG_MMC
struct mmc *_mmc_get_dev(u32 dev, int part, bool write);
int _mmc_write(struct mmc *mmc, u64 offset, size_t max_size, const void *data,
	       size_t size, bool verify);
int _mmc_read(struct mmc *mmc, u64 offset, void *data, size_t size);
int _mmc_erase(struct mmc *mmc, u64 offset, size_t max_size, u64 size);

int mmc_write_generic(u32 dev, int part, u64 offset, size_t max_size,
		      const void *data, size_t size, bool verify);
int mmc_read_generic(u32 dev, int part, u64 offset, void *data, size_t size);
int mmc_erase_generic(u32 dev, int part, u64 offset, u64 size);
int mmc_erase_env_generic(u32 dev, int part, u64 offset, u64 size);

#ifdef CONFIG_PARTITIONS
#include <part.h>
int _mmc_find_part(struct mmc *mmc, const char *part_name,
		   struct disk_partition *dpart);
int mmc_write_part(u32 dev, int part, const char *part_name, const void *data,
		   size_t size, bool verify);
int mmc_read_part(u32 dev, int part, const char *part_name, void *data,
		  size_t size);
int mmc_erase_part(u32 dev, int part, const char *part_name, u64 offset,
		   u64 size);
int mmc_erase_env_part(u32 dev, int part, const char *part_name, u64 size);
#endif /* CONFIG_PARTITIONS */

int mmc_write_gpt(u32 dev, int part, size_t max_size, const void *data,
		  size_t size);
#endif /* CONFIG_GENERIC_MMC */

#ifdef CONFIG_MTD
#include <linux/mtd/mtd.h>
void gen_mtd_probe_devices(void);
int mtd_erase_skip_bad(struct mtd_info *mtd, u64 offset, u64 size,
		       u64 maxsize, u64 *erasedsize, const char *name,
		       bool fullerase);
int mtd_write_skip_bad(struct mtd_info *mtd, u64 offset, size_t size,
		       u64 maxsize, size_t *writtensize, const void *data,
		       bool verify);
int mtd_read_skip_bad(struct mtd_info *mtd, u64 offset, size_t size,
		      u64 maxsize, size_t *readsize, void *data);
int mtd_update_generic(struct mtd_info *mtd, const void *data, size_t size,
		       bool verify);
#endif

int check_data_size(u64 total_size, u64 offset, size_t max_size, size_t size,
		    bool write);
bool verify_data(const u8 *orig, const u8 *rdback, u64 offset, size_t size);

#endif /* _UPGRADE_HELPER_H_ */
