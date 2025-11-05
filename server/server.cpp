#include "server.hpp"



void Server::run()
{
    if(socket.bind(m_port) != sf::Socket::Status::Done)
    {
        std::cerr << "cannot bind to port: " << m_port << std::endl;
        throw "bind error\n";
    }

    server_run = true;
    
    std::cout << "server is running\n";
    sf::Clock timer;
    sf::Time time;

    const float FIXED_TIMESTEP = 1/60.f; 
    float accumulate_time = 0.f;

    socket.setBlocking(false);
    while(server_run)
    {

        accumulate_time += timer.restart().asSeconds();


        while(accumulate_time >= FIXED_TIMESTEP)
        {
            deltaTime = FIXED_TIMESTEP;
            accumulate_time -= FIXED_TIMESTEP;
        }

        sf::Packet packet;
        sf::IpAddress sender_ip_address;
        unsigned short sender_port;

        sf::Socket::Status status = socket.receive(packet, sender_ip_address, sender_port);

        if(status == sf::Socket::Done)
        {

            uint8_t type;
            packet >> type;

            MessageType msg_type = static_cast<MessageType>(type);

            //std::cout << "type = " << type << std::endl;

            switch (msg_type)
            {
            case MessageType::player_state:
            {
                handlePlayerState(packet, sender_ip_address, sender_port);
                break;
            }
            case MessageType::projectile_state :
            {
                handleProjectileState(packet, sender_ip_address, sender_port);
                break;
            }
            default:
                break;
            }


        }

        projectileUpdate(deltaTime);


    }



}


void Server::handlePlayerState(sf::Packet& packet, sf::IpAddress senderIp, unsigned short sender_port)
{
    PlayerState state;
    packet >> state;

    auto it = mClients.find(state.id);
    
    if(it == mClients.end())
    {
        std::cout << "player: " << state.name << " connected!\n"; 
        std::cout << "id = " << state.id << std::endl;
        std::cout << "IP: " << senderIp << std::endl;
        std::cout << "port: " << sender_port << std::endl;

        std::string welcome = "welcome to the server!\n";
        sf::Packet packet;
        
        uint8_t type = (uint8_t)MessageType::text;
        packet << type;
        packet << welcome;

        socket.send(packet, senderIp, sender_port);

        mClients.insert(std::make_pair(state.id, std::pair(senderIp, sender_port)));
    }
    else{
        proccessInput(state);
        mPlayers[state.id] = state;
        
        sf::Packet new_packet;
        new_packet << state;
        sendToAll(new_packet);
    }
    
    

}



void Server::handleProjectileState(sf::Packet& packet, sf::IpAddress senderIp, unsigned short sender_port)
{
    
}



void Server::sendToAll(sf::Packet& packet)
{

    for(const auto [id, addr] : mClients)
    {
        if(socket.send(packet, addr.first, addr.second) != sf::Socket::Done)
        {
            std::cerr << "error sent data: ip = " << addr.first << std::endl;
        }

    }

}


void Server::proccessInput(PlayerState& state)
{
    PlayerAction act = state.action;

    if((act & PlayerAction::moveUp) == PlayerAction::moveUp)
    {
        state.positionY -= state.speed * deltaTime;
    }
    if((act & PlayerAction::moveDown) == PlayerAction::moveDown)
    {
        state.positionY += state.speed * deltaTime;
    }
    if((act & PlayerAction::moveRight) == PlayerAction::moveRight)
    {
        state.positionX += state.speed * deltaTime;

    }
    if((act & PlayerAction::moveLeft) == PlayerAction::moveLeft)
    {
        state.positionX -= state.speed * deltaTime;

    }
    if((act & PlayerAction::shoot) == PlayerAction::shoot)
    {
        ProjectileState bullet;
        if(couldown.getElapsedTime().asMilliseconds() > 500){
            bullet = createProjectile(state);
            mProjectiles.emplace_back(bullet);
            couldown.restart();
        }
        sf::Packet new_packet;
        new_packet << bullet;
        sendToAll(new_packet);
    }

}


ProjectileState Server::createProjectile(PlayerState& player_state)
{
    ProjectileState projectile;
    if(player_state.ammoCount > 0)
    {
        player_state.ammoCount--;

        projectile.id = projectile_id++;
        projectile.owner_id = player_state.id;
        projectile.live = true;
        projectile.positionX = player_state.positionX;
        projectile.positionY = player_state.positionY;
        projectile.velocityX *= player_state.dirX;
        projectile.velocityY *= player_state.dirY;

        projectile.lastTimeUpdate = player_state.lastTimeUpdate;

    }

    return projectile;
}


void Server::projectileUpdate(float deltaTime)
{

    for(auto it = mProjectiles.begin(); it != mProjectiles.end(); ++it)
    {

        if(it->live)
        {

            float dt = it->lastTimeUpdate;
            it->positionX += it->velocityX;
            it->positionY += it->velocityY;

            projectileCollision(*it);

            sf::Packet new_packet;
            new_packet << *it;

            sendToAll(new_packet);


            if(it->positionX > 2000 || it->positionX < -100 || it->positionY > 1000 || it->positionY < -100)
            {
                it->live = false;
            }
        }
    }



}



void Server::projectileCollision(ProjectileState& bullet)
{

    for(auto& [id, player]: mPlayers)
    {
        float diffX = player.positionX - bullet.positionX;
        float diffY = player.positionY - bullet.positionY;
        
        if(diffX*diffX + diffY*diffY < 40*40 && bullet.owner_id != id) 
        {
            player.health -= 5;
            bullet.live = false;
            sf::Packet packet;
            packet << player;
            sendToAll(packet);
        }
        

    }


}