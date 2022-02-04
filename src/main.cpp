#include "Application.h"

int main()
{
    const auto app = std::make_shared<Application>(glm::ivec2(800, 600), std::string("TestTaskSamsung"));
    return 0;
}
