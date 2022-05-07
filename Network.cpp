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
        channels_[i].get()->connect_to_channel();
        channels_[i]->set_connection_time(10 * 1000);
    }
}

void Network::RadarDisconnection() {
    for (int i=0; i<5; i++){
        channels_[i].get()->disconnect_from_channel();
    }
}

void Network::UserArrival(unsigned int arrival_time, unsigned int transmission_time) {
    int user_type_drawing = rand() % 100;

    if (user_type_drawing <= 60){
        decision_list_.push_back(std::make_shared<User>(arrival_time, 10 * transmission_time, User::UserType::LICENSED));
    }
    else if (user_type_drawing >= 60){
        decision_list_.push_back(std::make_shared<User>(arrival_time, 10 * transmission_time, User::UserType::NOT_LICENSED));
    }
}


void Network::U2ConnectToChannel(std::shared_ptr<User> user) {
    bool go_to_buffer = U2ChannelAccess(user);
    // Add user to buffer
    if(buffer_.get_buffer_size() < 5 && go_to_buffer){
//        std::cout<<"User U2 added to buffer."<<std::endl;
        spdlog::info("User U2 added to buffer.");
        buffer_.add_user_to_buffer(user);
        decision_list_.remove(user);
    }
    // Delete user if buffer is full and no free channels
    else{
        spdlog::info("User removed from decision list.");
//        std::cout<<"User removed from decision list\n";
        decision_list_.remove(user);
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
            spdlog::info("User U2 connected to the channel of index {} ", i);
            connection_success = true;
            break;
        }
    }
    return go_to_buffer;
}

void Network::U3ConnectToChannel(std::shared_ptr<User> user) {
    bool go_to_buffer = U3ChannelAccess(user);
    // Add user to buffer
    if (buffer_.get_buffer_size() < 5 && go_to_buffer) {
        spdlog::info("User U3 added to buffer.");
//        std::cout << "User U3 added to buffer." << std::endl;
        buffer_.add_user_to_buffer(user);
        decision_list_.remove(user);
    }
    // Delete user if buffer is full and no free channels
    else{
        spdlog::info("User removed from decision list");
//        std::cout<<"User removed from decision list\n";
        decision_list_.remove(user);
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
            spdlog::info("User U3 connected to the channel of index {}", i);
//            std::cout << "User U3 connected to the channel of index " << i << std::endl;
            connection_success = true;
//            break;
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
            spdlog::info("User U2 connected from buffer.");
//            std::cout<<"User U2 connected from buffer."<<std::endl;
            buffer.pop();
        }
        if (!connection_success){
            buffer.front()->set_connection_try_count(buffer.front()->get_connection_try_count() + 1);
        }
        if (buffer.front()->get_connection_try_count() == 5){
            spdlog::info("USER DELETED FROM BUFFER");
//            std::cout<<"USER DELETED FROM BUFFER"<<std::endl;
            buffer.pop();
        }
    }
    else if (buffer.front()->get_user_type() == User::UserType::NOT_LICENSED){
        connection_success = false;
        bool _ = U3ChannelAccess(buffer.front());
        if (connection_success){
            spdlog::info("User U3 connected from buffer.");
//            std::cout<<"User U3 connected from buffer."<<std::endl;
            buffer.pop();
        }
        if (!connection_success){
            buffer.front()->set_connection_try_count(buffer.front()->get_connection_try_count() + 1);
        }
        if (buffer.front()->get_connection_try_count() == 5){
            spdlog::info("USER DELETED FROM BUFFER");
//            std::cout<<"USER DELETED FROM BUFFER"<<std::endl;
            buffer.pop();
        }
    }
}

