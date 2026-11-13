// Primary Header
#include <picoflight/core/config.h>

// Local Headers
#include <picoflight/core/utilities.h>

// Pico-SDK Headers
#include <hardware/flash.h>
#include <pico/flash.h>
#include <pico/stdlib.h>

// Standard Headers
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define INVALID_KEY (-1)

#define STORAGE_SIZE (4 * 1024)

// Calculate offset relative to the top of flash.
#define RESERVED_FLASH_OFFSET (PICO_FLASH_SIZE_BYTES - STORAGE_SIZE)
#define RESERVED_SECTION_START (XIP_BASE + RESERVED_FLASH_OFFSET)

typedef struct {
    uint32_t key; // hashed value
    uint32_t data;
} configTableEntry_t;

#define CONFIG_TABLE_NUMEL 128
#define CONFIG_TABLE_NUMPAGES (CONFIG_TABLE_NUMEL*sizeof(configTableEntry_t)/256)
configTableEntry_t *_flashTableBase = (void*)RESERVED_SECTION_START;
configTableEntry_t _memTableBase[CONFIG_TABLE_NUMEL];

void configFetch(void)
{
    memcpy(
        _memTableBase,
        _flashTableBase,
        sizeof(configTableEntry_t) * CONFIG_TABLE_NUMEL
    );
}

static void call_flash_range_erase(void *func_params)
{
    uint32_t offset = (uint32_t)func_params;
    flash_range_erase(offset, FLASH_SECTOR_SIZE);
}

static void flash_erase(uintptr_t sector_offset)
{
    flash_safe_execute(call_flash_range_erase, (void*)sector_offset, UINT32_MAX);
}

static void call_flash_range_program(void *func_params) {
    uint32_t offset = ((uintptr_t*)func_params)[0];
    const uint8_t *data = (const uint8_t *)((uintptr_t*)func_params)[1];
    flash_range_program(offset, data, FLASH_PAGE_SIZE);
}

static void flash_page_program(uintptr_t page_offset, uint8_t *data)
{
    uintptr_t params[] = {page_offset, (uintptr_t)data};
    flash_safe_execute(call_flash_range_program, params, UINT32_MAX);
}

void configCommit(void)
{
    flash_erase(RESERVED_FLASH_OFFSET);
    for (uint32_t i = 0; i < CONFIG_TABLE_NUMPAGES; ++i) {
        uintptr_t wrtOffset = RESERVED_FLASH_OFFSET + i * 256;
        uint8_t *srcPtr = (uint8_t*)_memTableBase + i * 256;
        flash_page_program(wrtOffset, srcPtr);
    }
}

void configReset(void) 
{
    flash_erase(RESERVED_FLASH_OFFSET);
}

static int32_t findNextAvailable(void)
{
    for (int32_t i = 0; i < CONFIG_TABLE_NUMEL; ++i) {
        if (_memTableBase[i].key == 0xFFFFFFFF) // all 1s is the unitialized value
            return i;
    }
    return -1;
}

int32_t configAddNewEntry(const char *name, uint32_t data)
{
    uint32_t key = hashFNV1A32(name, strlen(name));

    int32_t idx = findNextAvailable();
    if (idx == -1)
        return 1;

    _memTableBase[idx].key = key;
    _memTableBase[idx].data = data;

    return 0;
}

int32_t configSeek(const char *name)
{
    uint32_t key = hashFNV1A32(name, strlen(name));

    for (uint32_t i = 0; i < CONFIG_TABLE_NUMEL; ++i) {
        if (_memTableBase[i].key == key) {
            return i;
        }
    }
    return -1;
}

bool configCheckExists(const char *name)
{
    return (configSeek(name) != -1);
}

void configModifyEntry(const char *name, uint32_t data)
{
    int32_t idx = configSeek(name);
    if (idx == -1)
        return;

    _memTableBase[idx].data = data;
}

int32_t configCheckModify(const char *name, uint32_t data)
{
    if (configCheckExists(name)) {
        configModifyEntry(name, data);
        return 0;
    }
    else {
        return configAddNewEntry(name, data);
    }
}

uint32_t configGetEntry(const char *name)
{
    return _memTableBase[configSeek(name)].data;
}
