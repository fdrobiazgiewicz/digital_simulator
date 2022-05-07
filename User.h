#ifndef USER_H_
#define USER_H_
#include <stdio.h>


class User
{
public:
    unsigned int test(){ return user_arrival_time; }
	enum class UserType
	{
		LICENSED,
		NOT_LICENSED
	};
    User(size_t arrival_time, size_t transmission_time, UserType type);
    UserType get_user_type(){ return user_type_; }
    size_t get_transmission_time(){ return user_transmission_time; }
    size_t get_arrival_time(){ return user_arrival_time; }

    size_t get_connection_try_count(){ return connection_try_count; }
    void set_connection_try_count(size_t try_count){ connection_try_count = try_count; }

private:
	UserType user_type_ = UserType::LICENSED;
    size_t user_transmission_time;
    size_t user_arrival_time;
    size_t connection_try_count = 0;
};

#endif // !USER_H

