#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#define SFML_STATIC
#include <SFML/System.h>
#include <SFML/Network.h>

#include "bvhreader.h"
#include <memory.h>

#define RELEASE
static atomic_bool running = true;

int main(int argc, char* argv[])
{
    unsigned short port = 0;
    const char* bvh_path = NULL;
    if (argc != 3)
    {
        #ifdef RELEASE
        printf("incorrect parameter's format: $port $bvh_file_name\n");
        return -1;
        #else
        bvh_path = "test.bvh";
        port = 7001;
        #endif
    }
    else
    {
        port = atoi(argv[1]);
        bvh_path = argv[2];
    }

    //------------- parse BVH --------------
    float *data;
    uint32_t frames_count;
    float frame_time_in_sec;
    uint32_t data_count;
    if (BVH_Parse(bvh_path, &frames_count, &data_count, &data, &frame_time_in_sec))
    {
        printf("ERROR: couldn't parse BVH file\n");
        return -1;
    }

    // --------------- Create packages -----------
    size_t package_size = (BVH_HeaderSize + data_count * sizeof(float));
    void* packages = malloc(frames_count * package_size);
    for(size_t i = 0; i < frames_count; ++i)
    {
        void* package = packages + i * package_size;
        BVH_FillHeader(package, "Char00", data_count, i);
        memcpy(package + BVH_HeaderSize, data + i * data_count, data_count * sizeof(float));
    }
    free(data);


    // create socket server
    sfTcpListener *server = sfTcpListener_create();
    sfTcpListener_listen(server, port, sfIpAddress_Any);
    printf("server started at %i port\n", port);
    sfTime delay = sfSeconds(frame_time_in_sec);
    // wait for connection
    while(running)
    {
        sfTcpSocket *client;
        sfSocketStatus connection_status = sfTcpListener_accept(server, &client);
        if (connection_status == sfSocketDone)
        {
            printf("client connected. Starting send the animation\n");
            while(connection_status == sfSocketDone)
            {
                for(uint32_t i = 0; i < frames_count && connection_status == sfSocketDone; ++i)
                {
                    connection_status = sfTcpSocket_send(client, packages + i * package_size, package_size);
                    sfSleep(delay);
                }

            }
        }
        sfTcpSocket_destroy(client);
        printf("client disconnected. Wait for other.\n");
    }

    sfTcpListener_destroy(server);
    return 0;
}


