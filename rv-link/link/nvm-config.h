/*
Copyright (c) 2019 zoomdy@163.com
RV-LINK is licensed under the Mulan PSL v1.
You can use this software according to the terms and conditions of the Mulan PSL v1.
You may obtain a copy of Mulan PSL v1 at:
    http://license.coscl.org.cn/MulanPSL
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
PURPOSE.
See the Mulan PSL v1 for more details.
 */
#ifndef __RVL_CONFIG_H__
#define __RVL_CONFIG_H__

#include <stddef.h>
#include <stdint.h>

/* TODO: Refactor API to contain configuration items */
int rvl_config_write(const uint32_t* config, size_t size);
int rvl_config_read(uint32_t* config, size_t size);

#endif // __RVL_CONFIG_H__
