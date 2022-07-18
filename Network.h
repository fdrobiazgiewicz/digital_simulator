#ifndef NETWORK_H_
#define NETWORK_H_

#include <memory>
#include "Buffer.h"
#include "User.h"
#include "Channel.h"
#include <list>
#include <queue>


class Network
{
public:
    void Init();
    void RadarConnection();
    void RadarDisconnection();
    void UserArrival(unsigned int arrival_time, unsigned int transmission_time, int user_type);
    bool U2ConnectToChannel(std::shared_ptr<User> user);
    bool U3ConnectToChannel(std::shared_ptr<User> user);
    bool U2ChannelAccess(std::shared_ptr<User> user);
    bool U3ChannelAccess(std::shared_ptr<User> user);
    void BufferPolling();

    size_t radar_get_generate_time() const { return radar_generate_time; }
    void radar_set_generate_timer(size_t time) { radar_generate_time = time; }
    size_t radar_get_transmission_time() const { return radar_transmission_time; }
    void radar_set_transmission_time(size_t time) { radar_transmission_time = time; }

    size_t user_get_generate_time() { return user_generate_time; }
    void user_set_generate_time(size_t time) { user_generate_time = time; }

    Buffer get_buffer() { return buffer_; };
    size_t get_buffer_polling_time()  { return buffer_polling; }
    void set_buffer_polling_time(size_t polling_time)  { buffer_polling = polling_time; }

    std::vector<std::shared_ptr<Channel>> get_channels(){ return channels_; }

    std::list<std::shared_ptr<User>> get_decision_list() { return decision_list_; }

    void set_after_initial_state() {if_after_initial = true; }

    void print_decision_list();

    // Statistic purpose
    double licensed_users_connections = 0;
    double unlicensed_users_connections = 0;
    double users_connections = 0;
    double licensed_users_connections_lost = 0;
    double unlicensed_users_connections_lost = 0;
    double users_connections_lost = 0;


private:
	Buffer buffer_;
	std::vector<std::shared_ptr<Channel>> channels_;
	std::list<std::shared_ptr<User>> decision_list_;
    const int number_of_channels = 20;
    size_t radar_generate_time = 19999999;
    size_t radar_transmission_time = 199999999;
    size_t radar_connection_time = 10;
    size_t user_generate_time = -1;
    size_t buffer_polling = 0;
    bool connection_success = false;
    bool if_after_initial = false;

};

#endif // !NETWORK_H_



