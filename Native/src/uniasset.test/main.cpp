//
// Created by qingy on 2024/2/16.
//

#include <iostream>
#include "uniasset/audio/AudioAsset.hpp"
#include "uniasset/audio/AudioPlayer.hpp"
#include "uniasset/audio/IAudioDecoder.hpp"

using namespace uniasset;

int main() {
    auto audioAsset{std::make_shared<AudioAsset>()};
    auto audioPlayer{std::make_shared<AudioPlayer>()};

    while (true) {
        std::string command{};
        std::cin >> command;

        if (command == "exit") {
            break;
        } else if (command == "load") {
            std::cin >> command;

            audioAsset->load(command);
            audioPlayer->open(audioAsset);
        } else if (command == "resume") {
            audioPlayer->resume();
        } else if (command == "pause") {
            audioPlayer->pause();
        } else if (command == "unload") {
            audioPlayer->close();
            audioAsset->unload();
        } else if (command == "seekf") {
            float time{0};
            std::cin >> time;
            audioPlayer->setTime(time);
        } else if (command == "tellf") {
            std::cout << audioPlayer->getTime() << "\n";
        } else if (command == "setvol") {
            float vol{0};
            std::cin >> vol;
            audioPlayer->setVolume(vol);
        } else if (command == "getvol") {
            std::cout << audioPlayer->getVolume() << "\n";
        }

        std::cout << ".\n";
    }

    return 0;
}