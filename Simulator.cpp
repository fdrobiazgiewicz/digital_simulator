#include "Simulator.h"
#include "Network.h"
#include "Radar.h"
#include <iostream>
#include "spdlog/spdlog.h"
#include "RandomGenerator.h"

int max_arrival_time = 100;

//spdlog::info("Welcome to spdlog!");

Simulator::Simulator(Network* network) : network_(network)
{
}

void Simulator::M1(int time, double tau_lambda, double q_lambda, double n) {

    spdlog::set_level(spdlog::level::info);

    auto generator = new RandomGenerator(24324234);
    std::cout << "RndExp: " << (generator->RndExp(tau_lambda) + 1) * 1000 << std::endl;
    std::cout<<"Previous: "<<rand() % (1000 * max_arrival_time) + 1<<std::endl;
    std::cout << "RndExp 2: " << (generator->RndExp(tau_lambda) + 1) * 1000 << std::endl;
    std::cout<<"Previous 2: "<<rand() % (1000 * max_arrival_time) + 1<<std::endl;
    std::cout << "RndExp 3: " << (generator->RndExp(tau_lambda) + 1) * 1000 << std::endl;
    std::cout<<"Previous 3: "<<rand() % (1000 * max_arrival_time) + 1<<std::endl;
    std::cout << "RndExp 4: " << (generator->RndExp(tau_lambda) + 1) * 1000 << std::endl;
    std::cout<<"Previous 4: "<<rand() % (1000 * max_arrival_time) + 1<<std::endl;
    std::cout << "RndExp 5: " << (generator->RndExp(tau_lambda) + 1) * 1000 << std::endl;
    std::cout<<"Previous 5: "<<rand() % (1000 * max_arrival_time) + 1<<std::endl;
    std::cout << "RndExp 6: " << (generator->RndExp(tau_lambda) + 1) * 1000 << std::endl;
    std::cout<<"Previous 6: "<<rand() % (1000 * max_arrival_time) + 1<<std::endl;
    std::cout << "RndExp 7: " << (generator->RndExp(tau_lambda) + 1) * 1000 << std::endl;
    std::cout<<"Previous 7: "<<rand() % (1000 * max_arrival_time) + 1<<std::endl;
    std::cout << "RndExp 8: " << (generator->RndExp(tau_lambda) + 1) * 1000 << std::endl;
    std::cout<<"Previous 8: "<<rand() % (1000 * max_arrival_time) + 1<<std::endl;
    std::cout << "RndExp 9: " << (generator->RndExp(tau_lambda) + 1) * 1000 << std::endl;
    std::cout<<"Previous 9: "<<rand() % (1000 * max_arrival_time) + 1<<std::endl;

    // Dla radaru - czas do aktywacji (1,5)s
    std::cout<<"Uniform <1,5ms>"<<round((generator->Rand()) * 5) * 1000<<std::endl;
    std::cout<<"Uniform <1,5ms>"<<round((generator->Rand()) * 5) * 1000<<std::endl;
    std::cout<<"Uniform <1,5ms>"<<round((generator->Rand()) * 5) * 1000<<std::endl;

    // Czas trwania połączeń użytkowników o intensywności 1ms^-1
    std::cout<<"q1: "<<(generator->RndExp(q_lambda) + 1) * 1000<<std::endl;
    std::cout<<"q2: "<<(generator->RndExp(q_lambda) + 1) * 1000<<std::endl;
    std::cout<<"q3: "<<(generator->RndExp(q_lambda) + 1) * 1000<<std::endl;
    std::cout<<"q4: "<<(generator->RndExp(q_lambda) + 1) * 1000<<std::endl;
    std::cout<<"q5: "<<(generator->RndExp(q_lambda) + 1) * 1000<<std::endl;

    // Rozkład 0-1 nkowy
    std::cout<<"01: "<<generator->RndZeroOne(n)<<std::endl;




    time *= 1000;
    clock_ = 0;
    network_->Init();
    network_->radar_set_generate_timer(rand() % (1000 * max_arrival_time) + 1);
    network_->user_set_generate_time(rand() % (1000 * max_arrival_time) + 1);
//    std::cout << "Started Simulation method M1: \n";
    spdlog::info("Started Simulation method M1");
    float clock_counter = 0;

    while (clock_ < static_cast<size_t>(time)) {
        bool no_event = false;

//        std::cout << "Simulation time_: " << clock_ << "\n";
        if (clock_ / 1000 >= clock_counter){
            spdlog::debug("Simulation time_: {}", clock_counter);
            clock_counter += 1;
        }
//        spdlog::info("Simulation time_: {}", clock_);

        // Handle events
        while (!no_event) {
            no_event = true;
            // Radar connection
            if (network_->radar_get_generate_time() == 0) {
//                std::cout << "Radar connect." << std::endl;
                spdlog::debug("Radar connect");
                network_->RadarConnection();
                network_->radar_set_transmission_time(10 * 1000 );
                network_->radar_set_generate_timer(-1);
                no_event = false;
            }
            // Radar disconnection
            if (network_->radar_get_transmission_time() == 0) {
//                std::cout << "Radar disconnect." << std::endl;
                spdlog::debug("Radar disconnect");
                network_->RadarDisconnection();
                network_->radar_set_transmission_time(-1);
                network_->radar_set_generate_timer(rand() % (1000 * max_arrival_time) + 1);
                no_event = false;
            }
            // User U2/U3 arrival
            if (network_->user_get_generate_time() == 0) {
                network_->UserArrival(clock_, (rand() % (1000 * max_arrival_time) + 1));
                network_->user_set_generate_time(rand() % (10 * 1000) + 1);
//                std::cout << "New user!" << std::endl;
                spdlog::debug("New user!");
                no_event = false;
            }

            // Checking decision list
            std::list<std::shared_ptr<User>> decision_list = network_->get_decision_list();
            for (const auto &i: decision_list) {
                if (i.get()->get_user_type() == User::UserType::LICENSED) {
                    network_->U2ConnectToChannel(i);
                }
                else if (i.get()->get_user_type() == User::UserType::NOT_LICENSED){
                    network_->U3ConnectToChannel(i);
                }
            }

            // Checking for user transmission end (User disconnection)
            std::vector<std::shared_ptr<Channel>> channel_list = network_->get_channels();
            for (const auto &i: channel_list){
//                std::cout<<"Get connection time: "<<i.get()->get_connection_time()<<std::endl;
                if(i.get()->get_connection_time() == clock_ && i.get()->get_channel_license() != Channel::ChannelLicense::RADAR){
                    i.get()->disconnect_from_channel();
                    spdlog::debug("User disconnected from channel");
                }
            }

            if (network_->get_buffer_polling_time() == 0){
                network_->BufferPolling();
                network_->set_buffer_polling_time(0.2 * 1000);
            }






        }

        // Update time
        network_->radar_set_transmission_time((network_->radar_get_transmission_time() - 1));
        network_->radar_set_generate_timer((network_->radar_get_generate_time() - 1));
        network_->user_set_generate_time((network_->user_get_generate_time() - 1));
        network_->set_buffer_polling_time((network_->get_buffer_polling_time() - 1));
        clock_++;

//            network_->print_decision_list();


    }

}