#include "Application.h"

int main()
{
    auto app = Application(glm::ivec2(800, 600), std::string("TestTaskSamsung"));
    return app.run();
}
