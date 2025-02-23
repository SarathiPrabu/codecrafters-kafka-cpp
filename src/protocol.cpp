#include "protocol.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

// Parse incoming request into a struct
RequestMessage RequestMessage::parse(const std::vector<uint8_t>& buffer) {
    RequestMessage req{};

    req.message_size = ntohl(*reinterpret_cast<const uint32_t*>(buffer.data()));
    req.api_key = ntohs(*reinterpret_cast<const int16_t*>(buffer.data() + 4));
    req.api_version = ntohs(*reinterpret_cast<const int16_t*>(buffer.data() + 6));
    req.correlation_id = ntohl(*reinterpret_cast<const int32_t*>(buffer.data() + 8));

    return req;
}

template <typename T>
void write_to_buffer(std::vector<uint8_t>& buffer, T value) {
    T net_value = value;  // Default for non-network types

    // Convert to network byte order for multi-byte values
    if constexpr (sizeof(T) == 2) {
        net_value = htons(value);
    } else if constexpr (sizeof(T) == 4) {
        net_value = htonl(value);
    }

    const uint8_t* byte_data = reinterpret_cast<const uint8_t*>(&net_value);
    buffer.insert(buffer.end(), byte_data, byte_data + sizeof(T));
}

void encodeUnsignedVarint(uint32_t value, std::vector<uint8_t>& buffer) {
    while (value >= 0x80) {  // While value is greater than 7 bits
        buffer.push_back((value & 0x7F) | 0x80);  // Store 7 bits and set continuation bit
        value >>= 7;  // Shift right by 7 bits
    }
    buffer.push_back(value & 0x7F);  // Store final byte with no continuation bit
}

std::vector<uint8_t> ResponseMessage::serialize() const {
    std::vector<uint8_t> buffer;

    //Reserve space for message size (4 bytes) - Will be updated later
    buffer.resize(sizeof(message_size));  // Placeholder for message size

    //Step 1: Correlation ID (4 bytes)
    write_to_buffer(buffer, correlation_id);

    //Step 2: Error Code (2 bytes)
    write_to_buffer(buffer, error_code);

    //Step 3: Encode `num_api_keys` as an **Unsigned Varint**
    encodeUnsignedVarint(api_keys.size()+1, buffer);

    //Step 4: API Key List (Loop for `num_api_keys` entries)
    for (const auto& api_entry : api_keys) {
        write_to_buffer(buffer, api_entry.api_key);
        write_to_buffer(buffer, api_entry.min_version);
        write_to_buffer(buffer, api_entry.max_version);

    //Step 5: Tagged Fields for API Key Entry (Unsigned Varint = 0)
        encodeUnsignedVarint(0, buffer);
    }

    //Step 6: Throttle Time (4 bytes) - Should be 0 if no throttling
    write_to_buffer(buffer, throttle_time_ms);

    //Step 7: Tagged Fields for Response Body (Unsigned Varint = 0)
    encodeUnsignedVarint(0, buffer);

    //Compute and update message size
    uint32_t final_message_size = htonl(buffer.size() - sizeof(message_size));
    std::memcpy(buffer.data(), &final_message_size, sizeof(final_message_size));

    return buffer;
}
