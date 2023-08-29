#define MINIAUDIO_IMPLEMENTATION

//*-----------------------------------------------------------------------------
//*                                   Includes
//*-----------------------------------------------------------------------------
#include "mini_audio.h"

#include <stdio.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
// The result of various mini_audio library function calls
static ma_result result;

// The decoder instance used to decode the audio file
static ma_decoder decoder;

// The number of audio frames per packet
static ma_uint32 packetSizeInFrames = 1024;

// The size of each audio packet in bytes
static ma_uint32 packetSizeInBytes;

// The total number of audio frames in the audio file
static ma_uint64 total_frames;

// The total number of audio packets in the audio file
static ma_uint64 total_packets;

// The number of audio frames remaining to be read for the current packet
static ma_uint64 framesRemaining;

// The number of audio frames to read for the current packet
static ma_uint64 framesToRead;

// The file path of the audio file to be processed
static const char* audio_file_path = "audio_file.wav";


//*-----------------------------------------------------------------------------
//*                          Static Function Declarations
//*-----------------------------------------------------------------------------
// Main function for the audio processing
int audio_main();

// Initializes the audio file and decoder
static int audio_init_file();

// Calculates the packet size in bytes based on frames and format
static void audio_calculate_packet_size_bytes();

// Calculates the total number of frames in the audio file
static int audio_calculate_total_frames();

// Calculates the total number of packets based on frames and packet size
static void audio_calculate_total_packets();

// Initializes the audio data buffer
static int audio_init_buffer();

// Populates the audio data buffer with audio frames
static int audio_populate_buffer();

// Cleans up allocated resources
static void audio_cleanup();



// A buffer to hold audio data for a single packet
ma_uint8* audio_data_buffer;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/*
 * THIS FUNCTION WAS USED TO TEST THIS SOURCE FILE
 */
int main(int argc, char** argv)
{
    printf("Begin audio Program\n");

    if (audio_main() != 0) {
        printf("Error occurred in audio_main()\n");
        return -1;
    }

    printf("End Audio Program\n");

    return 0;
}
/**/

/**
 * @brief Entry point for the audio processing application.
 *
 * This function initializes the audio file, calculates packet and frame sizes, initializes the
 * audio buffer, populates the buffer with audio data, and then performs cleanup operations.
 *
 * @return 0 if the audio processing is successful, otherwise returns a negative error code.
 */
int audio_main()
{
    // Initialize the audio file
    if (audio_init_file() != 0) {
        return -1;
    }

    // Calculate the size of each audio packet in bytes
    audio_calculate_packet_size_bytes();

    // Calculate the total number of audio frames in the file
    audio_calculate_total_frames();

    // Calculate the total number of audio packets in the file
    audio_calculate_total_packets();

    // Initialize the audio buffer
    audio_init_buffer();

    // Print the total frames and packets for informational purposes
    printf("Total Frames: %llu\n", total_frames);
    printf("Total Packets: %llu\n", total_packets);

    // Populate the audio buffer with data from the audio file
    audio_populate_buffer();

    // Clean up allocated resources and uninitialize the decoder
    audio_cleanup();

    return 0;
}

/**
 * @brief Initializes the audio file for decoding.
 *
 * This function initializes the audio decoder using the provided audio file path. If initialization
 * fails, an error message is printed and a negative error code is returned.
 *
 * @return 0 if initialization is successful, otherwise returns a negative error code.
 */
static int audio_init_file()
{
    result = ma_decoder_init_file(audio_file_path, NULL, &decoder);
    if (result != MA_SUCCESS) {
        printf("Could not load file: %s\n", audio_file_path);
        return -1;
    }

    return 0;
}

/**
 * @brief Calculates the size of each audio packet in bytes.
 *
 * This function calculates the size of each audio packet in bytes based on the number of frames
 * in a packet and the format of the audio data.
 */
static void audio_calculate_packet_size_bytes()
{
    packetSizeInBytes = packetSizeInFrames * ma_get_bytes_per_frame(decoder.outputFormat, decoder.outputChannels);
}

/**
 * @brief Calculates the total number of audio frames in the file.
 *
 * This function retrieves the total number of audio frames in the audio file using the decoder and
 * stores the value in the `total_frames` variable. If retrieving the frame count fails, an error
 * message is printed, and the decoder is uninitialized.
 *
 * @return 0 if retrieving the frame count is successful, otherwise returns a negative error code.
 */
static int audio_calculate_total_frames()
{
    result = ma_decoder_get_length_in_pcm_frames(&decoder, &total_frames);
    if (result != MA_SUCCESS) {
        printf("Failed to get total frames.\n");
        ma_decoder_uninit(&decoder);
        return -1;
    }

    return 0;
}

/**
 * @brief Calculates the total number of audio packets in the file.
 *
 * This function calculates the total number of audio packets needed to cover all audio frames in
 * the file, based on the total frames and the frames per packet.
 */
static void audio_calculate_total_packets()
{
    total_packets = (total_frames + packetSizeInFrames - 1) / packetSizeInFrames;
}

/**
 * @brief Initializes the audio data buffer.
 *
 * This function allocates memory for the audio data buffer based on the size of each audio packet.
 * If memory allocation fails, an error message is printed, and the decoder is uninitialized.
 *
 * @return 0 if memory allocation is successful, otherwise returns a negative error code.
 */
static int audio_init_buffer()
{
    audio_data_buffer = (ma_uint8*)malloc(packetSizeInBytes);
    if (audio_data_buffer == NULL) {
        printf("Memory allocation failed\n");
        ma_decoder_uninit(&decoder);
        return -3;
    }

    return 0;
}

/**
 * @brief Populates the audio data buffer with audio data packets.
 *
 * This function iterates over each audio packet, reads audio frames from the decoder, and stores
 * the data in the audio data buffer. If reading the audio data fails, an error message is printed,
 * and allocated resources are cleaned up.
 *
 * @return 0 if populating the buffer is successful, otherwise returns a negative error code.
 */
static int audio_populate_buffer()
{
    for (ma_uint64 packetIndex = 0; packetIndex < total_packets; ++packetIndex) {
        framesRemaining = total_frames - (packetIndex * packetSizeInFrames);
        framesToRead = (framesRemaining < packetSizeInFrames) ? framesRemaining : packetSizeInFrames;

        result = ma_decoder_read_pcm_frames(&decoder, audio_data_buffer, framesToRead, NULL);
        if (result != MA_SUCCESS) {
            printf("Failed to read audio data\n");
            free(audio_data_buffer);
            ma_decoder_uninit(&decoder);
            return -4;
        }

        // Process the audio data in the buffer (you can copy it to your radio buffer here)
        // ...
    }

    printf("audio_populate_buffer completed without error\n");
    return 0;
}

/**
 * @brief Cleans up resources and uninitializes the decoder.
 *
 * This function frees the memory allocated for the audio data buffer and uninitializes the audio
 * decoder.
 */
static void audio_cleanup()
{
    free(audio_data_buffer);
    ma_decoder_uninit(&decoder);
}
