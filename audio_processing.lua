
--*************************************************************************************
-- Description: This script processes audio data from a WAV file and outputs the data
--              packets as a space-separated list. The audio file is read in packets,
--              and each packet is simulated with sample data. The total number of
--              frames and packets is calculated and printed for informational purposes.
--              The script uses the 'os.execute' function to ensure that execution is
--              limited to 10 seconds.
--
-- Author: Ryan Woodward
-- File: audio_processing.lua
--*************************************************************************************

audio_file_path = "audio_file.wav"
packetSizeInFrames = 1024

-- Function to simulate reading and processing audio data
function process_audio_packet(packetIndex, framesToRead)
    local audio_packet = {}
    for frameIndex = 0, framesToRead - 1 do
        -- Simulate reading audio frame data
        table.insert(audio_packet, frameIndex + packetIndex * packetSizeInFrames)
    end
    return audio_packet
end

-- Calculate the size of each audio packet in bytes
bytesPerFrame = tonumber(io.popen('soxi -b ' .. audio_file_path):read("*a"))
packetSizeInBytes = packetSizeInFrames * bytesPerFrame

-- Calculate the total number of audio frames in the file
total_frames = tonumber(io.popen('soxi -s ' .. audio_file_path):read("*a"))

-- Calculate the total number of audio packets
total_packets = math.floor((total_frames + packetSizeInFrames - 1) / packetSizeInFrames)

-- Print the calculated values for informational purposes
print("Total Frames: " .. total_frames)
print("Total Packets: " .. total_packets)

-- Use the 'os.execute' function to limit execution to 10 seconds
os.execute([[
    timeout 10s lua -e '
        -- Initialize a table to store audio data
        audio_data_buffer = {}

        -- Populate the audio data buffer with data from the audio file
        for packetIndex = 0, total_packets - 1 do
            local framesRemaining = total_frames - packetIndex * packetSizeInFrames
            local framesToRead = math.min(framesRemaining, packetSizeInFrames)

            -- Process audio packet and store in buffer
            local audio_packet = process_audio_packet(packetIndex, framesToRead)
            table.insert(audio_data_buffer, table.concat(audio_packet, " "))
        end

        -- Output the audio data buffer as a space-separated list
        for _, item in ipairs(audio_data_buffer) do
            print(item)
        end
    '
]])