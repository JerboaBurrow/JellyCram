#include <common.h>

glm::vec4 textColour(bool darkMode)
{
    if (darkMode)
    {
        return glm::vec4(1.,1.,1.,1.0);
    }
    else
    {
        return glm::vec4(0.,0.,0.,1.);
    }
}

glm::vec4 backgroundColour(bool darkMode)
{
    if (darkMode)
    {
        return glm::vec4(72./255.,72./255.,72./255.,1.0);
    }
    else
    {
        return glm::vec4(1.,1.,1.,1.);
    }
}

glm::vec3 randomColour()
{
    static std::uniform_int_distribution<size_t> RNGColour(0, colours.size());
    return colours[RNG().sample<std::uniform_int_distribution<size_t>, size_t>(RNGColour)];
}

std::string fixedLengthNumber(double x, unsigned length)
{
    std::string d = std::to_string(x);
    std::string dtrunc(length,' ');
    for (unsigned c = 0; c < dtrunc.length(); c++/*ayy lmao*/)
    {

        if (c >= d.length())
        {
            dtrunc[c] = '0';
        }
        else
        {
            dtrunc[c] = d[c];
        }
    }
    return dtrunc;
}

bool objectOverTop(const cCollideable & o, double topy)
{
    const auto & bb = o.mesh.getBoundingBox();
    return bb.ul.y > topy || bb.ur.y > topy || bb.ll.y > topy || bb.lr.y > topy;
}

void fadeAll(std::vector<Id> & objects, EntityComponentSystem & manager, double a, bool fadePreview)
{
    for (auto & o : objects)
    {
        auto & r = manager.getComponent<cRenderable>(o);
        r.a = a;
    }

    if (fadePreview)
    {
        Id preview = manager.idFromHandle("preview");
        cRenderable & c = manager.getComponent<cRenderable>(preview);
        c.a = a;
    }
}

double pickX(std::vector<Id> & objects, uint8_t bins, double r, double xmax, EntityComponentSystem & manager)
{
    std::vector<uint16_t> counts = std::vector<uint16_t>(uint16_t(xmax/r), 1);

    for (auto o : objects)
    {
        const auto & c = manager.getComponent<cCollideable>(o);
        double x = c.mesh.getBoundingBox().centre.x;
        x = std::max(0.0, x);
        x = std::min(x, xmax);
        size_t hash = x/r;
        counts[hash]+=3;
    }

    unsigned sum = 0;
    for (auto i : counts)
    {
        sum += i;
    }

    std::vector<double> weights(counts.size(),0.0);
    
    std::transform(counts.begin(), counts.end(), weights.begin(), [sum](uint16_t c) {return double(c)/double(sum);});

    std::discrete_distribution<int> distribution(weights.begin(), weights.end()); 

    int bin = RNG().sample<std::discrete_distribution<int>, int>(distribution);

    return bin*r;
}

std::vector<std::pair<Id, uint64_t>> checkDelete(std::vector<Id> & objects, EntityComponentSystem & manager, double r, uint8_t binSize, double y0)
{

    std::multimap<uint64_t, std::pair<Id, uint64_t>> bins;  

    for (const auto & o : objects)
    {
        auto & c = manager.getComponent<cCollideable>(o);

        for (uint64_t t : c.mesh.getTags())
        {
            uint64_t bin = uint64_t((c.mesh.getBoundingBox(t).centre.y-y0)/r);
            bins.insert(std::pair(bin, std::pair(o, t)));
        }
    }

    uint64_t maxKey = (1.0+y0)/r;

    std::vector<std::pair<Id, uint64_t>> toDelete;

    for (uint64_t k = 0; k < maxKey; k++)
    {
        auto range = bins.equal_range(k);
        
        if (std::distance(range.first, range.second) >= binSize)
        {
            for (auto iter = range.first; iter != range.second; iter++)
            {
                toDelete.push_back(iter->second);
            }
            // only delete once
            break;
        }
    }

    return toDelete;

}

void deleteToPulse
(
    std::vector<std::pair<Id, uint64_t>> & toDelete, 
    std::vector<Id> & objects, 
    std::vector<Id> & pulsing,
    EntityComponentSystem & manager,
    double outOfPlayFade
)
{
    std::multimap<Id, uint64_t> tagsToDelete;
    std::set<Id> uniqueObjects;

    // collect tags to delete for unique objects
    for (auto p : toDelete)
    {
        tagsToDelete.insert(std::pair(p.first, p.second));
        uniqueObjects.insert(p.first);
    }

    for (auto p : uniqueObjects)
    {
        cCollideable & c = manager.getComponent<cCollideable>(p);
        auto & renp = manager.getComponent<cRenderable>(p);
        auto trans = manager.getComponent<cTransform>(p);
        auto phys = manager.getComponent<cPhysics>(p);
        renp.a = outOfPlayFade;

        auto range = tagsToDelete.equal_range(p);

        // delete all tags at once
        for (auto t = range.first; t != range.second; t++)
        {
            Hop::System::Physics::CollisionMesh nm = c.mesh.getSubMesh(t->second);
            auto bb = c.mesh.getBoundingBox(t->second);
            Id nid = manager.createObject();
            manager.addComponent<cTransform>(nid, cTransform(bb.centre.x,bb.centre.y,trans.theta,trans.scale));
            manager.addComponent<cRenderable>(nid, renp);
            manager.addComponent<cPhysics>(nid, phys);
            manager.addComponent<cCollideable>(nid, cCollideable(nm));
            pulsing.push_back(nid);
            manager.getComponent<cCollideable>(nid).mesh.reinitialise();

            c.mesh.removeByTag(t->second);
        }
    }
} 

void finaliseDelete
(
    std::vector<std::pair<Id, uint64_t>> & toDelete, 
    std::vector<Id> & objects, 
    std::vector<Id> & pulsing,
    EntityComponentSystem & manager,
    double outOfPlayFade
)
{
    std::multimap<Id, uint64_t> tagsToDelete;
    std::set<Id> uniqueObjects;

    // collect tags to delete for unique objects
    for (auto p : toDelete)
    {
        tagsToDelete.insert(std::pair(p.first, p.second));
        uniqueObjects.insert(p.first);
    }

    for (auto p : pulsing)
    {
        manager.remove(p);
    }

    for (auto p : uniqueObjects)
    {
        cCollideable & c = manager.getComponent<cCollideable>(p);
        auto & renp = manager.getComponent<cRenderable>(p);
        renp.a = outOfPlayFade;

        if (c.mesh.size() == 0)
        {
            manager.remove(p);
            auto it = std::find(objects.begin(), objects.end(), p);
            if (it != objects.end())
            {
                objects.erase(it);
            }
        }
        else // not fully deleted...
        {
            /*
                Check for contiguity

                    Assume bounding boxes of equal side lengths w
                    
                    x x
                    x x

                    The centre of a box must be within w units to be
                    contiguous (for rigid bodies)

                    Contiguity parameter is,

                    c := dij / w

                    For rigid bodies c <= 1 is contiguous

                    For soft we have c ~ 1

            */ 
            if (c.mesh.getTags().size() > 1)
            {
                for (auto ti : c.mesh.getTags())
                {
                    auto bbi = c.mesh.getBoundingBox(ti);  
                    double w = Hop::Maths::norm(bbi.ll-bbi.lr);
                    if (c.mesh.getTags().size() > 1)
                    {
                        bool contiguous = false;
                        for (auto tj : c.mesh.getTags())
                        {
                            if (ti != tj)
                            {
                                auto bbj = c.mesh.getBoundingBox(tj);
                                double d = Hop::Maths::norm(bbi.centre-bbj.centre);
                                // less than 5% discontiguous
                                if (d / w < 1.05)
                                {
                                    contiguous = true;
                                    break;
                                }
                            }
                        }
                        if (!contiguous)
                        {
                            // remove this piece, ti, into a new object
                            double x = bbi.centre.x;
                            double y = bbi.centre.y;
                            auto trans = manager.getComponent<cTransform>(p);
                            auto ren = manager.getComponent<cRenderable>(p);
                            auto phys = manager.getComponent<cPhysics>(p);

                            phys.lastX = x;
                            phys.lastY = y;

                            Hop::System::Physics::CollisionMesh nm = c.mesh.getSubMesh(ti);
                            c.mesh.removeByTag(ti);

                            auto bb = c.mesh.getBoundingBox().centre;
                            auto & transO = manager.getComponent<cTransform>(p);
                            auto & physO = manager.getComponent<cPhysics>(p);
                            transO.x = bb.x;
                            transO.y = bb.y;
                            physO.lastX = bb.x;
                            physO.lastY = bb.y;
                            c.mesh.reinitialise();

                            glm::vec3 rngc = randomColour();
                            ren.r = rngc.r;
                            ren.g = rngc.g;
                            ren.b = rngc.b;

                            Id nid = manager.createObject();
                            manager.addComponent<cTransform>(nid, cTransform(x,y,trans.theta,trans.scale));
                            manager.addComponent<cRenderable>(nid, ren);
                            manager.addComponent<cPhysics>(nid, phys);
                            manager.addComponent<cCollideable>(nid, cCollideable(nm));
                            objects.push_back(nid);

                            manager.getComponent<cCollideable>(nid).mesh.reinitialise();
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

    toDelete.clear();
    pulsing.clear();
    return;
}

double energy(std::vector<Id> & objects, EntityComponentSystem & manager)
{
    double e = 0.0;
    uint16_t o = 0;
    for (const auto & id : objects)
    {
        cPhysics & p = manager.getComponent<cPhysics>(id);
        e += p.vx*p.vx + p.vy*p.vy;
        o += 1;
    }

    return e;
}

void smash(Id with, std::vector<Id> & objects, EntityComponentSystem & ecs)
{
    auto c = ecs.getComponent<cCollideable>(with);
    auto trans = ecs.getComponent<cTransform>(with);
    auto ren = ecs.getComponent<cRenderable>(with);
    auto phys = ecs.getComponent<cPhysics>(with);
    for (auto tag : c.mesh.getTags())
    {
        auto bb = c.mesh.getBoundingBox(tag);
        double x = bb.centre.x;
        double y = bb.centre.y;

        Hop::System::Physics::CollisionMesh nm = c.mesh.getSubMesh(tag);

        glm::vec3 rngc = randomColour();
        ren.r = rngc.r;
        ren.g = rngc.g;
        ren.b = rngc.b;

        Id nid = ecs.createObject();
        ecs.addComponent<cTransform>(nid, cTransform(x,y,trans.theta,trans.scale));
        ecs.addComponent<cRenderable>(nid, ren);
        phys.lastX = x;
        phys.lastY = y;
        ecs.addComponent<cPhysics>(nid, phys);
        ecs.addComponent<cCollideable>(nid, cCollideable(nm));
        objects.push_back(nid);

        ecs.getComponent<cCollideable>(nid).mesh.reinitialise();
    }
    ecs.remove(with);
    auto it = std::find(objects.begin(), objects.end(), with);
    if (it != objects.end())
    {
        objects.erase(it);
    }
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void message()
{
	__attribute__((unused)) static const char * message = "// ******************************** Hello data miners, you are welcome to poke around! The code is FOSS though ;) \n Checkout the repo https://github.com/JerboaBurrow/JellyCram ********************************";
}

#pragma GCC pop_options