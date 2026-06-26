#ifndef YUUKI_UCI_H
#define YUUKI_UCI_H

#include "types.h"
#include <vector>
#include <string>

namespace Yuuki {

void sendUCI(const std::string& msg);
void sendInfoString(const std::string& str);
void sendBestMove(const Move& move, const Move& ponder);
void sendInfo(const std::string& infoStr);
void sendID();
void sendUCIOptions();

void handleSetOption(const std::vector<std::string>& tokens);
void handlePosition(const std::string& cmd);
void handleGo(const std::string& cmd);
void handlePonderHit();

void printBoard();

void initializeEngine();

}

#endif
