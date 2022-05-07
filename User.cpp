#include "User.h"

User::User(size_t arrival_time, size_t transmission_time, UserType type) {
    user_transmission_time = transmission_time;
    user_arrival_time = arrival_time;
    user_type_ = type;
}
