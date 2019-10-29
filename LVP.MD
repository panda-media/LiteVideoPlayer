## LVP 概要


对外接口 

创建一个lvp播放器                                                                  
```code
lvp_alloc()                                |---> reader init
    |                                      |
    |-->lvp_core_alloc                     |---> pkt/frame filter init
            |                              |
            |--> create/init core module   |---> decoder init
                        |                  |
                        |----------------->|---> pkt/frame cache init
                                           |
                                           |---> render init
                                           |
                                           |---> audio video sync init
                                           |
                                           |---> other module
```

设置播放地址

```
lvp_set_url() ----> lvp_core_set_url
```


设置参数
```
lvp_set_options() ----> lvp_core_parse_options
```


开始播放
```
lvp_play() ----> lvp_core_play 
```


暂停
```
lvp_pause() ----> lvp_core_pause
```


seek
```
lvp_seek() ----> lvp_core_seek
```


停止
```
lvp_stop() ----> lvp_core_stop
```


销毁播放器
```
lvp_free() ----> lvp_core_free
```

## LVP 数据流向

```
    reader--->reader_module--->packet_filter--->packet_cache--->decoder--->decoder_module--->frame_filter--->frame_cache--->render
```

## LVP MOUDLE

LVP 的module 有两个公共接口 init 和 close ，init 用于初始化模块，并且会传入 option、event_control 和 log，供模块使用。close用于关闭模块以及释放私有对象内存。
