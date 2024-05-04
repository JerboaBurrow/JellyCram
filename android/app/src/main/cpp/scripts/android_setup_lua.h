static const char * android_setup_lua = R"(

jiggleometerScale = 0.030*xmax
for i = 1,10 do

    c = colours[math.random(#colours)]

    o = {

        ["transform"] = {2.0*3.0*jiggleometerScale+2.0*i*jiggleometerScale,0.075,0.0,1.0},
        ["colour"] = c,
        ["moveable"] = false,
        ["ghost"] = true,
        ["collisionMesh"] = {{0.0,0.0,jiggleometerScale}},
        ["name"] = "jiggleometer"..i,
        ["renderPriority"] = 100000

    }
    hop.loadObject(o)
end

for i = 1,64 do

    c = colours[math.random(#colours)]

    o = {

        ["transform"] = {(i-32-1)*s,y0-s/2.0,0.0,s},
        ["colour"] = c,
        ["moveable"] = false,
        ["ghost"] = true,
        ["collisionMesh"] = {{0.0,0.0,0.5}},
        ["name"] = "floor"..i,
        ["renderPriority"] = 100000

    }
    hop.loadObject(o)
end

)";
