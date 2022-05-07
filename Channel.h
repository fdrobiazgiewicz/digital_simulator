#pragma once
#include "User.h"
#include <stdio.h>

class Channel
{
public:
	enum class ChannelLicense
	{
		UNLICENSED = 1,
		LICENSED = 2,
		RADAR = 3,
	};
    void connect_to_channel(){is_free_ = false ;}
    void disconnect_from_channel(){is_free_ = true ;}
    bool is_free(){ return is_free_; }

    void set_connection_time(size_t time){ connection_time = time; }
    size_t get_connection_time(){ return connection_time; }

    User::UserType get_user_type(){ return serving_user_; }
    void set_user_type(User::UserType type){serving_user_ = type; }

    ChannelLicense get_channel_license(){ return channel_license_; }

//    void get_licenese();
    Channel(ChannelLicense license);
private:
	User::UserType serving_user_;
    size_t connection_time = -1;
	bool is_free_ = true;
	ChannelLicense channel_license_ = ChannelLicense::UNLICENSED;
	

};

