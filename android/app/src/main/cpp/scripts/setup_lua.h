static const char * setup_lua = R"(

config =
{
    ["timeStep"]        = 1.0/(4*900.0),   -- physics time step
    ["subSample"]       = 5,               -- how many physics steps (collisions, movement, etc) per frame
    ["cofr"]            = 0.25,            -- coefficient of restitution,
    ["surfaceFriction"] = 0.5              -- friction at surfaces
}

-- can always call again to reconfigure, the magic of embedded Lua
hop.configure(config);

jiggleometerScale = 0.030*xmax
for i = 1,10 do

    r = math.random(255)
    g = math.random(255)
    b = math.random(255)

    o = {

        ["transform"] = {2.0*3.0*jiggleometerScale+2.0*i*jiggleometerScale,0.075,0.0,1.0},
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

meshes = {
    -- L
    --  2
    --  1
    --  0 3
    {
        {-1.75, -3.25, 0.5, 0},
        {-0.75, -3.25, 0.5, 0},
        {0.25, -3.25, 0.5, 0},
        {0.25, -2.25, 0.5, 0},
        {0.25, -1.25, 0.5, 0},
        {-0.75, -1.25, 0.5, 0},
        {-1.75, -1.25, 0.5, 0},
        {-1.75, -2.25, 0.5, 0},
        {-1.75, -0.25, 0.5, 1},
        {-0.75, -0.25, 0.5, 1},
        {0.25, -0.25, 0.5, 1},
        {0.25, 0.75, 0.5, 1},
        {-1.75, 0.75, 0.5, 1},
        {-1.75, 1.75, 0.5, 1},
        {-0.75, 1.75, 0.5, 1},
        {0.25, 1.75, 0.5, 1},
        {0.25, 2.75, 0.5, 2},
        {-0.75, 2.75, 0.5, 2},
        {-1.75, 2.75, 0.5, 2},
        {-1.75, 3.75, 0.5, 2},
        {-1.75, 4.75, 0.5, 2},
        {-0.75, 4.75, 0.5, 2},
        {0.25, 4.75, 0.5, 2},
        {0.25, 3.75, 0.5, 2},
        {1.25, -3.25, 0.5, 3},
        {2.25, -3.25, 0.5, 3},
        {3.25, -3.25, 0.5, 3},
        {3.25, -2.25, 0.5, 3},
        {3.25, -1.25, 0.5, 3},
        {2.25, -1.25, 0.5, 3},
        {1.25, -1.25, 0.5, 3},
        {1.25, -2.25, 0.5, 3}
    },
    -- rod
    --  3
    --  2
    --  1
    --  0
    {
        {-1, -5.5, 0.5, 0},
        {0, -5.5, 0.5, 0},
        {1, -5.5, 0.5, 0},
        {1, -4.5, 0.5, 0},
        {1, -3.5, 0.5, 0},
        {0, -3.5, 0.5, 0},
        {-1, -3.5, 0.5, 0},
        {-1, -4.5, 0.5, 0},
        {-1, -2.5, 0.5, 1},
        {0, -2.5, 0.5, 1},
        {1, -2.5, 0.5, 1},
        {1, -1.5, 0.5, 1},
        {-1, -1.5, 0.5, 1},
        {-1, -0.5, 0.5, 1},
        {0, -0.5, 0.5, 1},
        {1, -0.5, 0.5, 1},
        {1, 0.5, 0.5, 2},
        {0, 0.5, 0.5, 2},
        {-1, 0.5, 0.5, 2},
        {-1, 1.5, 0.5, 2},
        {-1, 2.5, 0.5, 2},
        {0, 2.5, 0.5, 2},
        {1, 2.5, 0.5, 2},
        {1, 1.5, 0.5, 2},
        {-1, 3.5, 0.5, 3},
        {0, 3.5, 0.5, 3},
        {1, 3.5, 0.5, 3},
        {1, 4.5, 0.5, 3},
        {1, 5.5, 0.5, 3},
        {0, 5.5, 0.5, 3},
        {-1, 5.5, 0.5, 3},
        {-1, 4.5, 0.5, 3}
    },
    -- square
    --   1 2
    --   0 3
    {
        {-2.5, -2.5, 0.5, 0},
        {-2.5, -1.5, 0.5, 0},
        {-2.5, -0.5, 0.5, 0},
        {-1.5, -0.5, 0.5, 0},
        {-0.5, -0.5, 0.5, 0},
        {-0.5, -1.5, 0.5, 0},
        {-0.5, -2.5, 0.5, 0},
        {-1.5, -2.5, 0.5, 0},
        {-2.5, 0.5, 0.5, 1},
        {-1.5, 0.5, 0.5, 1},
        {-0.5, 0.5, 0.5, 1},
        {-0.5, 1.5, 0.5, 1},
        {-0.5, 2.5, 0.5, 1},
        {-1.5, 2.5, 0.5, 1},
        {-2.5, 2.5, 0.5, 1},
        {-2.5, 1.5, 0.5, 1},
        {0.5, 1.5, 0.5, 2},
        {0.5, 2.5, 0.5, 2},
        {1.5, 2.5, 0.5, 2},
        {2.5, 2.5, 0.5, 2},
        {2.5, 1.5, 0.5, 2},
        {2.5, 0.5, 0.5, 2},
        {1.5, 0.5, 0.5, 2},
        {0.5, 0.5, 0.5, 2},
        {0.5, -0.5, 0.5, 3},
        {1.5, -0.5, 0.5, 3},
        {2.5, -0.5, 0.5, 3},
        {2.5, -1.5, 0.5, 3},
        {2.5, -2.5, 0.5, 3},
        {1.5, -2.5, 0.5, 3},
        {0.5, -2.5, 0.5, 3},
        {0.5, -1.5, 0.5, 3}
    },
    -- T
    --     2 1 3
    --       0
    {
        {-1, -3.25, 0.5, 0},
        {-1, -2.25, 0.5, 0},
        {-1, -1.25, 0.5, 0},
        {0, -1.25, 0.5, 0},
        {1, -1.25, 0.5, 0},
        {1, -2.25, 0.5, 0},
        {1, -3.25, 0.5, 0},
        {0, -3.25, 0.5, 0},
        {-1, -0.25, 0.5, 1},
        {0, -0.25, 0.5, 1},
        {1, -0.25, 0.5, 1},
        {1, 0.75, 0.5, 1},
        {1, 1.75, 0.5, 1},
        {0, 1.75, 0.5, 1},
        {-1, 1.75, 0.5, 1},
        {-1, 0.75, 0.5, 1},
        {-2, 1.75, 0.5, 2},
        {-3, 1.75, 0.5, 2},
        {-4, 1.75, 0.5, 2},
        {-4, 0.75, 0.5, 2},
        {-4, -0.25, 0.5, 2},
        {-3, -0.25, 0.5, 2},
        {-2, -0.25, 0.5, 2},
        {-2, 0.75, 0.5, 2},
        {2, 0.75, 0.5, 3},
        {2, 1.75, 0.5, 3},
        {3, 1.75, 0.5, 3},
        {4, 1.75, 0.5, 3},
        {4, 0.75, 0.5, 3},
        {4, -0.25, 0.5, 3},
        {3, -0.25, 0.5, 3},
        {2, -0.25, 0.5, 3}
    },
    -- zig    2 3
    --      0 1
    {
        {-3, -2.5, 0.5, 0},
        {-4, -2.5, 0.5, 0},
        {-4, -1.5, 0.5, 0},
        {-4, -0.5, 0.5, 0},
        {-3, -0.5, 0.5, 0},
        {-2, -0.5, 0.5, 0},
        {-2, -1.5, 0.5, 0},
        {-2, -2.5, 0.5, 0},
        {-1, -0.5, 0.5, 1},
        {0, -0.5, 0.5, 1},
        {1, -0.5, 0.5, 1},
        {1, -1.5, 0.5, 1},
        {1, -2.5, 0.5, 1},
        {0, -2.5, 0.5, 1},
        {-1, -2.5, 0.5, 1},
        {-1, -1.5, 0.5, 1},
        {-1, 0.5, 0.5, 2},
        {-1, 1.5, 0.5, 2},
        {-1, 2.5, 0.5, 2},
        {0, 2.5, 0.5, 2},
        {1, 2.5, 0.5, 2},
        {1, 1.5, 0.5, 2},
        {1, 0.5, 0.5, 2},
        {0, 0.5, 0.5, 2},
        {2, 2.5, 0.5, 3},
        {3, 2.5, 0.5, 3},
        {4, 2.5, 0.5, 3},
        {4, 1.5, 0.5, 3},
        {4, 0.5, 0.5, 3},
        {3, 0.5, 0.5, 3},
        {2, 0.5, 0.5, 3},
        {2, 1.5, 0.5, 3}
    }
}

previewIndex = math.random(#meshes)

)";
