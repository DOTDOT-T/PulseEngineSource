#include "SceneManager.h"

SceneManager *SceneManager::GetInstance()
{
    static SceneManager* sm;
    if(!sm)
    {
        sm = new SceneManager;
    } 
    return sm;
}