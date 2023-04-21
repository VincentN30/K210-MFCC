#include <rtthread.h>
#include <rtdevice.h>
#include <dfs_posix.h>
#include <spi_msd.h>
#include <dfs_fs.h>
#include<stdio.h>

extern int kws_init ( void );
extern int kws_input ( const int16_t *audio );
extern float mfcc_features[];

#define RECORD_TIME_MS      5000
#define RECORD_SAMPLERATE   8000
#define RECORD_CHANNEL      2
#define RECORD_CHUNK_SZ     ((RECORD_SAMPLERATE * RECORD_CHANNEL * 2) * 20 / 1000)

#define MIC_DEVICE_NAME    "sound1"      /* Audio �豸���� */
static rt_device_t mic_dev;              /* Audio �豸��� */

struct wav_header
{
    char  riff_id[4];              /* "RIFF" */
    int   riff_datasize;           /* RIFF chunk data size,exclude riff_id[4] and riff_datasize,total - 8 */
    char  riff_type[4];            /* "WAVE" */
    char  fmt_id[4];               /* "fmt " */
    int   fmt_datasize;            /* fmt chunk data size,16 for pcm */
    short fmt_compression_code;    /* 1 for PCM */
    short fmt_channels;            /* 1(mono) or 2(stereo) */
    int   fmt_sample_rate;         /* samples per second */
    int   fmt_avg_bytes_per_sec;   /* sample_rate * channels * bit_per_sample / 8 */
    short fmt_block_align;         /* number bytes per sample, bit_per_sample * channels / 8 */
    short fmt_bit_per_sample;      /* bits of each sample(8,16,32). */
    char  data_id[4];              /* "data" */
    int   data_datasize;           /* data chunk size,pcm_size - 44 */
};


int wavrecord_sample(void)
{
    int fd = -1;
    rt_uint8_t *buffer = NULL;
    struct wav_header header;
    struct rt_audio_caps caps = {0};
    int length, total_length = 0;

    fd = open("/test.wav", O_WRONLY | O_CREAT);
    if (fd < 0)
    {
        rt_kprintf("open file failed!\n");
        return -1;
    }
    write(fd, &header, sizeof(struct wav_header));
    rt_kprintf("%d\n",fd);
    buffer = rt_malloc(RECORD_CHUNK_SZ);
    if (buffer == RT_NULL)
        goto __exit;
    /* �����豸���Ʋ��� Audio �豸����ȡ�豸��� */
    mic_dev = rt_device_find(MIC_DEVICE_NAME);
    if (mic_dev == RT_NULL)
    {
        rt_kprintf("find mic failed\n");
        goto __exit;

    }
    /* ��ֻ����ʽ�� Audio ¼���豸 */
    rt_device_open(mic_dev, RT_DEVICE_OFLAG_RDONLY);
    rt_kprintf("open mic successfully!\n");
    /* ���ò����ʡ�ͨ��������λ������Ƶ������Ϣ */
    caps.main_type               = AUDIO_TYPE_INPUT;                            /* �������ͣ�¼���豸 ��*/
    caps.sub_type                = AUDIO_DSP_PARAM;                             /* ����������Ƶ������Ϣ */
    caps.udata.config.samplerate = RECORD_SAMPLERATE;                           /* ������ */
    caps.udata.config.channels   = RECORD_CHANNEL;                              /* ����ͨ�� */
    caps.udata.config.samplebits = 16;                                          /* ����λ�� */
    rt_device_control(mic_dev, AUDIO_CTL_CONFIGURE, &caps);
    kws_init();
    while (1)
    {
        /* �� Audio �豸�У���ȡ 20ms ����Ƶ����  */
        length = rt_device_read(mic_dev, 0, buffer, RECORD_CHUNK_SZ);

        if (length)
        {
            /* д����Ƶ���ݵ����ļ�ϵͳ */
            write(fd, buffer, length);
            total_length += length;
        }

        if ((total_length / RECORD_CHUNK_SZ) >  (RECORD_TIME_MS / 20))
            break;
     }
    /* ����д�� wav �ļ���ͷ */
    wavheader_init(&header, RECORD_SAMPLERATE, RECORD_CHANNEL, total_length);
    lseek(fd, 0, SEEK_SET);
    write(fd, &header, sizeof(struct wav_header));
    close(fd);
    fd=-1;
    rt_free(buffer);

    fd = open("/test.wav", O_RDWR);
    fd=read(fd, buffer, total_length);
    rt_kprintf("fd:%d\n",fd);
    kws_input (buffer);
    for(int i=0;i<10*49;i++)
    {
        printf("%f ",mfcc_features[i]);
    }
    printf("\n");
    /* �ر� Audio �豸 */
    rt_device_close(mic_dev);
    rt_kprintf("close mic successfully!\n");

__exit:
    if (fd >= 0)
        close(fd);

    if (buffer)
       rt_free(buffer);

    return 0;
}

MSH_CMD_EXPORT(wavrecord_sample, record voice to a wav file);

