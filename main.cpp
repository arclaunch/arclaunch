#include <quiche.h>
#include <iostream>

int main(void)
{
    std::wcout << "Quiche version:" << quiche_version() << std::endl;
}