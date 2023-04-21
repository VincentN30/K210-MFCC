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

#define SOUND_DEVICE_NAME    "sound1"      /* Audio �豸���� */
static rt_device_t mic_dev;              /* Audio �豸��� */
#define SOUND_DEVICE_NAME_2    "sound0"    /* Audio �豸���� */
static rt_device_t snd_dev;              /* Audio �豸��� */



int audio_sample(int argc, char **argv)
{
    int fd = -1;
    uint8_t *buffer = NULL;
    struct rt_audio_caps caps_mic = {0};
    struct rt_audio_caps caps_snd = {0};
    int length, total_length = 0;


    /* �����豸���Ʋ��� Audio �豸����ȡ�豸��� */
    mic_dev = rt_device_find(SOUND_DEVICE_NAME);
    if (mic_dev == RT_NULL){
        rt_kprintf("err 1\n");
        //goto __exit;
    }
    /* �����豸���Ʋ��� Audio �豸����ȡ�豸��� */
    snd_dev = rt_device_find(SOUND_DEVICE_NAME_2);
    if (snd_dev == RT_NULL){
        rt_kprintf("err 2\n");
        //goto __exit;
    }

    /* ��ֻ����ʽ�� Audio ¼���豸 */
    rt_device_open(mic_dev, RT_DEVICE_OFLAG_RDONLY);

    /* ���ò����ʡ�ͨ��������λ������Ƶ������Ϣ */
    caps_mic.main_type               = AUDIO_TYPE_INPUT;                            /* �������ͣ�¼���豸 ��*/
    caps_mic.sub_type                = AUDIO_DSP_PARAM;                             /* ����������Ƶ������Ϣ */
    caps_mic.udata.config.samplerate = RECORD_SAMPLERATE;                           /* ������ */
    caps_mic.udata.config.channels   = RECORD_CHANNEL;                              /* ����ͨ�� */
    caps_mic.udata.config.samplebits = 16;                                          /* ����λ�� */
    rt_device_control(mic_dev, AUDIO_CTL_CONFIGURE, &caps_mic);



    /* ��ֻд��ʽ�� Audio �����豸 */
    rt_device_open(snd_dev, RT_DEVICE_OFLAG_WRONLY);


    /* ���ò����ʡ�ͨ��������λ������Ƶ������Ϣ */
    caps_snd.main_type               = AUDIO_TYPE_OUTPUT;                           /* ������ͣ������豸 ��*/
    caps_snd.sub_type                = AUDIO_DSP_PARAM;                             /* ����������Ƶ������Ϣ */
    caps_snd.udata.config.samplerate = RECORD_SAMPLERATE;    /* ������ */
    caps_snd.udata.config.channels   = RECORD_CHANNEL;         /* ����ͨ�� */
    caps_snd.udata.config.samplebits = 16;                                          /* ����λ�� */
    rt_device_control(snd_dev, AUDIO_CTL_CONFIGURE, &caps_snd);

    while (1)
    {
        /* �� Audio �豸�У���ȡ 20ms ����Ƶ����  */
        length = rt_device_read(mic_dev, 0, buffer, RECORD_CHUNK_SZ);
        rt_kprintf("read length = %d", length);

        if (length)
        {
            /* д����Ƶ���ݵ����ļ�ϵͳ */
            length = rt_device_write(snd_dev, 0, buffer, length);
            rt_kprintf("write length = %d", length);
        }

    }

    /* �ر� Audio �豸 */
    rt_device_close(mic_dev);
    /* �ر� Audio �豸 */
    rt_device_close(snd_dev);

__exit:

    if (buffer)
        rt_free(buffer);

    return 0;
}
MSH_CMD_EXPORT(audio_sample, record voice to a wav file);
