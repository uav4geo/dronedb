/* Copyright 2019 MasseranoLabs LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include <iostream>

#include "libs/cxxopts.hpp"

#include "logger.h"
#include "database.h"
#include "exceptions.h"

#include "cmd/build.hpp"

#define VERSION "0.9.0"

using namespace std;

cxxopts::ParseResult parse(int argc, char* argv[]) {
    try {
        bool noArgs = argc <= 1;

        cxxopts::Options options(argv[0], "DDB v" VERSION " - Aerial data management utility");
        options
        .positional_help("[args]")
        .custom_help("<command>")
        .show_positional_help();

        options
        .allow_unrecognised_options()
        .add_options()

        ("command", "[build]", cxxopts::value<std::string>())
        ("i,input", "Input directory", cxxopts::value<std::string>())
        ("o,output", "Output file", cxxopts::value<std::string>())
        ("h,help", "Print help")
        ("v,verbose", "Show verbose output")
        ("version", "Show version");

        options.parse_positional({"command", "input", "output"});

        auto result = options.parse(argc, argv);

        if (result.count("help") || noArgs || (result.count("command") && result["command"].as<std::string>() == "help")) {
            LOGI << options.help({""});
            exit(0);
        }

        return result;
    } catch (const cxxopts::OptionException& e) {
        LOGE << "Error parsing options: " << e.what() << std::endl;
        exit(1);
    }
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i=0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    init_logger();

    auto result = parse(argc, argv);
    auto arguments = result.arguments();

    if (result.count("verbose")) set_logger_verbose();

    if (result.count("version")) {
        LOGI << VERSION;
        exit(0);
    }

    try {
        // Initialization steps
        Database::Initialize();

        LOGV << "DDB v" VERSION;
        LOGV << "SQLite version: " << sqlite3_libversion();
        LOGV << "SpatiaLite version: " << spatialite_version();

        auto cmd = result["command"].as<std::string>();
        if (cmd == "build") {
            if (result.count("input")) {
                cmd::Build(result["input"].as<std::string>());
            } else {
                throw InvalidArgsException("No input path specified");
            }
        } else {
            throw InvalidArgsException("Invalid command \"" + cmd + "\"");
        }

        if (result.count("command")) {
            std::cout << "Command = " << result["command"].as<std::string>()
                      << std::endl;
        }

        if (result.count("input")) {
            std::cout << "Input = " << result["input"].as<std::string>()
                      << std::endl;
        }

        if (result.count("output")) {
            std::cout << "Output = " << result["output"].as<std::string>()
                      << std::endl;
        }
    } catch (const InvalidArgsException &exception) {
        LOGE << exception.what() << ". Run ./dbb --help for usage information.";
    } catch (const AppException &exception) {
        LOGF << exception.what();
        return 1;
    }

    return 0;
}