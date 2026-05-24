#include "flash.h"

#include "hardware/flash.h"
#include "pico/flash.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define INVALID_KEY (-1)

extern char __config_reserved_flash;

#define RESERVED_FLASH_OFFSET ((uintptr_t)&__config_reserved_flash - XIP_BASE)

typedef struct {
    char *key;
    void *data;
    uint size;
} config_entry_t;

static uint configTableEntries = 0;
static config_entry_t *configTable = NULL;


void config_RegisterParam(const char *key, uint size)
{
    size_t key_length;

    configTable = realloc(configTable, (++configTableEntries) * sizeof(config_entry_t));

    configTable[configTableEntries - 1].data = malloc(size);
    key_length = strlen(key) + 1; // include \0 character
    configTable[configTableEntries - 1].key = malloc(key_length);

    memcpy(configTable[configTableEntries - 1].key, key, key_length);
    configTable[configTableEntries - 1].size = size;
}


static bool simple_strcmp(const char *str1, const char *str2)
{
    while (*str1 && *str2) {
        if (*str1 != *str2)
            return false;
        str1++;
        str2++;
    }

    if (*str1 != *str2)
        return false;

    return true;
}

static int getTableIndex(const char *key)
{
    for (uint i = 0; i < configTableEntries; ++i) {
        if (simple_strcmp(configTable[i].key, key))
            return i;
    }

    return INVALID_KEY;
}


int config_SetParam(const char *key, void *data)
{
    int index;
    config_entry_t entry;

    index = getTableIndex(key);
    if (index == INVALID_KEY)
        return -1;

    entry = configTable[index];

    memcpy(entry.data, data, entry.size);

    return 0;
}


void* config_GetParam(char *key)
{
    int index;
    config_entry_t entry;

    index = getTableIndex(key);
    if (index == INVALID_KEY)
        return NULL;

    entry = configTable[index];

    return entry.data;
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


int config_WriteAll(void)
{
    uint8_t programData[FLASH_PAGE_SIZE]; // aligned to 256 byte addresses
    uint8_t *dataPtr = programData;

    memset(programData, 0xFF, FLASH_PAGE_SIZE);

    for (uint i = 0; i < configTableEntries; ++i) {
        config_entry_t entry = configTable[i];
        memcpy(dataPtr, entry.data, entry.size);
        dataPtr += entry.size;
    }

    flash_erase(RESERVED_FLASH_OFFSET);
    flash_page_program(RESERVED_FLASH_OFFSET, programData);

    return 0; // useful for errors in the future
}


void config_ReadAll(void)
{
    uint8_t *dataPtr = (uint8_t*)&__config_reserved_flash;

    for (uint i = 0; i < configTableEntries; ++i) {
        config_entry_t entry = configTable[i];
        memcpy(entry.data, dataPtr, entry.size);
        dataPtr += entry.size;
    }
}


void config_FreeAll(void)
{
    for (uint i = 0; i < configTableEntries; ++i) {
        config_entry_t entry = configTable[i];
        free(entry.data);
        free(entry.key);
    }

    free(configTable);
}
