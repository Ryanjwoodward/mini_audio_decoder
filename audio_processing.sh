#!/bin/bash

#*************************************************************************************
#                    _ _       
#     /\            | (_)      
#    /  \  _   _  __| |_  ___  
#   / /\ \| | | |/ _` | |/ _ \ 
#  / ____ \ |_| | (_| | | (_) |
# /_/    \_\__,_|\__,_|_|\___/ 
#
#                                    _                   _     
#                                   (_)                 | |    
#  _ __  _ __ ___   ___ ___  ___ ___ _ _ __   __ _   ___| |__  
# | '_ \| '__/ _ \ / __/ _ \/ __/ __| | '_ \ / _` | / __| '_ \ 
# | |_) | | | (_) | (_|  __/\__ \__ \ | | | | (_| |_\__ \ | | |
# | .__/|_|  \___/ \___\___||___/___/_|_| |_|\__, (_)___/_| |_|
# | |                                         __/ |            
# |_|                                        |___/             
#
#
# Description: This script processes audio data from a WAV file and outputs the data
#              packets as a space-separated list. The audio file is read in packets,
#              and each packet is simulated with sample data. The total number of
#              frames and packets is calculated and printed for informational purposes.
#              The script uses the 'timeout' command to ensure that execution is
#              limited to 10 seconds.
#
# Author: Ryan Woodward
# File: audio_processing.sh
#*************************************************************************************

audio_file_path="audio_file.wav"
packetSizeInFrames=1024

#? Calculate the size of each audio packet in bytes
bytesPerFrame=$(soxi -b "$audio_file_path")
packetSizeInBytes=$((packetSizeInFrames * bytesPerFrame))

#? Calculate the total number of audio frames in the file
total_frames=$(soxi -s "$audio_file_path")

#? Calculate the total number of audio packets
total_packets=$(( (total_frames + packetSizeInFrames - 1) / packetSizeInFrames ))

#? Print the calculated values for informational purposes
echo "Total Frames: $total_frames"
echo "Total Packets: $total_packets"

#? Use the timeout command to limit execution to 10 seconds
timeout 10s bash -c '
    #? Initialize an array to store audio data
    audio_data_buffer=()

    #? Populate the audio data buffer with data from the audio file
    for (( packetIndex=0; packetIndex<total_packets; ++packetIndex )); do
        framesRemaining=$((total_frames - packetIndex * packetSizeInFrames))
        framesToRead=$((framesRemaining < packetSizeInFrames ? framesRemaining : packetSizeInFrames))
        
        #? Simulate reading and processing audio data
        audio_packet=()
        for (( frameIndex=0; frameIndex<framesToRead; ++frameIndex )); do
            #? Simulate reading audio frame data
            audio_packet+=($((frameIndex + packetIndex * packetSizeInFrames)))
        done

        #? Store the audio packet data in the array
        audio_data_buffer+=("${audio_packet[@]}")
    done

    #? Output the audio data buffer as a space-separated list
    for item in "${audio_data_buffer[@]}"; do
        echo "$item"
    done
'
