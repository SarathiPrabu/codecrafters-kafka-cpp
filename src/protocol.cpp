#include "protocol.hpp"
#include <arpa/inet.h>
#include <cstring>

// Parse incoming request into a struct
RequestMessage RequestMessage::parse(const std::vector<uint8_t>& buffer) {
    RequestMessage req{};

    req.message_size = ntohl(*reinterpret_cast<const uint32_t*>(buffer.data()));
    req.request_api_key = ntohs(*reinterpret_cast<const int16_t*>(buffer.data() + 4));
    req.request_api_version = ntohs(*reinterpret_cast<const int16_t*>(buffer.data() + 6));
    req.correlation_id = ntohl(*reinterpret_cast<const int32_t*>(buffer.data() + 8));

    return req;
}

// Serialize response into a binary buffer
std::vector<uint8_t> ResponseMessage::serialize() const {
    std::vector<uint8_t> buffer;

    // **Placeholder for message size (4 bytes, updated later)**
    uint32_t message_size = 0;
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&message_size),
                  reinterpret_cast<const uint8_t*>(&message_size) + sizeof(message_size));

    // **Correlation ID (4 bytes)**
    uint32_t correlation_id_net = htonl(correlation_id);
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&correlation_id_net),
                  reinterpret_cast<const uint8_t*>(&correlation_id_net) + sizeof(correlation_id_net));

    // **Error Code (2 bytes)**
    int16_t error_code_net = htons(error_code);
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&error_code_net),
                  reinterpret_cast<const uint8_t*>(&error_code_net) + sizeof(error_code_net));

    // **API Versions Count (4 bytes, at least 1)**
    uint32_t api_versions_count_net = htonl(1);  // 1 API version entry
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&api_versions_count_net),
                  reinterpret_cast<const uint8_t*>(&api_versions_count_net) + sizeof(api_versions_count_net));

    // **API Key Entry (API_VERSIONS = 18)**
    int16_t api_key_net = htons(api_key);
    int16_t min_version_net = htons(min_version);
    int16_t max_version_net = htons(max_version);

    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&api_key_net),
                reinterpret_cast<const uint8_t*>(&api_key_net) + sizeof(api_key_net));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&min_version_net),
                reinterpret_cast<const uint8_t*>(&min_version_net) + sizeof(min_version_net));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&max_version_net),
                reinterpret_cast<const uint8_t*>(&max_version_net) + sizeof(max_version_net));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&throttle_time_ms),
                reinterpret_cast<const uint8_t*>(&throttle_time_ms) + sizeof(throttle_time_ms));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&no_tags),
                reinterpret_cast<const uint8_t*>(&no_tags) + sizeof(no_tags));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&api_key_tags),
                reinterpret_cast<const uint8_t*>(&api_key_tags) + sizeof(api_key_tags));
                
    // **Update message size**
    uint32_t final_message_size = htonl(buffer.size() - sizeof(message_size));
    std::memcpy(buffer.data(), &final_message_size, sizeof(final_message_size));

    return buffer;
}
