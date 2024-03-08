config = 
{
    ["timeStep"]        = 1.0/(4*900.0),   -- physics time step
    ["subSample"]       = 5,               -- how many physics steps (collisions, movement, etc) per frame
    ["cofr"]            = 0.25,            -- coefficient of restitution,
    ["surfaceFriction"] = 0.5              -- friction at surfaces
}

-- can always call again to reconfigure, the magic of embedded Lua
hop.configure(config);

-- some code for mesh generation
require("meshes")

jiggleometerScale = 0.015
for i = 1,10 do

    r = math.random(255)
    g = math.random(255)
    b = math.random(255)

    o = {

        ["transform"] = {0.025+2.0*i*jiggleometerScale,0.975,0.0,1.0},
        ["colour"] = {r/255,g/255,b/255,1.0},
        ["moveable"] = false,
        ["ghost"] = true,
        ["collisionMesh"] = {{0.0,0.0,jiggleometerScale}},
        ["name"] = "jiggleometer"..i,
        ["renderPriority"] = 100000

    }
    hop.loadObject(o)
end

math.randomseed(os.time())