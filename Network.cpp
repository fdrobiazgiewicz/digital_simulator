#include "Network.h"
#include "User.h"
#include "Channel.h"
#include <iostream>
#include "spdlog/spdlog.h"


void Network::Init(){
    decision_list_.clear();
    // Creating channels
    for (int i=0; i < 5; i++){
//        Channel channel(Channel::ChannelLicense::RADAR);
        channels_.push_back(std::make_shared<Channel>(Channel::ChannelLicense::RADAR));
    }

    for (int i=5; i<10; i++){
//        Channel channel(Channel::ChannelLicense::UNLICENSED);
        channels_.push_back(std::make_shared<Channel>(Channel::ChannelLicense::UNLICENSED));
    }
    for (int i=10; i<20; i++){
//        Channel channel(Channel::ChannelLicense::LICENSED);
        channels_.push_back(std::make_shared<Channel>(Channel::ChannelLicense::LICENSED));
    }


    // Setting channels licenses

}

void Network::RadarConnection() {
    for (int i=0; i<5; i++){
        if (!channels_[i].get()->is_free()){
            spdlog::debug("User removed from channel due to Radar priority connection.");
            channels_[i].get()->disconnect_from_channel();
            channels_[i].get()->set_connection_time(radar_connection_time * 1000);
            // Statistics
            users_connections_lost++;
            if (channels_[i].get()->get_user_type() == User::UserType::LICENSED){
                licensed_users_connections_lost++;
            }
            else if (channels_[i].get()->get_user_type() == User::UserType::NOT_LICENSED){
                unlicensed_users_connections_lost++;
            }
        }
        channels_[i].get()->connect_to_channel();
        channels_[i]->set_connection_time(radar_connection_time * 1000);
    }
}

void Network::RadarDisconnection() {
    for (int i=0; i<5; i++){
        channels_[i].get()->disconnect_from_channel();
    }
}

void Network::UserArrival(unsigned int arrival_time, unsigned int transmission_time, int user_type) {
    users_connections++;
    if (user_type){
        decision_list_.push_back(std::make_shared<User>(arrival_time, 10 * transmission_time, User::UserType::LICENSED));
        licensed_users_connections++;
    }
    else{
        decision_list_.push_back(std::make_shared<User>(arrival_time, 10 * transmission_time, User::UserType::NOT_LICENSED));
        unlicensed_users_connections++;
    }
}


bool Network::U2ConnectToChannel(std::shared_ptr<User> user) {
    bool go_to_buffer = U2ChannelAccess(user);
    // Add user to buffer
    if(buffer_.get_buffer_size() < 5 && go_to_buffer){
//        std::cout<<"User U2 added to buffer."<<std::endl;
        spdlog::debug("User U2 added to buffer.");
        buffer_.add_user_to_buffer(user);
        decision_list_.remove(user);
        return false;
    }
    // Delete user if buffer is full and no free channels
    else if (buffer_.get_buffer_size() >= 5 && go_to_buffer){
        spdlog::debug("Buffer full. User removed from decision list.");
//        std::cout<<"User removed from decision list\n";
        decision_list_.remove(user);
        licensed_users_connections_lost++;
        return true;
    }
    else {
        spdlog::debug("User removed from list after connection.");
        return false;
    }

}

bool Network::U2ChannelAccess(std::shared_ptr<User> user) {
    bool go_to_buffer = true;
    connection_success = false;
    // Check for dedicated channels
    for (int i = 10; i < 20; i++) {
        // Add user if channel is empty
        if (channels_[i].get()->is_free()) {
            go_to_buffer = false;
            channels_[i].get()->connect_to_channel();
            channels_[i].get()->set_user_type(User::UserType::LICENSED);
            channels_[i].get()->set_connection_time(
                    ((user.get()->get_arrival_time()) + (user.get()->get_transmission_time())));
            decision_list_.remove(user);
//            std::cout << "User U2 connected to the channel of index " << i << std::endl;
            spdlog::debug("User U2 connected to the channel of index {} ", i);
            connection_success = true;
            return go_to_buffer;
//            break;
        }
    }
    for (int i = 5; i < 10; i++) {
        if (channels_[i].get()->is_free()) {
            go_to_buffer = false;
            channels_[i].get()->connect_to_channel();
            channels_[i].get()->set_user_type(User::UserType::LICENSED);
            channels_[i].get()->set_connection_time(
                    ((user.get()->get_arrival_time()) + (user.get()->get_transmission_time())));
            decision_list_.remove(user);
            spdlog::debug("User U2 connected to the channel of index {} (Common channel)", i);
//            std::cout << "User U3 connected to the channel of index " << i << std::endl;
            connection_success = true;
//            break;
            return go_to_buffer;
        }

        else if (channels_[i].get()->get_user_type() == User::UserType::NOT_LICENSED){
            go_to_buffer = false;
            spdlog::debug("User U2 connected to the channel of index {} ", i);
            spdlog::debug("User U3 deleted due to U2 connection priority.");
            users_connections_lost++;
            unlicensed_users_connections_lost++;
            channels_[i].get()->connect_to_channel();
            channels_[i].get()->set_user_type(User::UserType::LICENSED);
            channels_[i].get()->set_connection_time(
                    ((user.get()->get_arrival_time()) + (user.get()->get_transmission_time())));
            decision_list_.remove(user);
            connection_success = true;
//            break;
            return go_to_buffer;
        }
    }
    for (int i = 0; i < 5; i++) {
        if (channels_[i].get()->is_free()) {
            go_to_buffer = false;
            channels_[i].get()->connect_to_channel();
            channels_[i].get()->set_user_type(User::UserType::LICENSED);
            channels_[i].get()->set_connection_time(
                    ((user.get()->get_arrival_time()) + (user.get()->get_transmission_time())));
            decision_list_.remove(user);
            spdlog::debug("User U2 connected to the channel of index {} (Dedicated for Radar)", i);
//            std::cout << "User U3 connected to the channel of index " << i << std::endl;
            connection_success = true;
//            break;
            return go_to_buffer;
        }
    }
    return go_to_buffer;
}

bool Network::U3ConnectToChannel(std::shared_ptr<User> user) {
    bool go_to_buffer = U3ChannelAccess(user);
    // Add user to buffer
    if (buffer_.get_buffer_size() < 5 && go_to_buffer) {
        spdlog::debug("User U3 added to buffer.");
//        std::cout << "User U3 added to buffer." << std::endl;
        buffer_.add_user_to_buffer(user);
        decision_list_.remove(user);
        return false;
    }
    // Delete user if buffer is full and no free channels
    else if (buffer_.get_buffer_size() >= 5 && go_to_buffer) {
        spdlog::debug("Buffer full. User removed from decision list.");
//        std::cout<<"User removed from decision list\n";
        users_connections_lost++;
        unlicensed_users_connections_lost++;
        decision_list_.remove(user);
        return true;
    }
    else {
        spdlog::debug("User removed from list after connection.");
        return false;
    }
}

bool Network::U3ChannelAccess(std::shared_ptr<User> user) {
    bool go_to_buffer = true;
    connection_success = false;
    // Connection to dedicated channels
    for (int i = 5; i < 10; i++) {
        if (channels_[i].get()->is_free()) {
            go_to_buffer = false;
            channels_[i].get()->connect_to_channel();
            channels_[i].get()->set_user_type(User::UserType::NOT_LICENSED);
            channels_[i].get()->set_connection_time(
                    ((user.get()->get_arrival_time()) + (user.get()->get_transmission_time())));
            decision_list_.remove(user);
            spdlog::debug("User U3 connected to the channel of index {}", i);
//            std::cout << "User U3 connected to the channel of index " << i << std::endl;
            connection_success = true;
//            break;
            return go_to_buffer;
        }
    }
    // Connecting to radar channels
    for (int i = 0; i < 5; i++){
        if (channels_[i].get()->is_free()){
            go_to_buffer = false;
            channels_[i].get()->connect_to_channel();
            channels_[i].get()->set_user_type(User::UserType::NOT_LICENSED);
            channels_[i].get()->set_connection_time(
                    ((user.get()->get_arrival_time()) + (user.get()->get_transmission_time())));
            decision_list_.remove(user);
            spdlog::debug("User U3 connected to the channel of index {} (Dedicated for Radar)", i);
            connection_success = true;
//            break;
            return go_to_buffer;
        }
    }
    // Connecting to LICENSED channels
    for (int i = 10; i < 20; i++){
        if (channels_[i].get()->is_free()){
            go_to_buffer = false;
            channels_[i].get()->connect_to_channel();
            channels_[i].get()->set_user_type(User::UserType::NOT_LICENSED);
            channels_[i].get()->set_connection_time(
                    ((user.get()->get_arrival_time()) + (user.get()->get_transmission_time())));
            decision_list_.remove(user);
            spdlog::debug("User U3 connected to the channel of index {} (Dedicated for U2)", i);
            connection_success = true;
//            break;
            return go_to_buffer;
        }
    }
    return go_to_buffer;
}

void Network::print_decision_list() {
    for (const auto& i: decision_list_) {
        std::cout << i.get() << std::endl;
    }
}

void Network::BufferPolling() {
    std::queue<std::shared_ptr<User>> buffer = buffer_.get_buffer();
    if (buffer.front()->get_user_type() == User::UserType::LICENSED){
        connection_success = false;
        bool _ = U2ChannelAccess(buffer.front());
        if (connection_success){
            spdlog::debug("(Connection from buffer)");
//            std::cout<<"User U2 connected from buffer."<<std::endl;
            buffer_.delete_user_from_buffer();
        }
        if (!connection_success){
            buffer.front()->set_connection_try_count(buffer.front()->get_connection_try_count() + 1);
        }
        if (buffer.front()->get_connection_try_count() == 5){
            spdlog::debug("USER DELETED FROM BUFFER");
//            std::cout<<"USER DELETED FROM BUFFER"<<std::endl;
            buffer_.delete_user_from_buffer();
            users_connections_lost++;
            licensed_users_connections_lost++;
        }
    }
    else if (buffer.front()->get_user_type() == User::UserType::NOT_LICENSED){
        connection_success = false;
        bool _ = U3ChannelAccess(buffer.front());
        if (connection_success){
            spdlog::debug("(Connection from buffer)");
//            std::cout<<"User U3 connected from buffer."<<std::endl;
            buffer_.delete_user_from_buffer();
        }
        if (!connection_success){
            buffer.front()->set_connection_try_count(buffer.front()->get_connection_try_count() + 1);
        }
        if (buffer.front()->get_connection_try_count() == 5){
            spdlog::debug("USER DELETED FROM BUFFER");
//            std::cout<<"USER DELETED FROM BUFFER"<<std::endl;
            buffer_.delete_user_from_buffer();
            users_connections_lost++;
            unlicensed_users_connections_lost++;
        }
    }
}

