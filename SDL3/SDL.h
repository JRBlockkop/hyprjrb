#ifndef SDL3_H
#define SDL3_H

#define SDL_INIT_VIDEO "SDL_INIT_VIDEO"
#define SDL_WINDOW_RESIZABLE "SDL_WINDOW_RESIZABLE"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define SERVER_IP "127.0.0.1"
#define PORT 53745
#define BUFFER_SIZE 1024

#include <stdio.h>
#include <stdarg.h>

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

typedef struct
{

} SDL_Window;

typedef struct
{
    
} SDL_Renderer;

typedef struct
{
    int data1;
    int data2;
} SDL_WindowEvent;

typedef enum {
    SDL_EVENT_NONE = 0,
    SDL_EVENT_QUIT,
    SDL_EVENT_WINDOW_RESIZED
} SDL_EventType;

typedef struct
{
    SDL_EventType type;

    union
    {
        SDL_WindowEvent window;
    };
} SDL_Event;

typedef struct
{
    int w;
    int h;
} SDL_FRect;

static SDL_EventType current_event = SDL_EVENT_NONE;

char* udp_request(const char* message,int timeout)
{
    WSADATA wsa;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in server;
    char buffer[4096];
    int len;
    char* result = NULL;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
        return NULL;

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
        goto cleanup;

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (sendto(sock,
               message,
               (int)strlen(message),
               0,
               (struct sockaddr*)&server,
               sizeof(server)) == SOCKET_ERROR)
        goto cleanup;

    int serverLen = sizeof(server);

    len = recvfrom(sock,
                   buffer,
                   sizeof(buffer) - 1,
                   0,
                   (struct sockaddr*)&server,
                   &serverLen);

    if (len == SOCKET_ERROR)
        goto cleanup;

    buffer[len] = '\0';

    result = (char*)malloc(len + 1);
    if (result)
        strcpy(result, buffer);

cleanup:
    if (sock != INVALID_SOCKET)
        closesocket(sock);

    WSACleanup();

    return result;
}


static inline void SDL_Log(const char *restrict format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

static inline int SDL_Init(const char* str)
{
    return 1;
}

static inline char* SDL_GetError()
{
    return "Lol hyprjrb!";
}

static inline int SDL_CreateWindowAndRenderer(const char *title, int width, int height, const char *window_flags, SDL_Window **window, SDL_Renderer **renderer)
{
    char* string;
    if(0 > asprintf(&string, "CreateWindowAndRenderer|%d|%d|%s", width, height, title)) return 0;
    udp_request(string,1);
    free(string);
    return 1;
}

static inline int SDL_SetRenderDrawColor(SDL_Renderer *renderer, int r, int g, int b, int a)
{
    char* string;
    if(0 > asprintf(&string, "SetRenderDrawColor|%d|%d|%d|%d", r, g, b, a)) return 0;
    udp_request(string,1);
    free(string);
     return 1;
}

static inline void SDL_RenderClear(SDL_Renderer *renderer)
{
    udp_request("RenderClear",1);
}

static inline void SDL_RenderFillRect(SDL_Renderer *renderer, SDL_FRect *frect)
{
    // render rect on forground
}

static inline void SDL_RenderPresent(SDL_Renderer *renderer)
{
    udp_request("RenderPresent",1);
}


static inline int SDL_PollEvent(SDL_Event *event)
{
    if(current_event != SDL_EVENT_NONE){
        event->type = current_event;
        current_event = SDL_EVENT_NONE;
        return 1;
    }
    return 0;
}


static inline void SDL_Delay(int ms)
{
    char *response = udp_request("Delay",ms);
    
    if (response == NULL)
    {
        current_event = SDL_EVENT_NONE;
        return;
    }

    if (strcmp(response, "SDL_EVENT_QUIT") == 0)
        current_event = SDL_EVENT_QUIT;

    if (strcmp(response, "SDL_EVENT_WINDOW_RESIZED") == 0)
        current_event = SDL_EVENT_WINDOW_RESIZED;
}

static inline void SDL_DestroyWindow(SDL_Window *window)
{
    udp_request("DestroyWindow",1);
}

static inline void SDL_DestroyRenderer(SDL_Renderer *renderer)
{
    
}

#endif