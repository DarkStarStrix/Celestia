// simulation.h
//
// Copyright (C) 2001, Chris Laurel <claurel@shatters.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <celengine/texture.h>
#include <celengine/universe.h>
#include <celengine/astro.h>
#include <celengine/galaxy.h>
#include <celengine/globular.h>
#include <celengine/texmanager.h>
#include <celengine/frame.h>
#include <celengine/observer.h>


class Renderer;

class Simulation
{
 public:
    Simulation(Universe*);
    ~Simulation();

    double getTime() const; // Julian date
    void setTime(double jd);

    double getRealTime() const;
    double getArrivalTime() const;

    void update(double dt);
    void render(Renderer&);
    void draw(Renderer&);
    void render(Renderer&, Observer&);

    Selection pickObject(const Eigen::Vector3f& pickRay, std::uint64_t renderFlags, float tolerance = 0.0f);

    Universe* getUniverse() const;

    void orbit(const Eigen::Quaternionf& q);
    void rotate(const Eigen::Quaternionf& q);
    void changeOrbitDistance(float d);
    void setTargetSpeed(float s);
    float getTargetSpeed();

    Selection getSelection() const;
    void setSelection(const Selection&);
    Selection getTrackedObject() const;
    void setTrackedObject(const Selection&);

    void selectPlanet(int);
    Selection findObject(std::string_view s, bool i18n = false) const;
    Selection findObjectFromPath(std::string_view s, bool i18n = false) const;
    void getObjectCompletion(std::vector<std::string>& completion,
                             std::string_view s,
                             bool i18n,
                             bool withLocations = false) const;
    void gotoSelection(double gotoTime,
                       const Eigen::Vector3f& up,
                       ObserverFrame::CoordinateSystem upFrame);
    void gotoSelection(double gotoTime, double distance,
                       const Eigen::Vector3f& up,
                       ObserverFrame::CoordinateSystem upFrame);
    void gotoSelectionLongLat(double gotoTime,
                              double distance,
                              float longitude, float latitude,
                              const Eigen::Vector3f& up);
    void gotoLocation(const UniversalCoord& toPosition,
                      const Eigen::Quaterniond& toOrientation,
                      double duration);
    void getSelectionLongLat(double& distance,
                             double& longitude,
                             double& latitude);
    void gotoSurface(double duration);
    void centerSelection(double centerTime = 0.5);
    void centerSelectionCO(double centerTime = 0.5);
    void follow();
    void geosynchronousFollow();
    void phaseLock();
    void chase();
    void cancelMotion();

    Observer& getObserver();
    void setObserverPosition(const UniversalCoord&);
    void setObserverOrientation(const Eigen::Quaternionf&);
    void reverseObserverOrientation();

    Observer* addObserver();
    void removeObserver(Observer*);
    Observer* getActiveObserver();
    void setActiveObserver(Observer*);

    SolarSystem* getNearestSolarSystem() const;

    double getTimeScale() const;
    void setTimeScale(double);
    bool getSyncTime() const;
    void setSyncTime(bool);
    void synchronizeTime();
    bool getPauseState() const;
    void setPauseState(bool);

    float getFaintestVisible() const;
    void setFaintestVisible(float);

    void setObserverMode(Observer::ObserverMode);
    Observer::ObserverMode getObserverMode() const;

    void setFrame(ObserverFrame::CoordinateSystem, const Selection& refObject, const Selection& targetObject);
    void setFrame(ObserverFrame::CoordinateSystem, const Selection& refObject);
    const ObserverFrame::SharedConstPtr& getFrame() const;

 private:
    SolarSystem* getSolarSystem(const Star* star);

 private:
    double realTime{ 0.0 };
    double timeScale{ 1.0 };
    double storedTimeScale{ 1.0 };
    bool syncTime{ true };

    Universe* universe;

    mutable std::optional<SolarSystem*> closestSolarSystem{ std::nullopt };
    Selection selection;

    Observer* activeObserver;
    std::vector<Observer*> observers;

    float faintestVisible{ 5.0f };
    bool pauseState{ false };
};
