/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-05     dell       the first version
 */
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-05     dell       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <dfs_posix.h>

#define RECORD_TIME_MS      5000
#define RECORD_SAMPLERATE   16000
#define RECORD_CHANNEL      2
#define RECORD_CHUNK_SZ     ((RECORD_SAMPLERATE * RECORD_CHANNEL * 2) * 20 / 1000)

#define SOUND_DEVICE_NAME    "sound1"      /* Audio 设备名称 */
static rt_device_t mic_dev;              /* Audio 设备句柄 */
#define SOUND_DEVICE_NAME_2    "sound0"    /* Audio 设备名称 */
static rt_device_t snd_dev;              /* Audio 设备句柄 */



int audio_sample(int argc, char **argv)
{
    int fd = -1;
    uint8_t *buffer = NULL;
    struct rt_audio_caps caps_mic = {0};
    struct rt_audio_caps caps_snd = {0};
    int length, total_length = 0;


    /* 根据设备名称查找 Audio 设备，获取设备句柄 */
    mic_dev = rt_device_find(SOUND_DEVICE_NAME);
    if (mic_dev == RT_NULL){
        rt_kprintf("err 1\n");
        //goto __exit;
    }
    /* 根据设备名称查找 Audio 设备，获取设备句柄 */
    snd_dev = rt_device_find(SOUND_DEVICE_NAME_2);
    if (snd_dev == RT_NULL){
        rt_kprintf("err 2\n");
        //goto __exit;
    }

    /* 以只读方式打开 Audio 录音设备 */
    rt_device_open(mic_dev, RT_DEVICE_OFLAG_RDONLY);

    /* 设置采样率、通道、采样位数等音频参数信息 */
    caps_mic.main_type               = AUDIO_TYPE_INPUT;                            /* 输入类型（录音设备 ）*/
    caps_mic.sub_type                = AUDIO_DSP_PARAM;                             /* 设置所有音频参数信息 */
    caps_mic.udata.config.samplerate = RECORD_SAMPLERATE;                           /* 采样率 */
    caps_mic.udata.config.channels   = RECORD_CHANNEL;                              /* 采样通道 */
    caps_mic.udata.config.samplebits = 16;                                          /* 采样位数 */
    rt_device_control(mic_dev, AUDIO_CTL_CONFIGURE, &caps_mic);



    /* 以只写方式打开 Audio 播放设备 */
    rt_device_open(snd_dev, RT_DEVICE_OFLAG_WRONLY);


    /* 设置采样率、通道、采样位数等音频参数信息 */
    caps_snd.main_type               = AUDIO_TYPE_OUTPUT;                           /* 输出类型（播放设备 ）*/
    caps_snd.sub_type                = AUDIO_DSP_PARAM;                             /* 设置所有音频参数信息 */
    caps_snd.udata.config.samplerate = RECORD_SAMPLERATE;    /* 采样率 */
    caps_snd.udata.config.channels   = RECORD_CHANNEL;         /* 采样通道 */
    caps_snd.udata.config.samplebits = 16;                                          /* 采样位数 */
    rt_device_control(snd_dev, AUDIO_CTL_CONFIGURE, &caps_snd);

    while (1)
    {
        /* 从 Audio 设备中，读取 20ms 的音频数据  */
        length = rt_device_read(mic_dev, 0, buffer, RECORD_CHUNK_SZ);
        rt_kprintf("read length = %d", length);

        if (length)
        {
            /* 写入音频数据到到文件系统 */
            length = rt_device_write(snd_dev, 0, buffer, length);
            rt_kprintf("write length = %d", length);
        }

    }

    /* 关闭 Audio 设备 */
    rt_device_close(mic_dev);
    /* 关闭 Audio 设备 */
    rt_device_close(snd_dev);

__exit:

    if (buffer)
        rt_free(buffer);

    return 0;
}
MSH_CMD_EXPORT(audio_sample, record voice to a wav file);
