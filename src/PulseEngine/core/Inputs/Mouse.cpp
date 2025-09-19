// #include "Mouse.h"

// void MouseInput::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
// {
//     PulseEngineBackend::GetActiveCamera()->ProcessMouseScroll(yoffset);
    
//     #ifdef ENGINE_EDITOR
//         ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
//     #endif
// }

// void MouseInput::MouseCallback(GLFWwindow *window, double xpos, double ypos)
// {
//     static bool isMousePressed = false;
//     // static float startX = 0.0f, startY = 0.0f;

//     #ifdef ENGINE_EDITOR
//         //imGUI -- editor script    
//         ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
//     #endif


//     // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
//     // {

//     //     float xoffset = xpos - startX;
//     //     float yoffset = startY - ypos;


//     //     PulseEngineBackend::GetActiveCamera()->ProcessMouseMovement(xoffset, yoffset);
//     // }
//     // else
//     // {
//     //     isMousePressed = false;
//     // }
//     // startX = xpos;
//     // startY = ypos;
// }