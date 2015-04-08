/*
 * emmc.h
 *
 *  Created on: Apr 3, 2013
 *      Author: johannes
 */


#pragma once

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

	int32_t emmc_init();
	int32_t emmc_read(uint8_t *buf, uint32_t buf_size,
			uint32_t block_no);
	int32_t emmc_write(const uint8_t *buf, uint32_t buf_size,
			uint32_t block_no);


#ifndef NO_RODOS_NAMESPACE
}
#endif
