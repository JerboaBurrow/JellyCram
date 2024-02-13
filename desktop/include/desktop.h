#ifndef DESKTOP_H
#define DESKTOP_H

const int resX = 1000;
const int resY = 1000;

// for smoothing delta numbers
uint8_t frameId = 0;
double deltas[60];

std::shared_ptr<jGL::jGLInstance> jGLInstance;

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
