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
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&message_size),
                  reinterpret_cast<const uint8_t*>(&message_size) + sizeof(message_size));

    //Step 1: Correlation ID (4 bytes)
    uint32_t correlation_id_net = htonl(correlation_id);
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&correlation_id_net),
                  reinterpret_cast<const uint8_t*>(&correlation_id_net) + sizeof(correlation_id_net));

    //Step 2: Error Code (2 bytes)
    int16_t error_code_net = htons(error_code);
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&error_code_net),
                  reinterpret_cast<const uint8_t*>(&error_code_net) + sizeof(error_code_net));

    //Step 3: Encode `num_api_keys` as an **Unsigned Varint**
    encodeUnsignedVarint(api_keys+1, buffer);  // Only one API key (ApiVersions)

    //Step 4: API Key List (Loop for `num_api_keys` entries)
    int16_t api_key_net = htons(api_key);  // API_VERSIONS = 18
    int16_t min_version_net = htons(min_version);
    int16_t max_version_net = htons(max_version);  // Must be at least 4

    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&api_key_net),
                  reinterpret_cast<const uint8_t*>(&api_key_net) + sizeof(api_key_net));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&min_version_net),
                  reinterpret_cast<const uint8_t*>(&min_version_net) + sizeof(min_version_net));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&max_version_net),
                  reinterpret_cast<const uint8_t*>(&max_version_net) + sizeof(max_version_net));

    //Step 5: Tagged Fields for API Key Entry (Unsigned Varint = 0)
    encodeUnsignedVarint(0, buffer);  // No additional fields

    //Step 6: Throttle Time (4 bytes) - Should be 0 if no throttling
    int32_t throttle_time_ms_net = htonl(throttle_time_ms);
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&throttle_time_ms_net),
                  reinterpret_cast<const uint8_t*>(&throttle_time_ms_net) + sizeof(throttle_time_ms_net));

    //Step 7: Tagged Fields for Response Body (Unsigned Varint = 0)
    encodeUnsignedVarint(0, buffer);  // No additional fields

    //Compute and update message size
    uint32_t final_message_size = htonl(buffer.size() - sizeof(message_size));
    std::memcpy(buffer.data(), &final_message_size, sizeof(final_message_size));

    return buffer;
}
