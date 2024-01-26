#ifndef VORBIS_H
#define VORBIS_H

#include <miniaudio/miniaudio.h>

/*

    Miniaudio's libvorbis example
    https://miniaud.io/docs/examples/custom_decoder.html

*/

namespace Hop::System::Sound
{
    static ma_result ma_decoding_backend_init__libvorbis(void* pUserData, ma_read_proc onRead, ma_seek_proc onSeek, ma_tell_proc onTell, void* pReadSeekTellUserData, const ma_decoding_backend_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_data_source** ppBackend)
    {
        ma_result result;
        ma_libvorbis* pVorbis;

        (void)pUserData;

        pVorbis = (ma_libvorbis*)ma_malloc(sizeof(*pVorbis), pAllocationCallbacks);
        if (pVorbis == NULL) {
            return MA_OUT_OF_MEMORY;
        }

        result = ma_libvorbis_init(onRead, onSeek, onTell, pReadSeekTellUserData, pConfig, pAllocationCallbacks, pVorbis);
        if (result != MA_SUCCESS) {
            ma_free(pVorbis, pAllocationCallbacks);
            return result;
        }

        *ppBackend = pVorbis;

        return MA_SUCCESS;
    }

    static ma_result ma_decoding_backend_init_file__libvorbis(void* pUserData, const char* pFilePath, const ma_decoding_backend_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_data_source** ppBackend)
    {
        ma_result result;
        ma_libvorbis* pVorbis;

        (void)pUserData;

        pVorbis = (ma_libvorbis*)ma_malloc(sizeof(*pVorbis), pAllocationCallbacks);
        if (pVorbis == NULL) {
            return MA_OUT_OF_MEMORY;
        }

        result = ma_libvorbis_init_file(pFilePath, pConfig, pAllocationCallbacks, pVorbis);
        if (result != MA_SUCCESS) {
            ma_free(pVorbis, pAllocationCallbacks);
            return result;
        }

        *ppBackend = pVorbis;

        return MA_SUCCESS;
    }

    static void ma_decoding_backend_uninit__libvorbis(void* pUserData, ma_data_source* pBackend, const ma_allocation_callbacks* pAllocationCallbacks)
    {
        ma_libvorbis* pVorbis = (ma_libvorbis*)pBackend;

        (void)pUserData;

        ma_libvorbis_uninit(pVorbis, pAllocationCallbacks);
        ma_free(pVorbis, pAllocationCallbacks);
    }

    #pragma GCC diagnostic push                             
    #pragma GCC diagnostic ignored "-Wunused-function"
    static ma_result ma_decoding_backend_get_channel_map__libvorbis(void* pUserData, ma_data_source* pBackend, ma_channel* pChannelMap, size_t channelMapCap)
    {
        ma_libvorbis* pVorbis = (ma_libvorbis*)pBackend;

        (void)pUserData;

        return ma_libvorbis_get_data_format(pVorbis, NULL, NULL, NULL, pChannelMap, channelMapCap);
    }
    #pragma GCC diagnostic pop

    static ma_decoding_backend_vtable g_ma_decoding_backend_vtable_libvorbis =
    {
        ma_decoding_backend_init__libvorbis,
        ma_decoding_backend_init_file__libvorbis,
        NULL, /* onInitFileW() */
        NULL, /* onInitMemory() */
        ma_decoding_backend_uninit__libvorbis
    };

}

#endif /* VORBIS_H */
