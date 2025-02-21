#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <vector>
#include <cstdint>

// Struct for parsing Kafka request
struct RequestMessage {
    uint32_t message_size;
    int16_t request_api_key;
    int16_t request_api_version;
    int32_t correlation_id;

    static RequestMessage parse(const std::vector<uint8_t>& buffer);
};

// Struct for API Versions Response
struct ResponseMessage {
    uint32_t correlation_id;
    int16_t error_code;  // 0 = No error
    int32_t api_versions_count;
    int16_t api_key;
    int16_t min_version;
    int16_t max_version;
    int32_t throttle_time_ms;
    uint8_t no_tags;
    uint8_t api_key_tags;

    std::vector<uint8_t> serialize() const;
};

#endif // PROTOCOL_HPP
