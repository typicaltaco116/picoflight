#pragma once

// Standard Headers
#include <stdint.h>
#include <stdbool.h>

void configFetch(void);
void configCommit(void);
void configReset(void);
int32_t configAddNewEntry(const char *name, uint32_t data);
int32_t configSeek(const char *name);
bool configCheckExists(const char *name);
void configModifyEntry(const char *name, uint32_t data);
int32_t configCheckModify(const char *name, uint32_t data);
uint32_t configGetEntry(const char *name);
