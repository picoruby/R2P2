#ifndef FLASH_DISK_DEFINED_H_
#define FLASH_DISK_DEFINED_H_

#include <ff.h>
#include <diskio.h>

#ifdef __cplusplus
extern "C" {
#endif

int Flash_disk_initialize(void);
int Flash_disk_status(void);
int Flash_disk_read(BYTE *buff, LBA_t sector, UINT count);
int Flash_disk_write(const BYTE *buff, LBA_t sector, UINT count);
DRESULT Flash_disk_ioctl(BYTE cmd, int *buff);
DWORD get_fattime(void);

#ifdef __cplusplus
}
#endif

#endif /* FLASH_DISK_DEFINED_H_ */

