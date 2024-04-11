static const char * loop_lua = R"(

if s == nil then
    s = xmax/(3*9)
end

if (lastPreviewIndex ~= previewIndex) then

    if (preview ~= nil) then
        hop.deleteObject(preview)
    end

    mesh = meshes[previewIndex]

    lastPreviewIndex = previewIndex

    o = {

        ["transform"] = {xmax*0.9,0.925,0.0,s*0.3},
        ["colour"] = {0.0,0.0,0.0,1.0},
        ["moveable"] = false,
        ["ghost"] = true,
        ["collisionMesh"] = mesh,
        ["name"] = "preview",
        ["renderPriority"] = 100000

    }

    preview = hop.loadObject(o)
end

if (nextPiece) then

    mesh = meshes[previewIndex]

    x = xmax/2.0
    y = 1.05

    c = colours[math.random(#colours)]

    o = {

        ["transform"] = {x,y,0.0,s},
        ["colour"] = c,
        ["moveable"] = true,
        ["collisionMesh"] = mesh,
        ["meshParameters"] = {35000.0, 16.0, 1.0},
        ["translationalDrag"] = 0.0,
        ["rotationalDrag"] = 100.0,
        ["mass"] = 1.0,
        ["inertia"] = 0.01,
        ["name"] = "current"

    }

    hop.loadObject(o)
    lastTime = hop.timeMillis()
    nextPiece = false
end
)";