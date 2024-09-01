#include <discordpp/bot.hh>
#include <discordpp/event.hh>
#include <discordpp/message.hh>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

struct Config {
    std::string token;
    std::string prefix;
    std::string owner_id;
};


Config loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    json config_json;
    file >> config_json;
    Config config;
    config.token = config_json["token"];
    config.prefix = config_json["prefix"];
    config.owner_id = config_json["owner_id"];
    return config;
}


class Bot : public discordpp::Bot {
public:
    Bot(const Config& config) : discordpp::Bot(config.token) {
        onMessageCreate([this](const discordpp::Message& message) {
            handleCommand(message);
        });
    }

    void handleCommand(const discordpp::Message& message) {
        if (message.content().find(config_.prefix) != 0) return;

        std::string command = message.content().substr(config_.prefix.length());
        std::string args = "";
        size_t space_pos = command.find(" ");
        if (space_pos != std::string::npos) {
            args = command.substr(space_pos + 1);
            command = command.substr(0, space_pos);
        }

        if (command == "ping") {
            message.channel().send("Pong!");
        } else if (command == "help") {
            message.channel().send("Available commands: !ping, !help, !userinfo");
        } else if (command == "userinfo") {
            if (args.empty()) {
                message.channel().send("Please provide a user ID or mention a user.");
                return;
            }
            discordpp::User user;
            if (args.find("@") == 0) {
                user = message.mentions()[0];
            } else {
                user = discordpp::User(args, *this);
            }
            message.channel().send("User info: " + user.username() + "#" + user.discriminator());
        }
    }

private:
    Config config_;
};

int main() {

    Config config = loadConfig("config.json");

    MyBot bot(config);

    bot.run();

    return 0;
}
