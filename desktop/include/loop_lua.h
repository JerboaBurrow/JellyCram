#ifndef LOOP_LUA_H
#define LOOP_LUA_H

const char * loop_lua = R"(
s = 1.0/(3*9)

if (lastPreviewIndex == nil) then
    lastPreviewIndex = math.random(#meshes-1)
end

if (lastPreviewIndex ~= previewIndex) then

    if (preview ~= nil) then
        hop.deleteObject(preview)
    end

    mesh = meshes[previewIndex]

    lastPreviewIndex = previewIndex

    o = {

        ["transform"] = {0.95,0.925,0.0,s*0.3},
        ["colour"] = {0.0,0.0,0.0,1.0},
        ["moveable"] = false,
        ["ghost"] = true,
        ["collisionMesh"] = mesh,
        ["name"] = "preview",
        ["renderPriority"] = 100000

    }

    preview = hop.loadObject(o)
end

if lastTime == nil then
    lastTime = hop.timeMillis()

    mesh = meshes[previewIndex]

    x = 0.5 + math.random()*0.5 -0.25
    y = 1.25

    r = math.random(255)
    g = math.random(255)
    b = math.random(255)

    o = {

        ["transform"] = {x,y,0.0,s},
        ["colour"] = {r/255,g/255,b/255,1.0},
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
end

if (nextPiece) then
    mesh = meshes[previewIndex]

    x = nextX
    
    if x < 0.25 or x > 0.75 then
        x = 0.5 + math.random()*0.5 -0.25
    end
    
    y = 1.25
    
    r = math.random(255)
    g = math.random(255)
    b = math.random(255)

    o = {

        ["transform"] = {x,y,0.0,s},
        ["colour"] = {r/255,g/255,b/255,1.0},
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

#endif /* LOOP_LUA_H */
