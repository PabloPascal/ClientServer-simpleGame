#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <map>


struct Player{

    int health;
    float radius;
    sf::Vector2f pos;
    sf::Color color;
    std::chrono::steady_clock::time_point lastUpdate;

    unsigned int id;

};




int main(){


    unsigned int port = 5341;
    
    sf::UdpSocket socket;

    if(socket.bind(port) != sf::Socket::Done){
        std::cerr << "cannot bind this port: " << port << std::endl;
        return -1;
    }
    std::cout << "server waiting clients\n";

    std::map<unsigned int, std::pair<sf::IpAddress, unsigned int>> clients; //храним адреса клиентов 
    std::map<unsigned int, Player> players;


    socket.setBlocking(false);
    while(true){

        sf::Packet packet;
        sf::IpAddress sender_ip_address;
        unsigned short sender_port;
        sf::Socket::Status status = socket.receive(packet, sender_ip_address, sender_port);

        if(status == sf::Socket::Done){

            Player player_data;

            packet >> player_data.health >> player_data.radius >> player_data.pos.x >> player_data.pos.y 
            >> player_data.color.r >> player_data.color.g >> player_data.color.b >> player_data.id;

            player_data.lastUpdate = std::chrono::steady_clock::now();

            players[player_data.id] = player_data;
            clients[player_data.id] = std::make_pair(sender_ip_address, sender_port);

            for(const auto [id, addr] : clients){

                sf::Packet response;
                response << player_data.health << player_data.radius << player_data.pos.x << player_data.pos.y 
            << player_data.color.r << player_data.color.g << player_data.color.b << player_data.id;


                if(socket.send(response, addr.first, addr.second) != sf::Socket::Done){
                    std::cerr << "didnt send packet to address: ip_address: " << addr.first
                              << ", port: "<< addr.second << std::endl; 
                }
            }


        }else if(status == sf::Socket::Error){
            std::cerr <<  "receive socket error\n";
        }
        auto now = std::chrono::steady_clock::now();
        for(auto it = players.begin(); it != players.end();){

            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.lastUpdate).count();

            if(duration > 5){
                clients.erase(it->first);
                it = players.erase(it);
            }
            else
                ++it;
        }



    }


}