#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

static const size_t BVH_HeaderSize = 64;

int BVH_Parse(const char* path, uint32_t *_frames_count, uint32_t *_data_count, float* _data[], float *frame_time_in_sec);

int BVH_FillHeader(void* header, const char* avarar_name, uint16_t data_count, uint32_t sequence);


