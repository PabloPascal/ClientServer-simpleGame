#ifndef SERVER_HPP
#define SERVER_HPP

#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <unordered_map>


#include "../protocol.hpp"

using PORT = unsigned int;
using ID = unsigned int;


class Server
{
public:
    Server(unsigned int port) : m_port(port)
    {   
        std::cout << "server loading...\n";
    }

    void run();

private:
    void handlePacket();
    void proccessInput(PlayerState& action);
    void handlePlayerState(sf::Packet& packet, sf::IpAddress senderIp, unsigned short sender_port);
    void handleProjectileState(sf::Packet& packet, sf::IpAddress senderIp, unsigned short sender_port);
    void handleDisconnect(sf::Packet& packet, sf::IpAddress senderIp, unsigned short sender_port);
    void sendToAll(sf::Packet& packet);
    ProjectileState createProjectile(PlayerState& player_state);
    void projectileUpdate(float deltaTime);
    void projectileCollision(ProjectileState& bullet);
private:
    
    float deltaTime = 0.001;
    unsigned int m_port;
    sf::UdpSocket socket;
    bool server_run;

    std::unordered_map<ID, std::pair<sf::IpAddress, PORT>>  mClients;
    std::unordered_map<ID, PlayerState>                     mPlayers;
    std::vector<ProjectileState>                            mProjectiles;

    sf::Clock couldown;

    ID projectile_id = 0;

};


#endif

