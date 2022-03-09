// StartupWindow.cxx - run window on startup 
// Copyright (C) 2022 James Turner
// SPDX-License-Identifier: GPL-2.0-or-later

#include "config.h"

#include "StartupWindow.hxx"

#include <cassert>

#include <osg/GraphicsContext>
#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>

#include <simgear/canvas/Canvas.hxx>
#include <simgear/canvas/CanvasMgr.hxx>
#include <simgear/debug/logstream.hxx>
#include <simgear/structure/commands.hxx>

#include <Main/fg_props.hxx>
#include <Viewer/WindowSystemAdapter.hxx>
#include <Network/HTTPClient.hxx>
#include <Scripting/NasalSys.hxx>
#include <GUI/new_gui.hxx>
#include <Canvas/gui_mgr.hxx>
#include <Canvas/FGCanvasSystemAdapter.hxx>
#include <Navaids/NavDataCache.hxx> // needed by NewGUI XML cache
#include <Main/util.hxx>
#include <Main/fg_commands.hxx>
#include <Viewer/splash.hxx>

namespace sc = simgear::canvas;

namespace flightgear
{

class StartupWindow::StartupWindowPrivate
{
public:
    bool _done = false;
    osg::ref_ptr<osgViewer::Viewer> _viewer;
    osg::ref_ptr<flightgear::GraphicsWindow> _window;
    osg::ref_ptr<SplashScreen> _splash;
};

///////////////////////////////////////////////////////////////////////////////

class StartupEventHandler : public osgGA::GUIEventHandler
{
public:
    
};

///////////////////////////////////////////////////////////////////////////////

class StartupCanvasManager : public sc::CanvasMgr
{
public:
    StartupCanvasManager(osg::GraphicsContext* gc) :
        sc::CanvasMgr( fgGetNode("/canvas/by-index", true) )
    {
    }

    static const char* staticSubsystemClassId() { return "Canvas"; }

private:
};

/**
 @ brief customise the system adapter to avoid touching the renderer (which doesn't exist during startup)
 */
class StartupSystemAdapter : public canvas::FGCanvasSystemAdapter
{
public:
    StartupSystemAdapter(StartupWindow* sv) :
        _startupViewer(sv)
    {
        assert(sv);
    }
    
    void addCamera(osg::Camera* camera) const override
    {
        _startupViewer->d->_viewer->addSlave(camera);
    }
    
    void removeCamera(osg::Camera* camera) const override
    {
        auto osgViewer = _startupViewer->d->_viewer;
        const auto index = osgViewer->findSlaveIndexForCamera(camera);
        osgViewer->removeSlave(index);
    }
    
private:
    StartupWindow* _startupViewer;
};

///////////////////////////////////////////////////////////////////////////////

StartupWindow::StartupWindow() :
    d(new StartupWindowPrivate)
{

}

StartupWindow::~StartupWindow()
{
    fgSetBool("/sim/gui/startup", false);
}

void StartupWindow::init()
{
    d->_viewer = new osgViewer::Viewer;
    //viewer->getCamera()->setProjectionResizePolicy(osg::Camera::FIXED);
    // /d->_viewer->setEventHandler(eh);

    fgSetBool("/sim/gui/startup", true);
    fgSetBool("/sim/startup/splash-screen", true);
    fgSetDouble("/sim/startup/splash-alpha", 1.0);
}

void StartupWindow::createWindow()
{
    flightgear::WindowSystemAdapter::setWSA(new flightgear::WindowSystemAdapter);

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = "FlightGear Startup";
    traits->width = 1024;
    traits->height = 768;
    traits->supportsResize = true;
    traits->windowDecoration = true;
    traits->vsync = true;
    traits->setUndefinedScreenDetailsToDefaultScreen();

    osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits);
    if (!gc) {
        return;
    }
    
    d->_window = flightgear::WindowSystemAdapter::getWSA()->registerWindow(gc, "Startup");
    
    fgSetInt("/sim/startup/xsize", traits->width);
    fgSetInt("/sim/startup/ysize", traits->height);
    
#if 1

    auto cam = d->_viewer->getCamera();
    cam->setGraphicsContext(gc);
    cam->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
  //  cam->setProjectionResizePolicy(osg::Camera::FIXED);
    cam->setCullingMode(osg::CullSettings::NO_CULLING);
    //cam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    cam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    cam->setViewMatrix(osg::Matrix::identity());
    cam->setProjectionMatrixAsOrtho2D(0.0, traits->width, 0.0, traits->height);
    
   // cam->setProjectionMatrix( osg::Matrix::perspective( 30., (double)traits->width/(double)traits->height, 1., 100. ) );

    cam->setDrawBuffer(GL_BACK);
    cam->setReadBuffer(GL_BACK);
    
    osg::ref_ptr<osg::DisplaySettings> ds = new osg::DisplaySettings;
    cam->setDisplaySettings(ds.get());
#endif
    
    gc->setClearColor(osg::Vec4f(0.2f,0.2f,0.6f,1.0f));
    gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
 //   d->_viewer->addSlave(cam);
//

    
    d->_splash = new SplashScreen;
    d->_viewer->setSceneData(d->_splash.get());
    
    d->_viewer->realize();
}

void StartupWindow::setNasalModules()
{
    SGPropertyNode_ptr nasalLoadNode = fgGetNode("/sim/nasal-load-priority");
    nasalLoadNode->removeAllChildren();
    for (auto n : string_list{"props", "globals", "math", "string", "io", "std",
        "debug", "modules", "gui", "geo"
    })
    {
        auto cn = nasalLoadNode->addChild("file");
        cn->setStringValue(n + ".nas");
    }
  
    const auto startupModules = string_list{"std", "debug", "canvas", "gui"};
    SGPropertyNode_ptr nasalModuleNode = fgGetNode("/nasal");
    for (int c=0; c < nasalModuleNode->nChildren(); ++c) {
        auto cn = nasalModuleNode->getChild(c);
        auto it = std::find(startupModules.begin(), startupModules.end(), cn->getNameString());
        cn->setBoolValue("enabled", it != startupModules.end());
    }
}

void StartupWindow::createSubsystems()
{
    // NewGUI needs the cache to exist to record stamps for dialog XML files
    // we don't trigger a rebuild here though, that is handled later.
    if (!flightgear::NavDataCache::instance()) {
        flightgear::NavDataCache::createInstance();
    }
    
    fgInitAllowedPaths();
    
    sc::Canvas::setSystemAdapter(sc::SystemAdapterPtr(new StartupSystemAdapter{this}));
    
    auto guiMgr = new GUIMgr;
    guiMgr->setGUIViewAndCamera(d->_viewer.get(), nullptr);
    globals->add_subsystem("CanvasGUI", guiMgr, SGSubsystemMgr::DISPLAY);
    
    auto canvasManager = new StartupCanvasManager{d->_window->gc.get()};
    globals->add_subsystem("Canvas", canvasManager, SGSubsystemMgr::DISPLAY);
    
    globals->add_new_subsystem<NewGUI>(SGSubsystemMgr::INIT);
    globals->add_new_subsystem<FGHTTPClient>(SGSubsystemMgr::GENERAL);
    
    fgInitCommands();
    fgInitSceneCommands();
    
    setNasalModules();
}

void StartupWindow::shutdown()
{
    // anything more needed to destroy the window?
    // eg to unregister it from the WSA?
    d->_window = nullptr;
    
    d->_viewer->setDone(true);
    d->_viewer = nullptr;
    
    fgSetBool("/sim/gui/startup", false);
}

void StartupWindow::frame()
{
    if (!d->_viewer->isRealized()) {
        d->_viewer->realize();
    }

    if (d->_viewer->done()) {
        d->_done = true;
        return;
    }

    d->_viewer->frame();
}

bool StartupWindow::isDone() const
{
    return d->_done;
}

///////////////////////////////////////////////////////////////////////////////


void runStartupWindow()
{
    std::unique_ptr<StartupWindow> sv{new StartupWindow};
    sv->init();
    sv->createWindow();
    
    sv->createSubsystems();
    
    
    auto subsystemManager = globals->get_subsystem_mgr();
    
    try {
        subsystemManager->bind();
        subsystemManager->init();

        // add Nasal now, after other systems did their init
        globals->add_new_subsystem<FGNasalSys>(SGSubsystemMgr::INIT);

        
        subsystemManager->postinit();
    } catch (sg_exception& e) {
        SG_LOG(SG_GUI, SG_ALERT, "Failed to init startup subsystems");
    }
    
    ////////
    
    SGCommandMgr* mgr = SGCommandMgr::instance();
    SGPropertyNode_ptr propArgs(new SGPropertyNode);
    propArgs->setStringValue("dialog-name", "location-in-air");
    mgr->execute("dialog-show", propArgs, globals->get_props());
    
    //////
    
    while (!sv->isDone()) {
        // calculate dt from the wall clock?
        subsystemManager->update(0.0);
        fgSplashProgress("", 0);
        sv->frame();
    }
    
    subsystemManager->shutdown();
//    subsystemManager->unbind();
    
    // delete subsystems
    for (int g=0; g<SGSubsystemMgr::MAX_GROUPS; ++g) {
        auto group = subsystemManager->get_group(static_cast<SGSubsystemMgr::GroupType>(g));
        for (auto nm : group->member_names()) {
            subsystemManager->remove(nm.c_str());
        }
    }
}

} // of namespace flightgear

