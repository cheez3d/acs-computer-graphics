#pragma once

#include <include/glm.h>

class Gui {
protected:
    static inline glm::mat4 const GUI_PROJECTION { glm::ortho<float>(-1, 1, 1, -1) };

private:
    bool isGui;

protected:
    Gui() {
        SetGui(false);
    }

    Gui(Gui const& other) {
        SetGui(other.isGui);
    }

    Gui& operator=(Gui const& other) = delete;

    Gui(Gui&& other) = default;
    Gui& operator=(Gui&& other) = default;

public:
    bool IsGui() const {
        return isGui;
    }

    void SetGui(bool gui) {
        this->isGui = gui;
    }
};
