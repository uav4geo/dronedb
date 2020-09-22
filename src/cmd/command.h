/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#ifndef COMMAND_H
#define COMMAND_H

#include "../vendor/cxxopts.hpp"

namespace cmd {

class Command{
private:
    std::string help;
protected:
    virtual void run(cxxopts::ParseResult &opts) = 0;
    virtual void setOptions(cxxopts::Options &opts) = 0;
    void printHelp();
public:
    Command();
    void run(int argc, char* argv[]);
    virtual std::string description(){ return ""; }
    virtual std::string extendedDescription(){ return ""; }
};

}

#endif // COMMAND_H
