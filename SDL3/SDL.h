#ifndef SDL3_H
#define SDL3_H

#define SDL_INIT_VIDEO "SDL_INIT_VIDEO"
#define SDL_WINDOW_RESIZABLE "SDL_WINDOW_RESIZABLE"

#define SERVER_IP "127.0.0.1"
#define PORT 53745
#define BUFFER_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
typedef int SOCKET;
#endif

#ifdef _WIN32
#define socket_close closesocket
#else
#define socket_close close
#endif

typedef struct
{

} SDL_Window;
typedef struct{} SDL_Renderer;
typedef struct{int data1,data2;} SDL_WindowEvent;
typedef enum{SDL_BUTTON_LEFT,SDL_BUTTON_RIGHT} SDL_ButtonType;
typedef struct{SDL_ButtonType button;int y;} SDL_ButtonEvent;
typedef struct{char* text;} SDL_TextEvent;
typedef enum{SDLK_BACKSPACE,SDLK_RETURN} SDL_KeyType;
typedef struct{SDL_KeyType key;} SDL_KeyEvent;
typedef enum{SDL_EVENT_NONE=0,SDL_EVENT_QUIT,SDL_EVENT_WINDOW_RESIZED,SDL_EVENT_MOUSE_BUTTON_DOWN,SDL_EVENT_TEXT_INPUT,SDL_EVENT_KEY_DOWN} SDL_EventType;
typedef struct{SDL_EventType type; union{SDL_WindowEvent window;SDL_ButtonEvent button;SDL_TextEvent text;SDL_KeyEvent key;};} SDL_Event;
typedef struct{int w;int h;int x;int y;} SDL_FRect;
static SDL_EventType current_event=SDL_EVENT_NONE;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} SDL_Color;

typedef struct {} SDL_Surface;
typedef struct {} SDL_Texture;

static char* string_printf(const char *fmt,...){
 va_list a,b; va_start(a,fmt); va_copy(b,a); int n=vsnprintf(NULL,0,fmt,a); va_end(a);
 if(n<0) {va_end(b); return NULL;}
 char* s=(char*)malloc(n+1); if(!s){va_end(b); return NULL;}
 vsnprintf(s,n+1,fmt,b); va_end(b); return s;
}

static char* udp_request(const char* message,int timeout){
#ifdef _WIN32
 WSADATA w; if(WSAStartup(MAKEWORD(2,2),&w)!=0) return NULL;
#endif
 SOCKET sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); if(sock==INVALID_SOCKET) goto fail;
#ifdef _WIN32
 setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(timeout));
#else
 struct timeval tv={timeout/1000,(timeout%1000)*1000};
 setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
#endif
 struct sockaddr_in server; memset(&server,0,sizeof(server));
 server.sin_family=AF_INET; server.sin_port=htons(PORT); inet_pton(AF_INET,SERVER_IP,&server.sin_addr);
 if(sendto(sock,message,(int)strlen(message),0,(struct sockaddr*)&server,sizeof(server))==SOCKET_ERROR) goto fail;
 char buf[4096];
#ifdef _WIN32
 int slen=sizeof(server);
#else
 socklen_t slen=sizeof(server);
#endif
 int len=recvfrom(sock,buf,sizeof(buf)-1,0,(struct sockaddr*)&server,&slen);
 if(len==SOCKET_ERROR) goto fail;
 buf[len]=0; char* r=(char*)malloc(len+1); if(r) memcpy(r,buf,len+1);
 socket_close(sock);
#ifdef _WIN32
 WSACleanup();
#endif
 return r;
fail:
 if(sock!=INVALID_SOCKET) socket_close(sock);
#ifdef _WIN32
 WSACleanup();
#endif
 return NULL;
}

static inline void SDL_Log(const char* f,...){va_list a;va_start(a,f);vprintf(f,a);va_end(a);}

static inline int SDL_snprintf(char *buffer, size_t size, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf(buffer, size, fmt, args);
    va_end(args);
    return ret;
}

static inline int SDL_Init(const char* str)
{(void)str;return 1;}

static inline char* SDL_GetError(){return "Lol hyprjrb!";}
static inline int SDL_CreateWindowAndRenderer(const char*t,int w,int h,const char*fl,SDL_Window**win,SDL_Renderer**ren){(void)fl;(void)win;(void)ren;char*s=string_printf("CreateWindowAndRenderer|%d|%d|%s",w,h,t); if(!s)return 0; udp_request(s,1); free(s); return 1;}
static inline void SDL_SetWindowTitle(SDL_Window*win,const char*t){(void)win;char*s=string_printf("SetWindowTitle|%s",t); udp_request(s,1); free(s);}

static inline SDL_Texture* SDL_CreateTextureFromSurface(SDL_Renderer*r,SDL_Surface*s){(void)r;(void)s;SDL_Texture *t;return t;}
static inline void SDL_GetTextureSize(SDL_Texture*t,float *w,float *h){(void)t;(void)w;(void)h;}

static inline int SDL_SetRenderDrawColor(SDL_Renderer*r,int R,int G,int B,int A){(void)r;char*s=string_printf("SetRenderDrawColor|%d|%d|%d|%d",R,G,B,A); if(!s)return 0; udp_request(s,1); free(s); return 1;}

static inline void SDL_RenderClear(SDL_Renderer*r){(void)r;udp_request("RenderClear",1);}
static inline void SDL_RenderFillRect(SDL_Renderer*r,SDL_FRect*f){(void)r;(void)f;}
static inline void SDL_RenderTexture(SDL_Renderer*r,SDL_Texture*t,const SDL_FRect *src,SDL_FRect *dst){(void)r;(void)t;(void)src;(void)dst;}
static inline void SDL_RenderPresent(SDL_Renderer*r){(void)r;udp_request("RenderPresent",1);}

static inline int SDL_PollEvent(SDL_Event*e){
    if(current_event!=SDL_EVENT_NONE){
        e->type=current_event;current_event=SDL_EVENT_NONE;
        return 1;
    }
    return 0;
}
static inline void SDL_Delay(int ms){
    char*res=udp_request("Delay",ms);
    if(!res){
        current_event=SDL_EVENT_NONE;
        return;
    }
    if(!strcmp(res,"SDL_EVENT_QUIT"))
        current_event=SDL_EVENT_QUIT;
    if(!strcmp(res,"SDL_EVENT_WINDOW_RESIZED"))
        current_event=SDL_EVENT_WINDOW_RESIZED;
    free(res);
}
static inline void SDL_StartTextInput(SDL_Window*w){(void)w;udp_request("StartTextInput",1);}
static inline void SDL_StopTextInput(SDL_Window*w){(void)w;udp_request("StopTextInput",1);}
static inline void SDL_DestroyWindow(SDL_Window*w){(void)w;udp_request("DestroyWindow",1);}
static inline void SDL_DestroyRenderer(SDL_Renderer*r){(void)r;}
static inline void SDL_DestroySurface(SDL_Surface*s){(void)s;}
static inline void SDL_Quit(){}
#endif
