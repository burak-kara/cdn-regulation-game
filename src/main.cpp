#include <iostream>

#include "lib/manager.h"

int main(const int argc, char *argv[]) {
    std::cout << "Started..." << std::endl;

    Manager *manager = Manager::get_instance();

    manager->init(argc, argv);
    manager->play_optimal_sp();
    manager->play_optimal_cdn2("cdn1", "cdn2");
    manager->play_br("cdn1", "cdn2");
    manager->play_br("cdn2", "cdn1");
    manager->finish();

    return 0;
}
