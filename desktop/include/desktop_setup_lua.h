#ifndef DESKTOP_SETUP_LUA_H
#define DESKTOP_SETUP_LUA_H

const char * desktop_setup_lua = R"(

jiggleometerScale = 0.015
for i = 1,10 do

    c = colours[math.random(#colours)]

    o = {

        ["transform"] = {0.025+2.0*i*jiggleometerScale,0.95,0.0,1.0},
        ["colour"] = c,
        ["moveable"] = false,
        ["ghost"] = true,
        ["collisionMesh"] = {{0.0,0.0,jiggleometerScale}},
        ["name"] = "jiggleometer"..i,
        ["renderPriority"] = 100000

    }
    hop.loadObject(o)
end

)";

#endif /* DESKTOP_SETUP_LUA_H */
