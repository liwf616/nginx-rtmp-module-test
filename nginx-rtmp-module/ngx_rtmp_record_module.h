
/*
 * Copyright (C) Roman Arutyunyan
 */


#ifndef _NGX_RTMP_RECORD_H_INCLUDED_
#define _NGX_RTMP_RECORD_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include "ngx_rtmp.h"


#define NGX_RTMP_RECORD_OFF             0x01
#define NGX_RTMP_RECORD_AUDIO           0x02
#define NGX_RTMP_RECORD_VIDEO           0x04
#define NGX_RTMP_RECORD_KEYFRAMES       0x08
#define NGX_RTMP_RECORD_MANUAL          0x10


typedef struct {
    ngx_str_t                           id;
    ngx_uint_t                          flags;                  //record [off|all|audio|video|keyframes|manual]*
    ngx_str_t                           path;                   //record_path
    size_t                              max_size;               //record_max_size
    size_t                              max_frames;             //record_max_frames
    ngx_msec_t                          interval;
    ngx_str_t                           suffix;                 //在录制的文件后面添加后缀
    ngx_flag_t                          unique;
    ngx_flag_t                          append;                 //record_append on
    ngx_flag_t                          lock_file;
    ngx_flag_t                          notify;                 //record_notify on
    ngx_url_t                          *url;

    void                              **rec_conf;
    ngx_array_t                         rec;                    //ngx_rtmp_record_app_conf_t
} ngx_rtmp_record_app_conf_t;

// 这个结构体是某一个stream对应的结构，包括该流写入的文件等信息
typedef struct {
    ngx_rtmp_record_app_conf_t         *conf;                   //这个流对应的app conf结构
    ngx_file_t                          file;                   //对应的文件
    ngx_uint_t                          nframes;                //该文件里面已经写入的audio和video帧数
    uint32_t                            epoch, time_shift;      //time_shift 时间戳的起始偏移
    ngx_time_t                          last;
    time_t                              timestamp;
    unsigned                            failed:1;
    unsigned                            initialized:1;          //是否是第一次开始写数据
    unsigned                            aac_header_sent:1;      //AAC sequence header
    unsigned                            avc_header_sent:1;      //AVC sequence header
    unsigned                            video_key_sent:1;       //video keyframe
    unsigned                            audio:1;                //有音频？
    unsigned                            video:1;                //有视频？
} ngx_rtmp_record_rec_ctx_t;

// 这个结构是包含了所有stream流的信息
typedef struct {
    ngx_array_t                         rec; /* ngx_rtmp_record_rec_ctx_t */
    u_char                              name[NGX_RTMP_MAX_NAME];                //流的名称
    u_char                              args[NGX_RTMP_MAX_ARGS];                //流的参数
} ngx_rtmp_record_ctx_t;


ngx_uint_t ngx_rtmp_record_find(ngx_rtmp_record_app_conf_t *racf,
           ngx_str_t *id);


/* Manual recording control,
 * 'n' is record node index in config array.
 * Note: these functions allocate path in static buffer */

ngx_int_t ngx_rtmp_record_open(ngx_rtmp_session_t *s, ngx_uint_t n,
          ngx_str_t *path);
ngx_int_t ngx_rtmp_record_close(ngx_rtmp_session_t *s, ngx_uint_t n,
          ngx_str_t *path);


typedef struct {
    ngx_str_t                           recorder;       //一个描述字符串
    ngx_str_t                           path;           //实际生成的路径串
} ngx_rtmp_record_done_t;


typedef ngx_int_t (*ngx_rtmp_record_done_pt)(ngx_rtmp_session_t *s,
        ngx_rtmp_record_done_t *v);


extern ngx_rtmp_record_done_pt          ngx_rtmp_record_done;


extern ngx_module_t                     ngx_rtmp_record_module;


#endif /* _NGX_RTMP_RECORD_H_INCLUDED_ */
