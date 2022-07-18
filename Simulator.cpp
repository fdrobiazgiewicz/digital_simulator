#include "Simulator.h"
#include "Network.h"
#include <iostream>
#include "spdlog/spdlog.h"
#include "RandomGenerator.h"
#include <fstream>
#include <vector>
#include <cmath>

double occupied_channels_counter = 0;
double licensed_users_counter = 0;
double unlicensed_users_counter = 0;


Simulator::Simulator(Network* network) : network_(network)
{
}

void Simulator::M1(int time, int initial_time, double tau_lambda, double q_lambda, double n, int seed_set,
                   int served_users, bool if_debug) {

    if (if_debug) {
        spdlog::set_level(spdlog::level::debug);
    }
    else{
        spdlog::set_level(spdlog::level::info);
    }

    // Seeds handling
    auto generator = new RandomGenerator(24324234);
    generator->GenerateSeeds(40);
    int seed;
    std::vector<int>seeds;
    std::ifstream seed_file("seeds.txt");

    for (int i = 0; i < (seed_set * 4); i++){
        seed_file >> seed;
        if (i >= (seed_set - 1) * 4){
            seeds.push_back(seed);
        }
    }

    auto tau_gen = new RandomGenerator(seeds[0]);
    auto uni_gen = new RandomGenerator(seeds[1]);
    auto q_gen = new RandomGenerator(seeds[2]);
    auto dec_gen = new RandomGenerator(seeds[3]);



//    std::cout << "RndExp: " << (tau_gen->RndExp(tau_lambda) + 1) * 1000 << std::endl;
//    std::cout<<"Previous: "<<rand() % (1000 * max_arrival_time) + 1<<std::endl;


    // Dla radaru - czas do aktywacji (1,5)s
//    std::cout<<"Uniform <1,5ms>"<<round((uni_gen->Rand()) * 5) * 1000<<std::endl;


    // Czas trwania połączeń użytkowników o intensywności 1ms^-1
//    std::cout<<"q1: "<<(q_gen->RndExp(q_lambda) + 1) * 1000<<std::endl;


    // Rozkład 0-1 nkowy
//    std::cout<<"01: "<<dec_gen->RndZeroOne(n)<<std::endl;


    time *= 10;
    clock_ = 0;
    network_->Init();
    network_->radar_set_generate_timer(round((uni_gen->Rand()) * 5) * 10);
    network_->user_set_generate_time((tau_gen->RndExp(tau_lambda) + 1) * 10);
    spdlog::info("Started Simulation method M1");
    float clock_counter = 0;
    bool if_lost_u2, if_lost_u3;
//    std::vector<double> channel_occupancy;
//    channel_occupancy.reserve(time);

    // Occupied channels txt file
    std::ofstream channels_file("occupied_channels.txt", std::ofstream::out);

    while (clock_ < static_cast<size_t>(time)) {
        bool no_event = false;
        std::vector<size_t> time_to_skip;
        time_to_skip.clear();
        time_to_skip.push_back(network_->radar_get_generate_time());
        time_to_skip.push_back(network_->user_get_generate_time());

//        if (clock_ / 10 >= clock_counter){
//            spdlog::debug("Simulation time_: {}", clock_counter);
//        }
        spdlog::debug("Simulation time_: {}", floor(clock_/10));


        // Handle events
        while (!no_event) {
            no_event = true;
            // Radar connection
            if (network_->radar_get_generate_time() == 0) {
                spdlog::debug("Radar connect");
                network_->RadarConnection();
                network_->radar_set_transmission_time(10 * 10 );
                time_to_skip.push_back(10*10);
                network_->radar_set_generate_timer(19999999);
                no_event = false;
            }
            // Radar disconnection
            if (network_->radar_get_transmission_time() == 0) {
                spdlog::debug("Radar disconnect");
                network_->RadarDisconnection();
                network_->radar_set_transmission_time(19999999);
                network_->radar_set_generate_timer(round((uni_gen->Rand()) * 5) * 10);
                time_to_skip.push_back(network_->radar_get_generate_time());
                no_event = false;
            }
            // User U2/U3 arrival
            if (network_->user_get_generate_time() == 0) {
                size_t transmission_time = (q_gen->RndExp(q_lambda) + 1) * 10;
                network_->UserArrival(clock_, transmission_time,
                                      dec_gen->RndZeroOne(n));
                network_->user_set_generate_time((tau_gen->RndExp(tau_lambda) + 1) * 10);
                spdlog::debug("New user!");
                time_to_skip.push_back(transmission_time);
                time_to_skip.push_back(network_->user_get_generate_time());
                no_event = false;
            }

            // Checking decision list
            std::list<std::shared_ptr<User>> decision_list = network_->get_decision_list();
            for (const auto &i: decision_list) {
                if (i.get()->get_user_type() == User::UserType::LICENSED) {
                    if_lost_u2 = network_->U2ConnectToChannel(i);
                }
                else if (i.get()->get_user_type() == User::UserType::NOT_LICENSED){
                    if_lost_u3 = network_->U3ConnectToChannel(i);
                }
            }

            // Looping through channels
            std::vector<std::shared_ptr<Channel>> channel_list = network_->get_channels();
            // Checking for user transmission end (User disconnection)
            for (const auto &i: channel_list){
                if (i.get()->get_connection_time() != 0 && i.get()->get_channel_license() != Channel::ChannelLicense::RADAR){
                    time_to_skip.push_back(i.get()->get_connection_time() - clock_);
                }
                if(i.get()->get_connection_time() == clock_ && i.get()->get_channel_license() != Channel::ChannelLicense::RADAR){
                    i.get()->disconnect_from_channel();
                    spdlog::debug("User disconnected from channel");
                }
            }

            if (network_->get_buffer_polling_time() == 0){
                network_->BufferPolling();
                network_->set_buffer_polling_time(0.2 * 10);
            }
        }

        double current_occupancy = 0;

        // Statistics - channel occupancy
        std::vector<std::shared_ptr<Channel>> channel_list = network_->get_channels();
        // Checking for user transmission end (User disconnection)
        for (const auto &i: channel_list){
            if (clock_ % 10 == 0){
                if (!i.get()->is_free() && (clock_/10 > initial_time)){
                    occupied_channels_counter++;
                    current_occupancy++;
                    if(i.get()->get_user_type() == User::UserType::LICENSED){
                        licensed_users_counter++;
                    }
                    else if(i.get()->get_user_type() == User::UserType::NOT_LICENSED){
                        unlicensed_users_counter++;
                    }
                }
            }
        }

        if (clock_ % 10 == 0){
            channels_file << current_occupancy/20 << "\n";
        }

        if (clock_/10 == initial_time){
            network_->set_after_initial_state();
        }

        // Find maximum time to skip (to the occurrence of any event)
        auto max_time_to_skip = *std::min_element(time_to_skip.begin(), time_to_skip.end());


        if (max_time_to_skip <= 10 || max_time_to_skip > 100 || network_->user_get_generate_time() > 10000000000){
            max_time_to_skip = 1;
        }
        else{
            max_time_to_skip = static_cast<size_t>(floor(max_time_to_skip/10) * 10);
        }

        // Update time
        network_->radar_set_transmission_time((network_->radar_get_transmission_time() - max_time_to_skip));
        network_->radar_set_generate_timer((network_->radar_get_generate_time() - max_time_to_skip));
        network_->user_set_generate_time((network_->user_get_generate_time() - max_time_to_skip));
        network_->set_buffer_polling_time((network_->get_buffer_polling_time() - 1));
        clock_ += max_time_to_skip;
        if(max_time_to_skip > 10){
            spdlog::debug("No events, skipping next {}ms", max_time_to_skip/10);
        }

        // Served users to end simulation
        if(network_->users_connections > served_users){
            spdlog::debug("Served users parameter exceeded. Finishing simulation...");
            break;
        }
    }

    double mean_channel_occupancy = occupied_channels_counter / (20.0 * (time/10-initial_time));
    double mean_serving_licensed_users = licensed_users_counter / (time/10-initial_time);
    double mean_serving_unlicensed_users = unlicensed_users_counter / (time/10-initial_time);

    std::cout<<std::endl;
    spdlog::info("Blockage probability: {}", network_->users_connections_lost/network_->users_connections);
    std::cout<<std::endl;
    spdlog::info("Mean channel occupancy: {}", mean_channel_occupancy);
    spdlog::info("Licensed users blockage probability: {}", network_->licensed_users_connections_lost/
                                                                    network_->licensed_users_connections);
    spdlog::info("Unlicensed users blockage probability: {}", network_->unlicensed_users_connections_lost/
                                                              network_->unlicensed_users_connections);
    spdlog::info("Mean serving licensed users: {}", mean_serving_licensed_users);
    spdlog::info("Mean serving unlicensed users: {}", mean_serving_unlicensed_users);

    channels_file.close();
}