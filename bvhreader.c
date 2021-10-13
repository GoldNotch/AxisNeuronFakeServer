#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "bvhreader.h"

int BVH_Parse(const char* path, uint32_t *_frames_count, uint32_t* _data_count, float* _data[], float *frame_time_in_sec)
{
    FILE* bvh = fopen(path, "r");
    if (!bvh)
        return -1;



    //----------------- skip hierarchy ---------------------------
    size_t buffer_size = 512;
    char buffer[buffer_size];
    while(fgets(buffer, buffer_size, bvh))
    {
        if (strstr(buffer, "Frames") != NULL)
            break;
    }

    //----------------- read frames info-----------------------
    float frame_time;
    uint32_t frames_count;
    uint32_t data_count;
    sscanf(buffer, "Frames: %i", &frames_count);
    fscanf(bvh, "Frame Time: %f\n", &frame_time);

    //------------------ read frames data --------------------
    //read first row to detect with displacement
    off_t data_offset = ftell(bvh);
    data_count = 0;
    char s;
    while((s = fgetc(bvh)) != '\n')
        if (s == ' ') ++data_count;
    fseek(bvh, data_offset, SEEK_SET);

    float *data = (float*)malloc(data_count * frames_count * sizeof(float));
    for(uint32_t i = 0; i < frames_count; ++i)
        for(uint32_t j = 0; j < data_count; ++j)
            fscanf(bvh, "%f ", data + i * data_count + j);

    *_frames_count = frames_count;
    *_data_count = data_count;
    *_data = data;
    *frame_time_in_sec = frame_time;
    fclose(bvh);
    return 0;
}

int BVH_FillHeader(void* header, const char* avatar_name, uint16_t data_count, uint32_t sequence)
{
    /*
    struct header   // with no compiler alignment
    {
        uint16_t header_begin; //0xDDFF
        uint32_t data_version;      // 0x01010000
        uint16_t data_count;        //180 if no displacement, either 360. xyz rpy for each bone (60 bones)
        uint8_t with_displacement;  //true or false
        uint8_t with_reference; //0
        uint32_t avarat_id; //0
        char avatar_name[32];
        uint32_t sequence;
        uint32_t reserve[3];    //any
        uint16_t header_end; //0xEEFF
    }
     */
    const uint16_t header_begin = 0xDDFF;
    const uint32_t data_version = 0x01010000;
    uint8_t with_displacement = data_count != 180;
    const uint8_t with_reference = 0;
    const uint32_t avatar_id = 0;
    const uint16_t header_end = 0xEEFF;

    size_t len = strlen(avatar_name);
    if (len >= 32) return -1;

    //fill buffer
    off_t offset = 0;
    memcpy(header + offset, &header_begin, sizeof(uint16_t)); offset += sizeof(uint16_t);
    memcpy(header + offset, &data_version, sizeof(uint32_t)); offset += sizeof(uint32_t);
    memcpy(header + offset, &data_count, sizeof(uint16_t)); offset += sizeof(uint16_t);
    memcpy(header + offset, &with_displacement, sizeof(uint8_t)); offset += sizeof(uint8_t);
    memcpy(header + offset, &with_reference, sizeof(uint8_t)); offset += sizeof(uint8_t);
    memcpy(header + offset, &avatar_id, sizeof(uint32_t)); offset += sizeof(uint32_t);
    memcpy(header + offset, avatar_name, len * sizeof(char)); offset += sizeof(char) * 32;
    memcpy(header + offset, &sequence, sizeof(uint32_t)); offset += sizeof(uint32_t);
    offset += sizeof(uint32_t) * 3;//skip reserve
    memcpy(header + offset, &header_end, sizeof(uint16_t)); offset += sizeof(uint16_t);
    return 0;
}
