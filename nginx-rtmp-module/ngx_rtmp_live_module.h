
/*
 * Copyright (C) Roman Arutyunyan
 */


#ifndef _NGX_RTMP_LIVE_H_INCLUDED_
#define _NGX_RTMP_LIVE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include "ngx_rtmp.h"
#include "ngx_rtmp_cmd_module.h"
#include "ngx_rtmp_bandwidth.h"
#include "ngx_rtmp_streams.h"


typedef struct ngx_rtmp_live_ctx_s ngx_rtmp_live_ctx_t;
typedef struct ngx_rtmp_live_stream_s ngx_rtmp_live_stream_t;


typedef struct {
    unsigned                            active:1;
    uint32_t                            timestamp;
    uint32_t                            csid;                       //NGX_RTMP_CSID_VIDEO or NGX_RTMP_CSID_AUDIO
    uint32_t                            dropped;
} ngx_rtmp_live_chunk_stream_t;


struct ngx_rtmp_live_ctx_s {
    ngx_rtmp_session_t                 *session;                    //session与ngx_rtmp_live_ctx_s一一对应
    ngx_rtmp_live_stream_t             *stream;                     //publish时生成的stream
    ngx_rtmp_live_ctx_t                *next;                       //指向下一个ngx_rtmp_live_ctx_s,这是一个链表结构
    ngx_uint_t                          ndropped;                   //记录这个session的丢包率
    ngx_rtmp_live_chunk_stream_t        cs[2];                      //0->video, 1->audio
    ngx_uint_t                          meta_version;               //这个字段是为了每个连接只发送一个meta data
    ngx_event_t                         idle_evt;                   //idle的定时器
    unsigned                            active:1;                   //active
    unsigned                            publishing:1;               //标示是publisher or subscriber
    unsigned                            silent:1;
    unsigned                            paused:1;
};

// ngx_rtmp_live_stream_s －>ctx －>ctx
struct ngx_rtmp_live_stream_s {
    u_char                              name[NGX_RTMP_MAX_NAME];    //流名称
    ngx_rtmp_live_stream_t             *next;                       //指向下一个stream结构
    ngx_rtmp_live_ctx_t                *ctx;                        //这个永远指向的是gx_rtmp_live_ctx_t链表的第一个指针
    ngx_rtmp_bandwidth_t                bw_in;                      //input 带宽
    ngx_rtmp_bandwidth_t                bw_in_audio;                //input audio带宽
    ngx_rtmp_bandwidth_t                bw_in_video;                //out video带宽
    ngx_rtmp_bandwidth_t                bw_out;                     //output 带宽
    ngx_msec_t                          epoch;                      //流创建的时间msecond
    unsigned                            active:1;                   
    unsigned                            publishing:1;               //该流已经被发布
};

// 这个结构里面的streams非常重要，这个结构里面记录了所有的stream
typedef struct {
    ngx_int_t                           nbuckets;                   //设定桶形结构的大小，默认的话1024
    ngx_rtmp_live_stream_t            **streams;                    //里面存储的是1024个流结构的指针数组
    ngx_flag_t                          live;                       //live on
    ngx_flag_t                          meta;                       //meta on|copy|off
    ngx_msec_t                          sync;                       //sync 音视频
    ngx_msec_t                          idle_timeout;
    ngx_flag_t                          atc;
    ngx_flag_t                          interleave;                 //In this mode audio and video data is 
                                                                    //transmitted on the same RTMP chunk stream
    ngx_flag_t                          wait_key;                   //Makes video stream start with a key frame
    ngx_flag_t                          wait_video;                 //Disable audio until first video frame is sent
    ngx_flag_t                          publish_notify;
    ngx_flag_t                          play_restart;
    ngx_flag_t                          idle_streams;
    ngx_msec_t                          buflen;                     //buflen
    ngx_pool_t                         *pool;                       //内存的管理
    ngx_rtmp_live_stream_t             *free_streams;               //空闲的stream链表
} ngx_rtmp_live_app_conf_t;


extern ngx_module_t  ngx_rtmp_live_module;


#endif /* _NGX_RTMP_LIVE_H_INCLUDED_ */
