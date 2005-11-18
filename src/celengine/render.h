// render.h
//
// Copyright (C) 2001, Chris Laurel <claurel@shatters.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#ifndef _RENDER_H_
#define _RENDER_H_

#include <vector>
#include <string>
#include <celengine/observer.h>
#include <celengine/universe.h>
#include <celengine/selection.h>
#include <celengine/glcontext.h>
#include <celengine/starcolors.h>
#include <celengine/rendcontext.h>
#include <celtxf/texturefont.h>


struct RenderListEntry
{
    const Star* star;
    Body* body;
    Point3f position;
    Vec3f sun;
    float distance;
    float radius;
    float nearZ;
    float farZ;
    float discSizeInPixels;
    float appMag;
    bool isCometTail;
    int depthBucket;
    int solarSysIndex;
};

static const unsigned int MaxSolarSystems = 16;


class Renderer
{
 public:
    Renderer();
    ~Renderer();

    struct DetailOptions
    {
        DetailOptions();
        unsigned int ringSystemSections;
        unsigned int orbitPathSamplePoints;
        unsigned int shadowTextureSize;
        unsigned int eclipseTextureSize;
    };

    bool init(GLContext*, int, int, DetailOptions&);
    void shutdown() {};
    void resize(int, int);

    float calcPixelSize(float fov, float windowHeight);
    void setFaintestAM45deg(float);
    float getFaintestAM45deg();

    void setRenderMode(int);
    void autoMag(float& faintestMag);
    void render(const Observer&,
                const Universe&,
                float faintestVisible,
                const Selection& sel);
    
    enum {
        NoLabels            = 0x000,
        StarLabels          = 0x001,
        PlanetLabels        = 0x002,
        MoonLabels          = 0x004,
        ConstellationLabels = 0x008,
        GalaxyLabels        = 0x010,
        AsteroidLabels      = 0x020,
        SpacecraftLabels    = 0x040,
        LocationLabels      = 0x080,
        CometLabels         = 0x100,
	NebulaLabels        = 0x200,
	OpenClusterLabels   = 0x400,
        I18nConstellationLabels = 0x800,
        BodyLabelMask       = (PlanetLabels | MoonLabels | AsteroidLabels | SpacecraftLabels | CometLabels),
    };

    enum {
        ShowNothing         = 0x0000,
        ShowStars           = 0x0001,
        ShowPlanets         = 0x0002,
        ShowGalaxies        = 0x0004,
        ShowDiagrams        = 0x0008,
        ShowCloudMaps       = 0x0010,
        ShowOrbits          = 0x0020,
        ShowCelestialSphere = 0x0040,
        ShowNightMaps       = 0x0080,
        ShowAtmospheres     = 0x0100,
        ShowSmoothLines     = 0x0200,
        ShowEclipseShadows  = 0x0400,
        ShowStarsAsPoints   = 0x0800,
        ShowRingShadows     = 0x1000,
        ShowBoundaries      = 0x2000,
        ShowAutoMag         = 0x4000,
        ShowCometTails      = 0x8000,
        ShowMarkers         = 0x10000,
        ShowPartialTrajectories = 0x20000,
	ShowNebulae         = 0x40000,
	ShowOpenClusters    = 0x80000,
    };

    enum StarStyle 
    {
        FuzzyPointStars  = 0,
        PointStars       = 1,
        ScaledDiscStars  = 2,
        StarStyleCount   = 3,
    };

    int getRenderFlags() const;
    void setRenderFlags(int);
    int getLabelMode() const;
    void setLabelMode(int);
    void addLabelledStar(Star*, const std::string&);
    void clearLabelledStars();
    float getAmbientLightLevel() const;
    void setAmbientLightLevel(float);
    float getMinimumOrbitSize() const;
    void setMinimumOrbitSize(float);
    float getMinimumFeatureSize() const;
    void setMinimumFeatureSize(float);
    float getDistanceLimit() const;
    void setDistanceLimit(float);
    int getOrbitMask() const;
    void setOrbitMask(int);
    const ColorTemperatureTable* getStarColorTable() const;
    void setStarColorTable(const ColorTemperatureTable*);

    bool getFragmentShaderEnabled() const;
    void setFragmentShaderEnabled(bool);
    bool fragmentShaderSupported() const;
    bool getVertexShaderEnabled() const;
    void setVertexShaderEnabled(bool);
    bool vertexShaderSupported() const;

    GLContext* getGLContext() { return context; }

    float getSaturationMagnitude() const;
    void setSaturationMagnitude(float);
    float getBrightnessBias() const;
    void setBrightnessBias(float);
    void setStarStyle(StarStyle);
    StarStyle getStarStyle() const;
    void setResolution(unsigned int resolution);
    unsigned int getResolution();

    void loadTextures(Body*);

    typedef struct {
        std::string text;
        Color color;
        Point3f position;
    } Label;

    void addLabel(std::string, Color, const Point3f&, float depth = -1);
    void addSortedLabel(std::string, Color, const Point3f&);
    void clearLabels();

    void setFont(TextureFont*);
    TextureFont* getFont() const;

 public:
    // Internal types
    // TODO: Figure out how to make these private.  Even with a friend
    // 
    struct Particle
    {
        Point3f center;
        float size;
        Color color;
        float pad0, pad1, pad2;
    };

    struct RenderProperties
    {
        RenderProperties() :
            surface(NULL),
            atmosphere(NULL),
            rings(NULL),
            radius(1.0f),
            semiAxes(1.0f, 1.0f, 1.0f),
            model(InvalidResource),
            orientation(1.0f),
            locations(NULL)
        {};

        Surface* surface;
        const Atmosphere* atmosphere;
        RingSystem* rings;
        float radius;
        Vec3f semiAxes;
        ResourceHandle model;
        Quatf orientation;
        std::vector<EclipseShadow>* eclipseShadows;
        std::vector<Location*>* locations;
    };

    class StarVertexBuffer
    {
    public:
        StarVertexBuffer(unsigned int _capacity);
        ~StarVertexBuffer();
        void start(bool _usePoints);
        void render();
        void finish();
        void addStar(const Point3f&, const Color&, float);
        void setBillboardOrientation(const Quatf&);

    private:
        unsigned int capacity;
        unsigned int nStars;
        float* vertices;
        float* texCoords;
        unsigned char* colors;
        Vec3f v0, v1, v2, v3;
        bool usePoints;
    };

    struct LightSource
    {
        Point3d position;
        Color color;
        float luminosity;
        float radius;
    };

 private:
    struct SkyVertex
    {
        float x, y, z;
        unsigned char color[4];
    };

    struct SkyContourPoint
    {
        Vec3f v;
        Vec3f eyeDir;
        float centerDist;
        float eyeDist;
        float cosSkyCapAltitude;
    };

    template <class OBJ> struct ObjectLabel
    {
        OBJ*        obj;
        std::string label;

        ObjectLabel() :
            obj  (NULL),
            label("")
        {};

        ObjectLabel(OBJ* _obj, const std::string& _label) :
            obj  (_obj),
            label(_label)
        {};

        ObjectLabel(const ObjectLabel& objLbl) :
            obj  (objLbl.obj),
            label(objLbl.label)
        {};

        ObjectLabel& operator = (const ObjectLabel& objLbl)
        {
            obj   = objLbl.obj;
            label = objLbl.label;
            return *this;
        };
    };

    typedef ObjectLabel<Star>          StarLabel;
    typedef ObjectLabel<DeepSkyObject> DSOLabel;    // currently not used

 private:
    void setFieldOfView(float);
    void renderStars(const StarDatabase& starDB,
                     float faintestVisible,
                     const Observer& observer);
    void renderDeepSkyObjects(const Universe&,
                              const Observer&,
                              float faintestMagNight);
    void renderCelestialSphere(const Observer& observer);
    void renderPlanetarySystem(const Star& sun,
                               const PlanetarySystem& solSystem,
                               const Observer& observer,
                               double now,
                               unsigned int solarSysIndex,
                               bool showLabels = false);

    void renderObject(Point3f pos,
                      float distance,
                      double now,
                      Quatf cameraOrientation,
                      float nearPlaneDistance,
                      float farPlaneDistance,
                      RenderProperties& obj,
                      const LightingState&);

    void renderPlanet(Body& body,
                      Point3f pos,
                      float distance,
                      float appMag,
                      double now,
                      Quatf orientation,
                      const vector<LightSource>& lightSources,
                      float, float);

    void renderStar(const Star& star,
                    Point3f pos,
                    float distance,
                    float appMag,
                    Quatf orientation,
                    double now,
                    float, float);

    void renderCometTail(const Body& body,
                         Point3f pos,
                         float distance,
                         float appMag,
                         double now,
                         Quatf orientation,
                         float, float);

    void renderBodyAsParticle(Point3f center,
                              float appMag,
                              float _faintestMag,
                              float discSizeInPixels,
                              Color color,
                              const Quatf& orientation,
                              float renderDistance,
                              bool useHaloes);

    void renderEllipsoidAtmosphere(const Atmosphere& atmosphere,
                                   Point3f center,
                                   const Quatf& orientation,
                                   Vec3f semiAxes,
                                   const Vec3f& sunDirection,
                                   Color ambientColor,
                                   float fade,
                                   bool lit);

    void renderLocations(const vector<Location*>& locations,
                         const Quatf& cameraOrientation,
                         const Point3f& position,
                         const Quatf& orientation,
                         float scale);

    bool testEclipse(const Body& receiver,
                     const Body& caster,
                     const DirectionalLight& light,
                     double now,
                     vector<EclipseShadow>& shadows);

    void labelStars(const std::vector<StarLabel>& labelledStars,
                    const StarDatabase&,
                    const Observer&);
    void labelConstellations(const AsterismList& asterisms,
                             const Observer& observer);
    void renderParticles(const std::vector<Particle>& particles,
                         Quatf orientation);
    void renderLabels();
    std::vector<Label>::iterator renderSortedLabels(std::vector<Label>::iterator,
                                                    float depth);
    void renderMarkers(const MarkerList&,
                       const UniversalCoord& position,
                       const Quatf& orientation,
                       double jd);

    void renderOrbit(Body*, double);
    void renderOrbits(PlanetarySystem*, const Selection&, double,
                      const Point3d&, const Point3d&);
    void renderForegroundOrbits(const PlanetarySystem* system,
                                const Point3f &center,
                                float distance,
                                float discSizeInPixels,
                                const Selection& sel, 
                                double t);

 private:
    GLContext* context;

    int windowWidth;
    int windowHeight;
    float fov;
    float corrFac;
    float pixelSize;
    float faintestAutoMag45deg;
    TextureFont* font;

    int renderMode;
    int labelMode;
    int renderFlags;
    int orbitMask;
    float ambientLightLevel;
    bool fragmentShaderEnabled;
    bool vertexShaderEnabled;
    float brightnessBias;

    float brightnessScale;
    float faintestMag;
    float faintestPlanetMag;
    float saturationMagNight;
    float saturationMag;
    StarStyle starStyle;

    Color ambientColor;
    std::string displayedSurface;

    StarVertexBuffer* starVertexBuffer;
    std::vector<RenderListEntry> renderList;
    std::vector<Particle> glareParticles;
    std::vector<Label> labels;
    std::vector<Label> depthSortedLabels;
    std::vector<EclipseShadow> eclipseShadows[MaxLights];
    std::vector<const Star*> nearStars;

    std::vector<LightSource> lightSourceLists[MaxSolarSystems];

    std::vector<StarLabel> labelledStars;
    std::vector<DSOLabel>  labelledDSOs;

    double modelMatrix[16];
    double projMatrix[16];

    bool useCompressedTextures;
    bool useVertexPrograms;
    bool useRescaleNormal;
    bool useMinMaxBlending;
    bool useClampToBorder;
    unsigned int textureResolution;

    DetailOptions detailOptions;

 public:
    struct OrbitSample 
    {
        double t;
        Point3f pos;
    };

 private:
    struct CachedOrbit
    {
        Body* body;
        std::vector<OrbitSample> trajectory;
        bool keep;
    };
    std::vector<CachedOrbit*> orbitCache;

    float minOrbitSize;
    float distanceLimit;
    float minFeatureSize;
    uint32 locationFilter;

    SkyVertex* skyVertices;
    uint32* skyIndices;
    SkyContourPoint* skyContour;

    const ColorTemperatureTable* colorTemp;
};

#endif // _RENDER_H_
