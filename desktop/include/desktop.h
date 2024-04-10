#ifndef DESKTOP_H
#define DESKTOP_H

#include <tutorial.h>

#ifdef APPLE
#include <glob.h>
#include <filesystem>
std::string createPathByExpandingTildePath(char * path)
{
    // https://developer.apple.com/library/archive/qa/qa1549/_index.html
    glob_t globbuf;
    char ** v;
    char * expandedPath = NULL, * result = NULL;

    if (path == NULL) { return path; }

    if (glob(path, GLOB_TILDE, NULL, &globbuf) == 0) //success
    {
        v = globbuf.gl_pathv; //list of matched pathnames
        expandedPath = v[0]; //number of matched pathnames, gl_pathc == 1

        result = (char*)calloc(1, strlen(expandedPath) + 1); //the extra char is for the null-termination
        if(result)
            strncpy(result, expandedPath, strlen(expandedPath) + 1); //copy the null-termination as well

        globfree(&globbuf);
    }

    return std::string(result);
}
#endif

int resX = 1000;
int resY = 1000;
bool loadedIcons = false;
bool displayingMenu = false;
std::string selectingKey = "";

const float menuX = 0.95;
const float menuY = 0.8;
const float menuScale = 64.0/271.0 * 0.271;

const float keySelectX = resX*0.35f;
const float keySelectY = resY*0.7f;
const float keySelectHeight = 24.0f;
const float keySelectXGap = keySelectHeight*10;
// for smoothing delta numbers
uint8_t frameId = 0;
double deltas[60];

std::shared_ptr<jGL::jGLInstance> jGLInstance;

const std::vector<std::string> controls = {"Up", "Down", "Left", "Right", "Left rotate", "Right rotate", "Pause/unpause"};

const std::vector<int> restrictedKeys = {GLFW_KEY_F2, GLFW_KEY_ESCAPE};

bool isRestricted(int code)
{
    return std::find(restrictedKeys.cbegin(), restrictedKeys.cend(), code) != restrictedKeys.cend();
}

const std::map<int, std::string> keyCodes 
{
    {GLFW_KEY_UNKNOWN, "Unknown"}, 
    {GLFW_KEY_SPACE, "Space"},
    {GLFW_KEY_APOSTROPHE, "Apostrophe"},
    {GLFW_KEY_COMMA, "Comma"},
    {GLFW_KEY_MINUS, "Minus"},
    {GLFW_KEY_PERIOD, "Period"},
    {GLFW_KEY_SLASH, "Slash"},
    {GLFW_KEY_0, "0"},
    {GLFW_KEY_1, "1"},
    {GLFW_KEY_2, "2"},
    {GLFW_KEY_3, "3"},
    {GLFW_KEY_4, "4"},
    {GLFW_KEY_5, "5"},
    {GLFW_KEY_6, "6"},
    {GLFW_KEY_7, "7"},
    {GLFW_KEY_8, "8"},
    {GLFW_KEY_9, "9"},
    {GLFW_KEY_SEMICOLON, "Semicolon"},
    {GLFW_KEY_EQUAL, "Equal"},
    {GLFW_KEY_A, "A"},
    {GLFW_KEY_B, "B"},
    {GLFW_KEY_C, "C"},
    {GLFW_KEY_D, "D"},
    {GLFW_KEY_E, "E"},
    {GLFW_KEY_F, "F"},
    {GLFW_KEY_G, "G"},
    {GLFW_KEY_H, "H"},
    {GLFW_KEY_I, "I"},
    {GLFW_KEY_J, "J"},
    {GLFW_KEY_K, "K"},
    {GLFW_KEY_L, "L"},
    {GLFW_KEY_M, "M"},
    {GLFW_KEY_N, "N"},
    {GLFW_KEY_O, "O"},
    {GLFW_KEY_P, "P"},
    {GLFW_KEY_Q, "Q"},
    {GLFW_KEY_R, "R"},
    {GLFW_KEY_S, "S"},
    {GLFW_KEY_T, "T"},
    {GLFW_KEY_U, "U"},
    {GLFW_KEY_V, "V"},
    {GLFW_KEY_W, "W"},
    {GLFW_KEY_X, "X"},
    {GLFW_KEY_Y, "Y"},
    {GLFW_KEY_Z, "Z"},
    {GLFW_KEY_LEFT_BRACKET, "LeftBracket"},
    {GLFW_KEY_BACKSLASH, "Backslash"},
    {GLFW_KEY_RIGHT_BRACKET, "RightBracket"},
    {GLFW_KEY_GRAVE_ACCENT, "GraveAccent"},
    {GLFW_KEY_WORLD_1, "World1"},
    {GLFW_KEY_WORLD_2, "World2"},
    {GLFW_KEY_ESCAPE, "Escape"},
    {GLFW_KEY_ENTER, "Enter"},
    {GLFW_KEY_TAB, "Tab"},
    {GLFW_KEY_BACKSPACE, "Backspace"},
    {GLFW_KEY_INSERT, "Insert"},
    {GLFW_KEY_DELETE, "Delete"},
    {GLFW_KEY_RIGHT, "Right"},
    {GLFW_KEY_LEFT, "Left"},
    {GLFW_KEY_DOWN, "Down"},
    {GLFW_KEY_UP, "Up"},
    {GLFW_KEY_PAGE_UP, "PageUp"},
    {GLFW_KEY_PAGE_DOWN, "PageDown"},
    {GLFW_KEY_HOME, "Home"},
    {GLFW_KEY_END, "End"},
    {GLFW_KEY_CAPS_LOCK, "CapsLock"},
    {GLFW_KEY_SCROLL_LOCK, "ScrollLock"},
    {GLFW_KEY_NUM_LOCK, "NumLock"},
    {GLFW_KEY_PRINT_SCREEN, "PrintScreen"},
    {GLFW_KEY_PAUSE, "Pause"},
    {GLFW_KEY_F1, "F1"},
    {GLFW_KEY_F2, "F2"},
    {GLFW_KEY_F3, "F3"},
    {GLFW_KEY_F4, "F4"},
    {GLFW_KEY_F5, "F5"},
    {GLFW_KEY_F6, "F6"},
    {GLFW_KEY_F7, "F7"},
    {GLFW_KEY_F8, "F8"},
    {GLFW_KEY_F9, "F9"},
    {GLFW_KEY_F10, "F10"},
    {GLFW_KEY_F11, "F11"},
    {GLFW_KEY_F12, "F12"},
    {GLFW_KEY_F13, "F13"},
    {GLFW_KEY_F14, "F14"},
    {GLFW_KEY_F15, "F15"},
    {GLFW_KEY_F16, "F16"},
    {GLFW_KEY_F17, "F17"},
    {GLFW_KEY_F18, "F18"},
    {GLFW_KEY_F19, "F19"},
    {GLFW_KEY_F20, "F20"},
    {GLFW_KEY_F21, "F21"},
    {GLFW_KEY_F22, "F22"},
    {GLFW_KEY_F23, "F23"},
    {GLFW_KEY_F24, "F24"},
    {GLFW_KEY_F25, "F25"},
    {GLFW_KEY_KP_0, "Keypad0"},
    {GLFW_KEY_KP_1, "Keypad1"},
    {GLFW_KEY_KP_2, "Keypad2"},
    {GLFW_KEY_KP_3, "Keypad3"},
    {GLFW_KEY_KP_4, "Keypad4"},
    {GLFW_KEY_KP_5, "Keypad5"},
    {GLFW_KEY_KP_6, "Keypad6"},
    {GLFW_KEY_KP_7, "Keypad7"},
    {GLFW_KEY_KP_8, "Keypad8"},
    {GLFW_KEY_KP_9, "Keypad9"},
    {GLFW_KEY_KP_DECIMAL, "KeypadDecimal"},
    {GLFW_KEY_KP_DIVIDE, "KeypadDivide"},
    {GLFW_KEY_KP_MULTIPLY, "KeypadMultiply"},
    {GLFW_KEY_KP_SUBTRACT, "KeypadSubtract"},
    {GLFW_KEY_KP_ADD, "KeypadAdd"},
    {GLFW_KEY_KP_ENTER, "KeypadEnter"},
    {GLFW_KEY_KP_EQUAL, "KeypadEqual"},
    {GLFW_KEY_LEFT_SHIFT, "LeftShift"},
    {GLFW_KEY_LEFT_CONTROL, "LeftControl"},
    {GLFW_KEY_LEFT_ALT, "LeftAlt"},
    {GLFW_KEY_LEFT_SUPER, "LeftSuper"},
    {GLFW_KEY_RIGHT_SHIFT, "RightShift"},
    {GLFW_KEY_RIGHT_CONTROL, "RightControl"},
    {GLFW_KEY_RIGHT_ALT, "RightAlt"},
    {GLFW_KEY_RIGHT_SUPER, "RightSuper"},
    {GLFW_KEY_MENU, "Menu"}
};

struct Settings
{

    Settings()
    : tutorial(false)
    {
        #ifdef APPLE
        std::string home = createPathByExpandingTildePath("~/");
        std::string support = home+"Library/Application Support/app.jerboa.jellycram";
	    if (!std::filesystem::exists(support)) { std::filesystem::create_directory(support); }
        fileName = std::string(support+"/settings.json").c_str();
        #endif
        load();
    }

    void defaults()
    {
        keyBindings["Up"] = GLFW_KEY_W;
        keyBindings["Down"] = GLFW_KEY_S;
        keyBindings["Left"] = GLFW_KEY_A;
        keyBindings["Right"] = GLFW_KEY_D;

        keyBindings["Left rotate"] = GLFW_KEY_LEFT;
        keyBindings["Right rotate"] = GLFW_KEY_RIGHT;

        keyBindings["Pause/unpause"] = GLFW_KEY_SPACE;
    }

    void load()
    {
        std::ifstream in(fileName);
        if (in.is_open())
        {
            json data = json::parse(in);
            if (data.contains("tutorial_done"))
            {
                tutorial.skip();
            }
            for (auto control : controls)
            {
                if (data.contains(control))
                {
                    if (data[control].is_number_integer())
                    {
                        int code = data[control];
                        if (keyCodes.find(code) != keyCodes.cend() && !isRestricted(code))
                        {
                            keyBindings[control] = code;
                        }
                        else
                        {
                            return defaults();
                        }
                    }
                    else if (data[control].is_string())
                    {
                        std::string key = data[control];
                        bool ok = false;
                        for (auto code : keyCodes)
                        {
                            if (code.second == key && !isRestricted(code.first))
                            {
                                keyBindings[control] = code.first;
                                ok = true;
                                break;
                            }
                        }
                        if (!ok)
                        {
                            return defaults();
                        }
                    }
                    else
                    {
                        return defaults();
                    }
                }
            }   
            in.close();
        }
        else
        {
            return defaults();
        }

        if (!ok())
        {
            return defaults();
        }
    }

    void save()
    {
        std::ofstream out(fileName);
        json data(keyBindings);
        data["tutorial_done"] = tutorial.isDone();
        std::string dump = data.dump();
        out << dump;
        out.close();
    }

    bool ok()
    {
        for (auto binding : keyBindings)
        {
            if (binding.second == GLFW_KEY_UNKNOWN)
            {
                return false;
            }
        }

        return true;
    }

    bool set(std::string key, int code)
    {
        if (isRestricted(code))
        {
            return false;
        }
        
        for (auto binding : keyBindings)
        {
            if (binding.second == code)
            {
                keyBindings[binding.first] = GLFW_KEY_UNKNOWN;
            }
        }
        if (keyBindings.find(key) != keyBindings.end())
        {
            keyBindings[key] = code;
        }

        return ok();
    }

    int get(std::string key)
    {
        if (keyBindings.find(key) != keyBindings.end())
        {
            return keyBindings[key];
        }
        else
        {
            return GLFW_KEY_UNKNOWN;
        }
    }

    std::map<std::string, int>::const_iterator begin() const { return keyBindings.cbegin(); }
    std::map<std::string, int>::const_iterator end() const { return keyBindings.cend(); }

    Tutorial tutorial;

private:

    std::map<std::string, int> keyBindings;
    std::string fileName = "settings.json";
};

void icon(jGL::DesktopDisplay & display)
{
    if (RNG().nextFloat() < 0.01)
    {
        std::vector<uint8_t> compressed;
        compressed.assign(&icon2[0], &icon2[0]+sizeof(icon2));
        std::vector<uint8_t> icond = Hop::Util::Z::inflate(compressed, icon2Size);
        display.setIcon(icond.data(), icond.size());
    }
    else
    {
        std::vector<uint8_t> compressed;
        compressed.assign(&icon1[0], &icon1[0]+sizeof(icon1));
        std::vector<uint8_t> icond = Hop::Util::Z::inflate(compressed, icon1Size);
        display.setIcon(icond.data(), icond.size());
    }

}
#endif /* DESKTOP_H */
