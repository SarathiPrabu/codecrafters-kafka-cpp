#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <vector>
#include <cstdint>

// Struct for parsing Kafka request
struct RequestMessage {
    uint32_t message_size;
    int16_t api_key;
    int16_t api_version;
    int32_t correlation_id;

    static RequestMessage parse(const std::vector<uint8_t>& buffer);
};
struct ResponseMessage {
    int32_t message_size;
    int32_t correlation_id;
    int16_t error_code;
    uint32_t api_keys;   
    int16_t api_key;
    int16_t min_version;
    int16_t max_version;
    int32_t throttle_time_ms;
    std::vector<uint8_t> serialize() const;
};


#endif // PROTOCOL_HPP
